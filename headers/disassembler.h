#ifndef DISASM_H
#define DISASM_H

#include <stdio.h>
#include "config.h"
#include "enum.h"
#include "utils.h"
#include "textfuncs.h"

ErrorCode Disassemble(const char* filenameIn, const char* filenameOut);

#endif