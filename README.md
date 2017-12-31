# Vortex VM

## Memory structure

### HEADER
* 3 bytes — "VVM"
* 1 byte  — version
* 1 byte  — code offset
* 3 bytes — reserved

### REGS
* 1 byte  — VM state [exec, end, error, etc]
* 4 bytes — stack head addr
* 4 bytes — EAX
* 4 bytes — EBX
* 4 bytes — ECX
* 1 byte  — flags [ZF, OUTF, INTF]
* 1 byte  — interrupt code
* 4 bytes — output port

## Supported instructions
* MOV  dst[addr int] src[const int] 
* ADD  dst[addr int] src[const int] 
* SUB  dst[addr int] src[const int] 
* CMP  a1[addr int]  a2[const int] 
* JNE  dst[addr int] 
* JE   dst[addr int] 
* OUT  src[addr int] 
* INT  irq[code byte] 
* PUSH src[addr int] 
* POP  dst[addr int] 
* NOP 

## Supported interruptions
* FF — stop execution
* 21 — get addr from ECX, read chars until '$' and print this