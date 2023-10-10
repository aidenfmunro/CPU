#include <stdio.h>
#include "spuconfig.h"
#include "assembler.h"

int main(void)
{
    SPU spu = {};

    Text text = {};

    CreateText(&text, "comms.txt", NONE);

    Compile(&text);

    RunProgram("code.bin", &spu);

}