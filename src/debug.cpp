#include "vvm/container.hpp"
#include "rang.hpp"
#include "format.h"
#include <sstream>

std::string detectAddress(const address addr)
{
	if (addr == EAX) {
		return "EAX";
	}
	else if (addr == EBX) {
		return "EBX";
	}
	else if (addr == ECX) {
		return "ECX";
	}
	return fmt::format("{}", addr);

}

void Container::printCode(std::string code,
	const address arg1,
	const address arg2
) {
	const auto op_addr = readRegAddress(EIP);
	const auto local_pointer = pointer;

	const auto a1 = detectAddress(arg1);
	const auto a2 = detectAddress(arg2);

	seek(FLAGS); const auto flags = static_cast<unsigned char>(readByte());
	seek(arg1); const auto v1 = readInt();
	seek(arg2); const auto v2 = readInt();

	const auto addr = fmt::format("{}", op_addr);
	std::cout
		<< rang::fg::green << addr << rang::style::reset
		<< fmt::format(" | {:4} {:8} {:8} | {:08b} | ", code, a1, a2, flags)
		<< rang::fg::cyan << fmt::format("{}", arg1) << rang::style::reset
		<< fmt::format("-> {:02X} || ", v1)
		<< rang::fg::cyan << fmt::format("{}", arg2) << rang::style::reset
		<< fmt::format("-> {:02X}", v2)
		<< std::endl << std::flush;

	seek(local_pointer);
}

void Container::printCode(std::string code,
	const address arg1,
	const unsigned int arg2
) {
	const auto op_addr = readRegAddress(EIP);
	const auto local_pointer = pointer;

	const auto a1 = detectAddress(arg1);

	seek(FLAGS); const auto flags = static_cast<unsigned char>(readByte());
	seek(arg1); const auto v1 = readInt();

	const auto addr = fmt::format("{}", op_addr);
	std::cout
		<< rang::fg::green << addr << rang::style::reset
		<< fmt::format(" | {:4} {:8} {:08X} | {:08b} | ", code, a1, arg2, flags)
		<< rang::fg::cyan << fmt::format("{}", arg1) << rang::style::reset
		<< fmt::format("-> {:08X}", v1)
		<< std::endl << std::flush;

	seek(local_pointer);
}

void Container::printCode(const std::string code, const address arg1) {
	const auto op_addr = readRegAddress(EIP);
	const auto local_pointer = pointer;

	const auto a1 = detectAddress(arg1);

	seek(FLAGS); const auto flags = static_cast<unsigned char>(readByte());
	seek(arg1); const auto v1 = readInt();

	const auto addr = fmt::format("{}", op_addr);
	std::cout
		<< rang::fg::green << addr << rang::style::reset
		<< fmt::format(" | {:4} {:17} | {:08b} | ", code, a1, flags)
		<< rang::fg::cyan << fmt::format("{}", arg1) << rang::style::reset
		<< fmt::format("-> {:08X}", v1)
		<< std::endl << std::flush;

	seek(local_pointer);
}

void Container::printIRQ(const std::byte code)
{
	std::cout
		<< rang::fg::red
		<< fmt::format("{:>8}", "IRQ") << rang::style::reset
		<< fmt::format(" | {:02X}", static_cast<unsigned char>(code))
		<< std::endl << std::flush;
}

void Container::printCode(const std::string code, const std::byte arg1) {
	const auto op_addr = readRegAddress(EIP);
	const auto local_pointer = pointer;

	seek(FLAGS); const auto flags = static_cast<unsigned char>(readByte());

	const auto addr = fmt::format("{}", op_addr);
	std::cout
		<< rang::fg::green << addr << rang::style::reset
		<< fmt::format(" | {:4} {:02X} {:14} | {:08b} | ", code, static_cast<unsigned char>(arg1), "", flags)
		<< std::endl << std::flush;

	seek(local_pointer);
}

void Container::printCode(const std::string code, const int arg1) {
	const auto op_addr = readRegAddress(EIP);
	const auto local_pointer = pointer;

	seek(FLAGS); const auto flags = static_cast<unsigned char>(readByte());

	const auto addr = fmt::format("{}", op_addr);
	std::cout
		<< rang::fg::green << addr << rang::style::reset
		<< fmt::format(" | {:4} {:08X} {:8} | {:08b} | ", code, arg1, "", flags)
		<< std::endl << std::flush;

	seek(local_pointer);
}

void Container::printCode(const std::string code) {
	const auto op_addr = readRegAddress(EIP);
	const auto addr = fmt::format("{}", op_addr);
	std::cout << rang::fg::green << addr << rang::style::reset << " | ";
	fmt::print("{} ", code);
	std::cout << std::endl << std::flush;
}

void Container::printJump(const std::string code, const address arg1, const bool jumped) {
	const auto op_addr = readRegAddress(EIP);
	const auto addr = fmt::format("{}", op_addr);
	std::cout << rang::fg::green << addr << rang::style::reset << " | ";
	fmt::print("{:4} {} {:8}", code, arg1, "");

	const auto local_pointer = pointer;
	seek(FLAGS);
	fmt::print(" | {:08b}", static_cast<unsigned char>(readByte()));
	seek(local_pointer);
	fmt::print(" | ");
	if (jumped) {
		std::cout << "--> "
			<< rang::fg::green << fmt::format("{}", pointer) << rang::style::reset;
	}
	else {
		std::cout << rang::fg::red << "vvv" << rang::style::reset;
	}
	std::cout << std::endl << std::flush;
}

void Container::printJump(const std::string code, const int arg1, const bool jumped) {
	const auto op_addr = readRegAddress(EIP);
	const auto addr = fmt::format("{}", op_addr);
	std::cout << rang::fg::green << addr << rang::style::reset << " | ";
	fmt::print("{:4} ", code);
	auto sign = "+";
	if (arg1 <= 0)
	{
		sign = "";
	}
	const auto a1 = fmt::format("{}{}", sign, arg1);
	fmt::print("{:8} {:8}", a1, "");

	const auto local_pointer = pointer;
	seek(FLAGS);
	fmt::print(" | {:08b}", static_cast<unsigned char>(readByte()));
	seek(local_pointer);
	fmt::print(" | ");
	if (jumped) {
		std::cout << "--> "
			<< rang::fg::green << fmt::format("{}", pointer) << rang::style::reset;
	}
	else {
		std::cout << rang::fg::red << "vvv" << rang::style::reset;
	}
	std::cout << std::endl << std::flush;
}

void Container::dumpState() {
	auto b = _bytes;
	for (unsigned int n = 0; n < _size / 16; n++) {
		const auto addr = fmt::format("0x{0:02X}", n * 16);
		std::cout << rang::fg::green << addr << rang::style::reset << " | ";
		for (auto i = n * 16; i < (n + 1) * 16; i++) {
			if (b[i] == std::byte{ 0x0 }) {
				fmt::print("{0:02X} ", static_cast<unsigned char>(b[i]));
			}
			else {
				const auto c = fmt::format("{0:02X} ", static_cast<unsigned char>(b[i]));
				std::cout << rang::fg::cyan << c << rang::style::reset;
			}
		}
		fmt::print("| ");
		for (auto i = n * 16; i < (n + 1) * 16; i++) {
			if (b[i] < std::byte{ 32 }) {
				fmt::print(".");
			}
			else if (b[i] >= std::byte{ 128 }) {
				std::cout << rang::fg::red << "." << rang::style::reset;
			}
			else {
				std::cout << rang::fg::cyan << static_cast<unsigned char>(b[i]) << rang::style::reset;
			}
		}
		fmt::print("\n");
	}
	std::cout << std::endl << std::flush;
}

