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

		os << rang::fg::green << fmt::format("{:<4}", spec.name) << rang::style::reset
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

const std::byte INT_END{ 0xFF };
const std::byte INT_TEST{ 0xF0 };
const std::byte INT_PRINT{ 0x21 };

const std::byte STATE_INIT{ 0x00 };
const std::byte STATE_EXEC{ 0x01 };
const std::byte STATE_PAUSE{ 0x02 };
const std::byte STATE_END{ 0xFF };
const std::byte STATE_ERROR{ 0x10 };

const std::byte ZF{ 1 << 0 }; // 0000 0001 
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

const address CODE_OFFSET = INTERRUPTS + INT_SIZE;

const address AL = address{EAX.dst, false, true};
const address BL = address{EBX.dst, false, true};
const address CL = address{ECX.dst, false, true};

#endif
