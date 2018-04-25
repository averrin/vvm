#ifndef CONTAINER_HPP_
#define CONTAINER_HPP_
#include <array>
#include <string>
#include <variant>
#include <map>
#include <functional>
#include <cstddef>
#include "format.h"
#include "vvm/address.hpp"
#include "vvm/constants.hpp"
#include "ostream.hpp"
#include "rang.hpp"

typedef std::vector<std::byte> vm_mem;
typedef std::function<void(vm_mem, unsigned int)> t_handler;
typedef std::variant<std::byte, unsigned int, address> op_arg;


struct instruction
{
	address offset;
	opSpec spec;
	std::array<std::byte, OP_long_length> mem;
    std::vector<std::string> aliases;
    op_arg arg1;
    op_arg arg2;

	friend std::ostream& operator<<(std::ostream& os, const instruction& i)
	{
        switch (i.spec.type)
        {
        case opSpec::MM:
            os << rang::fg::green << i.spec << rang::style::reset
               << fmt::format("({}, {})", std::get<address>(i.arg1), std::get<address>(i.arg2));
            break;
        case opSpec::MC:
            os << rang::fg::green << i.spec << rang::style::reset
               << fmt::format("({}, {:08X})", std::get<address>(i.arg1), std::get<unsigned int>(i.arg2));
            break;
        case opSpec::M:
            os << rang::fg::green << i.spec << rang::style::reset
               << fmt::format("({})", std::get<address>(i.arg1));
            break;
        case opSpec::C:
            os << rang::fg::green << i.spec << rang::style::reset
               << fmt::format("({:08X})", std::get<unsigned int>(i.arg1));
            break;
        case opSpec::B:
            os << rang::fg::green << i.spec << rang::style::reset
               << fmt::format("({:02X})", static_cast<char>(std::get<std::byte>(i.arg1)));
            break;
        case opSpec::Z:
            os << rang::fg::green << i.spec << rang::style::reset
               << fmt::format("()");
            break;
        default:;
        }
        os << " at " << rang::fg::yellow <<  i.offset << rang::style::reset;
		return os;
	}
};


class Container {
private:
	void writeHeader();

	t_handler _tickHandler;
	unsigned int _size;
	std::map<const std::byte, t_handler> _intHandlers;

	std::byte readByte();
	void checkInterruption();
	void writeByte(std::byte ch);
	void printCode(const std::string_view code, const address arg1, unsigned int arg2);
	void printCode(const std::string_view code, const address arg1);
	void printCode(const std::string_view code, const address arg1, const address arg2);
	// void printCode(const std::string_view code, const address arg2);
	void printCode(const std::string_view code, std::byte arg2);
	void printCode(const std::string_view code, const int arg1);
	void printCode(const std::string_view code);
	void printJump(const std::string_view code, const address arg1, bool jumped);
	void printJump(const std::string_view code, const int offset, bool jumped);
	void printIRQ(const std::byte code);

	address readAddress();
	unsigned int readInt();
	int readSignedInt();
	void writeAddress(const address n);
	address writeCode(const std::byte opcode, address arg1, unsigned int arg2);
	address writeCode(const std::byte opcode, address arg1, address arg2);
	address writeCode(const std::byte opcode, address arg1);
	address writeCode(const std::byte opcode, const std::byte arg1);
	address writeCode(const std::byte opcode, const int arg1);
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
	Container(vm_mem b, t_handler th);
	opSpec::OP_TYPE current_spec_type;
	opSpec::OP_TYPE next_spec_type;

	vm_mem _bytes;
	address readRegAddress(const address reg);
	int readRegInt(const address reg);
	address execStart();

	std::byte getState();
	void setState(const std::byte state);
	bool checkFlag(const std::byte intf);
	void setFlag(const std::byte flag, const bool value);
	 
	void setReg(const address reg, const address value);
	void setReg(const address reg, const int value);


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

	address _CMP(const address dst, int src);
	address _CMP(const address dst, address src);

	address _NOP();
	address _INT(const std::byte code);

	//todo
	address _AND(const address dst, int src);
	address _AND(const address dst, address src);
	address _OR(const address dst, int src);
	address _OR(const address dst, address src);

	address _JE(int offset);
	address _JE(address dst);
	// --

	void execCode();
	void execCode(address local_pointer);
	address execStep(address local_pointer);
	std::vector<instruction> disassemble();
    std::vector<instruction> compile(std::string filename);
	void seek(address addr);
	void init(unsigned int size);
	void dumpState();
	void setInterruptHandler(const std::byte interrupt, t_handler handler);
	void saveBytes(const std::string_view name);

	address pointer;
};

#endif

