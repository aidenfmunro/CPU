#include <stdio.h>
#include <string.h>
#include <math.h>
#include "assembler.h"
#include "textfuncs.h"
#include "utils.h"
#include "spuconfig.h"
#include "dsl.h"

#define ADD_CMD_FLAGS(flag) result |= (flag);

#define ASSIGN_CMD_ARG(arg, type, shift) *(type*)(bytecode + shift + *curPosition * 16) = (arg)

const uint32_t POSITION_SHIFT = 8;

ErrorCode Compile(const char* filename)
{
    Text assemblyText = {};

    CreateText(&assemblyText, filename, NONE);
    
    size_t numLines = assemblyText.numLines;

    byte* bytecode = (byte*)calloc(numLines, sizeof(elem_t) * 2); // TODO: calloc return macro/function check

    // ----labels----

    Labels labels = {.count = 0};

    labels.label = (Label*)calloc(MAX_LABEL_AMOUNT, sizeof(Label)); // NOTE: max is half of the amount of lines

    //----------------

    CheckPointerValidation(bytecode); // TODO: mycalloc to check for return value

    size_t curPosition = 0;

    for (size_t runNum = 0; runNum < 1; runNum++) // TODO: change runnum to 2
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

ErrorCode proccessLabel(char* curLine, Labels* labels, size_t* curPosition)
{
    char labelName[MAX_LABELNAME_LENGTH] = "";

    sscanf(curLine, "%s", labelName);

    printf("<%s> yes/no: %d\n", labelName, findLabel(labels, labelName));

    if (findLabel(labels, labelName) == LABEL_NOT_FOUND)
      {
        labels->label[labels->count].address = *curPosition + 1;

        memcpy(labels->label[labels->count++].name, labelName, strlen(labelName));

        return OK;
      }

    printf("label <%s> already exists!\n", labelName);

    return REPEATING_LABEL;    
}

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

ErrorCode proccessLine(Text* assemblyText, byte* bytecode, size_t index, size_t* curPosition, Labels* labels) // TODO: input const char*
{
    char* curLine = assemblyText->lines[index].string;

    if (StringIsEmpty(&assemblyText->lines[index])) // ??? remake to char* 
        return OK;

    char* commentPtr = strchr(curLine, int(';'));

    if (commentPtr) // TODO: seperate to functions
        *commentPtr = 0;

    char* labelPtr = strchr(curLine, int(':'));

    if (labelPtr)
      {
        *labelPtr = '\0';

        return proccessLabel(curLine, labels, curPosition);
      }  
  
    char command[MAX_COMMAND_LENGTH]     = "";

    size_t commandLength                 = 0;

    char labelName[MAX_LABELNAME_LENGTH] = "";

    if (sscanf(curLine, "%4s%ln", command, &commandLength) != 1)
        return INCORRECT_SYNTAX;
    printf("command: %s\n", command);

    printf("result: %d\n",   parseArgument(curLine + commandLength + 1,  \
                                                           curPosition,                  \
                                                           bytecode,                     \
                                                           labels));

    #define DEF_COMMAND(name, num, argc, code)                                           \
      if (strncasecmp(#name, command, commandLength) == 0)                               \
        {                                                                                \
          *(bytecode + *curPosition * 16) |= CMD_ ## name ||                             \
                                             parseArgument(curLine + commandLength + 1,  \
                                                           curPosition,                  \
                                                           bytecode,                     \
                                                           labels);                      \
        }                                                            
        
    #include "commands.h"

    #undef DEF_COMMAND

    *curPosition += 1;

    return OK;                        
}

byte parseArgument(char* argument, size_t* curPosition, byte* bytecode, Labels* labels)
{
    char result  = 0; // TODO: change result name

    char* openBracketPtr  = strchr(argument, '[');
    char* closeBracketPtr = strchr(argument, ']');

    if (openBracketPtr && closeBracketPtr)
      {
        argument = closeBracketPtr + 1;
        ADD_CMD_FLAGS(ARG_FORMAT_RAM);
      }

    char regArg = 0;

    int check   = 0;

    if (sscanf(argument, "r%cx%n", &regArg, &check) == 1)
      {
        ADD_CMD_FLAGS(ARG_FORMAT_REG);

        char regNum = regArg - 'a' + 1;

        printf(" %c", regNum);

        ASSIGN_CMD_ARG(regNum, char, sizeof(char));

        argument += check;
      }
    
    char* plusPtr = strchr(argument, '+');

    if (plusPtr)
        argument = plusPtr + 1;
      
    elem_t value = POISON;

    if (sscanf(argument, "%lg%n", &value, &check) == 1)
      {
        if (check != 0)
          {
            printf(" value = %lg\n", value);
            ADD_CMD_FLAGS(ARG_FORMAT_IMMED);
            ASSIGN_CMD_ARG(value, elem_t, sizeof(elem_t));
          }
      }
    
    if (result == 0)
      {
        char labelName[MAX_LABELNAME_LENGTH] = "";

        sscanf(argument, "%s%n", labelName, &check);

        size_t labelAddress = findLabel(labels, labelName);

        if (check != 0 && labelAddress != LABEL_NOT_FOUND)
          {
            printf("label address: %ld\n", labelAddress);
            ASSIGN_CMD_ARG(labelAddress, size_t, sizeof(size_t));
          }
      }
    
    return result;
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