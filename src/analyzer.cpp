#include "vvm/analyzer.hpp"
#include "format.h"
#include "rang.hpp"
#include "vvm/address.hpp"
#include "vvm/analyzer/code_instruction.hpp"
#include "vvm/constants.hpp"
#include "vvm/core.hpp"
#include <fstream>
#include <optional>
#include <sstream>
#include <utility>
#include <variant>

using namespace analyzer;

Analyzer::Analyzer() {}

// TODO: to utils
std::vector<std::string> split(std::string strToSplit, char delimeter) {
  std::stringstream ss(strToSplit);
  std::string item;
  std::vector<std::string> splittedStrings;
  while (std::getline(ss, item, delimeter)) {
    splittedStrings.push_back(item);
  }
  return splittedStrings;
}

int get_spec_length(op_spec spec) {
  auto real_length = 1;
  switch (spec.type) {
  case op_spec::AA:
    real_length = OP_aa_length;
    break;
  case op_spec::AI:
    real_length = OP_ai_length;
    break;
  case op_spec::AW:
    real_length = OP_aw_length;
    break;
  case op_spec::A:
    real_length = OP_a_length;
    break;
  case op_spec::I:
    real_length = OP_i_length;
    break;
  case op_spec::W:
    real_length = OP_w_length;
    break;
  case op_spec::Z:
    break;
  default:;
  }
  return real_length;
}

script Analyzer::parseFile(std::string filename) {
  script code = {};
  std::ifstream vvmc_file(filename);
  std::string line;
  auto local_pointer = CODE_OFFSET.dst;

  std::string last_label = "";
  std::map<int, std::string> pending_jumps;
  if (vvmc_file.is_open()) {

    auto n = 0;
    while (getline(vvmc_file, line)) {
      if (line.front() == ';') {
        continue;
      }

      if (line.back() == ':') {
        line.erase(std::find_if(line.rbegin(), line.rend(),
                                [](int ch) { return ch != ':'; })
                       .base(),
                   line.end());
        last_label = line;
        continue;
      }
      auto tokens = split(line, ' ');
      if (tokens.size() == 0) continue;
      auto op = tokens.front();
      std::string arg1, arg2;
      auto specType = op_spec::Z;
      instruction_arg parsed_arg1;
      instruction_arg parsed_arg2;
      bool pending = false;

      if (tokens.size() > 1) {
        arg1 = tokens[1];
        if (auto a = Core::isReservedMem(arg1); a != std::nullopt) {
          specType = op_spec::A;
          parsed_arg1 = a.value();
        } else if (auto a = arg1.length() > 1 ? Core::isReservedMem(arg1.substr(1, arg1.length()-2)) : std::nullopt;
                   a != std::nullopt && arg1.front() == '[' && arg1.back() == ']') {
          specType = op_spec::A;
          auto addr = a.value();
          addr.redirect = true;
          parsed_arg1 = addr;
        } else {
          specType = op_spec::I;
          bool parsed = false;
          try {
            parsed_arg1 = std::stoul(arg1, nullptr, 16);
            parsed = true;
          } catch (std::invalid_argument e) {
            try {
              parsed_arg1 = std::stoul(arg1, nullptr, 10);
              parsed = true;
            } catch (std::invalid_argument e) {
              parsed = false;
            }
          }
          if (parsed && std::get<unsigned int>(parsed_arg1) < 256) {
            parsed_arg1 = std::byte{std::get<unsigned int>(parsed_arg1)}; 
            specType = op_spec::W;
          }
          if (op == "JMP" || op == "JE" || op == "JNE") {
            specType = op_spec::A;
            parsed_arg1 = address{0x0};
            pending = true;
          }
        }
      }
      if (tokens.size() > 2) {
        arg2 = tokens[2];
        if (auto a = Core::isReservedMem(arg2); a != std::nullopt && specType == op_spec::A) {
          specType = op_spec::AA;
          parsed_arg2 = a.value();
        } else if (auto a = arg2.length() > 1 ? Core::isReservedMem(arg2.substr(1, arg2.length()-2)) : std::nullopt;
                   a != std::nullopt && arg2.front() == '[' && arg2.back() == ']') {
          specType = op_spec::AA;
          auto addr = a.value();
          addr.redirect = true;
          parsed_arg1 = addr;
        } else {
          specType = op_spec::AI;

          bool parsed = false;
          try {
            parsed_arg2 = std::stoul(arg2, nullptr, 16);
            parsed = true;
          } catch (std::invalid_argument e) {
            try {
              parsed_arg2 = std::stoul(arg2, nullptr, 10);
              parsed = true;
            } catch (std::invalid_argument e) {
              parsed = false;
            }
          }
        if (parsed && std::get<unsigned int>(parsed_arg2) < 256) {
            parsed_arg2 = std::byte{std::get<unsigned int>(parsed_arg2)}; 
            specType = op_spec::AW;
        }
        }
      }

      const auto spec = Core::getSpec(
          [&](op_spec s) { return s.name == op && s.type == specType; });

      if (spec == std::nullopt) {
        std::cout << "Unable to parse line: " << line << std::endl;
        std::cout << op << " " << specType << std::endl;
        continue;
      }
      code_instruction i{{local_pointer}, *spec};
      auto real_length = get_spec_length(*spec);
      local_pointer += real_length;

      i.arg1 = parsed_arg1;
      i.arg2 = parsed_arg2;
      i.aliases.push_back(fmt::format("{}", n));
      if (last_label != "") {
        i.aliases.push_back(last_label);
        last_label = "";
      }

      code.push_back(i);
      if (pending) {
        pending_jumps.insert(std::pair(n, arg1));
      }

      n++;
    }

    code_instruction i{{local_pointer}, INT_spec};
    auto real_length = get_spec_length(INT_spec);

    i.arg1 = INT_END;
    i.aliases.push_back(fmt::format("{}", n));

    code.push_back(i);
    vvmc_file.close();
  } else
    std::cout << "Unable to open file";

  for (auto[n, label] : pending_jumps) {
      std::cout << n << " " << label << std::endl;
    const auto dst =
        std::find_if(code.begin(), code.end(), [&](code_instruction ins) {
          return std::find(ins.aliases.begin(), ins.aliases.end(), label) !=
                 ins.aliases.end();
        });
    if (dst != code.end()) {
      code[n].arg1 = (*dst).offset;
    } else
      fmt::print("not found");
  }

  fmt::print("\nParsed instructions:\n");
  for (auto i : code) {
    std::cout << i << std::endl;
  }
  fmt::print("\n");

  return code;
}

script Analyzer::disassemble(std::shared_ptr<Core> core) {
  const auto temp_pointer = core->pointer;
  script code = {};
  core->seek(CO_ADDR);
  auto offset = core->readByte();
  auto local_pointer = address{static_cast<unsigned int>(offset)};
  auto n = 0;
  while (local_pointer.dst < (core->code->offset + core->code->size).dst) {
    core->seek(local_pointer);
    const auto opcode = core->readByte();
    const auto spec =
        core->getSpec([&](op_spec s) { return s.opcode == opcode; });
    if (spec == std::nullopt) {
      fmt::print("Unknown opcode");
      break;
    }
    code_instruction i{{local_pointer}, *spec};
    local_pointer++;
    auto real_length = get_spec_length(*spec);
    local_pointer--;
    local_pointer -= real_length - 1;
    core->seek(local_pointer);

    switch (i.spec.type) {
    case op_spec::AA:
      i.arg1 = core->readAddress();
      i.arg2 = core->readAddress();
      break;
    case op_spec::AW:
      i.arg1 = core->readAddress();
      i.arg2 = core->readByte();
      break;
    case op_spec::AI:
      i.arg1 = core->readAddress();
      i.arg2 = core->readInt();
      break;
    case op_spec::A:
      i.arg1 = core->readAddress();
      break;
    case op_spec::I:
      i.arg1 = core->readInt();
      break;
    case op_spec::W:
      i.arg1 = core->readByte();
      break;
    case op_spec::Z:
      break;
    default:;
    }
    local_pointer = core->pointer;

    i.aliases.push_back(fmt::format("{}", n));
    n++;
    code.push_back(i);

    if (opcode == INT_spec.opcode && std::get<std::byte>(i.arg1) == INT_END) {
      break;
    }
  }
  core->seek(temp_pointer);
  return code;
}
