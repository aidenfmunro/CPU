
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

    size_t curPosition = 0;

    for (size_t i = 0;;i++)
      {
        if (execCommand(&spu, &curPosition) != EXIT_CODE) curPosition += 1;
        else break;
      }
    DestroySPU(&spu);  

    return OK;
}

ErrorCode CreateSPU(SPU* spu, const char* filename)
{
     // TODO: обертка над fopen -> какой файл не был открыт, perror: why can be useful?
    
    size_t fileSize = getSize(filename);
    
    byte* temp = (byte*)calloc(sizeof(byte), fileSize);// TODO: rename variable

    if (temp == NULL)
        return NO_MEMORY;

    spu->code = temp;

    Stack stack = {};

    CreateStack(stack);

    spu->stack = stack;

    myOpen("code.bin", "rb", codebin);

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

size_t getLabelAddress(const size_t curPosition, const byte* bytecode)
{
    CheckPointerValidation(bytecode + (2 * curPosition + 1) * SHIFT);

    return *(size_t*)(bytecode + (2 * curPosition + 1) * SHIFT);
}

elem_t getValue(const size_t curPosition, const byte* bytecode)
{ 
    CheckPointerValidation(bytecode + (2 * curPosition + 1) * SHIFT);  

    return *(elem_t*)(bytecode + (2 * curPosition + 1) * SHIFT);
}

byte getRegisterNum(const size_t curPosition, const byte* bytecode)
{
    CheckPointerValidation(bytecode + 2 * curPosition * SHIFT + 1);

    return *(byte*)(bytecode + 2 * curPosition * SHIFT + 1);
}

byte getCommandArgs(const size_t curPosition, const byte* bytecode)
{
    CheckPointerValidation(bytecode + 2 * curPosition * SHIFT);

    return *(byte*)(bytecode + 2 * curPosition * SHIFT);
}

ErrorCode execCommand(SPU* spu, size_t* curPosition)
{    
    CheckPointerValidation(spu);
    
    char instruction        = getCommandArgs(*curPosition, spu->code);               // TODO: if statements for immed and reg optimize

    char registerNum        = getRegisterNum(*curPosition, spu->code);

    elem_t value            = getValue(*curPosition, spu->code);

    size_t labelAddress     = getLabelAddress(*curPosition, spu->code);

    bool isArgReg           = instruction & ARG_FORMAT_REG;
    bool isArgIm            = instruction & ARG_FORMAT_IMMED;                    // TODO: change name to isArgReg
    bool isArgRam           = instruction & ARG_FORMAT_RAM;
    char commandCode        = instruction & ARG_FORMAT_CMD;                                // TODO: make const 0x0F
                                                                          // printf("command code = %d; ", commandCode);
                                                                          // printf("value = %lg\n", value); 
                                                                          // TODO: seperate macro for log

    printf("cmd: %d, reg: %d, val: %lg, ln: %ld\n", commandCode, registerNum, value, labelAddress);

    switch (commandCode)
      {
        #define DEF_COMMAND(name, number, argc, code) case CMD_ ## name: code break;  

        #include "commands.h"

        #undef DEF_COMMAND
        
        default:
            printf("Unknown command, code: %X \n", commandCode);
            return EXIT_CODE;
      } 

    return OK;   
}