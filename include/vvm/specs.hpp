#pragma once

#include "vvm/constants.hpp"
#include <cstddef>

typedef std::variant<std::byte, unsigned int, address> instruction_arg;

struct arguments {
    std::pair<instruction_arg, instruction_arg> args;
    std::pair<instruction_arg, instruction_arg> orig_args;
    address current_pointer;
};


//TODO: rename M->A, C->I, B->W
struct op_spec
{
	enum OP_TYPE {
		AA,
		AI,
		AW,
		A,
		I,
		W,
		Z,
	};

	const std::byte opcode;
	std::string name;
	OP_TYPE type;
    bool jump = false;

	friend std::ostream& operator<<(std::ostream& os, const op_spec& spec)
	{
        std::string spec_type = "x";
        switch (spec.type) {
            case OP_TYPE::AA:
                spec_type = "aa";
                break;
            case OP_TYPE::AI:
                spec_type = "ai";
                break;
            case OP_TYPE::AW:
                spec_type = "aw";
                break;
            case OP_TYPE::A:
                spec_type = "a_";
                break;
            case OP_TYPE::I:
                spec_type = "i_";
                break;
            case OP_TYPE::W:
                spec_type = "w_";
                break;
            case OP_TYPE::Z:
                spec_type = "__";
                break;
        }

		os << rang::fg::green << fmt::format("{:<4}", spec.name) << rang::style::reset
           << "|" << rang::fg::black << rang::style::bold << fmt::format("{:02X}", static_cast<unsigned int>(spec.opcode))  << rang::style::reset
           << " [" << rang::fg::blue << spec_type << rang::style::reset << "]";
		return os;
	}
	friend bool operator==(const op_spec& lhs, const op_spec&  rhs)
	{
		return lhs.opcode == rhs.opcode;
	}
};

const std::byte INVALID{ 0x00 };
const op_spec INVALID_spec{ INVALID, "INVALID", op_spec::OP_TYPE::Z };
const std::byte NOP{ 0x90 };
const op_spec NOP_spec{ NOP, "NOP", op_spec::OP_TYPE::Z };
const std::byte MOV_mm{ 0x88 };
const op_spec MOV_mm_spec{ MOV_mm, "MOV", op_spec::OP_TYPE::AA };
const std::byte MOV_mc{ 0x89 };
const op_spec MOV_mc_spec{ MOV_mc, "MOV", op_spec::OP_TYPE::AI };
const std::byte MOV_mb{ 0x8A };
const op_spec MOV_mb_spec{ MOV_mb, "MOV", op_spec::OP_TYPE::AW };
const std::byte ADD_mm{ 0x01 };
const op_spec ADD_mm_spec{ ADD_mm, "ADD", op_spec::OP_TYPE::AA };
const std::byte ADD_mc{ 0x02 };
const op_spec ADD_mc_spec{ ADD_mc, "ADD", op_spec::OP_TYPE::AI };
const std::byte ADD_mb{ 0x03 };
const op_spec ADD_mb_spec{ ADD_mb, "ADD", op_spec::OP_TYPE::AW };
const std::byte SUB_mm{ 0x28 };
const op_spec SUB_mm_spec{ SUB_mm, "SUB", op_spec::OP_TYPE::AA };
const std::byte SUB_mc{ 0x29 };
const op_spec SUB_mc_spec{ SUB_mc, "SUB", op_spec::OP_TYPE::AI };
const std::byte SUB_mb{ 0x2A };
const op_spec SUB_mb_spec{ SUB_mb, "SUB", op_spec::OP_TYPE::AW };
const std::byte CMP_mm{ 0x38 };
const op_spec CMP_mm_spec{ CMP_mm, "CMP", op_spec::OP_TYPE::AA };
const std::byte CMP_mc{ 0x39 };
const op_spec CMP_mc_spec{ CMP_mc, "CMP", op_spec::OP_TYPE::AI };
const std::byte CMP_mb{ 0x3A };
const op_spec CMP_mb_spec{ CMP_mb, "CMP", op_spec::OP_TYPE::AW };
const std::byte JNE_a{ 0x75 };
const op_spec JNE_a_spec{ JNE_a, "JNE", op_spec::OP_TYPE::A, true };
const std::byte JNE_r{ 0x76 };
const op_spec JNE_r_spec{ JNE_r, "JNE", op_spec::OP_TYPE::I, true };
const std::byte JE{ 0x74 };
const op_spec JE_spec{ JE, "JE", op_spec::OP_TYPE::A, true };
const std::byte JMP_a{ 0xE9 };
const op_spec JMP_a_spec{ JMP_a, "JMP", op_spec::OP_TYPE::A, true };
const std::byte JMP_r{ 0xEB };
const op_spec JMP_r_spec{ JMP_r, "JMP", op_spec::OP_TYPE::I, true };
const std::byte INTERRUPT{ 0xCC };
const op_spec INT_spec{ INTERRUPT, "INT", op_spec::OP_TYPE::W };
const std::byte PUSH_m{ 0x06 };
const op_spec PUSH_m_spec{ PUSH_m, "PUSH", op_spec::OP_TYPE::A };
const std::byte PUSH_c{ 0x0E };
const op_spec PUSH_c_spec{ PUSH_c, "PUSH", op_spec::OP_TYPE::I };
const std::byte POP{ 0x07 };
const op_spec POP_spec{ POP, "POP", op_spec::OP_TYPE::A };
const std::byte INC{ 0x40 };
const op_spec INC_spec{ INC, "INC", op_spec::OP_TYPE::A };
const std::byte DEC{ 0x48 };
const op_spec DEC_spec{ DEC, "DEC", op_spec::OP_TYPE::A };
const std::byte MEM{ 0xfa };
const op_spec MEM_spec{ MEM, "MEM", op_spec::OP_TYPE::AW };

//TODO
const std::byte AND_mm{ 0x20 };
const std::byte AND_mc{ 0x21 };
const std::byte AND_mb{ 0x22 };
const std::byte OR_mm{ 0x08 };
const std::byte OR_mc{ 0x09 };
const std::byte OR_mb{ 0x0a };
const std::byte XOR_mm{ 0x30 };
const std::byte XOR_mc{ 0x31 };
const std::byte XOR_mb{ 0x32 };

const std::byte JG{ 0x7F };
const std::byte JNG{ 0x7E };
const std::byte JL{ 0x7C };
const std::byte JNL{ 0x7D };
const std::byte JGE{ 0x7D };
const std::byte JNGE{ 0x7C };
const std::byte JLE{ 0x7E };
const std::byte JNLE{ 0x7F };

const std::byte DIV_mm{ 0xF6 };
const std::byte DIV_mc{ 0xF7 };
const std::byte DIV_mb{ 0xF8 };
const std::byte IMUL_mm{ 0x69 };
const std::byte IMUL_mc{ 0x6A };
const std::byte IMUL_mb{ 0x6B };
//
