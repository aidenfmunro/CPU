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

    int ret = 0;

    for (size_t index = 0; index < getSize(filename) / BYTES_PER_COMMAND; index++) // TODO: specify the meaning? of / ?
      {
        if (ret != EXIT_CODE)
          {
            ret = execCommand(&spu, index); // TODO: universal log system
            PrintStack(&spu.stack);
          }
        else if (ret == UNKNOWN_ASM_COMMAND)
          {
            printf("program failed");

            break;
          }
        else
          {
            break;
          }
      }

    DestroySPU(&spu);  

    return OK;
}

ErrorCode CreateSPU(SPU* spu, const char* filename)
{
    FILE* codebin = fopen(filename, "rb");

    myAssert(codebin, UNABLE_TO_OPEN_FILE);

    if (codebin == NULL)
        return UNABLE_TO_OPEN_FILE; // TODO: обертка над fopen -> какой файл не был открыт, perror: why can be useful?
    
    size_t fileSize = getSize(filename);
    
    byte* temp = (byte*)calloc(sizeof(byte), fileSize);// TODO: rename variable

    if (temp == NULL)
        return NO_MEMORY;

    spu->code = temp;

    Stack stack = {};

    CreateStack(stack);

    printf("%p\n", stack.data);

    spu->stack = stack;

    fread(spu->code, sizeof(byte), fileSize, codebin); //TODO: is fread successful?

    fclose(codebin);

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
    char instruction = getCommandArgs(position, spu->code); // TODO: if statements for immed and reg optimize

    char registerNum = getRegisterNum(position, spu->code);

    elem_t value = getValue(position, spu->code);

    char argReg = instruction & ARG_FORMAT_REG;
    char argIm = instruction & ARG_FORMAT_IMMED; // TODO: change name to isArgReg
    int commandCode = instruction & 0x0F; // TODO: make const 0x0F

    printf("command code = %d; ", commandCode);
    printf("value = %lg\n", value); // TODO: seperate macro for log

    switch (commandCode)
      {
        case CMD_PUSH:
          {
            if (argIm && argReg)
              {
                PUSH(spu->regs[registerNum] + value);
              }
            else if (argReg)
              {
                PUSH(spu->regs[registerNum]);
              }
            else
              {
                PUSH(value);
              }
          }

          break;

        case CMD_POP:
          {
            if (argIm && argReg)
              {
                spu->regs[registerNum] = POP + value;
              }
            else if (argReg)
              { 
                spu->regs[registerNum] = POP;
              }
          
            break;
          }
        
        case CMD_ADD:
          {
            PUSH(POP + POP);
          }

          break;

        case CMD_SUB:
          {
            PUSH(0 - POP + POP);
          }

          break;

        case CMD_DIV:
          {
            PUSH(pow(POP / POP, -1));
          }

          break;
        
        case CMD_MUL:
          {
            PUSH(POP * POP); 
          }

          break;
        
        case CMD_HLT:
          printf("%lg\n", Pop(&spu->stack));
          return EXIT_CODE;

          break;
        
        default:
          
          return UNKNOWN_ASM_COMMAND;
          break;
      }

  return OK;
}