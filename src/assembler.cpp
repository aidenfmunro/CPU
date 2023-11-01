#include <stdio.h>
#include <string.h>
#include <math.h>
#include "assembler.h"
#include "config.h"
#include "textfuncs.h"
#include "stackfuncs.h"
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

  ErrorCode error;
};

size_t findLabel(Labels* labels, const char* labelName);

bool labelIsInitialized(Labels* labels, const char* labelName);

ErrorCode proccessLabel(char* curLine, Labels* labels, size_t* curPosition);

ArgRes parseArgument(FILE* listingFile, char* argument, size_t* curPosition, byte* bytecode, Labels* labels, size_t runNum);

RegNum getRegisterNum(const char symbol);

ErrorCode proccessLine(Text* assemblyText, FILE* listingFile, byte* bytecode, size_t index, size_t* curPosition, Labels* lables, size_t runNum);

ErrorCode parseReg(char* argument, ArgRes* arg);

ErrorCode parseImmed(char* argument, ArgRes* arg);

ErrorCode parseLabel(char* argument, ArgRes* arg, Labels* labels, size_t runNum);

ErrorCode parseImmedOrLabel(char* argument, ArgRes* arg, Labels* labels, size_t runNum);


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

#define RETURN_ERROR(error)                                                                                         \
do                                                                                                                  \
{                                                                                                                   \
    __typeof__(error) _error = error;                                                                               \
    if (_error)                                                                                                     \
        return _error;                                                                                              \
} while (0)

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

#define RETURN_ERROR_ARG(arg) if (arg.error != 0) return arg; 

ArgRes parseArgument(FILE* listingFile, char* argument, size_t* curPosition, byte* bytecode, Labels* labels, size_t runNum)
{
    ArgRes arg = {};

    arg.argType = 0;

    char* openBracketPtr  = strchr(argument, '[');
    char* closeBracketPtr = strchr(argument, ']');

    if (openBracketPtr && closeBracketPtr)
      {
        argument = openBracketPtr + 1;
        arg.argType |= ARG_FORMAT_RAM;

        *closeBracketPtr = '\0';
      }
    if (!(openBracketPtr || closeBracketPtr))
      {
        arg.error = SYNTAX_ERROR;
      }
    
    arg.error = parseReg(argument, &arg); // RETURN_ERROR_ARG(arg);
    printf("error code: %d \n", arg.error);
    
    if ((arg.argType & ARG_FORMAT_REG) != 0) 
      {WRITE_LISTING(fprintf(listingFile, "%5sr%cx", "", arg.regNum));}
    else 
      {WRITE_LISTING(fprintf(listingFile, "%5s---", ""));}
    
    char* plusPtr = strchr(argument, '+');

    if (plusPtr)
        argument = plusPtr + 1;

    arg.error = parseImmedOrLabel(argument, &arg, labels, runNum);
    printf("error code: %d %d \n", arg.error, __LINE__);

    WRITE_LISTING(fprintf(listingFile, "%5s%lg", "", arg.immed));
    
    return arg;
}

ErrorCode parseReg(char* argument, ArgRes* arg)
{
    char regArg = 0;

    int check   = 0;

    if (sscanf(argument, "r%c%n", &regArg, &check) == 1 && *(argument + check) == 'x')
      {
        char regNum = getRegisterNum(regArg);

        if (regNum == NON_EXIST_REGISTER) return NON_EXIST_REGISTER;

        arg->argType |= ARG_FORMAT_REG; 
        
        arg->regNum = regNum; 

        argument += check;
      }
    
    return OK;
}

ErrorCode parseImmedOrLabel(char* argument, ArgRes* arg, Labels* labels, size_t runNum)
{
    arg->error = parseImmed(argument, arg);

    if (arg->argType == 0)  
        arg->error = parseLabel(argument, arg, labels, runNum);
    
    return arg->error;
}

ErrorCode parseImmed(char* argument, ArgRes* arg)
{
    elem_t value = POISON;

    int check = 0;

    if (sscanf(argument, "%lg%n", &value, &check) == 1)
      {
        if (check != 0 && !isnan(value))
          {
            arg->immed = value;
            arg->argType |= ARG_FORMAT_IMMED; // what if label is the argument
          }
        else
          {
            return SYNTAX_ERROR; 
          }
      }

    return OK;
}

ErrorCode parseLabel(char* argument, ArgRes* arg, Labels* labels, size_t runNum)
{
    int check = 0;

    char labelName[MAX_LABELNAME_LENGTH] = "";

        sscanf(argument, "%s%n", labelName, &check);

        size_t labelAddress = findLabel(labels, labelName);

        if (check != 0) // if no labels still add argument!!!
          {
            arg->argType |= ARG_FORMAT_IMMED; 
            
            if (labelAddress != LABEL_NOT_FOUND)
                memcpy(&arg->immed, &labelAddress, sizeof(size_t));
            else if(labelAddress == LABEL_NOT_FOUND && runNum == 2)
                return NON_EXIST_LABEL;
          }    

    return OK;
}

RegNum getRegisterNum(const char symbol)
{
    switch(symbol)
      {
        case RAX:
          return 0;

        case RBX:
          return 1;

        case RCX:
          return 2;

        case RDX:
          return 3;
        
        default:
          return NON_EXIST_REGISTER;
      }
}


