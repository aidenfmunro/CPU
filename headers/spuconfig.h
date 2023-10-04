#ifndef SPUCONFIG_H
#define SPUCONFIG_H

#include "stackfuncs.h"
#include "textfuncs.h"

enum Instruction
{
    HLT = -1,
    IN = 0,
    PUSH = 1,
    ADD = 2,
    SUB = 3,
    MUL = 4,
    DIV = 5,
    SQRT = 6,
    SIN = 7,
    COS = 8,
    OUT = 9
};

void followInstructions(stack_t* stack, Text* text);

void Proccess(void);

#endif