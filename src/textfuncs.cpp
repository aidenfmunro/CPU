//! @file
//! @author Aiden Munro

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <ctype.h>
#include "textfuncs.h"
#include "stackfuncs.h"

void CreateText(Text* text, const char* filename, size_t sortmode)
{
    myAssert(text, NULLPTR);

    text->size     = getSize(filename);
    text->buffer   = parseBuf(text, filename);
    text->numLines = countLines(text);
    text->lineptrs = getLinePointers(text);
    text->lines    = getLines(text);

    generalSort(text, sortmode);
}

void AppendText(Text* text, const char* filename)
{
  myAssert(text, NULLPTR);

  FILE* fp = fopen(filename, "a");

  for (size_t i = 0; i < text->numLines; i++)
    {
      char* str = getLine(text, i);
      if (*str != '\r') //skip lines with spaces
          fputs(str, fp);
    }
  fputs("------------------------------------------------\n", fp);

  fclose(fp);
}

void DestroyText(Text* text)
{
  free((void*)text->lineptrs);
  free((void*)text->buffer);
  free((void*)text->lines);
}

char* const* getLinePointers(Text *text)
{
    myAssert(text, NULLPTR);

    char** lineptrs = (char**)calloc(text->numLines, sizeof(char*));
    
    *lineptrs = text->buffer;

    char* textptr = strchr(text->buffer, '\n');

    while (textptr != NULL)
      {
        *textptr = '\0';
        lineptrs++;
        *lineptrs = textptr + 1;
        textptr = strchr(textptr + 1, '\n'); 
      }

    lineptrs -= (text->numLines - 1);

    return (char* const*)lineptrs;
}

char* parseBuf(Text* text, const char* filename)
{   
    myAssert(text, NULLPTR);

    FILE* fp = fopen(filename, "rb");

    char* buffer = (char*)calloc(text->size + 2, sizeof(char));
    fread(buffer, sizeof(char), text->size, fp);
    buffer[text->size] = '\0';
  
    fclose(fp);

    return buffer;
}

size_t getSize(const char* filename)
{
    myAssert(filename, NULLPTR);

    struct stat stats = {};
    stat(filename, &stats);

    return stats.st_size;
}

size_t countLines(const Text* text)
{
    myAssert(text, NULLPTR);

    size_t lines = 1;
    for (size_t i = 0; i < text->size; i++)
        if (text->buffer[i] == '\n')
            lines++;

    return lines;
}

char* getLine(Text* text, size_t numLine)
{
    myAssert(numLine < text->numLines, OUTOFBOUNDS);

    return text->lines[numLine].string;
}

struct Line* getLines(Text* text)
{
    myAssert(text, NULLPTR);

    Line* lines = (struct Line*)calloc(text->numLines, sizeof(Line));

    for (size_t i = 0; i < text->numLines; i++)
      {
        lines[i].length = strlen(text->lineptrs[i]);
        lines[i].string = text->lineptrs[i];
      }
    
    return lines;
}

void generalSort(Text* text, size_t sortmode)
{
    myAssert(text, NULLPTR);

    switch (sortmode)
    {
    case FORWARDS:
        quickSort((void*)text->lines, 0, text->numLines - 1, sizeof(struct Line), compareStringForw);
        break;
    
    case BACKWARDS:
        quickSort((void*)text->lines, 0, text->numLines - 1, sizeof(struct Line), compareStringBack);
        break;

    case NONE:
        break;
    
#define by    
#define segfault by default

    segfault:
        *(int*)0 = 0;
        break;
    }
}

void bubbleSort(void* array, size_t numElems, const size_t elemSize, compareFunc_t compareFunc)
{
    myAssert(array, NULLPTR);
    myAssert(numElems, NULLPTR);
    myAssert(elemSize, NULLPTR);

    for (size_t i = 0; i < numElems - 1; i++)
      for (size_t j = 0; j < numElems - 1 - i; j++)
        {
          if (compareFunc(array + j * elemSize, array + (j + 1) * elemSize) > 0)
            {
              SWAP(array + j * elemSize,  array + (j + 1) * elemSize, elemSize);
            }
        }             
}

void quickSort(void* array, int start, int end, size_t elemSize, compareFunc_t compareFunc)
{
    myAssert(array, NULLPTR);
    myAssert(elemSize, NULLPTR);

    if (start >= end)
        return;

    int point = partition(array, start, end, elemSize, compareFunc);

    quickSort(array, start, point - 1, elemSize, compareFunc);
    quickSort(array, point + 1, end, elemSize, compareFunc);

}

int partition(void* array, int left, int right, size_t elemSize, compareFunc_t compareFunc)
{
  myAssert(array, NULLPTR);

  int pivot = left;

  while (left < right)
    {
      while (compareFunc(array + left * elemSize, array + pivot * elemSize) < 0 && left < right)
          left++;

      while (compareFunc(array + right * elemSize, array + pivot * elemSize) >= 0 && left < right)
          right--;

      if (left < right)
          SWAP(array + left * elemSize, array + right * elemSize, elemSize);
    }
    
    SWAP(array + pivot * elemSize, array + right * elemSize, elemSize);

    return right;
}


int compareStringForw(const void* a, const void* b)
{
    myAssert(a, NULLPTR);
    myAssert(b, NULLPTR);
    
    struct Line* str1 = (Line*)a;
    struct Line* str2 = (Line*)b;

    char* strptr1 = str1->string;
    char* strptr2 = str2->string;

    char* fixptr1 = str1->string + str1->length - 1;
    char* fixptr2 = str2->string + str2->length - 1; 

    do
      {
        while(!isalpha(*strptr1))
            strptr1++;
        while(!isalpha(*strptr2))
            strptr2++;
        if (*strptr1 != *strptr2)
            return *strptr1 - *strptr2;
        else
          {
            strptr1++;
            strptr2++;
          }  
      } while(strptr1 < fixptr1 && strptr2 < fixptr2);
    
    return 0;
}

int compareStringBack(const void* a, const void* b)
{
    myAssert(a, NULLPTR);
    myAssert(b, NULLPTR);

    struct Line* str1 = (Line*)a;
    struct Line* str2 = (Line*)b;

    char* strptr1 = str1->string + str1->length - 1;
    char* strptr2 = str2->string + str2->length - 1; 

    char* fixptr1 = str1->string;
    char* fixptr2 = str2->string; 
  
    do 
      {
        while (!isalpha(*strptr1))
            strptr1--;
        while (!isalpha(*strptr2))
            strptr2--;
        if (*strptr1 != *strptr2)
            return *strptr1 - *strptr2;
        else
          {
            strptr1--;
            strptr2--;
          }
      } while(strptr1 > fixptr1 && strptr2 > fixptr2);
    
    return 0;
} 

size_t CheckFile(const char* filename)
{
    myAssert(filename, NULLPTR);

    FILE* fp = fopen(filename, "rb");
        if (fp == NULL)
          {
            printf("Unable to open file: \"%s\"\n", filename);
            return INCORRECT;
          }

    fclose(fp);

    const char* dotptr = strchr(filename, (int)('.')) + 1;

    if(strcmp("txt", dotptr) || strcmp("rtf", dotptr))
        return CORRECT;
      
    return INCORRECT;
}