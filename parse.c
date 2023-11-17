#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "headers/textfuncs.h"

int main(int argc, char* argv[])
{
    FILE* textIn  = fopen("badappleascii.txt", "r");
    FILE* textOut = fopen("badappleasm.txt", "w");

    int linesInFrame    = 30;
    int amountOfFrames  = 2180;
    int amountOfSymbols = 101;

    char* line = (char*)calloc(sizeof(char), 2 * amountOfSymbols);

    for (size_t y = 0; y < amountOfFrames; y++)
    {
        for (size_t i = 0; i < linesInFrame; i++)
        {            
            memset(line, '.', amountOfSymbols);

            // fgets(line, amountOfSymbols, textIn);

            fscanf(textIn, "%[^\n]%*c", line);

            for (size_t j = 0; j < amountOfSymbols; j++)
            {
                if (line[j] != ' ' && line[j] != '.')
                    fprintf(textOut, "push 1\npop [%lu]\n", i * amountOfSymbols + j);
                else
                    fprintf(textOut, "push 0\npop [%lu]\n", i * amountOfSymbols + j);
            }
        }

        fprintf(textOut, "scrn\n");
    }

    free(line);
    fprintf(textOut, "hlt");

    fclose(textIn);
    fclose(textOut);

    return 0;
}
