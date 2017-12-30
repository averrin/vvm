#ifndef CONTAINER_HPP_
#define CONTAINER_HPP_
#include <string>
#include <functional>


const unsigned char TERM = 0x00;
const unsigned char NOP = 0x90;
const unsigned char MOV = 0x88;
const unsigned char ADD = 0x01;
const unsigned char SUB = 0x28;
const unsigned char OUTPUT = 0xE6;
const unsigned char CMP = 0x38;
const unsigned char JNE = 0x75;
const unsigned char JE = 0x74;

const unsigned char ZF   = 1 << 0; // 0000 0001 
const unsigned char OUTF = 1 << 1; // 0000 0010
const unsigned char option3 = 1 << 2; // 0000 0100
const unsigned char option4 = 1 << 3; // 0000 1000
const unsigned char option5 = 1 << 4; // 0001 0000
const unsigned char option6 = 1 << 5; // 0010 0000
const unsigned char option7 = 1 << 6; // 0100 0000
const unsigned char option8 = 1 << 7; // 1000 0000

const unsigned int BUF_SIZE = 128;
const unsigned int INT_SIZE = 4;

const unsigned int HEADER_SIZE = 8;


const int EAX = HEADER_SIZE + 0x0;
const int EBX = EAX + INT_SIZE;
const int ECX = EBX + INT_SIZE;
const int FLAGS = ECX + INT_SIZE;
const int OUT_PORT = FLAGS + 1;

const unsigned char CODE_OFFSET = OUT_PORT + INT_SIZE;

typedef std::function<void(unsigned char*, unsigned int)> t_handler;

class Container {
private:
  void _writeHeader();
  
  t_handler _tickHandler;

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

  void saveBytes(std::string name);
  unsigned char readByte();
  void writeByte(unsigned char ch);
  void execCode();
  void printCode(std::string code, unsigned int op_addr, unsigned int arg1, unsigned int arg2);
  void printCode(std::string code, unsigned int op_addr, unsigned int arg2);
  void printJump(std::string code, unsigned int op_addr, unsigned int arg2, bool jumped);

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
  Container(unsigned char* b, t_handler th);
};

#endif

