#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "stackfuncs.h"
#include "spu.h"
#include "config.h"
#include "utils.h"
#include "dsl.h"

struct ArgRes
{
    elem_t* storePtr;
    elem_t* value;

    ErrorCode error;
};

const int HIDDEN_REG = 1;

struct SPU
{
    Stack stack;

    Stack calls;

    elem_t regs[AMOUNT_OF_REGISTERS + HIDDEN_REG];

    elem_t RAM[AMOUNT_OF_RAM_SLOTS];
    
    byte* code;

    size_t ip;
};

ErrorCode CreateSPU(SPU* spu, const char* filename);

ErrorCode DestroySPU(SPU* spu);

ErrorCode execCommand(SPU* spu); 

ErrorCode dumpRAM(SPU* spu);

ArgRes getArg(SPU* spu, byte command);

ErrorCode RunProgram(const char* filename)
{
    SPU spu = {};

    CreateSPU(&spu, filename);

    while (execCommand(&spu) != EXIT_CODE) {;}
    
    DestroySPU(&spu);  

    return OK;
}

ErrorCode CreateSPU(SPU* spu, const char* filename)
{
    size_t fileSize = getSize(filename);
    
    byte* tempcode = (byte*)calloc(sizeof(byte), fileSize);

    CheckPointerValidation(tempcode);

    spu->code = tempcode;

    Stack stack = {};

    Stack calls = {};

    CreateStack(calls);

    CreateStack(stack);

    spu->stack = stack;

    spu->calls = calls;
    
    spu->ip = 0;

    myOpen(filename, "rb", codebin);

    myRead(spu->code, sizeof(byte), fileSize, codebin);

    myClose(codebin);

    return OK;
}

ErrorCode DestroySPU(SPU* spu)
{
    DestroyStack(&spu->stack);

    DestroyStack(&spu->calls);

    memset(spu->RAM, 0, AMOUNT_OF_RAM_SLOTS);

    memset(spu->regs, 0, AMOUNT_OF_REGISTERS);

    free(spu->code);

    return OK;
}

ErrorCode dumpRAM(SPU* spu)
{
    for (size_t y = 0; y <= 40; y++)
    {
        for (size_t x = 0; x <= 40; x++)
        {
            if (doubleCompare(spu->RAM[40 * y + x], 1))
            {
                printf("#  ");
            }
            else
            {
                printf(".  ");
            }
        }

        printf("\n\n");
    }
    
    return OK;
}

ErrorCode execCommand(SPU* spu)
{    
    CheckPointerValidation(spu);
    
    char command = spu->code[spu->ip];
  
    switch (command & ARG_FORMAT_CMD)
    {
        #define DEF_COMMAND(name, number, argc, code)                           \
                                                                                \
        case number:                                                            \
        {                                                                       \
            spu->ip += sizeof(char);                                            \
            ArgRes arg = {};                                                    \
                                                                                \
            if (argc)                                                           \
            {                                                                   \
                arg = getArg(spu, command);                                     \
            }                                                                   \
                                                                                \
            code                                                                \
            break;                                                              \
        }

        #include "commands.h"

        default:
            printf("Unknown command, code: %d, line: %ld\n", command & ARG_FORMAT_CMD, spu->ip);
            return EXIT_CODE;
        
        #undef DEF_COMMAND
    }

    return OK;   
}

ArgRes getArg(SPU* spu, byte command)
{
    ArgRes result = {};

    elem_t tempvalue = 0;

    spu->regs[RHX] = 0;

    if (command & ARG_FORMAT_REG)
    {
        spu->regs[RHX] += spu->regs[int(spu->code[spu->ip])];
        result.storePtr = &spu->regs[int(spu->code[spu->ip])];
        spu->ip += sizeof(char);
    }

    if (command & ARG_FORMAT_IMMED)
    {
        memcpy(&tempvalue, &spu->code[spu->ip], sizeof(double));
        spu->regs[RHX] += tempvalue;
        spu->ip += sizeof(double);
    }

    if (command & ARG_FORMAT_RAM)
    {
        size_t index = (size_t)spu->regs[RHX];
        result.storePtr = &spu->RAM[index];
    }

    result.value = &spu->regs[RHX];

    return result;
}