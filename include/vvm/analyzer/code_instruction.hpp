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
#include "vvm/specs.hpp"

namespace analyzer {


    struct code_instruction
    {
        address offset;
        op_spec spec;
        std::vector<std::string> aliases;
        instruction_arg arg1;
        instruction_arg arg2;

        friend std::ostream& operator<<(std::ostream& os, const code_instruction& i)
        {
            os << i.spec;
            switch (i.spec.type)
            {
            case op_spec::AA:
                os << fmt::format("({}, {})", std::get<address>(i.arg1), std::get<address>(i.arg2));
                break;
            case op_spec::AI:
                os << fmt::format("({}, {:0{}X})", std::get<address>(i.arg1), std::get<unsigned int>(i.arg2), INT_SIZE*2);
                break;
            case op_spec::AW:
                os << fmt::format("({},    {:02X} )", std::get<address>(i.arg1), static_cast<unsigned int>(std::get<std::byte>(i.arg2)));
                break;
            case op_spec::A:
                os << fmt::format("({}        )", std::get<address>(i.arg1));
                break;
            case op_spec::I:
                os << fmt::format(" ({:0{}X})", std::get<unsigned int>(i.arg1), INT_SIZE*2);
                break;
            case op_spec::W:
                os << fmt::format("(   {:02X}         )", static_cast<unsigned int>(std::get<std::byte>(i.arg1)));
                break;
            case op_spec::Z:
                os << fmt::format("()");
                break;
            default:;
            }
            os << " at " << rang::fg::yellow <<  i.offset << rang::style::reset;

            auto real_length = 1;
            switch (i.spec.type) {
            case op_spec::AA:
                real_length = OP_aa_length;
                break;
            case op_spec::AI:
                real_length = OP_ai_length;
                break;
            case op_spec::AW:
                real_length = OP_aw_length;
                break;
            case op_spec::A:
                real_length = OP_a_length;
                break;
            case op_spec::I:
                real_length = OP_i_length;
                break;
            case op_spec::W:
                real_length = OP_w_length;
                break;
            case op_spec::Z:
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
