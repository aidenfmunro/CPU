#include <stdio.h>
#include <string.h>
#include <math.h>
#include "assembler.h"
#include "textfuncs.h"
#include "utils.h"

#define ADD_CMD_FLAGS(flag) result |= (flag);

#define ASSIGN_CMD_ARG(arg, type, shift) *(type*)(bytecode + shift + *curPosition * 16) = (arg) // *(byte*)(*byt)

const uint32_t POSITION_SHIFT = 8; 

ErrorCode Compile(const char* filename)
{
    Text assemblyText = {};

    CreateText(&assemblyText, filename, NONE);
    
    size_t numLines = assemblyText.numLines;

    byte* bytecode = (byte*)calloc(numLines, sizeof(elem_t) * 2); // NOTE: command | argument

    // ----labels----

    Labels labels = {.count = 0};

    labels.label = (Label*)calloc(MAX_LABEL_AMOUNT, sizeof(Label)); // NOTE: max is half of the amount of lines

    //----------------

    CheckPointerValidation(bytecode);

    size_t curPosition = 0;

    for (size_t runNum = 0; runNum < 1; runNum++)
      {
        curPosition = 0;
        for (size_t index = 0; index < numLines; index++)
            proccessLine(&assemblyText, bytecode, index, &curPosition, &labels); 
      }  
    
    myOpen("code.bin", "wb", codebin);

    myWrite(bytecode, sizeof(byte), numLines * 2 * sizeof(elem_t), codebin);

    myClose(codebin);

    DestroyText(&assemblyText);

    free(bytecode);

    return OK;
}

const size_t LABEL_NOT_FOUND = -1;

size_t findLabel(Labels* labels, const char* labelName) // TODO: change size_t to a typedef
{
    for (size_t i = 0; i < labels->count; i++)
      {
        if (strncmp(labelName, labels->label[i].name, strlen(labelName)) == 0)
          {
            return labels->label[i].address;
          }
      }

    return LABEL_NOT_FOUND;
}

ErrorCode proccessLine(Text* assemblyText, byte* bytecode, size_t index, size_t* curPosition, Labels* labels)
{
    char* curLine = assemblyText->lines[index].string;

    char* commentPtr = strchr(curLine, int(';'));

    if (commentPtr) // TODO: seperate to functions
        *commentPtr = 0;

    if (StringIsEmpty(&assemblyText->lines[index])) // remake to char* 
        return OK;

    char* labelPtr = strchr(curLine, int(':'));

    if (labelPtr)
      {
        *labelPtr = '\0';

        labels->label[labels->count].address = *curPosition + 1; // TODO: write label function

        sscanf(curLine, "%s", labels->label[labels->count++].name);

        return OK;
      }  
  
    char command[MAX_COMMAND_LENGTH]     = "";

    size_t commandLength                 = 0;

    char labelName[MAX_LABELNAME_LENGTH] = "";


    if (sscanf(curLine, "%4s%n", command, &commandLength) != 1)
        return INCORRECT_SYNTAX;

                                                                    printf("command: %s\n", command);

    if (command[0] == 'j')
      {
        if (findLabel(labels, labelName) != LABEL_NOT_FOUND)
          {
          printf("label found! -> %ld", findLabel(labels, labelName));
          ASSIGN_CMD_ARG(findLabel(labels, labelName), size_t, sizeof(size_t)); 
          }       
      }

    #define DEF_COMMAND(name, num, argc, code)                                                                  \
      if (strncasecmp(#name, command, commandLength) == 0)                                                      \
        {                                                                                                       \
          *(bytecode + *curPosition * 16) |= CMD_ ## name;                                                      \ 
          *(bytecode + *curPosition * 16) |= parseArgument(curLine + commandLength + 1, curPosition, bytecode); \
        }                                                            

    #define DEF_JMP(name, num, sign, code)                                                                      \
      if (strncasecmp(#name, command, commandLength) == 0)                                                      \
        {                                                                                                       \
          *(bytecode + *curPosition * 16) |= CMD_ ## name;                                                      \
          *(size_t*)(bytecode + *curPosition * 16 + sizeof(size_t)) = findLabel(labels, labelName);             \
        }                                                           
    
    #include "commands.h"

    #undef DEF_COMMAND
    #undef DEF_JMP

    *curPosition += 1;

    return OK;                        
}

byte parseArgument(char* argument, size_t* curPosition, byte* bytecode)
{
    char result  = 0;

    char* openBracketPtr = strchr(argument, '[');
    char* closeBracketPtr = strchr(argument, ']');

    if (openBracketPtr || closeBracketPtr)
      {
        argument = closeBracketPtr + 1;
        ADD_CMD_FLAGS(ARG_FORMAT_RAM);
      }

    char regArg = 0;

    int check   = 0;

    if (sscanf(argument, "r%cx%n", &regArg, &check) == 1)
      {
        ADD_CMD_FLAGS(ARG_FORMAT_REG);
        ASSIGN_CMD_ARG(getRegisterNum(regArg), char, sizeof(char));

        argument += check;
      }
    
    char* plusPtr = strchr(argument, '+');

    if (plusPtr)
        argument = plusPtr + 1;
      
    elem_t value = POISON;

    if (sscanf(argument, "%lg%n", &value, &check) == 1)
      {
        ADD_CMD_FLAGS(ARG_FORMAT_IMMED);
        ASSIGN_CMD_ARG(value, elem_t, sizeof(elem_t));
      }
    
    
    
    return result;
}

CommandCode getCommandCode(const char* command, const size_t commandLength)
{
    #define DEF_COMMAND(name, num, argc, code) if (strncasecmp(#name, command, commandLength) == 0) {return CMD_ ## name;} 

    #define DEF_JMP(name, num, sign, code) if (strncasecmp(#name, command, commandLength) == 0) {return CMD_ ## name;}
    
    #include "commands.h"

    #undef DEF_COMMAND

    #undef DEF_JMP

    else
        return UNKNOWN_ASM_COMMAND;     
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

void printbytecode(Text* text, SPU* spu)
{
    for(size_t i = 0; i < text->numLines; i++)
      {
        printf("info1 = %d ", *(spu->code + i * 2 * SHIFT));
        printf("info2 = %d ", *(spu->code + i * 2 * SHIFT + 1));
        printf("value = %lg\n", *(double*)(spu->code + i * 2 * SHIFT + SHIFT));
      }
}