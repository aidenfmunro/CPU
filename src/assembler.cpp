#include <stdio.h>
#include <string.h>
#include <math.h>
#include "assembler.h"
#include "textfuncs.h"


byte* Compile(Text* assemblyText)
{
    byte* bytecode = (byte*)calloc(assemblyText->numLines, sizeof(elem_t) * 2); // NOTE: command | argument

    for (size_t position = 0; position < assemblyText->numLines; position++)
      {
        char* line        = assemblyText->lines[position].string;
        size_t lineLength = assemblyText->lines[position].length;

        char commandCode  = 0; // NOTE: 00000000

        if (*line == ' ' || *line == ';') // NOTE: skip empty lines and comment lines
            continue;

        elem_t value = POISON;

        char* commentPtr = strchr(assemblyText->lines[position].string, int(';')); // NOTE: ignore comments ex.: "; Hello"

        if (commentPtr != NULL)
            *commentPtr = '\0';

        char command[5] = "";
    
        size_t commandLength = 0;
        size_t argumentLength = 0;

        sscanf(line, "%s%n %lg", command, &commandLength, &value);

        char* argument = strtok(line + commandLength, " ");

        printf("%s ", command);
        if (argument != NULL)
          {
            argumentLength = strlen(argument);
            printf("%s ", argument);
          }
        printf("%lg \n", value);

        if (argumentLength == 3 && isnan(value))
          {
            commandCode |= ARG_FORMAT_REG;
            writeRegisterNum(position, bytecode, getRegisterNum(argument[1]));
          }
        else if (argumentLength != 0)
          {
            char* tempPtr = strchr(line, '+');

            if (tempPtr != NULL)
              {
                sscanf(tempPtr, "%lg", &value);
                commandCode |= ARG_FORMAT_REG;

                writeRegisterNum(position, bytecode, getRegisterNum(argument[1]));
              }
            
            commandCode |= ARG_FORMAT_IMMED;
          }

        commandCode |= getCommandCode(command, commandLength);

        writeCommandArgs(position, bytecode, commandCode);
        writeValue(position, bytecode, value);   
      }
    
    FILE* codebin = fopen("code.bin", "wb");

    fwrite(bytecode, sizeof(byte), assemblyText->numLines * 2 * sizeof(elem_t), codebin);

    fclose(codebin);

    return bytecode;
}



void writeValue(const size_t position, const byte* bytecode, const elem_t value)
{
    *(elem_t*)(bytecode + (2 * position + 1) * SHIFT) = value;
}

void writeRegisterNum(const size_t position, const byte* bytecode, const char registerNum)
{
    *(byte*)(bytecode + 2 * position * SHIFT + 1) = registerNum;
}

void writeCommandArgs(const size_t position, const byte* bytecode, const char commandCode)
{
    *(byte*)(bytecode + 2 * position * SHIFT) |= commandCode;
}


RegNum getRegisterNum(const char argument)
{
    switch(argument)
      {
        case 'a':
          return 1;
          break;

        case 'b':
          return 2;
          break;

        case 'c':
          return 3;
          break;

        case 'd':
          return 4;
          break;

        default:
          perror("unknown register\n");
          break;
      }
}

CommandCode getCommandCode(const char* command, const size_t commandLength)
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
    else
      {
        perror("unknown command\n");
      }  
}

void printbytecode(Text* text, SPU* spu)
{
    for(size_t i = 0; i < text->numLines; i++)
      {
        printf("info1 = %d ", *(spu->code + i * 2 * SHIFT));
        printf("info2 = %d ", *(spu->code + i * 2 * SHIFT + 1));
        printf("value = %lg\n", *(double*)(spu->code + i * 2 * SHIFT + SHIFT));
      }
}