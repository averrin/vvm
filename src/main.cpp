#include <iostream>
#include <cstdlib>
#include <iomanip>
#include "format.h"
#include "rang.hpp"
#include "project/container.hpp"


int main() {
  unsigned char code[BUF_SIZE];
  for (int i=0; i < BUF_SIZE; i++) {
    code[i] = TERM;
  }

  Container* mem = new Container(code);
  mem->init();

  mem->writeCode(MOV, 0x43, EAX);
  mem->writeCode(MOV, 0x44, EBX);
  mem->writeCode(ADD, 0x01, EBX);
  mem->writeCode(SUB, 0x05, EAX);

  // mem->seek(0x05);
  // mem->writeByte(pointer);

  fmt::print("Init state: \n");
  mem->dumpState();

  mem->execCode();

  fmt::print("Final state: \n");
  mem->dumpState();

  return EXIT_SUCCESS;
}

