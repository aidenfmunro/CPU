#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "textfuncs.h"
#include "stackfuncs.h"
#include "spuconfig.h"
#include "assembler.h"
#include "utils.h"
#include "dsl.h"

ErrorCode RunProgram(const char* filename)
{
    SPU spu = {};

    CreateSPU(&spu, filename);

    for (size_t index = 0;; index++) // TODO: specify the meaning? of / ?
      {
        if (execCommand(&spu, index) == EXIT_CODE) return OK;
      }

    DestroySPU(&spu);  

    return OK;
}

ErrorCode CreateSPU(SPU* spu, const char* filename)
{
    myOpen("code.bin", "rb", codebin); // TODO: обертка над fopen -> какой файл не был открыт, perror: why can be useful?
    
    size_t fileSize = getSize(filename);
    
    byte* temp = (byte*)calloc(sizeof(byte), fileSize);// TODO: rename variable

    if (temp == NULL)
        return NO_MEMORY;

    spu->code = temp;

    Stack stack = {};

    CreateStack(stack);

    spu->stack = stack;

    myRead(spu->code, sizeof(byte), fileSize, codebin);

    myClose(codebin); //TODO: is fread successful? -> DONE

    return OK;
}

ErrorCode DestroySPU(SPU* spu)
{
    DestroyStack(&spu->stack);

    free(spu->code);

    return OK;
}

elem_t getValue(const size_t position, const byte* bytecode)
{ 
    CheckPointerValidation(bytecode + (2 * position + 1) * SHIFT);  

    return *(elem_t*)(bytecode + (2 * position + 1) * SHIFT);
}

byte getRegisterNum(const size_t position, const byte* bytecode)
{
    CheckPointerValidation(bytecode + 2 * position * SHIFT + 1);

    return *(byte*)(bytecode + 2 * position * SHIFT + 1);
}

byte getCommandArgs(const size_t position, const byte* bytecode)
{
    CheckPointerValidation(bytecode + 2 * position * SHIFT);

    return *(byte*)(bytecode + 2 * position * SHIFT);
}

ErrorCode execCommand(SPU* spu, const int position)
{    
    char instruction = getCommandArgs(position, spu->code);               // TODO: if statements for immed and reg optimize

    char registerNum = getRegisterNum(position, spu->code);

    elem_t value     = getValue(position, spu->code);

    bool isArgReg    = instruction & ARG_FORMAT_REG;
    bool isArgIm     = instruction & ARG_FORMAT_IMMED;                    // TODO: change name to isArgReg
    int commandCode  = instruction & 0x0F;                                // TODO: make const 0x0F

                                                                          // printf("command code = %d; ", commandCode);
                                                                          // printf("value = %lg\n", value); 

                                                                          // TODO: seperate macro for log

    switch (commandCode)
      {
        #define DEF_COMMAND(name, number, isArgIm, isArgReg, code) case CMD_ ## name: code; break;                                           

        #include "commands.h"

        #undef DEF_COMMAND

        default:
            perror("Unknown command");
            return EXIT_CODE;
      }
    
  return OK;
}