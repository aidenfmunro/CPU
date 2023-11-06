#ifndef ASSEMBLER_H
#define ASSEMBLER_H

#include <inttypes.h>
#include "config.h"

/*
	TODO: To add a new register 'x' to your system
	you have to at least:

	1) add cons char RXX = 'x' in assembler.h
	2) change nREGS in spu.h

	It's easy to forget 2).
	Can't you put process of adding new register in one action?
*/
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