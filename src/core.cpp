#include "vvm/core.hpp"
#include "format.h"
#include "rang.hpp"
#include <fstream>
#include <optional>
#include <sstream>
#include <utility>
#include <variant>

std::array<opSpec, 22> specs = {
    INVALID_spec, NOP_spec,    MOV_mm_spec, MOV_mc_spec, ADD_mm_spec,
    ADD_mc_spec,  SUB_mm_spec, SUB_mc_spec, OUT_spec,    CMP_mm_spec,
    CMP_mc_spec,  JNE_a_spec,  JNE_r_spec,  JE_spec,     JMP_a_spec,
    JMP_r_spec,   INT_spec,    PUSH_m_spec, PUSH_c_spec, POP_spec,
    INC_spec,     DEC_spec,
};

address address::BEGIN = address{0x0};
address address::CODE = address{CODE_OFFSET};

Core::Core(const vm_mem b, t_handler th)
    : _tickHandler(std::move(th)), _bytes(b){};

std::optional<opSpec> Core::getSpec(predicate filterFunc) {
  auto spec = std::find_if(specs.begin(), specs.end(), filterFunc);
  if (spec == specs.end()) {
    return {};
  }
  return *spec;
}

void Core::setInterruptHandler(const std::byte interrupt,
                               const t_handler handler) {
  _intHandlers[interrupt] = handler;
}

void Core::compile(analyzer::script instructions) {
  const auto temp_pointer = pointer;

  for (auto i : instructions) {
    auto parsed_arg1 = i.arg1;
    auto parsed_arg2 = i.arg2;
    seek(i.offset);

    switch (i.spec.type) {
    case opSpec::MM:
      writeCode(i.spec.opcode, std::get<address>(parsed_arg1),
                std::get<address>(parsed_arg2));
      break;
    case opSpec::MC:
      writeCode(i.spec.opcode, std::get<address>(parsed_arg1),
                std::get<unsigned int>(parsed_arg2));
      break;
    case opSpec::MB:
      writeCode(i.spec.opcode, std::get<address>(parsed_arg1),
                std::get<std::byte>(parsed_arg2));
      break;
    case opSpec::M:
      writeCode(i.spec.opcode, std::get<address>(parsed_arg1));
      break;
    case opSpec::C:
      writeCode(i.spec.opcode, std::get<unsigned int>(parsed_arg1));
      break;
    case opSpec::B:
      writeCode(i.spec.opcode, std::get<std::byte>(parsed_arg1));
      break;
    case opSpec::Z:
      writeCode(i.spec.opcode);
      break;
    default:;
    }
  }
  seek(temp_pointer);
}

void Core::saveBytes(const std::string_view name) {
  std::ofstream file(static_cast<std::string>(name), std::ios::binary);
  const size_t count = _size / sizeof(std::byte);
  file.write(reinterpret_cast<char *>(&_bytes[0]), count * sizeof(std::byte));
  file.close();
}

void Core::seek(address addr) { pointer = addr; }

unsigned int Core::readInt(vm_mem b, const unsigned int pointer) {
  return (static_cast<int>(b[pointer]) << 24) |
         (static_cast<int>(b[pointer + 1]) << 16) |
         (static_cast<int>(b[pointer + 2]) << 8) |
         (static_cast<int>(b[pointer + 3]));
}

address Core::readAddress() { return address{readInt()}; }

unsigned int Core::readInt() {
  auto b = _bytes;
  const auto n = (static_cast<int>(b[pointer.dst]) << 24) |
                 (static_cast<int>(b[pointer.dst + 1]) << 16) |
                 (static_cast<int>(b[pointer.dst + 2]) << 8) |
                 (static_cast<int>(b[pointer.dst + 3]));
  pointer += INT_SIZE;
  return n;
}

int Core::readSignedInt() {
  auto b = _bytes;
  const auto n = (static_cast<int>(b[pointer.dst]) << 24) |
                 (static_cast<int>(b[pointer.dst + 1]) << 16) |
                 (static_cast<int>(b[pointer.dst + 2]) << 8) |
                 (static_cast<int>(b[pointer.dst + 3]));
  pointer += INT_SIZE;
  return n;
}

void Core::writeAddress(const address n) { writeInt(n.dst); }

void Core::writeInt(const int n) {
  _bytes[pointer.dst] = static_cast<std::byte>((n >> 24) & 0xFF);
  pointer++;
  _bytes[pointer.dst] = static_cast<std::byte>((n >> 16) & 0xFF);
  pointer++;
  _bytes[pointer.dst] = static_cast<std::byte>((n >> 8) & 0xFF);
  pointer++;
  _bytes[pointer.dst] = static_cast<std::byte>(n & 0xFF);
  pointer++;
}

address Core::writeCode(std::byte opcode, address arg1, unsigned int arg2) {
  const auto local_pointer = pointer;
  writeByte(opcode);
  writeAddress(arg1);
  writeInt(arg2);
  return local_pointer;
}

address Core::writeCode(std::byte opcode, address arg1, address arg2) {
  const auto local_pointer = pointer;
  writeByte(opcode);
  writeAddress(arg1);
  writeAddress(arg2);
  return local_pointer;
}

address Core::writeCode(std::byte opcode, address arg1) {
  const auto local_pointer = pointer;
  writeByte(opcode);
  writeAddress(arg1);
  return local_pointer;
}

address Core::writeCode(std::byte opcode, address arg1, const std::byte arg2) {
  const auto local_pointer = pointer;
  writeByte(opcode);
  writeAddress(arg1);
  writeByte(arg2);
  return local_pointer;
}

address Core::writeCode(const std::byte opcode, const std::byte arg1) {
  const auto local_pointer = pointer;
  writeByte(opcode);
  writeByte(arg1);
  return local_pointer;
}

address Core::writeCode(const std::byte opcode, const int arg1) {
  const auto local_pointer = pointer;
  writeByte(opcode);
  writeInt(arg1);
  return local_pointer;
}

address Core::writeCode(const std::byte opcode) {
  const auto local_pointer = pointer;
  writeByte(opcode);
  return local_pointer;
}

void Core::init(unsigned int size) {
  _size = size; // TODO
  _bytes.reserve(_size);
  _bytes.assign(_size, std::byte{0x0});

  pointer = address::BEGIN;
  seek(ESP);
  writeInt(_size);
  writeHeader();
}

void Core::writeByte(const std::byte ch) {
  _bytes[pointer.dst] = ch;
  pointer++;
}

void Core::writeHeader() {
  seek(address::BEGIN);
  writeByte(static_cast<std::byte>('V'));
  writeByte(static_cast<std::byte>('V'));
  writeByte(static_cast<std::byte>('M'));

  writeByte(version);
  writeByte(static_cast<std::byte>(address::CODE.dst));
}

std::byte Core::readByte() {
  const auto ch = _bytes[pointer.dst];
  pointer++;
  return ch;
}

bool Core::checkFlag(const std::byte mask) {
  const auto local_pointer = pointer;
  seek(FLAGS);
  const auto flag = readByte();
  seek(local_pointer);
  return static_cast<bool>(flag & mask);
}

std::byte Core::getState() {
  const auto local_pointer = pointer;
  seek(STATE);
  const auto state = readByte();
  seek(local_pointer);
  return state;
}

void Core::setState(const std::byte state) {
  const auto local_pointer = pointer;
  seek(STATE);
  writeByte(state);
  seek(local_pointer);
}

void Core::setFlag(const std::byte flag, const bool value) {
  const auto local_pointer = pointer;
  seek(FLAGS);
  auto flags = readByte();
  if (value) {
    flags |= flag;
  } else {
    flags &= ~flag;
  }
  seek(FLAGS);
  writeByte(flags);
  seek(local_pointer);
}

void Core::checkInterruption() {
  const auto local_pointer = pointer;
  if (checkFlag(INTF)) {
    seek(INTERRUPTS);
    const auto interrupt = readByte();
    printIRQ(interrupt);
    if (interrupt == INT_END) {
      setState(STATE_END);
    } else if (_intHandlers.count(interrupt) == 1) {
      _intHandlers[interrupt](_bytes, pointer.dst);
    }
    setFlag(INTF, false);
  }
  seek(local_pointer);
}

void Core::setReg(const address reg, const address value) {
  const auto local_pointer = pointer;
  seek(reg);
  writeAddress(value);
  seek(local_pointer);
}

void Core::setReg(const address reg, const int value) {
  const auto local_pointer = pointer;
  seek(reg);
  writeInt(value);
  seek(local_pointer);
}

address Core::readRegAddress(const address reg) {
  const auto local_pointer = pointer;
  seek(reg);
  const auto value = readAddress();
  seek(local_pointer);
  return value;
}

int Core::readRegInt(const address reg) {
  const auto local_pointer = pointer;
  seek(reg);
  const auto value = readInt();
  seek(local_pointer);
  return value;
}

address Core::execStart() {
  setState(STATE_EXEC);
  seek(CO_ADDR);
  auto offset = readByte();
  const auto local_pointer = address{static_cast<unsigned int>(offset)};
  seek(local_pointer);
  return local_pointer;
}

void Core::execCode() {
  fmt::print(
      "= ADDR ==|====== INSTRUCTION =====|= FLAGS ==|===== VARIABLES ====\n");
  fmt::print(
      "         |                        |          |                    \n");
  auto local_pointer = execStart();

  while (getState() == STATE_EXEC) {
    setReg(EIP, local_pointer);

    local_pointer = execStep(local_pointer);
  }
  fmt::print(
      "         |                        |          |                    \n");
  fmt::print(
      "==================================================================\n\n");
}

void Core::execCode(address local_pointer) {
  while (getState() == STATE_EXEC) {
    local_pointer = execStep(local_pointer);
  }
  fmt::print(
      "         |                        |          |                    \n");
  fmt::print(
      "==================================================================\n\n");
}

address Core::execStep(address local_pointer) {
  setReg(EIP, local_pointer);
  const auto opcode = readByte();

  auto spec = std::find_if(specs.begin(), specs.end(),
                           [&](opSpec s) { return s.opcode == opcode; });
  if (spec != specs.end()) {
    current_spec_type = (*spec).type;
  }
  local_pointer++;
  // TODO: make map with opcodes
  if (opcode == MOV_mm) {
    local_pointer = MOV_mm_func(local_pointer);
  } else if (opcode == MOV_mc) {
    local_pointer = MOV_mc_func(local_pointer);
  } else if (opcode == ADD_mm) {
    local_pointer = ADD_mm_func(local_pointer);
  } else if (opcode == ADD_mc) {
    local_pointer = ADD_mc_func(local_pointer);
  } else if (opcode == SUB_mm) {
    local_pointer = SUB_mm_func(local_pointer);
  } else if (opcode == SUB_mc) {
    local_pointer = SUB_mc_func(local_pointer);
  } else if (opcode == CMP_mm) {
    local_pointer = CMP_mm_func(local_pointer);
  } else if (opcode == CMP_mc) {
    local_pointer = CMP_mc_func(local_pointer);
  } else if (opcode == JNE_a) {
    local_pointer = JNE_a_func(local_pointer);
  } else if (opcode == JNE_r) {
    local_pointer = JNE_r_func(local_pointer);
  } else if (opcode == OUTPUT) {
    local_pointer = OUT_func(local_pointer);
  } else if (opcode == INTERRUPT) {
    local_pointer = INT_func(local_pointer);
  } else if (opcode == NOP) {
    local_pointer = NOP_func(local_pointer);
  } else if (opcode == PUSH_m) {
    local_pointer = PUSH_m_func(local_pointer);
  } else if (opcode == PUSH_c) {
    local_pointer = PUSH_c_func(local_pointer);
  } else if (opcode == POP) {
    local_pointer = POP_func(local_pointer);
  } else if (opcode == JMP_a) {
    local_pointer = JMP_a_func(local_pointer);
  } else if (opcode == JMP_r) {
    local_pointer = JMP_r_func(local_pointer);
  } else if (opcode == INC) {
    local_pointer = INC_func(local_pointer);
  } else if (opcode == DEC) {
    local_pointer = DEC_func(local_pointer);
  }
  checkInterruption();
  _tickHandler(_bytes, pointer.dst);
  if (pointer.dst >= BUF_SIZE) {
    // TODO: implement irq and error handler
    setState(STATE_ERROR);
  }

  const auto next_opcode = readByte();
  seek(local_pointer);
  spec = std::find_if(specs.begin(), specs.end(),
                      [&](opSpec s) { return s.opcode == next_opcode; });
  if (spec != specs.end()) {
    next_spec_type = (*spec).type;
  }
  return local_pointer;
}
