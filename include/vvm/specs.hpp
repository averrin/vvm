#pragma once

#include "vvm/constants.hpp"
#include <cstddef>

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
