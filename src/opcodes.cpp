#include "project/container.hpp"
#include "format.h"


int Container::OUT_func(int _pointer) {
	const auto p = _pointer - 1;
	auto src = readInt();
	_pointer += INT_SIZE;
	seek(src);
	src = readInt();
	seek(OUT_PORT);
	writeInt(src);
	seek(FLAGS);
	auto flags = readByte();
	seek(FLAGS);
	flags |= OUTF;
	writeByte(flags);
	seek(_pointer);
	printCode("OUT", p, src);
	return _pointer;
}

int Container::MOV_func(int _pointer) {
	const auto p = _pointer - 1;
	const auto dst = readInt();
	_pointer += INT_SIZE;
	const auto src = readInt();
	_pointer += INT_SIZE;
	seek(dst);
	writeInt(src);
	seek(_pointer);
	printCode("MOV", p, dst, src);
	return _pointer;
}

int Container::INT_func(int _pointer) {
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

int Container::ADD_func(int _pointer) {
	auto p = _pointer - 1;
	unsigned int src = readInt();
	_pointer += INT_SIZE;
	unsigned int dst = readInt();
	_pointer += INT_SIZE;

	seek(dst);
	unsigned int value = readInt();
	value += src;
	seek(dst);
	writeInt(value);
	setFlag(ZF, value == 0);
	seek(_pointer);

	printCode("ADD", p, src, dst);
	return _pointer;
}

int Container::SUB_func(int _pointer) {
	auto p = _pointer - 1;
	unsigned int src = readInt();
	_pointer += INT_SIZE;
	unsigned int dst = readInt();
	_pointer += INT_SIZE;

	seek(dst);
	unsigned int value = readInt();
	value -= src;
	seek(dst);
	writeInt(value);
	setFlag(ZF, value == 0);
	seek(_pointer);

	printCode("SUB", p, src, dst);
	return _pointer;
}

int Container::CMP_func(int _pointer) {
	auto p = _pointer - 1;
	unsigned int src = readInt();
	_pointer += INT_SIZE;
	unsigned int dst = readInt();
	_pointer += INT_SIZE;

	seek(dst);
	unsigned int value = readInt();
	setFlag(ZF, src == value);
	seek(_pointer);
	printCode("CMP", p, src, dst);
	return _pointer;
}

int Container::JNE_func(int _pointer) {
	auto p = _pointer - 1;
	unsigned int src = readInt();
	_pointer += INT_SIZE;

	seek(FLAGS);
	auto value = readByte();
	seek(_pointer);
	bool jumped = false;
	if (!checkFlag(ZF)) {
		_pointer = src;
		seek(_pointer);
		jumped = true;
	}
	printJump("JNE", p, src, jumped);
	return _pointer;
}

int Container::JE_func(int _pointer) {
	auto p = _pointer - 1;
	unsigned int src = readInt();
	_pointer += INT_SIZE;

	seek(FLAGS);
	auto value = readByte();
	seek(_pointer);
	bool jumped = false;
	if (checkFlag(ZF)) {
		_pointer = src;
		seek(_pointer);
		jumped = true;
	}
	printJump("JNE", p, src, jumped);
	return _pointer;
}

int Container::NOP_func(int _pointer) {
	auto p = _pointer - 1;
	printCode("NOP", p);
	return _pointer;
}

int Container::PUSH_func(int _pointer) {
	const auto p = _pointer - 1;
	const auto src = readInt();
	_pointer += INT_SIZE;
	seek(src);
	const auto value = readInt();
	seek(STACK_ADDR);
	const auto s_addr = readInt() - INT_SIZE;
	seek(s_addr);
	writeInt(value);
	seek(STACK_ADDR);
	writeInt(s_addr);
	seek(_pointer);
	printCode("PUSH", p, src);
	return _pointer;
}
int Container::POP_func(int _pointer) {
	auto p = _pointer - 1;
	const auto dst = readInt();
	_pointer += INT_SIZE;
	seek(STACK_ADDR);
	const auto s_addr = readInt();
	seek(s_addr);
	const auto value = readInt();
	seek(s_addr);
	writeInt(0x0);
	seek(dst);
	writeInt(value);
	seek(STACK_ADDR);
	writeInt(s_addr + INT_SIZE);

	seek(_pointer);
	printCode("POP", p);
	return _pointer;
}
