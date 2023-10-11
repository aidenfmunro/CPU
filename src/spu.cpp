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

  FILE* codebin = fopen("code.bin", "rb");

  if (codebin == NULL)
      return 1;
  
  size_t fileSize = getSize(filename);
  
  spu.code = (byte*)calloc(sizeof(byte), fileSize);

  Stack stack = {};

  CreateStack(stack);

  spu.stack = &stack;

  fread(spu.code, sizeof(byte), fileSize, codebin);

  fclose(codebin);

  int ret = 0;

  for (size_t index = 0; index < (fileSize / sizeof(elem_t)) / 2; index++)
    {
      if (ret != EXIT_CODE)
        {
          ret = execCommand(&spu, index);
          PrintStack(spu.stack);
        }
      else
        {
          break;
        }
    }

  DestroyStack(&stack);  
  
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
    char instruction = getCommandArgs(position, spu->code);

    char registerNum = getRegisterNum(position, spu->code);

    elem_t value = getValue(position, spu->code);

    char argReg = ARG_FORMAT_REG & instruction;
    char argIm = ARG_FORMAT_IMMED & instruction;
    int commandCode = instruction & 0x0F;

    printf("command code = %d; ", commandCode);
    printf("value = %lg\n", value);

    elem_t a = POISON;
    elem_t b = POISON;

    switch (commandCode)
      {
        case PUSH:
          if (argIm && argReg)
            {
              Push(spu->stack, spu->regs[registerNum] + value);
              spu->regs[registerNum] = POISON;
            }
          else if (argReg)
            {
              Push(spu->stack, spu->regs[registerNum]);
              spu->regs[registerNum] = POISON;
            }
          else
            {
              Push(spu->stack, value);
            }

          break;

        case POP:
          if (argIm && argReg)
            {
              spu->regs[registerNum] = Pop(spu->stack) + value;
            }
          else if (argReg)
            { 
              spu->regs[registerNum] = Pop(spu->stack);
            }
          
          break;
        
        case ADD:
          b = Pop(spu->stack);
          a = Pop(spu->stack);
          Push(spu->stack, a + b);

          break;

        case SUB:
          b = Pop(spu->stack);
          a = Pop(spu->stack);
          Push(spu->stack, b - a);

          break;

        case DIV:
          b = Pop(spu->stack);
          a = Pop(spu->stack);
          Push(spu->stack, a / b);

          break;
        
        case MUL:
          b = Pop(spu->stack);
          a = Pop(spu->stack);
          Push(spu->stack, a * b);

          break;
        
        case HLT:
          printf("%lg\n", Pop(spu->stack));
          return EXIT_CODE;

          break;
        
        default:
          
          return UNKNOWN_ASM_COMMAND;
          break;
      }

  return OK;
}