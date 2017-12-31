#include "project/container.hpp"
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

address Container::MOV_mc_func(address _pointer) {
	const auto p = _pointer - 1;
	const auto dst = readAddress();
	_pointer += INT_SIZE;
	const auto src = readInt();
	_pointer += INT_SIZE;
	seek(dst);
	writeInt(src);
	seek(_pointer);
	printCode("MOV", p, dst, src);
	return _pointer;
}

address Container::MOV_mm_func(address _pointer) {
	const auto p = _pointer - 1;
	const auto dst = readAddress();
	_pointer += INT_SIZE;
	const auto src = readAddress();
	_pointer += INT_SIZE;
	seek(src);
	const auto value = readInt();
	seek(dst);
	writeInt(value);
	seek(_pointer);
	printCode("MOV", p, dst, src);
	return _pointer;
}

address Container::OUT_func(address _pointer) {
	const auto p = _pointer - 1;
	auto src = readAddress();
	_pointer += INT_SIZE;
	seek(src);
	src = readAddress();
	seek(OUT_PORT);
	writeAddress(src);
	setFlag(OUTF, true);
	seek(_pointer);
	printCode("OUT", p, src);
	return _pointer;
}

address Container::INT_func(address _pointer) {
	const auto p = _pointer - 1;
	const auto src = readByte();
	_pointer++;
	seek(INTERRUPTS);
	writeByte(src);
	setFlag(INTF, true);
	seek(_pointer);
	printCode("INT", p, src);
	return _pointer;
}

address Container::ADD_mc_func(address _pointer) {
	const auto p = _pointer - 1;
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

	printCode("ADD", p, dst, src);
	return _pointer;
}

address Container::ADD_mm_func(address _pointer) {
	const auto p = _pointer - 1;
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

	printCode("ADD", p, dst, src);
	return _pointer;
}

address Container::SUB_mc_func(address _pointer) {
	const auto p = _pointer - 1;
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

	printCode("SUB", p, dst, src);
	return _pointer;
}

address Container::SUB_mm_func(address _pointer) {
	const auto p = _pointer - 1;
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

	printCode("SUB", p, dst, src);
	return _pointer;
}

address Container::CMP_func(address _pointer) {
	const auto p = _pointer - 1;
	const auto a1 = readAddress();
	_pointer += INT_SIZE;
	const auto a2 = readInt();
	_pointer += INT_SIZE;

	seek(a1);
	const auto value = readInt();
	setFlag(ZF, a2 == value);
	seek(_pointer);
	printCode("CMP", p, a1, a2);
	return _pointer;
}

address Container::JNE_func(address _pointer) {
	const auto p = _pointer - 1;
	const auto src = readAddress();
	_pointer += INT_SIZE;

	seek(FLAGS);
	auto value = readByte();
	seek(_pointer);
	auto jumped = false;
	if (!checkFlag(ZF)) {
		_pointer = src;
		seek(_pointer);
		jumped = true;
	}
	printJump("JNE", p, src, jumped);
	return _pointer;
}

address Container::JE_func(address _pointer) {
	const auto p = _pointer - 1;
	const auto src = readAddress();
	_pointer += INT_SIZE;

	seek(FLAGS);
	auto value = readByte();
	seek(_pointer);
	auto jumped = false;
	if (checkFlag(ZF)) {
		_pointer = src;
		seek(_pointer);
		jumped = true;
	}
	printJump("JNE", p, src, jumped);
	return _pointer;
}

address Container::NOP_func(address _pointer) {
	const auto p = _pointer - 1;
	printCode("NOP", p);
	return _pointer;
}

address Container::PUSH_func(address _pointer) {
	const auto p = _pointer - 1;
	const auto src = readAddress();
	_pointer += INT_SIZE;
	seek(src);
	const auto value = readInt();
	seek(STACK_ADDR);
	const auto s_addr = readAddress() - INT_SIZE;
	seek(s_addr);
	writeInt(value);
	seek(STACK_ADDR);
	writeAddress(s_addr);
	seek(_pointer);
	printCode("PUSH", p, src);
	return _pointer;
}

address Container::POP_func(address _pointer) {
	const auto p = _pointer - 1;
	const auto dst = readAddress();
	_pointer += INT_SIZE;
	seek(STACK_ADDR);
	const auto s_addr = readAddress();
	seek(s_addr);
	const auto value = readInt();
	seek(s_addr);
	writeInt(0x0);
	seek(dst);
	writeInt(value);
	seek(STACK_ADDR);
	writeAddress(s_addr + INT_SIZE);

	seek(_pointer);
	printCode("POP", p);
	return _pointer;
}

address Container::JMP_a_func(address _pointer) {
	const auto p = _pointer - 1;
	const auto src = readAddress();
	_pointer += INT_SIZE;

	_pointer = src;
	seek(_pointer);
	printJump("JMP", p, src, true);
	return _pointer;
}

address Container::JMP_r_func(address _pointer) {
	const auto p = _pointer - 1;
	const auto src = readSignedInt();
	_pointer = _pointer - 1; //minus jmp size.

	_pointer += src;
	seek(_pointer);
	printJump("JMP", p, src, true);
	return _pointer;
}
