#include "project/container.hpp"
#include <iostream>
#include "rang.hpp"
#include "format.h"
#include <fstream>

Container::Container(unsigned char* b) : _bytes(b) {
  _size = BUF_SIZE; //TODO
};


void Container::saveBytes() {

	  std::ofstream file("myfile.bin", std::ios::binary);
	  file.write(reinterpret_cast<const char*>(_bytes), BUF_SIZE);
}

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

int Container::writeCode(unsigned char opcode, unsigned int arg1, unsigned int arg2) {
  int _pointer = pointer;
  writeByte(opcode);
  writeInt(arg1);
  writeInt(arg2);
  return _pointer;
}

int Container::writeCode(unsigned char opcode, unsigned int arg1) {
  int _pointer = pointer;
  writeByte(opcode);
  writeInt(arg1);
  return _pointer;
}

void Container::init() {
  _writeHeader();

  seek(EAX); writeInt(0xffffffff);
  seek(EBX); writeInt(0xffffffff);
  seek(ECX); writeInt(0xffffffff);
  seek(FLAGS); writeByte(0b00000000);
  seek(OUT_PORT); writeInt(0xffffffff);

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

void Container::printCode(std::string code, unsigned int op_addr, unsigned arg1, unsigned int arg2) {
  std::string addr = fmt::format("0x{0:02X}", op_addr);
  std::cout << rang::fg::green << addr << rang::style::reset << " | ";
  fmt::print("{} ", code);
  fmt::print("{0:02X} ", arg1);
  if (arg2 == EAX) {
    fmt::print("EAX");
  } else if (arg2 == EBX) {
    fmt::print("EBX");
  } else if (arg2 == ECX) {
    fmt::print("ECX");
  } else {
    fmt::print("{{0:02X}} ", arg2);
  }

  int _pointer = pointer;
  seek(FLAGS);
  fmt::print("\t| FLAGS= {:#b}", readByte());
  seek(arg2);
  fmt::print("\t| {:02X}= {:02X}", arg2, readInt());
  seek(_pointer);

  std::cout<<std::endl<<std::flush;
}

void Container::printCode(std::string code, unsigned int op_addr, unsigned arg1) {
  std::string addr = fmt::format("0x{0:02X}", op_addr);
  std::cout << rang::fg::green << addr << rang::style::reset << " | ";
  fmt::print("{} ", code);
    fmt::print("{:02X}    ", arg1);

  int _pointer = pointer;
  seek(FLAGS);
  fmt::print("\t| FLAGS= {:#b}", readByte());
  seek(_pointer);
  std::cout<<std::endl<<std::flush;
}

unsigned char Container::readByte() {
  unsigned char ch = _bytes[pointer];
  pointer++;
  return ch;
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
  printCode("CMP", p, src, dst);

  seek(dst);
  unsigned int value = readInt();
  seek(FLAGS);
  if (src == value) {
    writeByte(0b00000001);
  } else {
    writeByte(0b00000000);
  }
  seek(_pointer);
  return _pointer;
}

int Container::JNE_func(int _pointer) {
	auto p = _pointer - 1;
  unsigned int src = readInt();
  _pointer += INT_SIZE;
  printCode("JNE", p, src);

  seek(FLAGS);
  unsigned char value = readByte();
  seek(_pointer);
  if (value == 0x00) {
    _pointer = src;
    seek(_pointer);
    fmt::print("-->\n");
  }
  return _pointer;
}

int Container::JE_func(int _pointer) {
	auto p = _pointer - 1;
  unsigned int src = readInt();
  _pointer += INT_SIZE;
  printCode("JNE", p, src);

  seek(FLAGS);
  unsigned char value = readByte();
  seek(_pointer);
  if (value != 0x00) {
    _pointer = src;
    seek(_pointer);
    fmt::print("-->\n");
  }
  return _pointer;
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
      _pointer = MOV_func(_pointer);
    } else if (opcode == ADD) {
      _pointer = ADD_func(_pointer);
    } else if (opcode == SUB) {
      _pointer = SUB_func(_pointer);
    } else if (opcode == CMP) {
      _pointer = CMP_func(_pointer);
    } else if (opcode == JNE) {
      _pointer = JNE_func(_pointer);
    }
    opcode = readByte();
  }
  fmt::print("============== \n\n");
}
