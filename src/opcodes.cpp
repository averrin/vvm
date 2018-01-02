#include "vvm/container.hpp"
#include "format.h"


address Container::_SUB(const address dst, const address src) {
	return writeCode(SUB_mm, dst, src);
}

address Container::_SUB(const address dst, const int src) {
	return writeCode(SUB_mc, dst, src);
}

address Container::_ADD(const address dst, const address src) {
	return writeCode(ADD_mm, dst, src);
}

address Container::_ADD(const address dst, const int src) {
	return writeCode(ADD_mc, dst, src);
}

address Container::_MOV(const address dst, const address src) {
	return writeCode(MOV_mm, dst, src);
}

address Container::_MOV(const address dst, const int src_const) {
	return writeCode(MOV_mc, dst, src_const);
}

address Container::_INT(const std::byte code)
{
	return writeCode(INTERRUPT, code);
}

address Container::_NOP()
{
	return writeCode(NOP);
}

address Container::_JMP(const address addr) {
	return writeCode(JMP_a, addr);
}

address Container::_JMP(const int offset) {
	return writeCode(JMP_r, offset);
}

address Container::_JNE(const address addr) {
	return writeCode(JNE_a, addr);
}

address Container::_JNE(const int offset) {
	return writeCode(JNE_r, offset);
}

address Container::_INC(const address addr) {
	return writeCode(INC, addr);
}
address Container::_DEC(const address addr) {
	return writeCode(DEC, addr);
}

address Container::_PUSH(const address addr) {
	return writeCode(PUSH_m, addr);
}

address Container::_PUSH(const int value) {
	return writeCode(PUSH_c, value);
}

address Container::_POP(const address addr) {
	return writeCode(POP, addr);
}

address Container::_CMP(const address dst, const int src_const) {
	return writeCode(CMP_mc, dst, src_const);
}
