#pragma once

const int BYTE_SIZE = 1;
const int INT_SIZE = 2;
const int ADDRESS_SIZE = BYTE_SIZE + INT_SIZE;
const int OPCODE_SIZE =  BYTE_SIZE;

const int HEADER_SIZE = 8;
const int STACK_SIZE = 128;

const int OP_aa_length = OPCODE_SIZE + ADDRESS_SIZE + ADDRESS_SIZE;
const int OP_ai_length = OPCODE_SIZE + ADDRESS_SIZE + INT_SIZE;
const int OP_aw_length = OPCODE_SIZE + ADDRESS_SIZE + BYTE_SIZE;
const int OP_a_length =  OPCODE_SIZE + ADDRESS_SIZE;
const int OP_i_length =  OPCODE_SIZE + INT_SIZE;
const int OP_w_length =  OPCODE_SIZE + BYTE_SIZE;
const int OP_z_length =  OPCODE_SIZE;
