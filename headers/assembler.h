#ifndef ASSEMBLER_H
#define ASSEMBLER_H

#include "spuconfig.h"

const uint32_t MAX_LABELNAME_LENGTH = 16;

const uint32_t MAX_COMMAND_LENGTH   =  5;

const uint32_t MAX_LABEL_AMOUNT     = 64;

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

const int SHIFT = 8;

const char ARG_FORMAT_IMMED = (1 << 5);

const char ARG_FORMAT_REG = (1 << 6);

const unsigned char ARG_FORMAT_RAM = (1 << 7);

typedef int CommandCode;

typedef char RegNum;

ErrorCode Compile(const char* filename, const char* listingFileName);

size_t findLabel(Labels* labels, const char* labelName);

bool labelIsInitialized(Labels* labels, const char* labelName);

ErrorCode proccessLabel(char* curLine, Labels* labels, size_t* curPosition);

ArgRes parseArgument(FILE* listingFile, char* argument, size_t* curPosition, byte* bytecode, Labels* labels, size_t runNum);

RegNum getRegisterNum(const char argument);

ErrorCode proccessLine(Text* assemblyText, FILE* listingFile, byte* bytecode, size_t index, size_t* curPosition, Labels* lables, size_t runNum);


#endif