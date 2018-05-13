# Vortex VM

![screenshot](https://raw.githubusercontent.com/averrin/vvm/master/screenshot.png)

## Usage
`./bin/vvm ./examples/example.vvmc`

[Code example](https://github.com/averrin/vvm/blob/master/examples/example.vvmc)

## Memory structure

### HEADER
```
* 3 bytes — "VVM"
* 1 byte  — version
* 1 byte  — code offset
* 3 bytes — reserved
```

### REGS
```
* 1 byte  — VM state [exec, end, error, etc]
* 4 bytes — stack head addr
* 4 bytes — EAX
* 4 bytes — EBX
* 4 bytes — ECX
* 4 bytes — EIP (instruction pointer)
* 4 bytes — EDI (mapped memory offset. may be removed)
* 1 byte  — flags [ZF, OUTF, INTF]
* 1 byte  — interrupt code
* 4 bytes — output port
```

## Supported instructions
```
* MOV  dst[addr int] src[const int] 
* MOV  dst[addr int] src[const byte] 
* MOV  dst[addr int] src[addr  int] 
* ADD  dst[addr int] src[const int] 
* ADD  dst[addr int] src[const byte] 
* ADD  dst[addr int] src[addr  int] 
* SUB  dst[addr int] src[const int] 
* SUB  dst[addr int] src[const byte] 
* SUB  dst[addr int] src[addr  int] 
* JMP  dst[addr int] 
* JMP  dst[label str] 
* CMP  a1[addr int]  a2[const int] 
* JNE  dst[addr int] 
* JNE  dst[label str] 
* JE   dst[addr int] 
* JE  dst[label str] 
* OUT  src[addr int] 
* INT  irq[code byte] 
* PUSH src[addr int] 
* POP  dst[addr int] 
* NOP 

You can use `[REG_NAME]` notation for using stored address
```

## Supported interruptions
```
* FF — stop execution
* 21 — get addr from ECX, read chars until '$' and print this
```

## TODO
- Math and logic operations
- Relativa jumps
- Address literals
- Save/load for vvmc and vvm files
- Better memory mapping
- Breakpoints
- ...
