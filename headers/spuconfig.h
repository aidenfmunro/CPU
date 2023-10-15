#ifndef SPUCONFIG_H
#define SPUCONFIG_H

#include "stackfuncs.h"
#include "textfuncs.h"

typedef char byte;

const int nREGS = 4;

const int EXIT_CODE = -1;

const int BYTES_PER_COMMAND = 16;

enum Instruction
{

    PUSH = 1,
    POP  = 2,
    IN   = 3,
    ADD  = 4,
    SUB  = 5,
    MUL  = 6,
    DIV  = 7,
    SQRT = 8,
    SIN  = 9,
    COS  = 10,
    OUT  = 11,
    HLT  = 12
};

struct SPU
{
    Stack stack;

    elem_t regs[nREGS];
    
    byte* code;
};

ErrorCode CreateSPU(SPU* spu, const char* filename);

ErrorCode DestroySPU(SPU* spu);

ErrorCode RunProgram(const char* filename);

ErrorCode execCommand(SPU* spu, const int position);

elem_t getValue(const size_t position, const byte* bytecode);

byte getRegisterNum(const size_t position, const byte* bytecode);

byte getCommandArgs(const size_t position, const byte* bytecode);

#endif