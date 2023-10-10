#ifndef ASSEMBLER_H
#define ASSEMBLER_H

#include "spuconfig.h"

byte* Compile(Text* initialText);

const int SHIFT = 8;

const char ARG_FORMAT_REG = (1 << 5);

const char ARG_FORMAT_IMMED = (1 << 4);

typedef int CommandCode;

typedef char RegNum;

RegNum getRegisterNum(const char argument);

CommandCode getCommandCode(const char* command, const size_t commandLength);

void writeValue(const size_t index, const byte* bytecode, const elem_t value);

void writeRegisterNum(const size_t position, const byte* bytecode, const char registerNum);

void writeCommandArgs(const size_t position, const byte* bytecode, const char commandCode);

void printbytecode(Text* text, SPU* spu);

#endif