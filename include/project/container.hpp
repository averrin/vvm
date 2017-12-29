#ifndef CONTAINER_HPP_
#define CONTAINER_HPP_
#include <string>


const unsigned char TERM = 0x00;
const unsigned char NOP = 0x90;
const unsigned char MOV = 0x88;
const unsigned char ADD = 0x01;
const unsigned char SUB = 0x28;
//const unsigned char OUT = 0xE6;
const unsigned char CMP = 0x38;
const unsigned char JNE = 0x75;
const unsigned char JE = 0x74;

const unsigned int BUF_SIZE = 128;
const unsigned int INT_SIZE = 4;

const unsigned int HEADER_SIZE = 8;


const int EAX = HEADER_SIZE + 0x0;
const int EBX = EAX + INT_SIZE;
const int ECX = EBX + INT_SIZE;
const int FLAGS = ECX + INT_SIZE;
const int OUT_PORT = FLAGS + 1;

const unsigned char CODE_OFFSET = OUT_PORT + INT_SIZE;


class Container {
private:
  void _writeHeader();

  unsigned char* _bytes;
  unsigned int _size;
public:

  int SUB_func(int _pointer);
  int ADD_func(int _pointer);
  int MOV_func(int _pointer);
  int OUT_func(int _pointer);
  int CMP_func(int _pointer);
  int JNE_func(int _pointer);
  int JE_func(int _pointer);

  void saveBytes();
  unsigned char readByte();
  void writeByte(unsigned char ch);
  void execCode();
  void printCode(std::string code, unsigned int op_addr, unsigned int arg1, unsigned int arg2);
  void printCode(std::string code, unsigned int op_addr, unsigned int arg2);

  unsigned int readInt();
  void seek(unsigned int addr);
  int writeCode(unsigned char opcode, unsigned int arg1, unsigned int arg2);
  int writeCode(unsigned char opcode, unsigned int arg1);
  void writeInt(int n);

  static const unsigned char version = 0x01;
  unsigned int code_offset;
  unsigned int pointer;

  void init();
  void dumpState();
  Container(unsigned char* b);
};

#endif

