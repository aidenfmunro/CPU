#include <stdio.h>
#include "spu.h"
#include "assembler.h"
#include "disassembler.h"
#include "utils.h"

int main(void)
{
    Compile("simple.txt", "listing.txt");

    RunProgram("code.bin");

    // Disassemble("code.bin", "disasm_res");
}