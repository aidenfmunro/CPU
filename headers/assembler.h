#ifndef ASSEMBLER_H
#define ASSEMBLER_H

#include <inttypes.h>
#include "config.h"

const char RAX = 'a';

const char RBX = 'b';

const char RCX = 'c';

const char RDX = 'd';

const uint32_t MAX_LABELNAME_LENGTH = 16;

const uint32_t MAX_COMMAND_LENGTH   =  5;

const uint32_t MAX_LABEL_AMOUNT     = 64;

typedef int CommandCode;

typedef char RegNum;

ErrorCode Compile(const char* filename, const char* listingFileName);

#endif