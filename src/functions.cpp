#include "vvm/core.hpp"

address Core::MOV_mc_func(address _pointer) {
  const auto dst = readAddress();
  _pointer += INT_SIZE;
  const auto src = readInt();
  _pointer += INT_SIZE;
  seek(dst);
  writeInt(src);
  seek(_pointer);
  printCode("MOV", dst, src);
  return _pointer;
}

address Core::MOV_mb_func(address _pointer) {
  const auto dst = readAddress();
  _pointer += INT_SIZE;
  const auto src = readByte();
  _pointer += 1;
  seek(dst);
  writeInt(static_cast<unsigned int>(src));
  seek(_pointer);
  printCode("MOV", dst, src);
  return _pointer;
}

address Core::MOV_mm_func(address _pointer) {
  const auto dst = readAddress();
  _pointer += INT_SIZE;
  const auto src = readAddress();
  _pointer += INT_SIZE;
  seek(src);
  const auto value = readInt();
  seek(dst);
  writeInt(value);
  seek(_pointer);
  printCode("MOV", dst, src);
  return _pointer;
}

address Core::OUT_func(address _pointer) {
  auto src = readAddress();
  _pointer += INT_SIZE;
  seek(src);
  src = readAddress();
  seek(OUT_PORT);
  writeAddress(src);
  setFlag(OUTF, true);
  seek(_pointer);
  printCode("OUT", src);
  return _pointer;
}

address Core::INT_func(address _pointer) {
  const auto src = readByte();
  _pointer++;
  seek(INTERRUPTS);
  writeByte(src);
  setFlag(INTF, true);
  seek(_pointer);
  printCode("INT", src);
  return _pointer;
}

address Core::INC_func(address _pointer) {
  const auto dst = readAddress();
  _pointer += INT_SIZE;

  seek(dst);
  auto value = readInt();
  value += 1;
  seek(dst);
  writeInt(value);
  setFlag(ZF, value == 0);
  seek(_pointer);

  printCode("INC", dst);
  return _pointer;
}

address Core::DEC_func(address _pointer) {
  const auto dst = readAddress();
  _pointer += INT_SIZE;

  seek(dst);
  auto value = readInt();
  value -= 1;
  seek(dst);
  writeInt(value);
  setFlag(ZF, value == 0);
  seek(_pointer);

  printCode("DEC", dst);
  return _pointer;
}

address Core::ADD_mc_func(address _pointer) {
  const auto dst = readAddress();
  _pointer += INT_SIZE;
  const auto src = readInt();
  _pointer += INT_SIZE;

  seek(dst);
  auto value = readInt();
  value += src;
  seek(dst);
  writeInt(value);
  setFlag(ZF, value == 0);
  seek(_pointer);

  printCode("ADD", dst, src);
  return _pointer;
}

address Core::ADD_mb_func(address _pointer) {
  const auto dst = readAddress();
  _pointer += INT_SIZE;
  const auto src = readByte();
  _pointer += 1;

  seek(dst);
  auto value = readInt();
  value += static_cast<unsigned int>(src);
  seek(dst);
  writeInt(value);
  setFlag(ZF, value == 0);
  seek(_pointer);

  printCode("ADD", dst, src);
  return _pointer;
}

address Core::ADD_mm_func(address _pointer) {
  const auto dst = readAddress();
  _pointer += INT_SIZE;
  const auto src = readAddress();
  _pointer += INT_SIZE;
  seek(src);
  const auto inc = readInt();

  seek(dst);
  auto value = readInt();
  value += inc;
  seek(dst);
  writeInt(value);
  setFlag(ZF, value == 0);
  seek(_pointer);

  printCode("ADD", dst, src);
  return _pointer;
}

address Core::SUB_mc_func(address _pointer) {
  const auto dst = readAddress();
  _pointer += INT_SIZE;
  const auto src = readInt();
  _pointer += INT_SIZE;

  seek(dst);
  auto value = readInt();
  value -= src;
  seek(dst);
  writeInt(value);
  setFlag(ZF, value == 0);
  seek(_pointer);

  printCode("SUB", dst, src);
  return _pointer;
}

address Core::SUB_mb_func(address _pointer) {
  const auto dst = readAddress();
  _pointer += INT_SIZE;
  const auto src = readByte();
  _pointer += 1;

  seek(dst);
  auto value = readInt();
  value -= static_cast<unsigned int>(src);
  seek(dst);
  writeInt(value);
  setFlag(ZF, value == 0);
  seek(_pointer);

  printCode("SUB", dst, src);
  return _pointer;
}

address Core::SUB_mm_func(address _pointer) {
  const auto dst = readAddress();
  _pointer += INT_SIZE;
  const auto src = readAddress();
  _pointer += INT_SIZE;

  seek(src);
  const auto dec = readInt();
  seek(dst);
  auto value = readInt();
  value -= dec;
  seek(dst);
  writeInt(value);
  setFlag(ZF, value == 0);
  seek(_pointer);

  printCode("SUB", dst, src);
  return _pointer;
}

address Core::CMP_mc_func(address _pointer) {
  const auto a1 = readAddress();
  _pointer += INT_SIZE;
  const auto a2 = readInt();
  _pointer += INT_SIZE;

  seek(a1);
  const auto value = readInt();
  setFlag(ZF, a2 == value);
  seek(_pointer);
  printCode("CMP", a1, a2);
  return _pointer;
}

address Core::CMP_mb_func(address _pointer) {
  const auto a1 = readAddress();
  _pointer += INT_SIZE;
  const auto a2 = readByte();
  _pointer += 1;

  seek(a1);
  const auto value = readInt();
  setFlag(ZF, static_cast<unsigned int>(a2) == value);
  seek(_pointer);
  printCode("CMP", a1, a2);
  return _pointer;
}

address Core::CMP_mm_func(address _pointer) {
  const auto a1 = readAddress();
  _pointer += INT_SIZE;
  const auto a2 = readAddress();
  _pointer += INT_SIZE;

  seek(a1);
  const auto value = readInt();
  seek(a2);
  const auto value2 = readInt();
  setFlag(ZF, value == value2);
  seek(_pointer);
  printCode("CMP", a1, a2);
  return _pointer;
}

address Core::JNE_a_func(address _pointer) {
  const auto src = readAddress();
  _pointer += INT_SIZE;

  seek(_pointer);
  auto jumped = false;
  if (!checkFlag(ZF)) {
    _pointer = src;
    seek(_pointer);
    jumped = true;
  }
  setReg(EIP, _pointer);
  printJump("JNE", src, jumped);
  return _pointer;
}

address Core::JNE_r_func(address _pointer) {
  const auto p = _pointer - 1;
  const auto src = readSignedInt();

  auto jumped = false;
  if (!checkFlag(ZF)) {
    _pointer = p + src;
    jumped = true;
  }
  seek(_pointer);
  setReg(EIP, _pointer);
  printJump("JNE", src, jumped);
  return _pointer;
}

address Core::JE_func(address _pointer) {
  const auto src = readAddress();
  _pointer += INT_SIZE;

  auto jumped = false;
  if (checkFlag(ZF)) {
    _pointer = src;
    jumped = true;
  }
  seek(_pointer);
  setReg(EIP, _pointer);
  printJump("JNE", src, jumped);
  return _pointer;
}

address Core::NOP_func(address _pointer) {
  printCode("NOP");
  return _pointer;
}

address Core::PUSH_m_func(address _pointer) {
  const auto src = readAddress();
  _pointer += INT_SIZE;
  seek(src);
  const auto value = readInt();
  seek(ESP);
  const auto s_addr = readAddress() - INT_SIZE;
  seek(s_addr);
  writeInt(value);
  seek(ESP);
  writeAddress(s_addr);
  seek(_pointer);
  printCode("PUSH", src);
  return _pointer;
}

address Core::PUSH_c_func(address _pointer) {
  const auto value = readInt();
  _pointer += INT_SIZE;
  seek(ESP);
  const auto s_addr = readAddress() - INT_SIZE;
  seek(s_addr);
  writeInt(value);
  seek(ESP);
  writeAddress(s_addr);
  seek(_pointer);
  printCode("PUSH", value);
  return _pointer;
}

address Core::POP_func(address _pointer) {
  const auto dst = readAddress();
  _pointer += INT_SIZE;
  seek(ESP);
  const auto s_addr = readAddress();
  seek(s_addr);
  const auto value = readInt();
  seek(s_addr);
  writeInt(0x0);
  seek(dst);
  writeInt(value);
  seek(ESP);
  writeAddress(s_addr + INT_SIZE);

  seek(_pointer);
  printCode("POP", dst);
  return _pointer;
}

address Core::JMP_a_func(address _pointer) {
  const auto src = readAddress();

  _pointer = src;
  seek(_pointer);
  setReg(EIP, _pointer);
  printJump("JMP", src, true);
  return _pointer;
}

address Core::JMP_r_func(address _pointer) {
  const auto src = readSignedInt();
  _pointer = _pointer - 1; // minus jmp size.

  _pointer += src;
  seek(_pointer);
  setReg(EIP, _pointer);
  printJump("JMP", src, true);
  return _pointer;
}

address Core::MEM_func(address _pointer) {
  const auto a1 = readAddress();
  _pointer += INT_SIZE;
  const auto a2 = readByte();
  _pointer += 1;

  seek(a1);
  const auto addr = readAddress();
  seek(addr);
  writeByte(a2);
  seek(_pointer);
  printCode("MEM", a1, a2);
  return _pointer;
}
