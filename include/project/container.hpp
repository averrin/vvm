#ifndef CONTAINER_HPP_
#define CONTAINER_HPP_
#include <string>
#include <map>
#include <functional>
#include <cstddef>

const std::byte TERM{ 0x00 };
const std::byte NOP{ 0x90 };
const std::byte MOV{ 0x88 };
const std::byte ADD{ 0x01 };
const std::byte SUB{ 0x28 };
const std::byte OUTPUT{ 0xE6 };
const std::byte CMP{ 0x38 };
const std::byte JNE{ 0x75 };
const std::byte JE{ 0x74 };
const std::byte INTERRUPT{ 0xCC };
const std::byte PUSH{ 0x06 };
const std::byte POP{ 0x07 };

const std::byte INT_END{ 0xFF };
const std::byte INT_TEST{ 0xF0 };
const std::byte INT_PRINT{ 0x21 };

const std::byte STATE_INIT{ 0x00 };
const std::byte STATE_EXEC{ 0x01 };
const std::byte STATE_PAUSE{ 0x02 };
const std::byte STATE_END{ 0xFF };
const std::byte STATE_ERROR{ 0x10 };

const std::byte ZF{ 1 << 0 }; // 0000 0001 
const std::byte OUTF{ 1 << 1 }; // 0000 0010
const std::byte INTF{ 1 << 2 }; // 0000 0100
const std::byte option4{ 1 << 3 }; // 0000 1000
const std::byte option5{ 1 << 4 }; // 0001 0000
const std::byte option6{ 1 << 5 }; // 0010 0000
const std::byte option7{ 1 << 6 }; // 0100 0000
const std::byte option8{ 1 << 7 }; // 1000 0000

const unsigned int BUF_SIZE = 128;
const unsigned int INT_SIZE = 4;

const unsigned int HEADER_SIZE = 8;

const int STATE = HEADER_SIZE + 0x0;
const int STACK_ADDR = STATE + 1;
const int EAX = STACK_ADDR + INT_SIZE;
const int EBX = EAX + INT_SIZE;
const int ECX = EBX + INT_SIZE;
const int FLAGS = ECX + INT_SIZE;
const int INTERRUPTS = FLAGS + 1;
const int OUT_PORT = INTERRUPTS + 1;

const int CODE_OFFSET = OUT_PORT + INT_SIZE;

typedef std::function<void(std::byte*, unsigned int)> t_handler;

class Container {
private:
	void writeHeader();

	t_handler _tickHandler;

	std::byte* _bytes;
	unsigned int _size;
	std::map<const std::byte, t_handler> _intHandlers;
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
	int PUSH_func(int _pointer);
	int POP_func(int _pointer);

	void saveBytes(std::string name);
	std::byte readByte();
	bool checkFlag(const std::byte intf);
	std::byte getState();
	void setState(const std::byte state);
	void setFlag(const std::byte flag, const bool value);
	void checkInterruption();
	void writeByte(std::byte ch);
	void execCode();
	void printCode(std::string code, unsigned int op_addr, unsigned int arg1, unsigned int arg2);
	void printCode(std::string code, unsigned int op_addr, unsigned int arg2);
	void printCode(std::string code, unsigned int op_addr, std::byte arg2);
	void printCode(std::string code, unsigned int op_addr);
	void printJump(std::string code, unsigned int op_addr, unsigned int arg2, bool jumped);

	unsigned int readInt();
	void seek(unsigned int addr);
	int writeCode(std::byte opcode, unsigned int arg1, unsigned int arg2);
	int writeCode(std::byte opcode, const int arg1);
	int writeCode(std::byte opcode, const std::byte arg1);
	int writeCode(const std::byte opcode);
	void writeInt(int n);

	static const std::byte version{ 0x01 };
	unsigned int code_offset;
	unsigned int pointer;

	void init();
	void dumpState();
	Container(std::byte* b, t_handler th);
	void setInterruptHandler(const std::byte interrupt, t_handler handler);
};

#endif

