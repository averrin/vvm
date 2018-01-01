#ifndef CONTAINER_HPP_
#define CONTAINER_HPP_
#include <string>
#include <map>
#include <functional>
#include <cstddef>
#include "format.h"
#include "project/address.hpp"
#include "project/constants.hpp"

typedef std::function<void(std::byte*, unsigned int)> t_handler;

class Container {
private:
	void writeHeader();

	t_handler _tickHandler;
	std::byte* _bytes;
	unsigned int _size;
	std::map<const std::byte, t_handler> _intHandlers;

	std::byte readByte();
	bool checkFlag(const std::byte intf);
	std::byte getState();
	void setState(const std::byte state);
	void setFlag(const std::byte flag, const bool value);
	void checkInterruption();
	void setReg(const address reg, const address value);
	void setReg(const address reg, const int value);
	address readRegAddress(const address reg);
	int readRegInt(const address reg);
	void writeByte(std::byte ch);
	void printCode(std::string code, const address arg1, unsigned int arg2);
	void printCode(std::string code, const address arg1, const address arg2);
	void printCode(std::string code, const address arg2);
	void printCode(std::string code, std::byte arg2);
	void printCode(const std::string code, const int arg1);
	void printCode(std::string code);
	void printJump(std::string code, const address arg1, bool jumped);
	void printJump(std::string code, const int offset, bool jumped);
	void printIRQ(const std::byte code);

	address readAddress();
	unsigned int readInt();
	int readSignedInt();
	void writeAddress(const address n);
	address writeCode(std::byte opcode, address arg1, unsigned int arg2);
	address writeCode(std::byte opcode, address arg1, address arg2);
	address writeCode(std::byte opcode, address arg1);
	address writeCode(std::byte opcode, const std::byte arg1);
	address writeCode(std::byte opcode, const int arg1);
	address writeCode(const std::byte opcode);
	void writeInt(int n);

	static const std::byte version{ 0x01 };
	address code_offset;

	address SUB_mm_func(address _pointer);
	address SUB_mc_func(address _pointer);
	address ADD_mm_func(address _pointer);
	address ADD_mc_func(address _pointer);
	address MOV_mm_func(address _pointer);
	address MOV_mc_func(address _pointer);
	address OUT_func(address _pointer);
	address CMP_mm_func(address _pointer);
	address CMP_mc_func(address _pointer);
	address JNE_a_func(address _pointer);
	address JNE_r_func(address _pointer);
	address JE_func(address _pointer);
	address INT_func(address _pointer);
	address INC_func(address _pointer);
	address DEC_func(address _pointer);
	address NOP_func(address _pointer);
	address PUSH_m_func(address _pointer);
	address PUSH_c_func(address _pointer);
	address POP_func(address _pointer);
	address JMP_a_func(address _pointer);
	address JMP_r_func(address _pointer);
public:
	Container(std::byte* b, t_handler th);

	address _MOV(const address dst, address src);
	address _MOV(const address dst, int src);
	address _MOV(const address dst, std::byte src_const); //todo

	address _ADD(const address dst, int src);
	address _ADD(const address dst, address src);
	address _SUB(const address dst, int src);
	address _SUB(const address dst, address src);
	address _INC(const address addr);
	address _DEC(const address addr);

	address _JMP(int offset);
	address _JMP(address dst);
	address _JNE(int offset);
	address _JNE(address dst);

	address _PUSH(address src);
	address _PUSH(int src);
	address _POP(address dst);

	address _NOP();
	address _INT(const std::byte code);

	//todo
	address _CMP(const address dst, int src);
	address _CMP(const address dst, address src);

	address _AND(const address dst, int src);
	address _AND(const address dst, address src);
	address _OR(const address dst, int src);
	address _OR(const address dst, address src);

	address _JE(int offset);
	address _JE(address dst);
	// --

	void saveBytes(std::string name);
	void execCode();
	void seek(address addr);
	void init();
	void dumpState();
	void setInterruptHandler(const std::byte interrupt, t_handler handler);

	address pointer;
};

#endif

