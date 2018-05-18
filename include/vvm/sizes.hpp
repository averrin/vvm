#pragma once

const int INT_SIZE = 2;
const int BYTE_SIZE = 1;
const int ADDRESS_SIZE = BYTE_SIZE + INT_SIZE;
const int OPCODE_SIZE = BYTE_SIZE;

const int HEADER_SIZE = 8;
const int STACK_SIZE = 128;

const int OP_max_length =    OPCODE_SIZE + ADDRESS_SIZE + ADDRESS_SIZE;
const int OP_long_length =   OPCODE_SIZE + ADDRESS_SIZE + INT_SIZE;
const int OP_med_ex_length = OPCODE_SIZE + ADDRESS_SIZE + BYTE_SIZE;
const int OP_med_length =    OPCODE_SIZE + ADDRESS_SIZE;
const int OP_ex_length =     OPCODE_SIZE + INT_SIZE;
const int OP_short_length =  OPCODE_SIZE + BYTE_SIZE;
const int OP_zero_length =   OPCODE_SIZE;
