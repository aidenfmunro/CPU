#ifndef SPU_H
#define SPU_H

#include "stackfuncs.h"
#include "textfuncs.h"

const int nREGS = 4;

const int nSLOTS = 2048;

const int EXIT_CODE = -1;

ErrorCode RunProgram(const char* filename);

#endif