#include <iostream>
#include <cstdlib>
#include "format.h"
#include "rang.hpp"
#include "vvm/container.hpp"
#include "vvm/constants.hpp"
#include "application.cpp"

std::vector<char> output;
int ticks = 0;

unsigned int readInt(const std::byte* bytes, const unsigned int pointer)
{
	return static_cast<int>(
		(bytes[pointer] << 24) |
		(bytes[pointer + 1] << 16) |
		(bytes[pointer + 2] << 8) |
		(bytes[pointer + 3]));

}

void tickHandler(std::byte* bytes, unsigned int pointer) {
	if (static_cast<bool>(bytes[FLAGS.dst] & OUTF)) {
		const auto n = readInt(bytes, OUT_PORT.dst);
		bytes[FLAGS.dst] &= ~OUTF;
		output.push_back(static_cast<char>(n));
	}
	ticks++;
}


void printHandler(const std::byte* bytes, unsigned int pointer)
{
	auto addr = readInt(bytes, ECX.dst);
	auto ch = bytes[addr];
	std::cout << rang::fg::cyan << "  >>   " << rang::style::reset;
	while (static_cast<char>(ch) != '$')
	{
		std::cout << static_cast<char>(ch);
		addr++;
		ch = bytes[addr];
	}
	std::cout << std::endl << std::flush;
}

std::string VERSION = "0.0.1";

int run_vm() {
	std::byte code[BUF_SIZE] = { std::byte{0x0} };

	auto mem = new Container(code, tickHandler);
	mem->init();
	mem->setInterruptHandler(INT_PRINT, printHandler);

	mem->_MOV(EAX, 0x11111111);
	mem->_MOV(EBX, EAX);
	mem->_ADD(EBX, EAX);
	mem->_ADD(EBX, 0x01);
	mem->_SUB(EBX, 0x05);
	mem->_SUB(EAX, EBX);
	mem->_INC(EAX);
	mem->_JMP(+OP_med_length); //next opcode
	const auto ja = mem->_JMP(address::BEGIN); //dummy jump addr
	const auto na = mem->_NOP();
	mem->seek(ja);
	mem->_JMP(na);
	mem->_PUSH(EAX);
	mem->_PUSH(0x02);
	mem->_POP(EAX);
	mem->_DEC(EAX);
	mem->_CMP(EAX, 0x0);
	mem->_JNE(-OP_long_length-OP_med_length); // pre-prev opcode
	mem->_JNE(address::CODE); //pass here
	
	mem->_INT(INT_END);
	
	mem->saveBytes("init.bin");
	fmt::print("Init state: \n");
	mem->dumpState();

	mem->execCode();

	mem->saveBytes("final.bin");
	fmt::print("Final state: \n");
	mem->dumpState();

	fmt::print("Total ticks: {}\n", ticks);

	/*
	fmt::print("Output:\n");
	for (auto ch : output) {
		fmt::print("{:c}", ch);
	}
	std::cout << std::endl << std::flush;
*/

	return EXIT_SUCCESS;
}

int main()
{
  Application app(VERSION);
  run_vm();
  app.serve();
}

