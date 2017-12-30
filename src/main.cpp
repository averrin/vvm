#include <iostream>
#include <cstdlib>
#include <iomanip>
#include "format.h"
#include "rang.hpp"
#include "project/container.hpp"

std::vector<char> output;

void tickHandler(unsigned char* _bytes, unsigned int pointer) {
	if (_bytes[FLAGS] & OUTF) {
		pointer = OUT_PORT;
		unsigned int n = (_bytes[pointer] << 24) | (_bytes[pointer + 1] << 16) | (_bytes[pointer + 2] << 8) | (_bytes[pointer + 3]);
		_bytes[FLAGS] &= ~OUTF;
		output.push_back((char)n);
	}
}

int main() {
  unsigned char code[BUF_SIZE] = {TERM};

  Container* mem = new Container(code, tickHandler);
  mem->init();

  mem->writeCode(MOV, 0x7F, EBX);
  int addr = mem->writeCode(SUB, 0x01, EBX);
  mem->writeCode(CMP, 0x21, EBX);
  mem->writeCode(OUTPUT, EBX);
  mem->writeCode(JNE, addr);

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
 