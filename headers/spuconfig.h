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

    CMD_PUSH = 1,
    CMD_POP  = 2,
    CMD_IN   = 3,
    CMD_ADD  = 4,
    CMD_SUB  = 5,
    CMD_MUL  = 6,
    CMD_DIV  = 7,
    CMD_SQRT = 8,
    CMD_SIN  = 9,
    CMD_COS  = 10,
    CMD_OUT  = 11,
    CMD_HLT  = 12
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