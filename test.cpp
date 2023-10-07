#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>

int getSum(char* command);

int main(void)
{

char* line = "    pop";

char comm[5] = "";
int len = 0;

sscanf(line, "%s%n", comm, &len);
printf("%s\n %d", comm, len);
}

int getSum(char* command)
{
    int sum = 0;
 for (int i = 0; i < strlen(command); i++)
    sum += int(command[i]);
    return sum;
}
// 448 push
// 335 pop