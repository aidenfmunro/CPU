#include <stdio.h>
#include <string.h>
#include <math.h>
#include "assembler.h"
#include "textfuncs.h"


byte* Compile(Text* initialText, CPU* spu)
{
    byte* code = (byte*)calloc(initialText->numLines, sizeof(elem_t) * 2); // NOTE: command | argument

    for (size_t index = 0; index < initialText->numLines; index++)
      {
        char* line        = initialText->lines[index].string;
        size_t lineLength = initialText->lines[index].length;

        char commandCode  = 0; // NOTE: 00000000

        if (*line == ' ' || *line == ';') // NOTE: skip empty lines and comment lines
            continue;

        elem_t value = POISON;

        char* commentPtr = strchr(initialText->lines[index].string, int(';')); // NOTE: ignore comments ex.: "; Hello"

        if (commentPtr != NULL)
            *commentPtr = '\0';

        char command[5] = "";
    
        size_t commandLength = 0;
        size_t argumentLength = 0;

        sscanf(line, "%s%n %lg", command, &commandLength, &value);

        // if (value == POISON) // NOTE: either push/pop with register, either math operation 
          // {

            char* argument = strtok(line + commandLength, " ");

            printf("%s ", command);
            if (argument != NULL)
              {
                argumentLength = strlen(argument);
                printf("%s ", argument);
              }
            printf("%lg\n", value);
            

            if (argumentLength == 3)
              {
                char registerNum = argument[1] - 'a' + 1; // register number

                commandCode |= ARG_FORMAT_IMMED;
                commandCode |= ARG_FORMAT_REG;

                *(code + 2 * index * SHIFT + 1) = registerNum;   
              }
            else if(argumentLength > 3)
              {
                char registerNum = argument[1] - 'a' + 1; // register number
                
                char* tempPtr = strchr(line, '+') + 1;

                sscanf(tempPtr, "%lg", &value);

                commandCode |= ARG_FORMAT_REG; // code | REG_ON;
                commandCode |= ARG_FORMAT_IMMED;
                
                *(code + 2 * index * SHIFT + 1) = registerNum;
                *(elem_t*)(code + 2 * index + SHIFT) = value;
              }
            else
              {
            *(elem_t*)(code + 2 * index * SHIFT + SHIFT) = value;
              }
        
        commandCode |= getCommandCode(command, commandLength);
        *(code + 2 * index * SHIFT) = commandCode;    
             
      }

    return code;
}

CommandCode getCommandCode(char* command, const size_t commandLength)
{
    if (strncmp(command, "push", commandLength) == 0)
      {
        return PUSH;
      }
    else if (strncmp(command, "pop", commandLength) == 0)
      {
        return POP;
      }
    else if (strncmp(command, "add", commandLength) == 0) // TODO: instead of 3 -> COMMAND_LEN
      {
        return ADD;                    
      }
    else if (strncmp(command, "sub", commandLength) == 0)
      {
        return SUB;
      }
    else if (strncmp(command, "div", commandLength) == 0)
      {
        return DIV;
      }
    else if (strncmp(command, "mul", commandLength) == 0)
      {
        return MUL;
      }
    else if (strncmp(command, "sqrt", commandLength) == 0)
      {
        return SQRT;
      }
    else if (strncmp(command, "sin", commandLength) == 0)
      {
        return SIN;
      }
    else if (strncmp(command, "cos", commandLength) == 0)
      {
        return COS;
      }
    else if (strncmp(command, "hlt", commandLength) == 0)
      {
        return HLT;
      }  
}

void printbytecode(Text* text, CPU* spu)
{
    for(size_t i = 0; i < text->numLines; i++)
      {
        printf("info1 = %d ", *(spu->code + i * 2 * SHIFT));
        printf("info2 = %d ", *(spu->code + i * 2 * SHIFT + 1));
        printf("value = %lg\n", *(double*)(spu->code + i * 2 * SHIFT + SHIFT));
      }
}