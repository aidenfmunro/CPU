#include <stdio.h>
#include "spuconfig.h"
#include "assembler.h"

int main(void)
{
    Compile("comms.txt");

    RunProgram("code.bin");

}