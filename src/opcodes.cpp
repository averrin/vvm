#include "format.h"
#include "vvm/core.hpp"

address Core::_SUB(const address dst, const address src) {
  return writeCode(SUB_mm, dst, src);
}

address Core::_SUB(const address dst, const int src) {
  return writeCode(SUB_mc, dst, src);
}

address Core::_ADD(const address dst, const address src) {
  return writeCode(ADD_mm, dst, src);
}

address Core::_ADD(const address dst, const int src) {
  return writeCode(ADD_mc, dst, src);
}

address Core::_MOV(const address dst, const address src) {
  return writeCode(MOV_mm, dst, src);
}

address Core::_MOV(const address dst, const int src_const) {
  return writeCode(MOV_mc, dst, src_const);
}

address Core::_INT(const std::byte code) { return writeCode(INTERRUPT, code); }

address Core::_NOP() { return writeCode(NOP); }

address Core::_JMP(const address addr) { return writeCode(JMP_a, addr); }

address Core::_JMP(const int offset) { return writeCode(JMP_r, offset); }

address Core::_JNE(const address addr) { return writeCode(JNE_a, addr); }

address Core::_JNE(const int offset) { return writeCode(JNE_r, offset); }

address Core::_INC(const address addr) { return writeCode(INC, addr); }
address Core::_DEC(const address addr) { return writeCode(DEC, addr); }

address Core::_PUSH(const address addr) { return writeCode(PUSH_m, addr); }

address Core::_PUSH(const int value) { return writeCode(PUSH_c, value); }

address Core::_POP(const address addr) { return writeCode(POP, addr); }

address Core::_CMP(const address dst, const int src_const) {
  return writeCode(CMP_mc, dst, src_const);
}
