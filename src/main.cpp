#include <iostream>
#include <cstdlib>
#include "format.h"
#include "rang.hpp"
#include "project/container.hpp"

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
	if (static_cast<bool>(bytes[FLAGS] & OUTF)) {
		const auto n = readInt(bytes, OUT_PORT);
		bytes[FLAGS] &= ~OUTF;
		output.push_back(static_cast<char>(n));
	}
	ticks++;
}


void printHandler(const std::byte* bytes, unsigned int pointer)
{
	auto addr = readInt(bytes, ECX);
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

int main() {
	std::byte code[BUF_SIZE] = { TERM };

	auto mem = new Container(code, tickHandler);
	mem->init();
	mem->setInterruptHandler(INT_PRINT, printHandler);

	const auto addr = mem->writeCode(MOV, ECX, 255);
	mem->writeCode(INTERRUPT, INT_PRINT);
	mem->writeCode(INTERRUPT, INT_END);
	const auto addr2 = mem->pointer;
	mem->seek(addr);
	mem->writeCode(MOV, ECX, addr2);
	mem->seek(addr2);
	char msg[] = "hello world!";
	for (auto i = 0; i < 12; i++)
	{
		mem->writeByte(static_cast<std::byte>(msg[i]));
	}
	mem->writeByte(static_cast<std::byte>('$'));

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
