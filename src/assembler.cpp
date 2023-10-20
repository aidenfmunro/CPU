#include <stdio.h>
#include <string.h>
#include <math.h>
#include "assembler.h"
#include "textfuncs.h"
#include "utils.h"

ErrorCode Compile(const char* filename)
{
    Text assemblyText = {};

    CreateText(&assemblyText, filename, NONE);

    byte* bytecode = (byte*)calloc(assemblyText.numLines, sizeof(elem_t) * 2); // NOTE: command | argument

    CheckPointerValidation(bytecode);

    for (size_t position = 0; position < assemblyText.numLines; position++)
      {
        char* curLine        = assemblyText.lines[position].string;
        size_t curLineLength = assemblyText.lines[position].length;

        char commandCode  = 0; // NOTE: 00000000
        elem_t value = POISON;
        char command[5] = "";

        if (*curLine == ';') // NOTE: skip empty lines and comment lines
            continue;


        char* commentPtr = strchr(assemblyText.lines[position].string, int(';')); // NOTE: ignore comments ex.: "; Hello"

        if (commentPtr != NULL) // TODO: seperate to functions
            *commentPtr = '\0';

    
        size_t commandLength = 0;
        size_t argumentLength = 0;

        sscanf(curLine, "%s%n %lg", command, &commandLength, &value);

        char* argument = strtok(curLine + commandLength, " ");

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
            char* tempPtr = strchr(curLine, '+');

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

    fwrite(bytecode, sizeof(byte), assemblyText.numLines * 2 * sizeof(elem_t), codebin);

    fclose(codebin);

    DestroyText(&assemblyText);

    free(bytecode);

    return 0;
}

ErrorCode writeValue(const size_t position, const byte* bytecode, const elem_t value)
{
    CheckPointerValidation(bytecode);

    *(elem_t*)(bytecode + (2 * position + 1) * SHIFT) = value;

    return OK;
}

ErrorCode writeRegisterNum(const size_t position, const byte* bytecode, const char registerNum)
{
    CheckPointerValidation(bytecode);

    *(byte*)(bytecode + 2 * position * SHIFT + 1) = registerNum;

    return OK;
}

ErrorCode writeCommandArgs(const size_t position, const byte* bytecode, const char commandCode)
{
    CheckPointerValidation(bytecode);

    *(byte*)(bytecode + 2 * position * SHIFT) |= commandCode;

    return OK;
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

          return NON_EXIST_REGISTER;
          break;
      }
}

CommandCode getCommandCode(const char* command, const size_t commandLength)
{
    
    #define DEF_COMMAND(name, num, argc, code) if (strncasecmp(#name, command, commandLength) == 0) {return CMD_ ## name;}

    #include "commands.h"

    #undef DEF_COMMAND

    else
      {
        return UNKNOWN_ASM_COMMAND;
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