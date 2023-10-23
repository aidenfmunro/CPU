#include <stdio.h>

int main(int argc, char* argv[])
{
    char regsym[2] = "";
    char command[5] = "";
    char line[20] = "    push rax";
    int count = sscanf(line, "%s r%1[a-d]x", command, regsym);


    printf("%d, %s, %c\n", count, command, regsym[0]);
    return 0;
}


push 20
pop [0]

push [0]

push [rbx + 10]

// RAM - random: read write
// ROM - read only memory
