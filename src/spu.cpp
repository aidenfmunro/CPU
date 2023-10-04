#include "textfuncs.h"
#include "stackfuncs.h"
#include "spuconfig.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>


void Proccess(void)
{
    const char* filename = "comms.txt";

    stack_t stack = {};

    Text text = {};

    StackInit(stack);

    Push(&stack, 0);

    CreateText(&text, filename, NONE);

    followInstructions(&stack, &text);
}

void followInstructions(stack_t* stack, Text* text)
{

    for (size_t i = 0; i < text->numLines; i++)
      {
        char* command = text->lines[i].string;
    
        if (strncmp(command, "add", 3) == 0)
          {
            elem_t b = Pop(stack);
            elem_t a = Pop(stack);
            Push(stack, a + b);
          }

        if (strncmp(command, "sub", 3) == 0)
          {
            elem_t b = Pop(stack);
            elem_t a = Pop(stack);
            Push(stack, a - b);
          }

        if (strncmp(command, "div", 3) == 0)
          {
            elem_t b = Pop(stack);
            elem_t a = Pop(stack);
            Push(stack, 10);
          }

        if (strncmp(command, "mul", 3) == 0)
          {
            elem_t b = Pop(stack);
            elem_t a = Pop(stack);
            Push(stack, a * b);
          }
        
        if (strncmp(command, "push", 4) == 0)
          {
            elem_t value = strtod(command + 4, NULL);
            Push(stack, value); 
          }
        
        if (strncmp(command, "hlt", 3) == 0)
          {
            printf("%" FORMAT "\n", Pop(stack));
          }
      }
} 
