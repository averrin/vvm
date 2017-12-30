#include "project/container.hpp"
#include "format.h"


int Container::OUT_func(int _pointer) {
	auto p = _pointer - 1;
	unsigned int src = readInt();
	_pointer += INT_SIZE;
	seek(src);
	src = readInt();
	seek(OUT_PORT);
	writeInt(src);
	seek(FLAGS);
	unsigned int flags = readByte();
	seek(FLAGS);
	flags |= OUTF;
	writeByte(flags);
	seek(_pointer);
	printCode("OUT", p, src);
	return _pointer;
}

int Container::MOV_func(int _pointer) {
	auto p = _pointer - 1;
	unsigned int src = readInt();
	_pointer += INT_SIZE;
	unsigned int dst = readInt();
	_pointer += INT_SIZE;
	seek(dst);
	writeInt(src);
	seek(_pointer);
	printCode("MOV", p, src, dst);
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
	unsigned char value = readByte();
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
	unsigned char value = readByte();
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
