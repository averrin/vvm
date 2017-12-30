#ifndef CONTAINER_HPP_
#define CONTAINER_HPP_
#include <string>
#include <map>
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
const unsigned char INTERRUPT = 0xCC;

const unsigned char INT_END = 0xFF;
const unsigned char INT_TEST = 0xF0;
const unsigned char INT_PRINT = 0x21;

const unsigned char STATE_INIT = 0x00;
const unsigned char STATE_EXEC = 0x01;
const unsigned char STATE_PAUSE = 0x02;
const unsigned char STATE_END = 0xFF;

const unsigned char ZF = 1 << 0; // 0000 0001 
const unsigned char OUTF = 1 << 1; // 0000 0010
const unsigned char INTF = 1 << 2; // 0000 0100
const unsigned char option4 = 1 << 3; // 0000 1000
const unsigned char option5 = 1 << 4; // 0001 0000
const unsigned char option6 = 1 << 5; // 0010 0000
const unsigned char option7 = 1 << 6; // 0100 0000
const unsigned char option8 = 1 << 7; // 1000 0000

const unsigned int BUF_SIZE = 128;
const unsigned int INT_SIZE = 4;

const unsigned int HEADER_SIZE = 8;

const int STATE = HEADER_SIZE + 0x0;
const int EAX = STATE + 1;
const int EBX = EAX + INT_SIZE;
const int ECX = EBX + INT_SIZE;
const int FLAGS = ECX + INT_SIZE;
const int INTERRUPTS = FLAGS + 1;
const int OUT_PORT = INTERRUPTS + 1;

const unsigned char CODE_OFFSET = OUT_PORT + INT_SIZE;

typedef std::function<void(unsigned char*, unsigned int)> t_handler;

class Container {
private:
	void writeHeader();

	t_handler _tickHandler;

	unsigned char* _bytes;
	unsigned int _size;
	std::map<const unsigned char, t_handler> _intHandlers;
public:

	int SUB_func(int _pointer);
	int ADD_func(int _pointer);
	int MOV_func(int _pointer);
	int OUT_func(int _pointer);
	int CMP_func(int _pointer);
	int JNE_func(int _pointer);
	int JE_func(int _pointer);
	int INT_func(int _pointer);
	int NOP_func(int _pointer);

	void saveBytes(std::string name);
	unsigned char readByte();
	bool checkFlag(const unsigned char intf);
	unsigned char getState();
	void setState(const unsigned char state);
	void setFlag(const unsigned char flag, const bool value);
	void checkInterruption();
	void writeByte(unsigned char ch);
	void execCode();
	void printCode(std::string code, unsigned int op_addr, unsigned int arg1, unsigned int arg2);
	void printCode(std::string code, unsigned int op_addr, unsigned int arg2);
	void printCode(std::string code, unsigned int op_addr);
	void printJump(std::string code, unsigned int op_addr, unsigned int arg2, bool jumped);

	unsigned int readInt();
	void seek(unsigned int addr);
	int writeCode(unsigned char opcode, unsigned int arg1, unsigned int arg2);
	int writeCode(unsigned char opcode, const int arg1);
	int writeCode(unsigned char opcode, const unsigned char arg1);
	int writeCode(const unsigned char opcode);
	void writeInt(int n);

	static const unsigned char version = 0x01;
	unsigned int code_offset;
	unsigned int pointer;

	void init();
	void dumpState();
	Container(unsigned char* b, t_handler th);
	void setInterruptHandler(const unsigned char interrupt, t_handler handler);
};

#endif

