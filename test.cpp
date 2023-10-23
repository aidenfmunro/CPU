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