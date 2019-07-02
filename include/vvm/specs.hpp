#pragma once

#include "vvm/constants.hpp"
#include <cstddef>

typedef std::variant<std::byte, unsigned int, address> instruction_arg;

struct arguments {
    std::pair<instruction_arg, instruction_arg> args;
    std::pair<instruction_arg, instruction_arg> orig_args;
    address current_pointer;
};

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
const std::byte MOV_aa{ 0x88 };
const op_spec MOV_aa_spec{ MOV_aa, "MOV", op_spec::OP_TYPE::AA };
const std::byte MOV_ai{ 0x89 };
const op_spec MOV_ai_spec{ MOV_ai, "MOV", op_spec::OP_TYPE::AI };
const std::byte MOV_aw{ 0x8A };
const op_spec MOV_aw_spec{ MOV_aw, "MOV", op_spec::OP_TYPE::AW };
const std::byte ADD_aa{ 0x01 };
const op_spec ADD_aa_spec{ ADD_aa, "ADD", op_spec::OP_TYPE::AA };
const std::byte ADD_ai{ 0x02 };
const op_spec ADD_ai_spec{ ADD_ai, "ADD", op_spec::OP_TYPE::AI };
const std::byte ADD_aw{ 0x03 };
const op_spec ADD_aw_spec{ ADD_aw, "ADD", op_spec::OP_TYPE::AW };
const std::byte SUB_aa{ 0x28 };
const op_spec SUB_aa_spec{ SUB_aa, "SUB", op_spec::OP_TYPE::AA };
const std::byte SUB_ai{ 0x29 };
const op_spec SUB_ai_spec{ SUB_ai, "SUB", op_spec::OP_TYPE::AI };
const std::byte SUB_aw{ 0x2A };
const op_spec SUB_aw_spec{ SUB_aw, "SUB", op_spec::OP_TYPE::AW };
const std::byte CMP_aa{ 0x38 };
const op_spec CMP_aa_spec{ CMP_aa, "CMP", op_spec::OP_TYPE::AA };
const std::byte CMP_ai{ 0x39 };
const op_spec CMP_ai_spec{ CMP_ai, "CMP", op_spec::OP_TYPE::AI };
const std::byte CMP_aw{ 0x3A };
const op_spec CMP_aw_spec{ CMP_aw, "CMP", op_spec::OP_TYPE::AW };
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
const std::byte PUSH_a{ 0x06 }; //broken, i think
const op_spec PUSH_a_spec{ PUSH_a, "PUSH", op_spec::OP_TYPE::A };
const std::byte PUSH_i{ 0x0E };
const op_spec PUSH_i_spec{ PUSH_i, "PUSH", op_spec::OP_TYPE::I };
const std::byte PUSH_w{ 0x0F };
const op_spec PUSH_w_spec{ PUSH_w, "PUSH", op_spec::OP_TYPE::W };
const std::byte POP{ 0x07 };
const op_spec POP_spec{ POP, "POP", op_spec::OP_TYPE::A };
const std::byte INC{ 0x40 };
const op_spec INC_spec{ INC, "INC", op_spec::OP_TYPE::A };
const std::byte DEC{ 0x48 };
const op_spec DEC_spec{ DEC, "DEC", op_spec::OP_TYPE::A };
const std::byte MEM{ 0xfa };
const op_spec MEM_spec{ MEM, "MEM", op_spec::OP_TYPE::AW };

//TODO
const std::byte AND_aa{ 0x20 };
const std::byte AND_ai{ 0x21 };
const std::byte AND_aw{ 0x22 };
const std::byte OR_aa{ 0x08 };
const std::byte OR_ai{ 0x09 };
const std::byte OR_aw{ 0x0a };
const std::byte XOR_aa{ 0x30 };
const std::byte XOR_ai{ 0x31 };
const std::byte XOR_aw{ 0x32 };

const std::byte JG{ 0x7F };
const std::byte JNG{ 0x7E };
const std::byte JL{ 0x7C };
const std::byte JNL{ 0x7D };
const std::byte JGE{ 0x7D };
const std::byte JNGE{ 0x7C };
const std::byte JLE{ 0x7E };
const std::byte JNLE{ 0x7F };

const std::byte DIV_aa{ 0xF6 };
const std::byte DIV_ai{ 0xF7 };
const std::byte DIV_aw{ 0xF8 };
const std::byte IMUL_aa{ 0x69 };
const std::byte IMUL_ai{ 0x6A };
const std::byte IMUL_aw{ 0x6B };
//
