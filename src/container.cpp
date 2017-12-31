#include "project/container.hpp"
#include <iostream>
#include "rang.hpp"
#include "format.h"
#include <fstream>
#include <utility>

Container::Container(std::byte* b, t_handler th) : _tickHandler(std::move(th)), _bytes(b), code_offset(0), pointer(0)
{
	_size = BUF_SIZE; //TODO
	seek(STACK_ADDR);
	writeInt(BUF_SIZE);
};

void Container::setInterruptHandler(const std::byte interrupt, t_handler handler)
{
	_intHandlers[interrupt] = handler;
}


void Container::saveBytes(const std::string name) {
	std::ofstream file(name, std::ios::binary);
	file.write(reinterpret_cast<const char*>(_bytes), BUF_SIZE);
}

void Container::seek(const unsigned int addr) {
	pointer = addr;
}

unsigned int Container::readInt() {
	/**/
	const auto n = (static_cast<int>(_bytes[pointer]) << 24) |
		(static_cast<int>(_bytes[pointer + 1]) << 16) |
		(static_cast<int>(_bytes[pointer + 2]) << 8) |
		(static_cast<int>(_bytes[pointer + 3]));
	pointer += INT_SIZE;
	return n;
}


void Container::writeInt(const int n) {
	_bytes[pointer] = static_cast<std::byte>((n >> 24) & 0xFF);
	pointer++;
	_bytes[pointer] = static_cast<std::byte>((n >> 16) & 0xFF);
	pointer++;
	_bytes[pointer] = static_cast<std::byte>((n >> 8) & 0xFF);
	pointer++;
	_bytes[pointer] = static_cast<std::byte>(n & 0xFF);
	pointer++;
}

int Container::writeCode(const std::byte opcode, const unsigned int arg1, const unsigned int arg2) {
	const int local_pointer = pointer;
	writeByte(opcode);
	writeInt(arg1);
	writeInt(arg2);
	return local_pointer;
}

int Container::writeCode(const std::byte opcode, const int arg1) {
	const int local_pointer = pointer;
	writeByte(opcode);
	writeInt(arg1);
	return local_pointer;
}

int Container::writeCode(const std::byte opcode, const std::byte arg1) {
	const int local_pointer = pointer;
	writeByte(opcode);
	writeByte(arg1);
	return local_pointer;
}

int Container::writeCode(const std::byte opcode) {
	const int local_pointer = pointer;
	writeByte(opcode);
	return local_pointer;
}

void Container::init() {
	writeHeader();

	/*
	seek(EAX); writeInt(0x0);
	seek(EBX); writeInt(0x0);
	seek(ECX); writeInt(0x0);
	*/
	seek(FLAGS); writeByte(static_cast<std::byte>(0b00000000));
	seek(OUT_PORT); writeInt(0xffffffff);

	code_offset = HEADER_SIZE + 0x0 + INT_SIZE * 2;
}

void Container::writeByte(const std::byte ch) {
	_bytes[pointer] = ch;
	pointer++;
}

void Container::writeHeader() {
	seek(0x0);
	writeByte(static_cast<std::byte>('V'));
	writeByte(static_cast<std::byte>('V'));
	writeByte(static_cast<std::byte>('M'));

	writeByte(version);
	writeByte(static_cast<std::byte>(code_offset));
}

std::byte Container::readByte() {
	const auto ch = _bytes[pointer];
	pointer++;
	return ch;
}

bool Container::checkFlag(const std::byte mask)
{
	const auto local_pointer = pointer;
	seek(FLAGS);
	const auto flag = readByte();
	seek(local_pointer);
	return static_cast<bool>(flag & mask);
}

std::byte Container::getState()
{
	const auto local_pointer = pointer;
	seek(STATE);
	const auto state = readByte();
	seek(local_pointer);
	return state;
}

void Container::setState(const std::byte state)
{
	const auto local_pointer = pointer;
	seek(STATE);
	writeByte(state);
	seek(local_pointer);
}

void Container::setFlag(const std::byte flag, const bool value)
{
	const auto local_pointer = pointer;
	seek(FLAGS);
	auto flags = readByte();
	if (value) {
		flags |= flag;
	}
	else {
		flags &= ~flag;
	}
	seek(FLAGS);
	writeByte(flags);
	seek(local_pointer);
}

void Container::checkInterruption()
{
	const auto local_pointer = pointer;
	if (checkFlag(INTF))
	{
		seek(INTERRUPTS);
		const auto interrupt = readByte();
		std::cout << rang::fg::red << " IRQ" << rang::style::reset << " | ";
		fmt::print("{:02X}\n", static_cast<unsigned char>(interrupt));
		if (interrupt == INT_END)
		{
			setState(STATE_END);
		} else if (_intHandlers.count(interrupt) == 1)
		{
			_intHandlers[interrupt](_bytes, pointer);
		}
		setFlag(INTF, false);
	}
	seek(local_pointer);
}

void Container::execCode() {
	fmt::print("\nExec: \n");
	fmt::print("============== \n");
	setState(STATE_EXEC);
	auto local_pointer = CODE_OFFSET;
	seek(local_pointer);

	while (getState() == STATE_EXEC) {
		const auto opcode = readByte();
		local_pointer++;
		if (opcode == MOV) {
			local_pointer = MOV_func(local_pointer);
		}
		else if (opcode == ADD) {
			local_pointer = ADD_func(local_pointer);
		}
		else if (opcode == SUB) {
			local_pointer = SUB_func(local_pointer);
		}
		else if (opcode == CMP) {
			local_pointer = CMP_func(local_pointer);
		}
		else if (opcode == JNE) {
			local_pointer = JNE_func(local_pointer);
		}
		else if (opcode == OUTPUT) {
			local_pointer = OUT_func(local_pointer);
		}
		else if (opcode == INTERRUPT) {
			local_pointer = INT_func(local_pointer);
		}
		else if (opcode == NOP) {
			local_pointer = NOP_func(local_pointer);
		}
		else if (opcode == PUSH) {
			local_pointer = PUSH_func(local_pointer);
		}
		else if (opcode == POP) {
			local_pointer = POP_func(local_pointer);
		}
		checkInterruption();
		_tickHandler(_bytes, pointer);
		if (pointer >= BUF_SIZE)
		{
			//TODO: implement irq and error handler
			setState(STATE_ERROR);
		}
	}
	fmt::print("============== \n\n");
}
