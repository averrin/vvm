#include "project/container.hpp"
#include <iostream>
#include "rang.hpp"
#include "format.h"
#include <fstream>
#include <utility>

address address::BEGIN = address{ 0x0 };
address address::CODE = address{ CODE_OFFSET };

Container::Container(std::byte* b, t_handler th) : _tickHandler(std::move(th)), _bytes(b)
{
	_size = BUF_SIZE; //TODO

	pointer = address::BEGIN;
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

void Container::seek(address addr) {
	pointer = addr;
}

address Container::readAddress()
{
	return address{ readInt() };
}

unsigned int Container::readInt() {
	const auto n = (static_cast<int>(_bytes[pointer.dst]) << 24) |
		(static_cast<int>(_bytes[pointer.dst + 1]) << 16) |
		(static_cast<int>(_bytes[pointer.dst + 2]) << 8) |
		(static_cast<int>(_bytes[pointer.dst + 3]));
	pointer += INT_SIZE;
	return n;
}

int Container::readSignedInt() {
	const auto n = (static_cast<int>(_bytes[pointer.dst]) << 24) |
		(static_cast<int>(_bytes[pointer.dst + 1]) << 16) |
		(static_cast<int>(_bytes[pointer.dst + 2]) << 8) |
		(static_cast<int>(_bytes[pointer.dst + 3]));
	pointer += INT_SIZE;
	return n;
}

void Container::writeAddress(const address n) {
	writeInt(n.dst);
}

void Container::writeInt(const int n) {
	_bytes[pointer.dst] = static_cast<std::byte>((n >> 24) & 0xFF);
	pointer++;
	_bytes[pointer.dst] = static_cast<std::byte>((n >> 16) & 0xFF);
	pointer++;
	_bytes[pointer.dst] = static_cast<std::byte>((n >> 8) & 0xFF);
	pointer++;
	_bytes[pointer.dst] = static_cast<std::byte>(n & 0xFF);
	pointer++;
}

address Container::writeCode(std::byte opcode, address arg1, unsigned int arg2) {
	const auto local_pointer = pointer;
	writeByte(opcode);
	writeAddress(arg1);
	writeInt(arg2);
	return local_pointer;
}

address Container::writeCode(std::byte opcode, address arg1, address arg2) {
	const auto local_pointer = pointer;
	writeByte(opcode);
	writeAddress(arg1);
	writeAddress(arg2);
	return local_pointer;
}

address Container::writeCode(std::byte opcode, address arg1) {
	const auto local_pointer = pointer;
	writeByte(opcode);
	writeAddress(arg1);
	return local_pointer;
}

address Container::writeCode(const std::byte opcode, const std::byte arg1) {
	const auto local_pointer = pointer;
	writeByte(opcode);
	writeByte(arg1);
	return local_pointer;
}

address Container::writeCode(const std::byte opcode, const int arg1) {
	const auto local_pointer = pointer;
	writeByte(opcode);
	writeInt(arg1);
	return local_pointer;
}

address Container::writeCode(const std::byte opcode) {
	const auto local_pointer = pointer;
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
}

void Container::writeByte(const std::byte ch) {
	_bytes[pointer.dst] = ch;
	pointer++;
}

void Container::writeHeader() {
	seek(address::BEGIN);
	writeByte(static_cast<std::byte>('V'));
	writeByte(static_cast<std::byte>('V'));
	writeByte(static_cast<std::byte>('M'));

	writeByte(version);
	writeByte(static_cast<std::byte>(address::CODE.dst));
}

std::byte Container::readByte() {
	const auto ch = _bytes[pointer.dst];
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
		printIRQ(interrupt);
		if (interrupt == INT_END)
		{
			setState(STATE_END);
		} else if (_intHandlers.count(interrupt) == 1)
		{
			_intHandlers[interrupt](_bytes, pointer.dst);
		}
		setFlag(INTF, false);
	}
	seek(local_pointer);
}

void Container::execCode() {
	fmt::print("= ADDR ==|====== INSTRUCTION =====|= FLAGS ==|===== VARIABLES ====\n");
	fmt::print("         |                        |          |                    \n");
	setState(STATE_EXEC);
	auto local_pointer = CODE_OFFSET;
	seek(local_pointer);

	while (getState() == STATE_EXEC) {
		const auto opcode = readByte();
		local_pointer++;
		if (opcode == MOV_mm) {
			local_pointer = MOV_mm_func(local_pointer);
		}
		else if (opcode == MOV_mc) {
			local_pointer = MOV_mc_func(local_pointer);
		}
		else if (opcode == ADD_mm) {
			local_pointer = ADD_mm_func(local_pointer);
		}
		else if (opcode == ADD_mc) {
			local_pointer = ADD_mc_func(local_pointer);
		}
		else if (opcode == SUB_mm) {
			local_pointer = SUB_mm_func(local_pointer);
		}
		else if (opcode == SUB_mc) {
			local_pointer = SUB_mc_func(local_pointer);
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
		else if (opcode == JMP_a) {
			local_pointer = JMP_a_func(local_pointer);
		}
		else if (opcode == JMP_r) {
			local_pointer = JMP_r_func(local_pointer);
		}
		checkInterruption();
		_tickHandler(_bytes, pointer.dst);
		if (pointer.dst >= BUF_SIZE)
		{
			//TODO: implement irq and error handler
			setState(STATE_ERROR);
		}
	}
	fmt::print("         |                        |          |                    \n");
	fmt::print("==================================================================\n\n");
}
