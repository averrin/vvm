#pragma once
#include <array>
#include <string>
#include <variant>
#include "format.h"
#include "ostream.hpp"
#include "rang.hpp"

#include "vvm/address.hpp"
#include "vvm/constants.hpp"


namespace analyzer {
    typedef std::variant<std::byte, unsigned int, address> instruction_arg;

    struct code_instruction
    {
        address offset;
        opSpec spec;
        std::array<std::byte, OP_long_length> mem;
        std::vector<std::string> aliases;
        instruction_arg arg1;
        instruction_arg arg2;

        friend std::ostream& operator<<(std::ostream& os, const code_instruction& i)
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
                << fmt::format("({:02X})", static_cast<unsigned int>(std::get<std::byte>(i.arg1)));
                break;
            case opSpec::Z:
                os << rang::fg::green << i.spec << rang::style::reset
                << fmt::format("()");
                break;
            default:;
            }
            os << " at " << rang::fg::yellow <<  i.offset << rang::style::reset;
            os << " aka " << rang::fg::yellow <<  i.aliases.front() << rang::style::reset;
            return os;
        }
    };

    typedef std::vector<code_instruction> script;
}