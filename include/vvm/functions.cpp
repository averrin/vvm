#include "vvm/core.hpp"
#include "vvm/constants.hpp"

arguments Core::readArgs(address _pointer,
    op_spec::OP_TYPE op_type, bool reread_first, bool reread_second) {
    instruction_arg arg1;
    instruction_arg arg2;
    instruction_arg orig_arg1;
    instruction_arg orig_arg2;
    switch (op_type) {
        case op_spec::AI:
            arg1 = readAddress();
            _pointer += ADDRESS_SIZE;
            arg2 = readInt();
            _pointer += INT_SIZE;
            if (std::get<address>(arg1).redirect) {
                seek(arg1);
                arg1 = address{readInt()};
            }
            break;
        case op_spec::AA:
            arg1 = readAddress();
            _pointer += ADDRESS_SIZE;
            arg2 = readAddress();
            _pointer += ADDRESS_SIZE;
            if (std::get<address>(arg1).redirect) {
                seek(arg1);
                arg1 = address{readInt()};
            }
            if (std::get<address>(arg2).redirect) {
                seek(arg2);
                arg2 = address{readInt()};
            }
            break;
        case op_spec::AW:
            arg1 = readAddress();
            _pointer += ADDRESS_SIZE;
            arg2 = readByte();
            _pointer += BYTE_SIZE;
            if (std::get<address>(arg1).redirect) {
                seek(arg1);
                arg1 = address{readInt()};
            }
            break;
        case op_spec::A:
            arg1 = readAddress();
            _pointer += ADDRESS_SIZE;
            if (std::get<address>(arg1).redirect) {
                seek(arg1);
                arg1 = address{readInt()};
            }
            break;
        case op_spec::I:
            arg1 = readInt();
            _pointer += INT_SIZE;
            break;
        case op_spec::W:
            arg1 = readByte();
            _pointer += BYTE_SIZE;
            break;
        case op_spec::Z: break;
    }

    if (reread_first) {
        orig_arg1 = arg1;
        seek(arg1);
        if (std::get<address>(arg1).storeByte) {
            seek(std::get<address>(arg1) + INT_SIZE - BYTE_SIZE);
            arg1 = readByte();
        } else {
            arg1 = readInt();
        }
    }
    if (reread_second) {
        orig_arg2 = arg2;
        seek(arg2);
        if (std::get<address>(arg2).storeByte) {
            seek(std::get<address>(arg2) + INT_SIZE - BYTE_SIZE);
            arg2 = readByte();
        } else {
            arg2 = readInt();
        }
    }

    return arguments{{arg1, arg2}, {orig_arg1, orig_arg2}, _pointer};
}

address Core::MOV_ai_func(address _pointer) {
  auto args = readArgs(_pointer, op_spec::AI);
  auto [dst, src] = args.args;
  _pointer = args.current_pointer;
  seek(dst);
  writeInt(src);
  seek(_pointer);
  printCode("MOV", std::get<address>(dst), std::get<unsigned int>(src));
  return _pointer;
}

address Core::MOV_aw_func(address _pointer) {
  auto args = readArgs(_pointer, op_spec::AW);
  auto [dst, src] = args.args;
  _pointer = args.current_pointer;
  seek(dst);
  if (std::get<address>(dst).dst < CODE_OFFSET.dst) {
    writeInt(static_cast<unsigned int>(std::get<std::byte>(src)));
  } else {
    writeByte(src);
  }
  seek(_pointer);
  printCode("MOV", std::get<address>(dst), std::get<std::byte>(src));
  return _pointer;
}

address Core::MOV_aa_func(address _pointer) {
  auto args = readArgs(_pointer, op_spec::AA, false, true);
  auto [dst, src] = args.args;
  _pointer = args.current_pointer;
  seek(dst);
  if (std::get<address>(args.orig_args.second).storeByte) {
    writeByte(src);
  } else {
    writeInt(src);
  }
  seek(_pointer);
  if (std::get<address>(args.orig_args.second).storeByte) {
    printCode("MOV", std::get<address>(dst), std::get<std::byte>(src));
  } else {
    printCode("MOV", std::get<address>(dst), std::get<unsigned int>(src));
  }
  return _pointer;
}

address Core::INT_func(address _pointer) {
  auto args = readArgs(_pointer, op_spec::W);
  auto [src, _] = args.args;
  _pointer = args.current_pointer;
  seek(INTERRUPTS);
  writeByte(src);
  setFlag(INTF, true);
  seek(_pointer);
  printCode("INT", std::get<std::byte>(src));
  return _pointer;
}

address Core::INC_func(address _pointer) {
  auto args = readArgs(_pointer, op_spec::A);
  auto [dst, _] = args.args;
  _pointer = args.current_pointer;

  seek(dst);
  auto value = readInt();
  value += 1;
  seek(dst);
  writeInt(value);
  setFlag(ZF, value == 0);
  seek(_pointer);

  printCode("INC", std::get<address>(dst));
  return _pointer;
}

address Core::DEC_func(address _pointer) {
  auto args = readArgs(_pointer, op_spec::A);
  auto [dst, _] = args.args;
  _pointer = args.current_pointer;

  seek(dst);
  auto value = readInt();
  value -= 1;
  seek(dst);
  writeInt(value);
  setFlag(ZF, value == 0);
  seek(_pointer);

  printCode("DEC", std::get<address>(dst));
  return _pointer;
}

address Core::ADD_ai_func(address _pointer) {
  auto args = readArgs(_pointer, op_spec::AI);
  auto [dst, src] = args.args;
  _pointer = args.current_pointer;

  seek(dst);
  auto value = readInt();
  value += std::get<unsigned int>(src);
  seek(dst);
  writeInt(value);
  setFlag(ZF, value == 0);
  seek(_pointer);

  printCode("ADD", std::get<address>(dst), std::get<unsigned int>(src));
  return _pointer;
}

address Core::ADD_aw_func(address _pointer) {
  auto args = readArgs(_pointer, op_spec::AW);
  auto [dst, src] = args.args;
  _pointer = args.current_pointer;

  seek(dst);
  auto value = readInt();
  value += static_cast<unsigned int>(std::get<std::byte>(src));
  seek(dst);
  writeInt(value);
  setFlag(ZF, value == 0);
  seek(_pointer);

  printCode("ADD", std::get<address>(dst), std::get<std::byte>(src));
  return _pointer;
}

address Core::ADD_aa_func(address _pointer) {
  auto args = readArgs(_pointer, op_spec::AA, false, true);
  auto [dst, src] = args.args;
  _pointer = args.current_pointer;

  seek(dst);
  auto value = readInt();
  value += std::get<unsigned int>(src);
  seek(dst);
  writeInt(value);
  setFlag(ZF, value == 0);
  seek(_pointer);

  printCode("ADD", std::get<address>(dst), std::get<address>(args.orig_args.second));
  return _pointer;
}

address Core::SUB_ai_func(address _pointer) {
  auto args = readArgs(_pointer, op_spec::AW);
  auto [dst, src] = args.args;
  _pointer = args.current_pointer;

  seek(dst);
  auto value = readInt();
  value -= std::get<unsigned int>(src);
  seek(dst);
  writeInt(value);
  setFlag(ZF, value == 0);
  seek(_pointer);

  printCode("SUB", std::get<address>(dst), std::get<unsigned int>(src));
  return _pointer;
}

address Core::SUB_aw_func(address _pointer) {
  auto args = readArgs(_pointer, op_spec::AW);
  auto [dst, src] = args.args;
  _pointer = args.current_pointer;

  seek(dst);
  auto value = readInt();
  value -= static_cast<unsigned int>(std::get<std::byte>(src));
  seek(dst);
  writeInt(value);
  setFlag(ZF, value == 0);
  seek(_pointer);

  printCode("SUB", std::get<address>(dst), std::get<std::byte>(src));
  return _pointer;
}

address Core::SUB_aa_func(address _pointer) {
  auto args = readArgs(_pointer, op_spec::AA, false, true);
  auto [dst, src] = args.args;
  _pointer = args.current_pointer;

  seek(dst);
  auto value = readInt();
  value -= std::get<unsigned int>(src);
  seek(dst);
  writeInt(value);
  setFlag(ZF, value == 0);
  seek(_pointer);

  printCode("SUB", std::get<address>(dst), std::get<address>(args.orig_args.second));
  return _pointer;
}

address Core::CMP_ai_func(address _pointer) {
  auto args = readArgs(_pointer, op_spec::AI, true);
  auto [a1, value] = args.args;
  _pointer = args.current_pointer;
 
  setFlag(ZF, a1 == value);
  seek(_pointer);
  printCode("CMP", std::get<address>(args.orig_args.first), std::get<unsigned int>(value));
  return _pointer;
}

address Core::CMP_aw_func(address _pointer) {
  auto args = readArgs(_pointer, op_spec::AW, true);
  auto [a1, value] = args.args;
  _pointer = args.current_pointer;

  setFlag(ZF, std::get<unsigned int>(a1) == static_cast<unsigned int>(std::get<std::byte>(value)));
  seek(_pointer);
  printCode("CMP", std::get<address>(args.orig_args.first), std::get<std::byte>(value));
  return _pointer;
}

address Core::CMP_aa_func(address _pointer) {
  auto args = readArgs(_pointer, op_spec::AA, true, true);
  auto [a1, a2] = args.args;
  _pointer = args.current_pointer;

  setFlag(ZF, a1 == a2);
  seek(_pointer);
  printCode("CMP", std::get<address>(args.orig_args.first), std::get<address>(args.orig_args.second));
  return _pointer;
}

address Core::JNE_a_func(address _pointer) {
  auto args = readArgs(_pointer, op_spec::A);
  auto [src, _] = args.args;
  _pointer = args.current_pointer;

  seek(_pointer);
  auto jumped = false;
  if (!checkFlag(ZF)) {
    _pointer = std::get<address>(src);
    seek(_pointer);
    jumped = true;
  }
  setReg(EIP, _pointer);
  printJump("JNE", std::get<address>(src), jumped);
  return _pointer;
}

//TODO: implement relative addr notation in vvmc
address Core::JNE_r_func(address _pointer) {
  const auto p = _pointer - 1;
  const auto src = readSignedInt();

  auto jumped = false;
  if (!checkFlag(ZF)) {
    _pointer = p + src;
    jumped = true;
  }
  seek(_pointer);
  setReg(EIP, _pointer);
  printJump("JNE", src, jumped);
  return _pointer;
}

//TODO: create conditional_jump function for all jumps
address Core::JE_func(address _pointer) {
  auto args = readArgs(_pointer, op_spec::A);
  auto [src, _] = args.args;
  _pointer = args.current_pointer;

  auto jumped = false;
  if (checkFlag(ZF)) {
    _pointer = std::get<address>(src);
    jumped = true;
  }
  seek(_pointer);
  setReg(EIP, _pointer);
  printJump("JE", std::get<address>(src), jumped);
  return _pointer;
}

address Core::NOP_func(address _pointer) {
  printCode("NOP");
  return _pointer;
}

address Core::PUSH_a_func(address _pointer) {
  auto args = readArgs(_pointer, op_spec::A);
  auto [src, _] = args.args;
  _pointer = args.current_pointer;
  seek(src);
  auto value = readInt();
  seek(ESP);
  auto s = address{readInt()};
  const auto s_addr = s - INT_SIZE;
  seek(s_addr);
  writeInt(value);
  seek(ESP);
  writeInt(s_addr.dst);
  seek(_pointer);
  printCode("PUSH", std::get<address>(src));
  return _pointer;
}

//TODO: migrate to readArgs
address Core::PUSH_i_func(address _pointer) {
  auto args = readArgs(_pointer, op_spec::I);
  auto [value, _] = args.args;
  _pointer = args.current_pointer;
  seek(ESP);
  auto s = address{readInt()};
  const auto s_addr = s - INT_SIZE;
  seek(s_addr);
  writeInt(value);
  seek(ESP);
  writeInt(s_addr.dst);
  seek(_pointer);
  printCode("PUSH", std::get<unsigned int>(value));
  return _pointer;
}

//TODO: migrate to readArgs
address Core::PUSH_w_func(address _pointer) {
  auto args = readArgs(_pointer, op_spec::W);
  auto [value, _] = args.args;
  _pointer = args.current_pointer;
  seek(ESP);
  auto s = address{readInt()};
  const auto s_addr = s - INT_SIZE;
  seek(s_addr);
  writeInt(static_cast<unsigned int>(std::get<std::byte>(value)));
  seek(ESP);
  writeInt(s_addr.dst);
  seek(_pointer);
  printCode("PUSH", std::get<std::byte>(value));
  return _pointer;
}

//TODO: migrate to readArgs
address Core::POP_func(address _pointer) {
  auto args = readArgs(_pointer, op_spec::A);
  auto [dst, _] = args.args;
  _pointer = args.current_pointer;
  seek(ESP);
  const auto s_addr = address{readInt()};
  seek(s_addr);
  const auto value = readInt();
  seek(s_addr);
  writeInt(0x0);
  seek(dst);
  writeInt(value);
  seek(ESP);
  writeInt((s_addr + INT_SIZE).dst);

  seek(_pointer);
  printCode("POP", std::get<address>(dst));
  return _pointer;
}

address Core::JMP_a_func(address _pointer) {
  auto args = readArgs(_pointer, op_spec::A);
  auto [src, _] = args.args;
  _pointer = args.current_pointer;

  _pointer = std::get<address>(src);
  seek(_pointer);
  setReg(EIP, _pointer);
  printJump("JMP", _pointer, true);
  return _pointer;
}

//TODO: implement relative notation in vvmc
address Core::JMP_r_func(address _pointer) {
  const auto src = readSignedInt();
  _pointer = _pointer - 1; // minus jmp size.

  _pointer += src;
  seek(_pointer);
  setReg(EIP, _pointer);
  printJump("JMP", src, true);
  return _pointer;
}

//TODO: remove it after implementing MOV [EAX] const
address Core::MEM_func(address _pointer) {
  const auto a1 = readAddress();
  _pointer += ADDRESS_SIZE;
  const auto a2 = readByte();
  _pointer += 1;

  seek(a1);
  const auto addr = readAddress();
  seek(addr);
  writeByte(a2);
  seek(_pointer);
  printCode("MEM", a1, a2);
  return _pointer;
}
