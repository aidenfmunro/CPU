#ifndef SPU_H
#define SPU_H

#include "stackfuncs.h"
#include "textfuncs.h"

const int AMOUNT_OF_REGISTERS = 4;

const int AMOUNT_OF_RAM_SLOTS = 4096;

const int EXIT_CODE = -1;

const size_t RHX = 4;

const size_t RECT_WIDTH = 10;

const size_t RECT_HEIGHT = 20;

const size_t RAM_HEIGHT = 30;

const size_t RAM_WIDTH = 101;

const double DELAY_FACTOR = 0.8;

ErrorCode RunProgram(const char* filename);

#define GRAPHICS

#ifdef GRAPHICS
    #define GRAPHICS_ON(...) \
        __VA_ARGS__
#else
    #define GRAPHICS_ON(...)
#endif

#endif