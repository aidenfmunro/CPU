#include <stdio.h>
#include "disassembler.h"
#include "assembler.h"
#include "spu.h"
#include "utils.h"

int main(void)
{
    Compile("qequation.txt", "listing.txt");

    RunProgram("code.bin");

    // Disassemble("code.bin", "disasm_res");
}