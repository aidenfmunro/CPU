#ifndef ASSEMBLER_H
#define ASSEMBLER_H

#include "spuconfig.h"

byte* Compile(Text* initialText, CPU* spu);

const int SHIFT = 8;

const char ARG_FORMAT_REG = (1 << 5);

const char ARG_FORMAT_IMMED = (1 << 4);

typedef int CommandCode;

typedef char RegNum;

RegNum getRegisterNum(const char argument);

CommandCode getCommandCode(const char* command, const size_t commandLength);

elem_t* writeValue(const size_t index, const byte* bytecode);

void printbytecode(Text* text, CPU* spu);

#endif