#include <stdio.h>
#include <string.h>
#include <math.h>
#include "assembler.h"
#include "config.h"
#include "textfuncs.h"
#include "utils.h"
#include "spuconfig.h"

#define ADD_CMD_FLAGS(flag) result |= (flag);

#define ASSIGN_CMD_ARGS(args) *(bytecode + *curPosition * 16) |= (args) 

#define ASSIGN_CMD_ARG(arg, type, shift) *(type*)(bytecode + shift + *curPosition * 16) = (arg)

#define freeEverything DestroyText(&assemblyText); free(bytecode); free(labels.label)

#define WRITE_LISTING(...) if (runNum == 2) __VA_ARGS__

const uint32_t POSITION_SHIFT = 8;

ErrorCode Compile(const char* filename, const char* listingFileName)
{
    Text assemblyText = {};

    CreateText(&assemblyText, filename, NONE);
    
    size_t numLines = assemblyText.numLines;

    byte* bytecode = (byte*)calloc(numLines, sizeof(elem_t) * 2); // TODO: calloc return macro/function check

    Labels labels = {.count = 0};

    labels.label = (Label*)calloc(MAX_LABEL_AMOUNT, sizeof(Label));

    CheckPointerValidation(bytecode); // TODO: mycalloc to check for return value

    myOpen("listing.txt", "w", listingFile);

    size_t curPosition = 0;

    for (size_t runNum = 1; runNum < 3; runNum++) // TODO: change runnum to 2
      {
        curPosition = 0;

        WRITE_LISTING(fprintf(listingFile, "%s\nAuthor: Aiden Munro\nVersion: 2.0\n\nAll labels:\n\n", getTime()));

        for (size_t i = 0; i < labels.count; i++)
          {
            WRITE_LISTING(fprintf(listingFile, "\t\t%8s\t0x%08lX\t(%ld)\n", labels.label[i].name, labels.label[i].address * SHIFT * 2, labels.label[i].address));
          }

        WRITE_LISTING(fprintf(listingFile, "\n%5s%14s%17s%8s%11s\n",
                                           "Line:", "Address:", "Cmd:", "Reg:", "Value:"));

        for (size_t index = 0; index < numLines; index++)
            proccessLine(&assemblyText, listingFile, bytecode, index, &curPosition, &labels, runNum); 
      }  
    
    myOpen("code.bin", "wb", codebin);

    myWrite(bytecode, sizeof(byte), numLines * 2 * sizeof(elem_t), codebin);

    myClose(codebin);

    myClose(listingFile);

    freeEverything;

    return OK;
}

const size_t LABEL_NOT_FOUND = -1;

ErrorCode proccessLabel(char* curLine, Labels* labels, size_t* curPosition)
{
    CheckPointerValidation(labels);

    char labelName[MAX_LABELNAME_LENGTH] = "";

    sscanf(curLine, "%s", labelName);

    if (findLabel(labels, labelName) == LABEL_NOT_FOUND)
      {
        labels->label[labels->count].address = *curPosition - 1; // !!!

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

    ON_DEBUG(printf("command: %s\n", command));

    #define DEF_COMMAND(name, num, argc, code)                                                                              \
      if (strcasecmp(#name, command) == 0)                                                                                  \
        {                                                                                                                   \
          WRITE_LISTING(fprintf(listingFile, "%5ld %5s[0x%016lX] %4s", *curPosition, "", *curPosition * SHIFT * 2, command));\
          ASSIGN_CMD_ARGS(parseArgument(listingFile, curLine + commandLength + 1,                                           \
                                                        curPosition,                                                        \
                                                           bytecode,                                                        \
                                                             labels,                                                        \
                                                              runNum));                                                     \
                                                                                                                            \
          if (runNum == 1)                                                                                                  \
              ASSIGN_CMD_ARGS(CMD_ ## name);                                                                                \
                                                                                                                            \
          *curPosition += 1;                                                                                                \
        }                                                            
        
    #include "commands.h"

    #undef DEF_COMMAND

    return OK;                        
}

byte parseArgument(FILE* listingFile, char* argument, size_t* curPosition, byte* bytecode, Labels* labels, size_t runNum)
{
    CheckPointerValidation(argument);
    CheckPointerValidation(bytecode);
    CheckPointerValidation(labels);

    char result  = 0; // TODO: change result name

    int check    = 0;

    char* openBracketPtr  = strchr(argument, '[');
    char* closeBracketPtr = strchr(argument, ']');

    if (openBracketPtr && closeBracketPtr)
      {
        argument = closeBracketPtr + 1;
        if (runNum == 1) ADD_CMD_FLAGS(ARG_FORMAT_RAM);
      }

    char regArg = 0;

    if (sscanf(argument, "r%c%n", &regArg, &check) == 1 && *(argument + check) == 'x')
      {
        ADD_CMD_FLAGS(ARG_FORMAT_REG);

        char regNum = regArg - 'a' + 1;

        ON_DEBUG(printf(" %c", regNum));

        if (runNum == 1) ASSIGN_CMD_ARG(regNum, char, sizeof(char));

        argument += check;
      }
    else
      {
        check = 0;
        regArg = 0;
      }
    
    if (regArg != 0) {WRITE_LISTING(fprintf(listingFile, "%5sr%cx", "", regArg));} else {WRITE_LISTING(fprintf(listingFile, "%5s---", ""));}
    
    char* plusPtr = strchr(argument, '+');

    if (plusPtr)
        argument = plusPtr + 1;
      
    elem_t value = POISON;

    if (sscanf(argument, "%lg%n", &value, &check) == 1)
      {
        if (check != 0)
          {
            WRITE_LISTING(fprintf(listingFile, "%5s0x%08lX", "", value));

            if (runNum == 1) ADD_CMD_FLAGS(ARG_FORMAT_IMMED);
            if (runNum == 1) ASSIGN_CMD_ARG(value, elem_t, sizeof(elem_t));
          }
        else
          {
            WRITE_LISTING(fprintf(listingFile, "%5s----------", ""));
          }
      }
    
    if (result == 0)
      {
        char labelName[MAX_LABELNAME_LENGTH] = "";

        sscanf(argument, "%s%n", labelName, &check);

        size_t labelAddress = findLabel(labels, labelName);

        if (check != 0 && labelAddress != LABEL_NOT_FOUND)
          {
            ON_DEBUG(printf("label address: %ld\n", labelAddress));

            ASSIGN_CMD_ARG(labelAddress, size_t, sizeof(size_t));

            WRITE_LISTING(fprintf(listingFile, "%5s0x%08lX", "", labelAddress * SHIFT * 2));
          }
      }
    
    WRITE_LISTING(fprintf(listingFile, "\n"));

    return result;
}