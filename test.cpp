#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>


int main(void)
{

char line[20] = "pop     rax";
char* command = strtok(line + 3, " ");
printf("%s", command);
}
