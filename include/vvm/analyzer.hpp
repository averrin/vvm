#pragma once
#include <string>
#include "vvm/analyzer/code_instruction.hpp"
#include "vvm/container.hpp"

namespace analyzer {
    class Analyzer {

    public:
        Analyzer();
        script disassemble(Container *mem);
        script compile(std::string filename);
    };
}
