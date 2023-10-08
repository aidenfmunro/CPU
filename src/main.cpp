#include <stdio.h>
#include "spuconfig.h"
#include "assembler.h"

int main(void)
{
    CPU cpu = {};

    Text text = {};

    CreateText(&text, "comms.txt", NONE);

    cpu.code = Compile(&text);

    printbytecode(&text, &cpu);

}