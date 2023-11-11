#ifndef SPU_H
#define SPU_H

#include "stackfuncs.h"
#include "textfuncs.h"

const int AMOUNT_OF_REGISTERS = 4;

const int AMOUNT_OF_RAM_SLOTS = 2048;

const int EXIT_CODE = -1;

const size_t RHX = 4;

ErrorCode RunProgram(const char* filename);

#endif