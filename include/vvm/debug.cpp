#include "format.h"
#include "rang.hpp"
#include "vvm/core.hpp"
#include <sstream>

//TODO: format table
//TODO: write one function for std::variant and op_size

std::string detectAddress(const address addr) {
  if (addr == EAX) {
    return "EAX";
  } else if (addr == EBX) {
    return "EBX";
  } else if (addr == ECX) {
    return "ECX";
  }
  return fmt::format("{}", addr);
}

void Core::printCode(const std::string_view code, const address arg1,
                     const address arg2) {
  const auto op_addr = readRegAddress(EIP);
  const auto local_pointer = pointer;

  const auto a1 = detectAddress(arg1);
  const auto a2 = detectAddress(arg2);

  seek(FLAGS);
  const auto flags = static_cast<unsigned char>(readByte());
  seek(arg1);
  const auto v1 = readInt();
  seek(arg2);
  const auto v2 = readInt();

  const auto addr = fmt::format("{}", op_addr);
  std::cout << rang::fg::green << addr << rang::style::reset
            << fmt::format(" | {:4} {:8} {:8} | {:08b} | ", code, a1, a2, flags)
            << rang::fg::cyan << fmt::format("{}", arg1) << rang::style::reset
            << fmt::format("-> {:02X} || ", v1) << rang::fg::cyan
            << fmt::format("{}", arg2) << rang::style::reset
            << fmt::format("-> {:02X}", v2) << std::endl
            << std::flush;

  seek(local_pointer);
}

void Core::printCode(const std::string_view code, const address arg1,
                     const unsigned int arg2) {
  const auto op_addr = readRegAddress(EIP);
  const auto local_pointer = pointer;

  const auto a1 = detectAddress(arg1);

  seek(FLAGS);
  const auto flags = static_cast<unsigned char>(readByte());
  seek(arg1);
  const auto v1 = readInt();

  const auto addr = fmt::format("{}", op_addr);
  std::cout << rang::fg::green << addr << rang::style::reset
            << fmt::format(" | {:4} {:8} {:08X} | {:08b} | ", code, a1, arg2,
                           flags)
            << rang::fg::cyan << fmt::format("{}", arg1) << rang::style::reset
            << fmt::format("-> {:08X}", v1) << std::endl
            << std::flush;

  seek(local_pointer);
}

void Core::printCode(const std::string_view code, const address arg1,
                     const std::byte arg2) {
  const auto op_addr = readRegAddress(EIP);
  const auto local_pointer = pointer;

  const auto a1 = detectAddress(arg1);

  seek(FLAGS);
  const auto flags = static_cast<unsigned char>(readByte());
  seek(arg1);
  const auto v1 = readInt();

  const auto addr = fmt::format("{}", op_addr);
  std::cout << rang::fg::green << addr << rang::style::reset
            << fmt::format(" | {:4} {:8} {:08X} | {:02b} | ", code, a1, static_cast<unsigned int>(arg2),
                           flags)
            << rang::fg::cyan << fmt::format("{}", arg1) << rang::style::reset
            << fmt::format("-> {:08X}", v1) << std::endl
            << std::flush;

  seek(local_pointer);
}

void Core::printCode(const std::string_view code, const address arg1) {
  const auto op_addr = readRegAddress(EIP);
  const auto local_pointer = pointer;

  const auto a1 = detectAddress(arg1);

  seek(FLAGS);
  const auto flags = static_cast<unsigned char>(readByte());
  seek(arg1);
  const auto v1 = readInt();

  const auto addr = fmt::format("{}", op_addr);
  std::cout << rang::fg::green << addr << rang::style::reset
            << fmt::format(" | {:4} {:17} | {:08b} | ", code, a1, flags)
            << rang::fg::cyan << fmt::format("{}", arg1) << rang::style::reset
            << fmt::format("-> {:08X}", v1) << std::endl
            << std::flush;

  seek(local_pointer);
}

void Core::printIRQ(const std::byte code) {
  std::cout << rang::fg::red << fmt::format("{:>9}", "IRQ")
            << rang::style::reset
            << fmt::format(" | {:02X}", static_cast<unsigned char>(code))
            << std::endl
            << std::flush;
}

void Core::printCode(const std::string_view code, const std::byte arg1) {
  const auto op_addr = readRegAddress(EIP);
  const auto local_pointer = pointer;

  seek(FLAGS);
  const auto flags = static_cast<unsigned char>(readByte());

  const auto addr = fmt::format("{}", op_addr);
  std::cout << rang::fg::green << addr << rang::style::reset
            << fmt::format(" | {:4} {:02X} {:14} | {:08b} | ", code,
                           static_cast<unsigned char>(arg1), "", flags)
            << std::endl
            << std::flush;

  seek(local_pointer);
}

void Core::printCode(const std::string_view code, const int arg1) {
  const auto op_addr = readRegAddress(EIP);
  const auto local_pointer = pointer;

  seek(FLAGS);
  const auto flags = static_cast<unsigned char>(readByte());

  const auto addr = fmt::format("{}", op_addr);
  std::cout << rang::fg::green << addr << rang::style::reset
            << fmt::format(" | {:4} {:08X} {:8} | {:08b} | ", code, arg1, "",
                           flags)
            << std::endl
            << std::flush;

  seek(local_pointer);
}

void Core::printCode(const std::string_view code) {
  const auto op_addr = readRegAddress(EIP);
  const auto addr = fmt::format("{}", op_addr);
  std::cout << rang::fg::green << addr << rang::style::reset << " | ";
  fmt::print("{} ", code);
  std::cout << std::endl << std::flush;
}

void Core::printJump(const std::string_view code, const address arg1,
                     const bool jumped) {
  const auto op_addr = readRegAddress(EIP);
  const auto addr = fmt::format("{}", op_addr);
  std::cout << rang::fg::green << addr << rang::style::reset << " | ";
  fmt::print("{:4} {} {:7}", code, arg1, "");

  const auto local_pointer = pointer;
  seek(FLAGS);
  fmt::print(" | {:08b}", static_cast<unsigned char>(readByte()));
  seek(local_pointer);
  fmt::print(" | ");
  if (jumped) {
    std::cout << "--> " << rang::fg::green << fmt::format("{}", pointer)
              << rang::style::reset;
  } else {
    std::cout << rang::fg::red << "vvv" << rang::style::reset;
  }
  std::cout << std::endl << std::flush;
}

void Core::printJump(const std::string_view code, const int arg1,
                     const bool jumped) {
  const auto op_addr = readRegAddress(EIP);
  const auto addr = fmt::format("{}", op_addr);
  std::cout << rang::fg::green << addr << rang::style::reset << " | ";
  fmt::print("{:4} ", code);
  auto sign = "+";
  if (arg1 <= 0) {
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
    std::cout << "--> " << rang::fg::green << fmt::format("{}", pointer)
              << rang::style::reset;
  } else {
    std::cout << rang::fg::red << "vvv" << rang::style::reset;
  }
  std::cout << std::endl << std::flush;
}
