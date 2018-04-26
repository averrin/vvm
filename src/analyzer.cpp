#include "vvm/analyzer.hpp"
#include "vvm/container.hpp"
#include "vvm/constants.hpp"
#include "vvm/address.hpp"
#include "vvm/analyzer/code_instruction.hpp"
#include "rang.hpp"
#include "format.h"
#include <utility>
#include <sstream>
#include <fstream>
#include <variant>

using namespace analyzer;

Analyzer::Analyzer() {}

std::map<std::string, address> reserved_addresses = {
    {"STATE", STATE},
    {"ESP", ESP},
    {"EAX", EAX},
    {"EBX", EBX},
    {"ECX", ECX},
    {"EIP", EIP},
    {"FLAGS", FLAGS},
    {"INTERRUPTS", INTERRUPTS},
    {"OUT_PORT", OUT_PORT}
};

std::vector<std::string> split(std::string strToSplit, char delimeter)
{
    std::stringstream ss(strToSplit);
    std::string item;
    std::vector<std::string> splittedStrings;
    while (std::getline(ss, item, delimeter))
    {
       splittedStrings.push_back(item);
    }
    return splittedStrings;
}

bool isReservedMem(std::string arg) {
    for_each(arg.begin(), arg.end(), [](char& in){ in = ::toupper(in); });
    auto it = reserved_addresses.find(arg);
    return it != reserved_addresses.end();
}

int get_spec_length(opSpec spec) {
    auto real_length = 1;
    switch (spec.type)
    {
    case opSpec::MM:
    case opSpec::MC:
        real_length = OP_long_length;
        break;
    case opSpec::MB:
        real_length = OP_med_ex_length;
        break;
    case opSpec::M:
    case opSpec::C:
        real_length = OP_med_length;
        break;
    case opSpec::B:
        real_length = OP_short_length;
        break;
    case opSpec::Z: break;
    default:;
    }
    return real_length;
}

script Analyzer::compile(std::string filename)
{
	script code = {};
    std::ifstream vvmc_file(filename);
    std::string line;
	auto local_pointer = CODE_OFFSET.dst;

    std::string last_label = "";
    std::map<std::string, std::unique_ptr<code_instruction>> pending_jumps;
    if (vvmc_file.is_open())
    {

        auto n = 0;
        while ( getline (vvmc_file, line) )
        {
        if(line[0] == '#') {
            continue;
        }

        if(line.back() == ':') {
            line.erase(std::find_if(line.rbegin(), line.rend(), [](int ch) {
                return ch != ':';
            }).base(), line.end());
            last_label = line;
            continue;
        }
        auto tokens = split(line, ' ');
        auto op = tokens.front();
        std::string arg1, arg2;
        auto specType = opSpec::Z;
        instruction_arg parsed_arg1;
        instruction_arg parsed_arg2 ;
        bool pending = false;

        if (tokens.size() > 1) {
            arg1 = tokens[1];
            if (isReservedMem(arg1)) {
                specType = opSpec::M;
                parsed_arg1 = address{reserved_addresses[arg1]};
            } else {
                specType = opSpec::C;
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
                    specType = opSpec::B;
                }
                if (op == "JMP" || op == "JMP" || op == "JE") {
                    specType = opSpec::M;
                    parsed_arg1 = address{0x0};
                    pending = true;
                }
            }
        }
        if (tokens.size() > 2) {
            arg2 = tokens[2];
            if (isReservedMem(arg2) && specType == opSpec::M) {
                specType = opSpec::MM;
                parsed_arg2 = address{reserved_addresses[arg2]};
            } else {
                specType = opSpec::MC;

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
                    if (parsed && std::get<unsigned int>(parsed_arg2) < 256) {
                        specType = opSpec::MB;
                    }
                }
            }
        }

        const auto spec = Container::getSpec([&](opSpec s) {
			return s.name == op && s.type == specType;
		});

        if (spec == INVALID_spec) {
            std::cout << "Unable to parse line: " << line << std::endl;;
            std::cout << op << " " << specType << std::endl;;
            continue;
        }
        std::array<std::byte, OP_long_length> mem{ std::byte{0x0} };
        code_instruction i{ local_pointer, spec, mem };
        auto real_length = get_spec_length(spec);
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
            pending_jumps.insert(std::pair(arg1, std::make_unique<code_instruction>(i)));
        }

        n++;
        }

        code_instruction i{ local_pointer, INT_spec };
        auto real_length = get_spec_length(INT_spec);

        i.arg1 = INT_END;
        i.aliases.push_back(fmt::format("{}", n));

        code.push_back(i);
        vvmc_file.close();
    }
    else std::cout << "Unable to open file";

    fmt::print("\n");
    for(auto const &jump : pending_jumps) {
        auto label = jump.first;
        auto i = *(jump.second.get());
        fmt::print("{} >> {} >> ", i, label);
		const auto dst = std::find_if(code.begin(), code.end(), [&](code_instruction ins) {
			return std::find(ins.aliases.begin(), ins.aliases.end(), label) != ins.aliases.end();
		});
        if (dst != code.end()) {
            fmt::print("{}\n", *dst);
            i.arg1 = (*dst).offset;
            fmt::print("{}\n", i);
        } else fmt::print("not found");
    }

    for(auto i : code) {
        std::cout << i << std::endl;
    }

    return code;
}

script Analyzer::disassemble(Container *mem)
{
	const auto temp_pointer = mem->pointer;
	script code = {};
	mem->seek(CO_ADDR);
	auto offset = mem->readByte();
	auto local_pointer = address{ static_cast<unsigned int>(offset) };
    auto n = 0;
	while (local_pointer.dst < BUF_SIZE)
	{
		mem->seek(local_pointer);
		const auto opcode = mem->readByte();
		const auto spec = mem->getSpec([&](opSpec s) {
			return s.opcode == opcode;
		});
		if (spec == INVALID_spec) {
			fmt::print("Unknown opcode");
			break;
		}
		std::array<std::byte, OP_long_length> instruction_mem{ std::byte{0x0} };
		code_instruction i{ local_pointer, spec, instruction_mem };
		local_pointer++;
        auto real_length = get_spec_length(spec);
		local_pointer--;
		for (auto n = 0; n < real_length; n++)
		{
			instruction_mem[n] = mem->_bytes[local_pointer.dst];
			local_pointer++;
		}
		i.mem = instruction_mem;
        local_pointer -= real_length - 1;
        mem->seek(local_pointer);

		switch (i.spec.type) {
		case opSpec::MM:
            i.arg1 = mem->readAddress();
            i.arg2 = mem->readAddress();
			break;
		case opSpec::MB:
            i.arg1 = mem->readAddress();
            i.arg2 = mem->readByte();
			break;
		case opSpec::MC:
            i.arg1 = mem->readAddress();
            i.arg2 = mem->readInt();
			break;
		case opSpec::M:
            i.arg1 = mem->readAddress();
			break;
		case opSpec::C:
            i.arg1 = mem->readInt();
			break;
		case opSpec::B:
            i.arg1 = mem->readByte();
			break;
		case opSpec::Z:
			break;
		default:;
		}
        local_pointer = mem->pointer;

        i.aliases.push_back(fmt::format("{}", n));
        n++;
		code.push_back(i);

		if (opcode == INT_spec.opcode && i.mem[1] == INT_END)
		{
			break;
		}
        // local_pointer += real_length + 1;
        // seek(local_pointer);

	}
	mem->seek(temp_pointer);
	return code;
}

