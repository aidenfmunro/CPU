#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "textfuncs.h"
#include "stackfuncs.h"
#include "spuconfig.h"
#include "assembler.h"
#include "utils.h"

ErrorCode RunProgram(const char* filename)
{
    SPU spu = {};

    CreateSPU(&spu, filename);

    int ret = 0;

    for (size_t index = 0; index < getSize(filename) / BYTES_PER_COMMAND; index++) // TODO: specify the meaning? of / ?
      {
        if (ret != EXIT_CODE)
          {
            PrintStack(&spu.stack);
            printf("stack data pointer = %p\n", &spu.stack.data);
            ret = execCommand(&spu, index); // TODO: universal log system
            PrintStack(&spu.stack);
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
    printf("stack pointer = %p\n", spu->stack);
    printf("stack data pointer = %p\n", &spu->stack.data);

    elem_t a = POISON;
    elem_t b = POISON;

    switch (commandCode)
      {
        case PUSH:
          if (argIm && argReg)
            {
              Push(&spu->stack, spu->regs[registerNum] + value);
            }
          else if (argReg)
            {
              Push(&spu->stack, spu->regs[registerNum]);
            }
          else
            {
              Push(&spu->stack, value);
            }

          break;

        case POP:
          if (argIm && argReg)
            {
              spu->regs[registerNum] = Pop(&spu->stack) + value;
            }
          else if (argReg)
            { 
              spu->regs[registerNum] = Pop(&spu->stack);
            }
          
          break;
        
        case ADD:
          b = Pop(&spu->stack);
          a = Pop(&spu->stack);
          Push(&spu->stack, a + b);

          break;

        case SUB:
          b = Pop(&spu->stack);
          a = Pop(&spu->stack);
          Push(&spu->stack, b - a);

          break;

        case DIV:
          b = Pop(&spu->stack); // DSL 
          a = Pop(&spu->stack);
          Push(&spu->stack, a / b);

          break;
        
        case MUL:
          b = Pop(&spu->stack);
          a = Pop(&spu->stack);
          Push(&spu->stack, a * b);

          break;
        
        case HLT:
          printf("%lg\n", Pop(&spu->stack));
          return EXIT_CODE;

          break;
        
        default:
          
          return UNKNOWN_ASM_COMMAND;
          break;
      }

  return OK;
}