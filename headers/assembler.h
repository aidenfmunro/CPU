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


const int SHIFT = 16;

const char ARG_FORMAT_IMMED = (1 << 5);

const char ARG_FORMAT_REG = (1 << 6);

const char ARG_FORMAT_RAM = (1 << 7);

typedef int CommandCode;

typedef char RegNum;

ErrorCode Compile(const char* filename);

size_t findLabel(Labels* labels, const char* labelName);

bool labelIsInitialized(Labels* labels, const char* labelName);

ErrorCode proccessLabel(char* curLine, Labels* labels, size_t* curPosition);

byte parseArgument(char* argument, size_t* curPosition, byte* bytecode, Labels* labels);

RegNum getRegisterNum(const char argument);

ErrorCode proccessLine(Text* assemblyText, byte* bytecode, size_t index, size_t* curPosition, Labels* lables);

CommandCode getCommandCode(const char* command, const size_t commandLength);

ErrorCode writeValue(const size_t index, const byte* bytecode, const elem_t value);

ErrorCode writeRegisterNum(const size_t position, const byte* bytecode, const char registerNum);

ErrorCode writeCommandArgs(const size_t position, const byte* bytecode, const char commandCode);

void printbytecode(Text* text, SPU* spu);

#endif