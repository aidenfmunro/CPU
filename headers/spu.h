#ifndef SPU_H
#define SPU_H

#include "stackfuncs.h"
#include "textfuncs.h"

const int nREGS = 4;

const int nSLOTS = 2048;

const int EXIT_CODE = -1;

const char ARG_FORMAT_CMD = 0x1F;

ErrorCode RunProgram(const char* filename);

#endif