#include "project/container.hpp"
#include "rang.hpp"
#include "format.h"

void Container::printCode(const std::string code,
	const unsigned int op_addr,
	const unsigned int arg1,
	const unsigned int arg2
) {
	const auto addr = fmt::format("0x{0:02X}", op_addr);
	std::cout << rang::fg::green << addr << rang::style::reset << " | ";
	fmt::print("{} ", code);
	if (arg1 == EAX) {
		fmt::print("EAX ");
	}
	else if (arg1 == EBX) {
		fmt::print("EBX ");
	}
	else if (arg1 == ECX) {
		fmt::print("ECX ");
	}
	else {
		fmt::print("{0:02X} ", arg1);
	}
	fmt::print("{0:02X}", arg2);

	const int local_pointer = pointer;
	seek(FLAGS);
	fmt::print("\t| {:08b}", readByte());
	fmt::print("\t| ");
	std::cout << rang::fg::cyan << fmt::format("0x{:02X}", arg2) << rang::style::reset;
	seek(arg2);
	std::cout << fmt::format(" = {:02X}", readInt());
	seek(local_pointer);

	std::cout << std::endl << std::flush;
}

void Container::printCode(const std::string code, const unsigned int op_addr, const unsigned arg1) {
	const auto addr = fmt::format("0x{0:02X}", op_addr);
	std::cout << rang::fg::green << addr << rang::style::reset << " | ";
	fmt::print("{} ", code);
	fmt::print("{:02X}    ", arg1);

	const auto local_pointer = pointer;
	seek(FLAGS);
	fmt::print("\t| {:08b}", readByte());
	seek(local_pointer);
	std::cout << std::endl << std::flush;
}

void Container::printCode(const std::string code, const unsigned int op_addr) {
	const auto addr = fmt::format("0x{0:02X}", op_addr);
	std::cout << rang::fg::green << addr << rang::style::reset << " | ";
	fmt::print("{} ", code);
	std::cout << std::endl << std::flush;
}

void Container::printJump(const std::string code, const unsigned int op_addr, const unsigned arg1, const bool jumped) {
	const auto addr = fmt::format("0x{0:02X}", op_addr);
	std::cout << rang::fg::green << addr << rang::style::reset << " | ";
	fmt::print("{} ", code);
	fmt::print("{:02X}    ", arg1);

	const auto local_pointer = pointer;
	seek(FLAGS);
	fmt::print("\t| {:08b}", readByte());
	seek(local_pointer);
	fmt::print("\t| ");
	if (jumped) {
		std::cout << rang::fg::green << "-->" << rang::style::reset;
	}
	else {
		std::cout << rang::fg::red << "vvv" << rang::style::reset;
	}
	std::cout << std::endl << std::flush;
}
void Container::dumpState() {
	for (unsigned int n = 0; n < _size / 16; n++) {
		const auto addr = fmt::format("0x{0:02X}", n * 16);
		std::cout << rang::fg::green << addr << rang::style::reset << " | ";
		for (auto i = n * 16; i < (n + 1) * 16; i++) {
			if (_bytes[i] == 0x0) {
				fmt::print("{0:02X} ", _bytes[i]);
			}
			else {
				const auto b = fmt::format("{0:02X} ", _bytes[i]);
				std::cout << rang::fg::cyan << b << rang::style::reset;
			}
		}
		fmt::print("| ");
		for (auto i = n * 16; i < (n + 1) * 16; i++) {
			if (_bytes[i] < 32) {
				fmt::print(".");
			}
			else if (_bytes[i] >= 128) {
				std::cout << rang::fg::red << "." << rang::style::reset;
			}
			else {
				std::cout << rang::fg::cyan << static_cast<char>(_bytes[i]) << rang::style::reset;
			}
		}
		fmt::print("\n");
	}
	std::cout << std::endl << std::flush;
}

