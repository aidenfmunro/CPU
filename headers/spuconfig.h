#ifndef SPUCONFIG_H
#define SPUCONFIG_H

#include "stackfuncs.h"
#include "textfuncs.h"

typedef char byte;

const int nREGS = 4;

const int nSLOTS = 256;

const int EXIT_CODE = -1;

const int BYTES_PER_COMMAND = 16;

const char ARG_FORMAT_CMD = 0x1F;

enum Command
{
    #define DEF_COMMAND(name, num, argc, code) CMD_ ## name = num,

    #include "commands.h"

    #undef DEF_COMMAND
};

struct SPU
{
    Stack stack;

    Stack calls;

    elem_t regs[nREGS];

    char* RAM[nSLOTS];
    
    byte* code;
};

ErrorCode CreateSPU(SPU* spu, const char* filename);

ErrorCode DestroySPU(SPU* spu);

ErrorCode RunProgram(const char* filename);

ErrorCode execCommand(SPU* spu, size_t* position);

size_t getLabelAddress(const size_t position, const byte* bytecode);

elem_t getValue(const size_t position, const byte* bytecode);

byte getRegisterNum(const size_t position, const byte* bytecode);

byte getCommandArgs(const size_t position, const byte* bytecode);

#endif