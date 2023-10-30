#ifndef SPUCONFIG_H
#define SPUCONFIG_H

#include "stackfuncs.h"
#include "textfuncs.h"

typedef char byte;

const int nREGS = 4;

const int nSLOTS = 512;

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

    elem_t RAM[nSLOTS];
    
    byte* code;
};

ErrorCode CreateSPU(SPU* spu, const char* filename);

ErrorCode DestroySPU(SPU* spu);

ErrorCode RunProgram(const char* filename);

ErrorCode execCommand(SPU* spu, size_t* position);

#endif