#pragma once
#include <string>
#include "vvm/analyzer/code_instruction.hpp"
#include "vvm/core.hpp"

namespace analyzer {
    class Analyzer {

    public:
        Analyzer();
        script disassemble(std::shared_ptr<Core> core);
        script parseFile(std::string filename);
    };
}
