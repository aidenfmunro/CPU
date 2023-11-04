#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "textfuncs.h"
#include "stackfuncs.h"
#include "spu.h"
#include "config.h"
#include "utils.h"
#include "dsl.h"

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

ErrorCode RunProgram(const char* filename)
{
    SPU spu = {};

    CreateSPU(&spu, filename);

    size_t curPosition = 0;

    while (execCommand(&spu, &curPosition) != EXIT_CODE) {; PrintStack(&spu.stack);}
    
    //-----------------------------------------------------
    //for (size_t y = 0; y <= 40; y++) // hardcode
    //  {
    //    for (size_t x = 0; x <= 40; x++)
    //      {
    //        if (doubleCompare(spu.RAM[10 * y + x], 1))
    //          {
    //            printf("# ");
    //          }
    //        else
    //          {
    //            printf(". ");
    //          }
    //      }
    //    printf("\n");
    //    printf("\n");
    //  }
    //------------------------------------------------------

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
    
    char instruction                    = *(spu->code + *curPosition);               // TODO: if statements for immed and reg optimize
    bool isArgReg                       = instruction & ARG_FORMAT_REG;
    bool isArgIm                        = instruction & ARG_FORMAT_IMMED;                    
    bool isArgRam                       = instruction & ARG_FORMAT_RAM;
    char commandCode                    = instruction & ARG_FORMAT_CMD;
    char registerNum                    = 0;
    elem_t value                        = 0;                                                   
    size_t labelAddress                 = 0;
  
    switch (commandCode)
      {
        #define DEF_COMMAND(name, number, argc, cde)                                \
                                                                                    \
            case number:                                                            \
                                                                                    \
              *curPosition += sizeof(char);                                         \
                                                                                    \
              if (argc)                                                             \
                {                                                                   \
                  if (isArgReg)                                                     \
                    {                                                               \  
                      memcpy(&registerNum, spu->code + *curPosition, sizeof(char)); \
                      *curPosition += sizeof(char);                                 \
                    }                                                               \
                  if (isArgIm)                                                      \
                    {                                                               \
                      memcpy(&value, spu->code + *curPosition, sizeof(double));            \
                      memcpy(&labelAddress, spu->code + *curPosition, sizeof(double));     \
                      *curPosition += sizeof(double);                                      \
                    }                                                               \
                                                                                    \
                }                                                                   \
                cde                                                                 \
                break;                                                              \

            #include "commands.h"

            default:
                printf("Unknown command, code: %X, line: %ld\n", commandCode, *curPosition);
                return EXIT_CODE;
        
        #undef DEF_COMMAND
      }

    return OK;   
}