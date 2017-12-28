#include <iostream>
#include <cstdlib>
#include <iomanip>
#include "format.h"
#include "rang.hpp"

const unsigned char TERM = 0x00;
const unsigned char NOP = 0x90;
const unsigned char MOV = 0x88;
const unsigned char ADD = 0x01;
const unsigned char SUB = 0x28;

const unsigned int BUF_SIZE = 128;
const unsigned int INT_SIZE = 4;


const int EAX = 0x0;
const int EBX = 0x0 + INT_SIZE;

const unsigned char CODE_OFFSET = 0x0 + INT_SIZE*2;

void writeInt(unsigned char* bytes, int addr, int n) {
  bytes[0 + addr] = (n >> 24) & 0xFF;
  bytes[1 + addr] = (n >> 16) & 0xFF;
  bytes[2 + addr] = (n >> 8) & 0xFF;
  bytes[3 + addr] = n & 0xFF;
}

void dumpState(unsigned char* bytes) {
  for (int n=0; n < BUF_SIZE/16; n++) {
    std::string addr = fmt::format("0x{0:02X}", n*16);
    std::cout << rang::fg::green << addr << rang::style::reset << " | ";
    for (int i=n*16; i < (n+1)*16; i++) {
      if (bytes[i] == 0x0) {
        fmt::print("{0:02X} ", bytes[i]);
      } else {
        std::string b = fmt::format("{0:02X} ", bytes[i]);
        std::cout << rang::fg::blue << b << rang::style::reset;
      }
    }
    fmt::print("| ");
    for (int i=n*16; i < (n+1)*16; i++) {
      if (bytes[i] < 32) {
        fmt::print(".");
      } else if (bytes[i] >= 128) {
        std::cout << rang::fg::red << "." << rang::style::reset;
      } else {
        std::cout << rang::fg::blue << (char)bytes[i] << rang::style::reset;
      }
    }
    fmt::print("\n");
  }
  std::cout<<std::endl<<std::flush;
}

unsigned int readInt(unsigned char* bytes, int pointer) {
  return (bytes[pointer] << 24) | (bytes[pointer+1] << 16) | (bytes[pointer+2] << 8) | (bytes[pointer+3]);
}

void printCode(std::string code, unsigned arg1, unsigned int arg2) {
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

void execCode(unsigned char* bytes) {
  fmt::print("\nExec: \n");
  fmt::print("============== \n");
  int pointer = CODE_OFFSET;

  while (bytes[pointer] != TERM) {
    unsigned char opcode = bytes[pointer];
    pointer++;
    if (opcode == MOV) {
      unsigned int src = readInt(bytes, pointer);
      pointer += INT_SIZE;
      unsigned int dst = readInt(bytes, pointer);
      writeInt(bytes, dst, src);
      printCode("MOV", src, dst);

      pointer += INT_SIZE;

    } else if (opcode == ADD) {
        unsigned int src = readInt(bytes, pointer);
        pointer += INT_SIZE;
        unsigned int dst = readInt(bytes, pointer);

        unsigned int value = readInt(bytes, dst);
        value += src;
        writeInt(bytes, dst, value);

        printCode("ADD", src, dst);

        pointer += INT_SIZE;

    } else if (opcode == SUB) {
      unsigned int src = readInt(bytes, pointer);
      pointer += INT_SIZE;
      unsigned int dst = readInt(bytes, pointer);

      unsigned int value = readInt(bytes, dst);
      value -= src;
      writeInt(bytes, dst, value);

      printCode("SUB", src, dst);

      pointer += INT_SIZE;
    }
  }
  fmt::print("============== \n\n");
}

unsigned int writeCode(unsigned char* bytes, int pointer, unsigned char opcode, unsigned int arg1, unsigned int arg2) {
  bytes[CODE_OFFSET + pointer] = opcode;
  pointer++;
  writeInt(bytes, CODE_OFFSET + pointer, arg1);
  pointer += INT_SIZE;
  writeInt(bytes, CODE_OFFSET + pointer, arg2);
  pointer += INT_SIZE;
  return pointer;
}

int main() {
  unsigned char code[BUF_SIZE];
  for (int i=0; i < BUF_SIZE; i++) {
    code[i] = TERM;
  }

  writeInt(code, EAX, 0x41);
  writeInt(code, EBX, 0x42);

  int pointer = 0x0;
  pointer = writeCode(code, pointer, MOV, 0x43, EAX);
  pointer = writeCode(code, pointer, MOV, 0x44, EBX);
  pointer = writeCode(code, pointer, ADD, 0x01, EBX);
  pointer = writeCode(code, pointer, SUB, 0x05, EAX);

  fmt::print("Init state: \n");
  dumpState(code);

  execCode(code);

  fmt::print("Final state: \n");
  dumpState(code);

  return EXIT_SUCCESS;
}

