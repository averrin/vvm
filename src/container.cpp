#include "project/container.hpp"
#include <iostream>
#include "rang.hpp"
#include "format.h"

Container::Container(unsigned char* b) : _bytes(b) {
  _size = BUF_SIZE; //TODO
};

void Container::seek(unsigned int addr) {
  pointer = addr;
}

unsigned int Container::readInt() {
  unsigned int n = (_bytes[pointer] << 24) | (_bytes[pointer+1] << 16) | (_bytes[pointer+2] << 8) | (_bytes[pointer+3]);
  pointer += INT_SIZE;
  return n;
}


void Container::writeInt(int n) {
  _bytes[pointer] = (n >> 24) & 0xFF;
  pointer++;
  _bytes[pointer] = (n >> 16) & 0xFF;
  pointer++;
  _bytes[pointer] = (n >> 8) & 0xFF;
  pointer++;
  _bytes[pointer] = n & 0xFF;
  pointer++;
}

void Container::writeCode(unsigned char opcode, unsigned int arg1, unsigned int arg2) {
  writeByte(opcode);
  writeInt(arg1);
  writeInt(arg2);
}

void Container::init() {
  _writeHeader();

  seek(EAX); writeInt(0xffffffff);
  seek(EBX); writeInt(0xffffffff);

  code_offset = HEADER_SIZE + 0x0 + INT_SIZE*2;
}

void Container::dumpState() {
  for (int n=0; n < _size/16; n++) {
    std::string addr = fmt::format("0x{0:02X}", n*16);
    std::cout << rang::fg::green << addr << rang::style::reset << " | ";
    for (int i=n*16; i < (n+1)*16; i++) {
      if (_bytes[i] == 0x0) {
        fmt::print("{0:02X} ", _bytes[i]);
      } else {
        std::string b = fmt::format("{0:02X} ", _bytes[i]);
        std::cout << rang::fg::blue << b << rang::style::reset;
      }
    }
    fmt::print("| ");
    for (int i=n*16; i < (n+1)*16; i++) {
      if (_bytes[i] < 32) {
        fmt::print(".");
      } else if (_bytes[i] >= 128) {
        std::cout << rang::fg::red << "." << rang::style::reset;
      } else {
        std::cout << rang::fg::blue << (char)_bytes[i] << rang::style::reset;
      }
    }
    fmt::print("\n");
  }
  std::cout<<std::endl<<std::flush;
}

void Container::writeByte(unsigned char ch) {
  _bytes[pointer] = ch;
  pointer++;
}

void Container::_writeHeader() {
  seek(0x0);
  writeByte('V');
  writeByte('V');
  writeByte('M');

  writeByte(version);
  writeByte(code_offset);
}

void Container::printCode(std::string code, unsigned arg1, unsigned int arg2) {
  fmt::print("{} ", code);
  fmt::print("{} ", arg1);
  if (arg2 == EAX) {
    fmt::print("EAX ");
  } else if (arg2 == EBX) {
    fmt::print("EBX ");
  } else {
    fmt::print("{} ", arg2);
  }
  std::cout<<std::endl<<std::flush;
}

unsigned char Container::readByte() {
  unsigned char ch = _bytes[pointer];
  pointer++;
  return ch;
}

void Container::execCode() {
  fmt::print("\nExec: \n");
  fmt::print("============== \n");
  int _pointer = CODE_OFFSET;
  seek(_pointer);

  unsigned char opcode = readByte();
  while (opcode != TERM) {
    _pointer++;
    if (opcode == MOV) {
      unsigned int src = readInt();
      _pointer += INT_SIZE;
      unsigned int dst = readInt();
      _pointer += INT_SIZE;
      seek(dst);
      writeInt(src);
      seek(_pointer);
      printCode("MOV", src, dst);

    } else if (opcode == ADD) {
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

        printCode("ADD", src, dst);

    } else if (opcode == SUB) {
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

      printCode("SUB", src, dst);
    }
    opcode = readByte();
  }
  fmt::print("============== \n\n");
}
