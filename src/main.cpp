#include <stdio.h>
#include "spu.h"
#include "assembler.h"
#include "utils.h"

int main(void)
{
    Compile("qequation.txt", "listing.txt");

    RunProgram("code.bin");
}