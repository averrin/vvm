#ifndef CONST_HPP_
#define CONST_HPP_

#include <string>
#include <cstdlib>
#include <cstddef>
#include <variant>
#include "rang.hpp"
#include "ostream.hpp"
#include "vvm/address.hpp"

const std::byte INT_END{ 0xFF };
const std::byte INT_TEST{ 0xF0 };
const std::byte INT_PRINT{ 0x21 };

const std::byte STATE_INIT{ 0x00 };
const std::byte STATE_EXEC{ 0x01 };
const std::byte STATE_PAUSE{ 0x02 };
const std::byte STATE_END{ 0xFF };
const std::byte STATE_ERROR{ 0x10 };

const std::byte ZF{ 1 << 0 }; // 0000 0001
const std::byte INTF{ 1 << 2 }; // 0000 0100


const std::byte ZERO{ 0x0 };
const std::byte REDIRECT{ 1 << 0 }; // 0000 0001
const std::byte STOREBYTE{ 1 << 1 }; // 0000 0001

const address CO_ADDR = address{0x4}; //TODO: convert offset to int

const address STATE = address{ HEADER_SIZE + 0x0 };
const address ESP = STATE + 1;
const address EAX = ESP + INT_SIZE;
const address EBX = EAX + INT_SIZE;
const address ECX = EBX + INT_SIZE;
const address EIP = ECX + INT_SIZE;
const address EDI = EIP + INT_SIZE;
const address FLAGS = EDI + INT_SIZE;
const address INTERRUPTS = FLAGS + BYTE_SIZE;

const address CODE_OFFSET = INTERRUPTS + INT_SIZE;

const address AL = address{EAX.dst, false, true};
const address BL = address{EBX.dst, false, true};
const address CL = address{ECX.dst, false, true};

#endif
