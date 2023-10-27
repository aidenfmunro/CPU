#include <stdio.h>
#include "spuconfig.h"
#include "assembler.h"
#include "utils.h"

int main(void)
{
    // printf("%d\n", isPointerValid(NULL));
    
    Compile("comms.txt", "listing.txt");

    RunProgram("code.bin");

}