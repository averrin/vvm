#pragma once
#include <string>
#include "vvm/analyzer/code_instruction.hpp"
#include "vvm/core.hpp"

namespace analyzer {
    class Analyzer {

    public:
        Analyzer();
        script disassemble(Core *mem);
        script compile(std::string filename);
    };
}
