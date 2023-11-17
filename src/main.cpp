#include <stdio.h>

#include "disassembler.h"
#include "assembler.h"
#include "spu.h"
#include "utils.h"

int main(int argc, char* argv[])
{
    if (argc == 4)
    {
        Compile(argv[1], argv[2], argv[3]);

        RunProgram(argv[3]);
    }
}