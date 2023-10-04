#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int main(void)
{

    FILE* fp = fopen("comms.txt", "rb");
    double a = 0;
    char* b;

    fscanf(fp, "%s %lg", b, &a);
    char* line = "   -100";
    char* end = NULL;
    long num = strtol(line, &end, 10);
    printf("%ld\n", num);

    printf("%s\n", b);
    printf("%lg\n", a);


}