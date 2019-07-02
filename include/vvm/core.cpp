#include "vvm/core.hpp"
#include "format.h"
#include "rang.hpp"
#include <fstream>
#include <optional>
#include <sstream>
#include <utility>
#include <variant>
#include <numeric>

std::array<op_spec, 27> specs = {
    INVALID_spec, NOP_spec,    MOV_aa_spec, MOV_ai_spec, MOV_aw_spec, ADD_aa_spec,
    ADD_ai_spec,  ADD_aw_spec, SUB_aa_spec, SUB_ai_spec, SUB_aw_spec, CMP_aa_spec,
    CMP_ai_spec,  CMP_aw_spec,JNE_a_spec,  JNE_r_spec,  JE_spec,     JMP_a_spec,
    JMP_r_spec,   INT_spec,    PUSH_a_spec, PUSH_w_spec, PUSH_i_spec, POP_spec,
    INC_spec,     DEC_spec,    MEM_spec
};

address address::BEGIN = address{0x0};
address address::CODE = address{CODE_OFFSET};

std::map<std::string, address> reserved_addresses = {
    {"STATE", STATE},       {"ESP", ESP},
    {"EAX", EAX},           {"EBX", EBX},
    {"ECX", ECX},           {"EIP", EIP},
    {"EDI", EDI},
    {"FLAGS", FLAGS},       {"INTERRUPTS", INTERRUPTS},
    {"AL", AL}, {"BL", BL}, {"CL", CL},
};

std::optional<address> Core::isReservedMem(std::string arg) {
  for_each(arg.begin(), arg.end(), [](char &in) { in = ::toupper(in); });
  auto it = reserved_addresses.find(arg);
  if (it != reserved_addresses.end()) {
      return reserved_addresses[arg];
  } else {
      return std::nullopt;
  }
}

Core::Core(t_handler th): _tickHandler(std::move(th)){
  meta = std::make_unique<MemoryContainer>(MemoryContainer(CODE_OFFSET.dst));
  meta->offset = address{0x0};

  pointer = address::BEGIN;
  writeHeader();
};

std::optional<op_spec> Core::getSpec(predicate filterFunc) {
  auto spec = std::find_if(specs.begin(), specs.end(), filterFunc);
  if (spec == specs.end()) {
    return {};
  }
  return *spec;
}

unsigned int Core::getSize() {
    auto mo = 0;
    for (auto m : memory) {
        mo += m->size;
    }
    return (code->offset + code->size + mo).dst;
}

void Core::setInterruptHandler(const std::byte interrupt,
                               const t_handler handler) {
  _intHandlers[interrupt] = handler;
}

void Core::compile(analyzer::script instructions) {
  const auto temp_pointer = pointer;
  code = std::make_unique<MemoryContainer>(MemoryContainer(instructions.size() * OP_aa_length));
  code->offset = CODE_OFFSET;

  for (auto i : instructions) {
    auto parsed_arg1 = i.arg1;
    auto parsed_arg2 = i.arg2;
    seek(i.offset);

    switch (i.spec.type) {
    case op_spec::AA:
      writeCode(i.spec.opcode, std::get<address>(parsed_arg1),
                std::get<address>(parsed_arg2));
      break;
    case op_spec::AI:
      writeCode(i.spec.opcode, std::get<address>(parsed_arg1),
                std::get<unsigned int>(parsed_arg2));
      break;
    case op_spec::AW:
      writeCode(i.spec.opcode, std::get<address>(parsed_arg1),
                std::get<std::byte>(parsed_arg2));
      break;
    case op_spec::A:
      writeCode(i.spec.opcode, std::get<address>(parsed_arg1));
      break;
    case op_spec::I:
      writeCode(i.spec.opcode, std::get<unsigned int>(parsed_arg1));
      break;
    case op_spec::W:
      writeCode(i.spec.opcode, std::get<std::byte>(parsed_arg1));
      break;
    case op_spec::Z:
      writeCode(i.spec.opcode);
      break;
    default:;
    }
  }
  auto written = pointer;
  seek(ESP);
  writeInt((written + STACK_SIZE).dst);
  seek(EIP);
  writeInt(CODE_OFFSET.dst);
  code->resize((written - code->offset).dst + STACK_SIZE);
  seek(EDI);

  address offset;
  if (memory.size() == 0) {
      offset = code->offset + code->size;
  } else {
      offset = memory.back()->offset;
  }
  writeInt(offset.dst);
  seek(CODE_OFFSET);

  next_spec_type = instructions.front().spec.type;
  std::cout << "Compiled. Code size: " << code->size << std::endl;
}

void Core::saveBytes(const std::string_view name) {
  std::ofstream file(static_cast<std::string>(name), std::ios::binary);
  size_t count = CODE_OFFSET.dst / sizeof(std::byte);
  file.write(reinterpret_cast<char *>(&(meta->data)[0]), count * sizeof(std::byte));
  count = code->size / sizeof(std::byte);
  file.write(reinterpret_cast<char *>(&(code->data)[0]), count * sizeof(std::byte));
  for (auto m : memory) {
    count = m->size / sizeof(std::byte);
    file.write(reinterpret_cast<char *>(&(m->data)[0]), count * sizeof(std::byte));
  }
  file.close();
}

void Core::seek(instruction_arg addr) { seek(std::get<address>(addr)); }
void Core::seek(address addr) { pointer = addr; }

address Core::readAddress() {
    auto rf = readByte();
    return address{readInt(), static_cast<bool>(rf & REDIRECT), static_cast<bool>(rf & STOREBYTE)};
}

MemoryContainer* Core::getMem() {
  MemoryContainer* mem;
  if (pointer.dst < CODE_OFFSET.dst) {
      mem = meta.get();
  } else if (pointer.dst < (CODE_OFFSET + code->size).dst) {
      mem = code.get();
  } else {
      for (auto m : memory) {
          if (m->offset.dst <= pointer.dst && pointer.dst < (m->offset + m->size).dst) {
              mem = m.get();
              break;
          }
      }
  }
  return mem;
}

unsigned int Core::readInt() {
  auto mem = getMem();
  const auto n = mem->readInt(pointer - mem->offset);
  pointer += INT_SIZE;
  return n;
}

int Core::readSignedInt() {
  auto mem = getMem();
  const auto n = mem->readSignedInt(pointer - mem->offset);
  pointer += INT_SIZE;
  return n;
}

//TODO: use setReg [writeCode] and protect meta [+code] memory range

void Core::writeAddress(const instruction_arg n) { writeAddress(std::get<address>(n)); }
void Core::writeAddress(const address n) {
    auto flags = ZERO;
    if (n.redirect) flags |= REDIRECT;
    if (n.storeByte) flags |= STOREBYTE;
    writeByte(flags);
    writeInt(n.dst);
}

void Core::writeInt(const instruction_arg n) { writeInt(std::get<unsigned int>(n)); }
void Core::writeInt(const int n) {
  auto mem = getMem();
  mem->writeInt(pointer - mem->offset, n);
  pointer += INT_SIZE;
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

void Core::writeByte(const instruction_arg ch) { writeByte(std::get<std::byte>(ch)); }
void Core::writeByte(const std::byte ch) {
    auto mem = getMem();
    mem->writeByte(pointer - mem->offset, ch);
    pointer += BYTE_SIZE;
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
    auto mem = getMem();
    auto ch = mem->readByte(pointer - mem->offset);
    pointer += BYTE_SIZE;
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

//TODO: fix interrupt handlers
void Core::checkInterruption() {
  const auto local_pointer = pointer;
  if (checkFlag(INTF)) {
    seek(INTERRUPTS);
    const auto interrupt = readByte();
    printIRQ(interrupt);
    if (interrupt == INT_END) {
      setState(STATE_END);
    } else if (_intHandlers.count(interrupt) == 1) {
      // _intHandlers[interrupt](_bytes, pointer.dst);
    }
    setFlag(INTF, false);
  }
  seek(local_pointer);
}

void Core::setReg(const address reg, const address value) {;
  const auto local_pointer = pointer;
  seek(reg);
  writeInt(value.dst);
  seek(local_pointer);
}

void Core::setReg(const address reg, const int value) {
  const auto local_pointer = pointer;
  seek(reg);
  writeInt(value);
  seek(local_pointer);
}

address Core::readRegAddress(const address reg) {
  return address{readRegInt(reg)};
}

int Core::readRegInt(const address reg) {
  const auto local_pointer = pointer;
  seek(reg);
  const auto value = readInt();
  seek(local_pointer);
  return value;
}

std::byte Core::readRegByte(const address reg) {
  const auto local_pointer = pointer;
  seek(reg + INT_SIZE - BYTE_SIZE);
  const auto value = readByte();
  seek(local_pointer);
  return value;
}


address Core::addDevice(std::shared_ptr<Device> device) {
  devices.push_back(device);
  return mapMem(device->memory);
}

//TODO: implement EDI switching
address Core::mapMem(std::shared_ptr<MemoryContainer> mem) {
    address offset;
    if (memory.size() == 0) {
        offset = code->offset + code->size;
    } else {
        offset = memory.back()->offset + memory.back()->size;
    }
    mem->offset = offset;
    memory.push_back(mem);

    seek(EDI);
    writeInt((offset).dst);
    return offset;
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
      "= ADDR ===|====== INSTRUCTION =====|= FLAGS ==|===== VARIABLES ====\n");
  fmt::print(
      "          |                        |          |                    \n");
  auto local_pointer = execStart();

  while (getState() == STATE_EXEC) {
    setReg(EIP, local_pointer);

    local_pointer = execStep(local_pointer);
  }
  fmt::print(
      "          |                        |          |                    \n");
  fmt::print(
      "==================================================================\n\n");
}

void Core::execCode(address local_pointer) {
  while (getState() == STATE_EXEC) {
    local_pointer = execStep(local_pointer);
  }
  fmt::print(
      "          |                        |          |                    \n");
  fmt::print(
      "==================================================================\n\n");
}

address Core::execStep(address local_pointer) {
  setReg(EIP, local_pointer);
  const auto opcode = readByte();

  auto spec = std::find_if(specs.begin(), specs.end(),
                           [&](op_spec s) { return s.opcode == opcode; });
  local_pointer++;
  // TODO: make map with opcodes
  if (opcode == MOV_aa) {
    local_pointer = MOV_aa_func(local_pointer);
  } else if (opcode == MOV_ai) {
    local_pointer = MOV_ai_func(local_pointer);
  } else if (opcode == MOV_aw) {
    local_pointer = MOV_aw_func(local_pointer);
  } else if (opcode == ADD_aa) {
    local_pointer = ADD_aa_func(local_pointer);
  } else if (opcode == ADD_ai) {
    local_pointer = ADD_ai_func(local_pointer);
  } else if (opcode == ADD_aw) {
    local_pointer = ADD_aw_func(local_pointer);
  } else if (opcode == SUB_aa) {
    local_pointer = SUB_aa_func(local_pointer);
  } else if (opcode == SUB_ai) {
    local_pointer = SUB_ai_func(local_pointer);
  } else if (opcode == SUB_aw) {
    local_pointer = SUB_aw_func(local_pointer);
  } else if (opcode == CMP_aa) {
    local_pointer = CMP_aa_func(local_pointer);
  } else if (opcode == CMP_ai) {
    local_pointer = CMP_ai_func(local_pointer);
  } else if (opcode == CMP_aw) {
    local_pointer = CMP_aw_func(local_pointer);
  } else if (opcode == JNE_a) {
    local_pointer = JNE_a_func(local_pointer);
  } else if (opcode == JNE_r) {
    local_pointer = JNE_r_func(local_pointer);
  } else if (opcode == JE) {
    local_pointer = JE_func(local_pointer);
  } else if (opcode == INTERRUPT) {
    local_pointer = INT_func(local_pointer);
  } else if (opcode == NOP) {
    local_pointer = NOP_func(local_pointer);
  } else if (opcode == PUSH_a) {
    local_pointer = PUSH_a_func(local_pointer);
  } else if (opcode == PUSH_i) {
    local_pointer = PUSH_i_func(local_pointer);
  } else if (opcode == PUSH_w) {
    local_pointer = PUSH_w_func(local_pointer);
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
  } else if (opcode == MEM) {
    local_pointer = MEM_func(local_pointer);
  }
  checkInterruption();
  _tickHandler(*getMem(), pointer.dst);
  for (auto d : devices) {
    d->tickHandler();
  }
  if (pointer.dst >= (code->offset + code->size).dst) {
    // TODO: implement irq and error handler
    setState(STATE_ERROR);
  }

  const auto next_opcode = readByte();
  seek(local_pointer);
  spec = std::find_if(specs.begin(), specs.end(),
                      [&](op_spec s) { return s.opcode == next_opcode; });
  if (spec != specs.end()) {
    next_spec_type = (*spec).type;
  }
  seek(local_pointer);
  setReg(EIP, local_pointer);
  return local_pointer;
}
