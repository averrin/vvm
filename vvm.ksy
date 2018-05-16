  meta:
    id: vvm
    endian: be
  seq:
    - id: head
      type: header
    - id: regs
      type: registers
    - id: flags
      type: flags
    - id: interrupt_code
      type: u1
    - id: out_port
      type: u4
    - id: code
      type: code_section
  types:
    header:
      seq:
        - id: magic
          contents: 'VVM'
        - id: version
          size: 1
        - id: code_offset
          size: 1
        - id: reserved
          size: 3
    registers:
      seq:
        - id: state
          type: u1
          enum: states
        - id: stack_head
          type: u4
        - id: eax
          type: u4
        - id: ebx
          type: u4
        - id: ecx
          type: u4
        - id: eip
          type: u4
        - id: edi
          type: u4
    flags:
      seq:
        - id: reserved_flag
          type: b1
        - id: zf
          type: b1
        - id: outf
          type: b1
        - id: intf
          type: b1
      
    instruction:
      seq:
        - id: opcode
          type: u1
          enum: opcodes
        - id: arg1_m
          if: opcode == opcodes::mov_mm
            or opcode == opcodes::mov_mb
            or opcode == opcodes::inc
            or opcode == opcodes::cmp_mm
            or opcode == opcodes::cmp_mc
            or opcode == opcodes::jne_m
            or opcode == opcodes::add_mb
          type: address

        - id: arg2_m
          if: opcode == opcodes::mov_mm
          type: address
        - id: arg2_b
          if: opcode == opcodes::mov_mb
            or opcode == opcodes::cmp_mb
            or opcode == opcodes::add_mb
          type: u1
        - id: arg2_c
          if: opcode == opcodes::cmp_mc
          type: u4
    address:
      seq:
        - id: meta
          seq:
            - id: redirect
              type: b1
        - id: offset
          type: u4
    code_section:
      seq:
        - type: instruction
          repeat: eos
  enums:
    opcodes:
      0x00: invalid
      0x03: add_mb
      0x88: mov_mm
      0x8a: mov_mb
      0x40: inc
      0x38: cmp_mm
      0x39: cmp_mc
      0x3a: cmp_mb
      0x75: jne_m
    states:
      0x01: init
      0x02: exec
      0x03: pause
      0x10: erro
      0xff: end
    interrupts:
      0x21: int_pritn
      0xfa: int_test
      0xff: int_end
