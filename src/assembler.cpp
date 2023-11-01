#include <stdio.h>
#include <string.h>
#include <math.h>
#include "assembler.h"
#include "config.h"
#include "textfuncs.h"
#include "utils.h"
#include "enum.h"

struct Label
{
    size_t address;
    char name[MAX_LABELNAME_LENGTH];
};

struct Labels
{
    size_t count;
    Label* label;
};

struct ArgRes
{
  char argType;
  char regNum;

  double immed;
};

size_t findLabel(Labels* labels, const char* labelName);

bool labelIsInitialized(Labels* labels, const char* labelName);

ErrorCode proccessLabel(char* curLine, Labels* labels, size_t* curPosition);

ArgRes parseArgument(FILE* listingFile, char* argument, size_t* curPosition, byte* bytecode, Labels* labels, size_t runNum);

RegNum getRegisterNum(const char argument);

ErrorCode proccessLine(Text* assemblyText, FILE* listingFile, byte* bytecode, size_t index, size_t* curPosition, Labels* lables, size_t runNum);

#define freeEverything DestroyText(&assemblyText); free(bytecode); free(labels.label)

#define WRITE_LISTING(...) if (runNum == 2) __VA_ARGS__

ErrorCode Compile(const char* filename, const char* listingFileName)
{
    Text assemblyText = {};

    CreateText(&assemblyText, filename, NONE);
    
    size_t numLines = assemblyText.numLines;

    byte* bytecode = (byte*)calloc(numLines, sizeof(elem_t) * 2); // TODO: calloc return macro/function check

    Labels labels = {.count = 0};

    labels.label = (Label*)calloc(MAX_LABEL_AMOUNT, sizeof(Label));

    CheckPointerValidation(bytecode); // TODO: mycalloc to check for return value

    myOpen(listingFileName, "w", listingFile);

    size_t curPosition = 0;

    int error = OK;

    for (size_t runNum = 1; runNum < 3; runNum++)
      {
        curPosition = 0;

        WRITE_LISTING(fprintf(listingFile, "%s\nAuthor: Aiden Munro\nVersion: 2.5\n\nAll labels:\n\n", getTime())); // TODO: split into two lines

        for (size_t i = 0; i < labels.count; i++)
          {
            WRITE_LISTING(fprintf(listingFile, "\t\t%8s\t0x%08lX\n", labels.label[i].name, labels.label[i].address));
          }

        WRITE_LISTING(fprintf(listingFile, "\n%5s%14s%17s%8s%11s\n",
                                           "Line:", "Address:", "Cmd:", "Reg:", "Value:"));

        for (size_t index = 0; index < numLines; index++)
            error = proccessLine(&assemblyText, listingFile, bytecode, index, &curPosition, &labels, runNum); 
      }  
    
    myOpen("code.bin", "wb", codebin);

    myWrite(bytecode, sizeof(byte), numLines * 2 * sizeof(elem_t), codebin);

    myClose(codebin);

    myClose(listingFile);

    freeEverything;

    return error;
}

const size_t LABEL_NOT_FOUND = -1;

ErrorCode proccessLabel(char* curLine, Labels* labels, size_t* curPosition)
{
    CheckPointerValidation(labels);

    char labelName[MAX_LABELNAME_LENGTH] = "";

    sscanf(curLine, "%s", labelName);

    if (findLabel(labels, labelName) == LABEL_NOT_FOUND)
      {
        labels->label[labels->count].address = *curPosition;

        memcpy(labels->label[labels->count++].name, labelName, strlen(labelName));

        return OK;
      }

    ON_DEBUG(printf("label <%s> already exists!\n", labelName));

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

ErrorCode proccessLine(Text* assemblyText, FILE* listingFile, byte* bytecode, size_t index, size_t* curPosition, Labels* labels, size_t runNum) // TODO: input const char*
{
    CheckPointerValidation(assemblyText);
    CheckPointerValidation(bytecode);
    CheckPointerValidation(labels);

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

    #define DEF_COMMAND(name, num, argc, code)                                                                              \
      if (strcasecmp(#name, command) == 0)                                                                                  \
        {                                                                                                                   \
          WRITE_LISTING(fprintf(listingFile, "%5ld %5s[0x%016lX] %4s", *curPosition, "", *curPosition, command));           \
          ArgRes arg = parseArgument(listingFile, curLine + commandLength + 1,                                              \
                                                        curPosition,                                                        \
                                                           bytecode,                                                        \
                                                             labels,                                                        \
                                                              runNum);                                                      \
          *(bytecode + *curPosition) |= CMD_ ## name;                                                                       \
          if (argc)                                                                                                         \
            {                                                                                                               \
              *(bytecode + *curPosition) |= arg.argType;                                                                    \
            }                                                                                                               \
          *curPosition += sizeof(char);                                                                                     \
          if (argc)                                                                                                         \
            {                                                                                                               \
              if (arg.argType & ARG_FORMAT_REG)                                                                             \
                {                                                                                                           \
                  memcpy(bytecode + *curPosition, &arg.regNum, sizeof(char));                                               \
                  *curPosition += sizeof(char);                                                                             \
                }                                                                                                           \
              if (arg.argType & ARG_FORMAT_IMMED)                                                                           \
                {                                                                                                           \  
                  memcpy(bytecode + *curPosition, &arg.immed, sizeof(double));                                              \
                  *curPosition += sizeof(double);                                                                           \
                }                                                                                                           \
            }                                                                                                               \
             WRITE_LISTING(fprintf(listingFile, "\n"));                                                                     \
        }                                                                                                                   \                                                        
        
    #include "commands.h"

        else
          return INCORRECT_SYNTAX;

    #undef DEF_COMMAND    

    return OK;                        
}

ArgRes parseArgument(FILE* listingFile, char* argument, size_t* curPosition, byte* bytecode, Labels* labels, size_t runNum)
{
    ArgRes arg = {};

    int check    = 0;

    arg.argType = 0;

    char* openBracketPtr  = strchr(argument, '[');
    char* closeBracketPtr = strchr(argument, ']');

    if (openBracketPtr && closeBracketPtr)
      {
        argument = openBracketPtr + 1;
        arg.argType |= ARG_FORMAT_RAM;
      }

    char regArg = 0;

    if (sscanf(argument, "r%c%n", &regArg, &check) == 1 && *(argument + check) == 'x')
      {
        char regNum = regArg - 'a';

        arg.argType |= ARG_FORMAT_REG; 
        
        arg.regNum = regNum; 

        ON_DEBUG(printf(" %c", regNum));

        argument += check;
      }
    else
      {
        check = 0; regArg = 0;
      }
    
    if (regArg != 0) 
      {WRITE_LISTING(fprintf(listingFile, "%5sr%cx", "", regArg));}
    else 
      {WRITE_LISTING(fprintf(listingFile, "%5s---", ""));}
    
    char* plusPtr = strchr(argument, '+');

    if (plusPtr)
        argument = plusPtr + 1;
      
    elem_t value = POISON;

    if (sscanf(argument, "%lg%n", &value, &check) == 1)
      {
        if (check != 0)
          {
            WRITE_LISTING(fprintf(listingFile, "%5s%lg", "", value));
            arg.argType |= ARG_FORMAT_IMMED; arg.immed = value;
          }
        else
          {
            WRITE_LISTING(fprintf(listingFile, "%5s----------", ""));
          }
      }

    check = 0;
    
    if (arg.argType == 0)
      {
        char labelName[MAX_LABELNAME_LENGTH] = "";

        sscanf(argument, "%s%n", labelName, &check);

        size_t labelAddress = findLabel(labels, labelName);

        if (check != 0) // if no labels still add argument!!!
          {
            ON_DEBUG(printf("label address: %ld\n", labelAddress));

            arg.argType |= ARG_FORMAT_IMMED; 
            
            if (labelAddress != LABEL_NOT_FOUND)
                memcpy(&arg.immed, &labelAddress, sizeof(size_t));

            WRITE_LISTING(fprintf(listingFile, "%5s%ld", "", labelAddress));
          }
      }

    return arg;
}