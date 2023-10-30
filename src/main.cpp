#include <stdio.h>
#include "spuconfig.h"
#include "assembler.h"
#include "utils.h"

int main(void)
{
    Compile("circle.txt", "listing.txt");

    RunProgram("code.bin");
}