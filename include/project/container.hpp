#ifndef CONTAINER_HPP_
#define CONTAINER_HPP_
#include <string>


const unsigned char TERM = 0x00;
const unsigned char NOP = 0x90;
const unsigned char MOV = 0x88;
const unsigned char ADD = 0x01;
const unsigned char SUB = 0x28;

const unsigned int BUF_SIZE = 128;
const unsigned int INT_SIZE = 4;

const unsigned int HEADER_SIZE = 8;


const int EAX = HEADER_SIZE + 0x0;
const int EBX = HEADER_SIZE + 0x0 + INT_SIZE;

const unsigned char CODE_OFFSET = HEADER_SIZE + 0x0 + INT_SIZE*2;


class Container {
private:
  void _writeHeader();

  unsigned char* _bytes;
  unsigned int _size;
public:
  unsigned char readByte();
  void writeByte(unsigned char ch);
  void execCode();
  void printCode(std::string code, unsigned int arg1, unsigned int arg2);

  unsigned int readInt();
  void seek(unsigned int addr);
  void writeCode(unsigned char opcode, unsigned int arg1, unsigned int arg2);
  void writeInt(int n);

  static const unsigned char version = 0x01;
  unsigned int code_offset;
  unsigned int pointer;

  void init();
  void dumpState();
  Container(unsigned char* b);
};

#endif

