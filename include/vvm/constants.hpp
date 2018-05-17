#ifndef CONST_HPP_
#define CONST_HPP_

#include <string>
#include <cstdlib>
#include <cstddef>
#include <variant>
#include "rang.hpp"
#include "ostream.hpp"
#include "vvm/address.hpp"

const int INT_SIZE = 2;
const int BYTE_SIZE = 1;
const int ADDRESS_SIZE = BYTE_SIZE + INT_SIZE;
const int OPCODE_SIZE = BYTE_SIZE;

const int HEADER_SIZE = 8;
const int STACK_SIZE = 128;

typedef std::variant<std::byte, unsigned int, address> instruction_arg;

struct arguments {
    std::pair<instruction_arg, instruction_arg> args;
    std::pair<instruction_arg, instruction_arg> orig_args;
    address current_pointer;
};

struct opSpec
{
	enum OP_TYPE {
		MM,
		MC,
		MB,
		M,
		C,
		B,
		Z,
	};

	const std::byte opcode;
	std::string name;
	OP_TYPE type;
    bool jump = false;

	friend std::ostream& operator<<(std::ostream& os, const opSpec& spec)
	{
        std::string spec_type = "x";
        switch (spec.type) {
            case OP_TYPE::MM:
                spec_type = "mm";
                break;
            case OP_TYPE::MC:
                spec_type = "mc";
                break;
            case OP_TYPE::MB:
                spec_type = "mb";
                break;
            case OP_TYPE::M:
                spec_type = "m_";
                break;
            case OP_TYPE::C:
                spec_type = "c_";
                break;
            case OP_TYPE::B:
                spec_type = "b_";
                break;
            case OP_TYPE::Z:
                spec_type = "__";
                break;
        }

		os << rang::fg::green << spec.name << rang::style::reset
           << "|" << rang::fg::black << rang::style::bold << fmt::format("{:02X}", static_cast<unsigned int>(spec.opcode))  << rang::style::reset
           << " [" << rang::fg::blue << spec_type << rang::style::reset << "]";
		return os;
	}
	friend bool operator==(const opSpec& lhs, const opSpec&  rhs)
	{
		return lhs.opcode == rhs.opcode;
	}
};

const int OP_max_length =    OPCODE_SIZE + ADDRESS_SIZE + ADDRESS_SIZE;
const int OP_long_length =   OPCODE_SIZE + ADDRESS_SIZE + INT_SIZE;
const int OP_med_ex_length = OPCODE_SIZE + ADDRESS_SIZE + BYTE_SIZE;
const int OP_med_length =    OPCODE_SIZE + ADDRESS_SIZE;
const int OP_ex_length =     OPCODE_SIZE + INT_SIZE;
const int OP_short_length =  OPCODE_SIZE + BYTE_SIZE;
const int OP_zero_length =   OPCODE_SIZE;

const std::byte INVALID{ 0x00 };
const opSpec INVALID_spec{ INVALID, "INVALID", opSpec::OP_TYPE::Z };
const std::byte NOP{ 0x90 };
const opSpec NOP_spec{ NOP, "NOP", opSpec::OP_TYPE::Z };
const std::byte MOV_mm{ 0x88 };
const opSpec MOV_mm_spec{ MOV_mm, "MOV", opSpec::OP_TYPE::MM };
const std::byte MOV_mc{ 0x89 };
const opSpec MOV_mc_spec{ MOV_mc, "MOV", opSpec::OP_TYPE::MC };
const std::byte MOV_mb{ 0x8A };
const opSpec MOV_mb_spec{ MOV_mb, "MOV", opSpec::OP_TYPE::MB };
const std::byte ADD_mm{ 0x01 };
const opSpec ADD_mm_spec{ ADD_mm, "ADD", opSpec::OP_TYPE::MM };
const std::byte ADD_mc{ 0x02 };
const opSpec ADD_mc_spec{ ADD_mc, "ADD", opSpec::OP_TYPE::MC };
const std::byte ADD_mb{ 0x03 };
const opSpec ADD_mb_spec{ ADD_mb, "ADD", opSpec::OP_TYPE::MB };
const std::byte SUB_mm{ 0x28 };
const opSpec SUB_mm_spec{ SUB_mm, "SUB", opSpec::OP_TYPE::MM };
const std::byte SUB_mc{ 0x29 };
const opSpec SUB_mc_spec{ SUB_mc, "SUB", opSpec::OP_TYPE::MC };
const std::byte SUB_mb{ 0x2A };
const opSpec SUB_mb_spec{ SUB_mb, "SUB", opSpec::OP_TYPE::MB };
const std::byte OUTPUT{ 0xE6 };
const opSpec OUT_spec{ OUTPUT, "OUT", opSpec::OP_TYPE::M };
const std::byte CMP_mm{ 0x38 };
const opSpec CMP_mm_spec{ CMP_mm, "CMP", opSpec::OP_TYPE::MM };
const std::byte CMP_mc{ 0x39 };
const opSpec CMP_mc_spec{ CMP_mc, "CMP", opSpec::OP_TYPE::MC };
const std::byte CMP_mb{ 0x3A };
const opSpec CMP_mb_spec{ CMP_mb, "CMP", opSpec::OP_TYPE::MB };
const std::byte JNE_a{ 0x75 };
const opSpec JNE_a_spec{ JNE_a, "JNE", opSpec::OP_TYPE::M, true };
const std::byte JNE_r{ 0x76 };
const opSpec JNE_r_spec{ JNE_r, "JNE", opSpec::OP_TYPE::C, true };
const std::byte JE{ 0x74 };
const opSpec JE_spec{ JE, "JE", opSpec::OP_TYPE::M, true };
const std::byte JMP_a{ 0xE9 };
const opSpec JMP_a_spec{ JMP_a, "JMP", opSpec::OP_TYPE::M, true };
const std::byte JMP_r{ 0xEB };
const opSpec JMP_r_spec{ JMP_r, "JMP", opSpec::OP_TYPE::C, true };
const std::byte INTERRUPT{ 0xCC };
const opSpec INT_spec{ INTERRUPT, "INT", opSpec::OP_TYPE::B };
const std::byte PUSH_m{ 0x06 };
const opSpec PUSH_m_spec{ PUSH_m, "PUSH", opSpec::OP_TYPE::M };
const std::byte PUSH_c{ 0x0E };
const opSpec PUSH_c_spec{ PUSH_c, "PUSH", opSpec::OP_TYPE::C };
const std::byte POP{ 0x07 };
const opSpec POP_spec{ POP, "POP", opSpec::OP_TYPE::M };
const std::byte INC{ 0x40 };
const opSpec INC_spec{ INC, "INC", opSpec::OP_TYPE::M };
const std::byte DEC{ 0x48 };
const opSpec DEC_spec{ DEC, "DEC", opSpec::OP_TYPE::M };
const std::byte MEM{ 0xfa };
const opSpec MEM_spec{ MEM, "MEM", opSpec::OP_TYPE::MB };
const std::byte AND_mm{ 0x20 };
const std::byte AND_mc{ 0x21 };
const std::byte OR_mm{ 0x08 };
const std::byte OR_mc{ 0x09 };

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


const std::byte ZERO{ 0x0 };
const std::byte REDIRECT{ 1 << 0 }; // 0000 0001 
const std::byte STOREBYTE{ 1 << 1 }; // 0000 0001 

const address CO_ADDR = address{0x4}; //TODO: convert offset to int

const address STATE = address{ HEADER_SIZE + 0x0 };
const address ESP = STATE + 1;
const address EAX = ESP + INT_SIZE;
const address EBX = EAX + INT_SIZE;
const address ECX = EBX + INT_SIZE;
const address EIP = ECX + INT_SIZE;
const address EDI = EIP + INT_SIZE;
const address FLAGS = EDI + INT_SIZE;
const address INTERRUPTS = FLAGS + BYTE_SIZE;
const address OUT_PORT = INTERRUPTS + BYTE_SIZE;

const address CODE_OFFSET = OUT_PORT + INT_SIZE;

const address AX = address{EAX.dst, false, true};
const address BX = address{EBX.dst, false, true};
const address CX = address{ECX.dst, false, true};

#endif
