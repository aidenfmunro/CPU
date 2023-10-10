#include "textfuncs.h"
#include "stackfuncs.h"
#include "spuconfig.h"
#include "assembler.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

void Proccess(void)
{
    const char* filename = "comms.txt";

    Stack stack = {};

    Text text = {};

    CreateStack(stack);

    CreateText(&text, filename, NONE);
}

ErrorCode RunProgram(const char* filename, SPU* spu)
{
  FILE* codebin = fopen("code.bin", "rb");

  if (codebin == NULL)
      return 1;
  
  size_t fileSize = getSize(filename);
  
  spu->code = (byte*)calloc(sizeof(byte), fileSize);

  Stack stack = {};

  CreateStack(stack);

  spu->stack = &stack;

  // fprintf(stderr, "%d\n", fileSize);

  fread(spu->code, sizeof(byte), fileSize, codebin);

  // printf("%d\n", sizeof(spu->code));

  // printf("%d\n", getSize(filename));

  // printf("%d\n", getSize(filename) / (2 * sizeof(elem_t)));

  fclose(codebin);

  for (size_t index = 0; index < (getSize(filename) / sizeof(elem_t)) / 2; index++)
    {
      execCommand(spu, index);
      PrintStack(spu->stack);
    }
  
  return 0;
}


ErrorCode execCommand(SPU* spu, const int position)
{
    char instruction = *(spu->code + 2 * position * SHIFT);
    char argReg = ARG_FORMAT_REG & instruction;
    char argIm = ARG_FORMAT_IMMED & instruction;

    int commandCode = instruction & 0x0F;
    elem_t value = *(elem_t*)(spu->code + (2 * position + 1) * SHIFT);
    char registerNum = *(spu->code + position * SHIFT + 1);

    printf("command code = %d", commandCode);
    printf("value = %lg", value);

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
          printf("%lg", Pop(spu->stack));

          break;
        
        default:
          printf("wtf");
          break;
      }

}

