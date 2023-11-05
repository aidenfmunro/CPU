#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "textfuncs.h"
#include "stackfuncs.h"
#include "spu.h"
#include "config.h"
#include "utils.h"
#include "dsl.h"

struct ArgRes
{
    elem_t* place;
    elem_t value;

    size_t labelAddress;
    ErrorCode error;
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

ErrorCode execCommand(SPU* spu, size_t* position); 

ArgRes getArg(SPU* spu, size_t* curPosition, byte command);

ErrorCode RunProgram(const char* filename)
{
    SPU spu = {};

    CreateSPU(&spu, filename);

    size_t curPosition = 0;

    while (execCommand(&spu, &curPosition) != EXIT_CODE) {PrintStack(&spu.stack);}
    
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

    myOpen("code.bin", "rb", codebin);

    myRead(spu->code, sizeof(byte), fileSize, codebin);

    myClose(codebin);

    return OK;
}

ErrorCode DestroySPU(SPU* spu)
{
    DestroyStack(&spu->stack);

    DestroyStack(&spu->calls);

    memset(spu->RAM, 0, nSLOTS);

    memset(spu->regs, 0, nREGS);

    free(spu->code);

    return OK;
}

ErrorCode execCommand(SPU* spu, size_t* curPosition)
{    
    CheckPointerValidation(spu);
    
    char command = *(spu->code + *curPosition);
  
    switch (command & ARG_FORMAT_CMD)
      {
        #define DEF_COMMAND(name, number, argc, code)                                \
                                                                                    \
            case number:                                                            \
              {                                                                     \
              *curPosition += sizeof(char);                                         \
              ArgRes arg = {};                                                      \
                                                                                    \
              if (argc)                                                             \
                {                                                                   \
                  arg = getArg(spu, curPosition, command);                          \
                }                                                                   \
                                                                                    \
                                                                                    \
                code                                                                \
                break;                                                              \
              }
            #include "commands.h"

            default:
                printf("Unknown command, code: %X, line: %ld\n", command & ARG_FORMAT_CMD, *curPosition);
                return EXIT_CODE;
        
        #undef DEF_COMMAND
      }

    return OK;   
}

ArgRes getArg(SPU* spu, size_t* curPosition, byte command)
{
    ArgRes result = {};
    elem_t tempvalue = 0;

    if (command & ARG_FORMAT_REG)
      {
        result.value += spu->regs[int(spu->code[*curPosition])];
        result.place = &spu->regs[int(spu->code[*curPosition])];
        *curPosition += sizeof(char);
      }
    if (command & ARG_FORMAT_IMMED)
      {
        memcpy(&tempvalue, &spu->code[*curPosition], sizeof(double));
        result.value += tempvalue;
        memcpy(&result.labelAddress,  &spu->code[*curPosition], sizeof(double)); // wtf?
        *curPosition += sizeof(double);
      }
    if (command & ARG_FORMAT_RAM)
      {
        size_t index = (size_t)result.value;
        result.place = &spu->RAM[index];
      }
    
    printf("%ld, %lg\n", result.labelAddress, size_t(result.value));

    return result;
}