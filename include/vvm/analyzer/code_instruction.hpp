#pragma once
#include <array>
#include <string>
#include <variant>
#include <sstream>
#include "args.hpp"
#include "format.h" 
#include "ostream.hpp"
#include "rang.hpp"

#include "vvm/address.hpp"
#include "vvm/constants.hpp"

namespace analyzer {


    struct code_instruction
    {
        address offset;
        opSpec spec;
        std::vector<std::string> aliases;
        instruction_arg arg1;
        instruction_arg arg2;

        friend std::ostream& operator<<(std::ostream& os, const code_instruction& i)
        {
            os << i.spec;
            switch (i.spec.type)
            {
            case opSpec::MM:
                os << fmt::format("({}, {})", std::get<address>(i.arg1), std::get<address>(i.arg2));
                break;
            case opSpec::MC:
                os << fmt::format("({}, {:08X})", std::get<address>(i.arg1), std::get<unsigned int>(i.arg2));
                break;
            case opSpec::MB:
                os << fmt::format("({}, {:02X})", std::get<address>(i.arg1), static_cast<unsigned int>(std::get<std::byte>(i.arg2)));
                break;
            case opSpec::M:
                os << fmt::format("({})", std::get<address>(i.arg1));
                break;
            case opSpec::C:
                os << fmt::format("({:08X})", std::get<unsigned int>(i.arg1));
                break;
            case opSpec::B:
                os << fmt::format("({:02X})", static_cast<unsigned int>(std::get<std::byte>(i.arg1)));
                break;
            case opSpec::Z:
                os << fmt::format("()");
                break;
            default:;
            }
            os << " at " << rang::fg::yellow <<  i.offset << rang::style::reset;

            auto real_length = 1;
            switch (i.spec.type) {
            case opSpec::MM:
                real_length = OP_max_length;
                break;
            case opSpec::MC:
                real_length = OP_long_length;
                break;
            case opSpec::MB:
                real_length = OP_med_ex_length;
                break;
            case opSpec::M:
                real_length = OP_med_length;
                break;
            case opSpec::C:
                real_length = OP_ex_length;
                break;
            case opSpec::B:
                real_length = OP_short_length;
                break;
            case opSpec::Z:
                break;
            default:;
            }
            os << " (" << rang::fg::yellow <<  real_length << rang::style::reset << ")";
            os << " aka " << rang::fg::yellow << args::detail::Join(i.aliases, ", ") << rang::style::reset;
            return os;
        }
    };

    typedef std::vector<code_instruction> script;
}
