#ifndef CONST_HPP_
#define CONST_HPP_

const std::byte NOP{ 0x90 };
const std::byte MOV_mm{ 0x88 };
const std::byte MOV_mc{ 0x89 };
const std::byte ADD_mm{ 0x01 };
const std::byte ADD_mc{ 0x02 };
const std::byte SUB_mm{ 0x28 };
const std::byte SUB_mc{ 0x29 };
const std::byte OUTPUT{ 0xE6 };
const std::byte CMP_mm{ 0x38 };
const std::byte CMP_mc{ 0x39 };
const std::byte JNE_a{ 0x75 };
const std::byte JNE_r{ 0x76 };
const std::byte JE{ 0x74 };
const std::byte JMP_a{ 0xE9 };
const std::byte JMP_r{ 0xEB };
const std::byte INTERRUPT{ 0xCC };
const std::byte PUSH_m{ 0x06 };
const std::byte PUSH_c{ 0x0E };
const std::byte POP{ 0x07 };
const std::byte INC{ 0x40 };
const std::byte DEC{ 0x48 };
const std::byte AND_mm{ 0x20 };
const std::byte AND_mc{ 0x21 };
const std::byte OR_mm{ 0x08 };
const std::byte OR_mc{ 0x09 };

const std::byte INT_END{ 0xFF };
const std::byte INT_TEST{ 0xF0 };
const std::byte INT_PRINT{ 0x21 };

const std::byte STATE_INIT{ 0x00 };
const std::byte STATE_EXEC{ 0x01 };
const std::byte STATE_PAUSE{ 0x02 };
const std::byte STATE_END{ 0xFF };
const std::byte STATE_ERROR{ 0x10 };

const std::byte ZF{ 1 << 0 }; // 0000 0001 
const std::byte OUTF{ 1 << 1 }; // 0000 0010
const std::byte INTF{ 1 << 2 }; // 0000 0100

const int BUF_SIZE = 160;
const int INT_SIZE = 4;

const int HEADER_SIZE = 8;
const int OP_long_length = 1 + INT_SIZE * 2;
const int OP_med_length = 1 + INT_SIZE;

const address CO_ADDR = address{0x4}; //TODO: convert offset to int

const address STATE = address{ HEADER_SIZE + 0x0 };
const address ESP = STATE + 1;
const address EAX = ESP + INT_SIZE;
const address EBX = EAX + INT_SIZE;
const address ECX = EBX + INT_SIZE;
const address EIP = ECX + INT_SIZE;
const address FLAGS = EIP + INT_SIZE;
const address INTERRUPTS = FLAGS + 1;
const address OUT_PORT = INTERRUPTS + 1;

const address CODE_OFFSET = OUT_PORT + INT_SIZE;


#endif
