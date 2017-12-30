#include <iostream>
#include <cstdlib>
#include "format.h"
#include "rang.hpp"
#include "project/container.hpp"

std::vector<char> output;

void tickHandler(unsigned char* bytes, unsigned int pointer) {
	if (bytes[FLAGS] & OUTF) {
		pointer = OUT_PORT;
		const unsigned int n =
			(bytes[pointer] << 24)		|
			(bytes[pointer + 1] << 16)	|
			(bytes[pointer + 2] << 8)	|
			(bytes[pointer + 3]);
		bytes[FLAGS] &= ~OUTF;
		output.push_back(static_cast<char>(n));
	}
}

void testHandler(unsigned char* bytes, unsigned int pointer)
{
	// fmt::print("TEST IRQ\n");
}

int main() {
	unsigned char code[BUF_SIZE] = { TERM };

	auto mem = new Container(code, tickHandler);
	mem->init();
	mem->setInterruptHandler(INT_TEST, testHandler);

	mem->writeCode(MOV, 0x2f, EBX);
	const auto addr = mem->writeCode(SUB, 0x01, EBX);
	mem->writeCode(CMP, 0x21, EBX);
	mem->writeCode(OUTPUT, EBX);
	mem->writeCode(JNE, addr);

	mem->writeCode(INTERRUPT, INT_TEST);
	mem->writeCode(NOP);
	mem->writeCode(INTERRUPT, INT_END);

	mem->saveBytes("init.bin");
	fmt::print("Init state: \n");
	mem->dumpState();

	mem->execCode();

	mem->saveBytes("final.bin");
	fmt::print("Final state: \n");
	mem->dumpState();

	fmt::print("Output:\n");
	for (auto ch : output) {
		fmt::print("{:c}", ch);
	}
	std::cout << std::endl << std::flush;

	return EXIT_SUCCESS;
}
