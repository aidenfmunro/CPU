//! @file
//! @author Aiden Munro

#ifndef TEXTFUNCS_H
#define TEXTFUNCS_H

#include <stdlib.h>

typedef int(*compareFunc_t) (const void*, const void* );

/**
 * 
 * Enum for error codes.
 * 
 */



const int OUTOFBOUNDS = 1;


/**
 * 
 * Enum for sort modes.
 * 
 */

enum SORT
{
    BACKWARDS = 0,
    FORWARDS  = 1,
    NONE      = 2
};

/**
 * 
 * Enum for file correction codes.
 * 
 */

enum CORRECTFILE
{
    INCORRECT = 0,
    CORRECT   = 1
};

/**
 * 
 * SWAP
 * macro.
 * 
 */

#define SWAP(a, b, size)						                                           \
    do									                                                       \
      {                                                                        \
        char *_a = (char*)a, *_b = (char*)b;					                         \
        for (size_t i = 0; i < size; i++)								                       \
	        {								                                                     \
	          char _temp = _a[i];						                                     \
	          _a[i]= _b[i];						                                           \
	          _b[i] = _temp;						                                         \
	        } 						                                                       \
      } while (0)

/**
 * 
 * Assert macro.
 * 
 */


#define myAssert(EXPRESSION, ERROR_CODE)                                       \
    if (!(EXPRESSION))                                                         \
      {                                                                        \
        fprintf (stderr,"%s, failed at %s:%d\n", #ERROR_CODE                   \
        , __FILE__, __LINE__);                                                 \
        exit(ERROR_CODE);                                                      \
      }

/**
 * 
 * Text structure that contains line pointers, buffer, size & amount of lines.
 * 
 */


struct Text 
{
    char*  buffer;
    char* const* lineptrs;
    size_t size,
           numLines;
    struct Line* lines;
};

/**
 * 
 * Line structure that contains a string and the length of the string.
 * 
 */

struct Line
{
    char* string;
    size_t length;
};

/**
 * 
 * \brief Creates a text with the chosen sortmode.
 * 
 * @param[in] text - text structure.
 * @param[in] filename - name of file.
 * @param[in] sortmode - sort mode.
 * 
 */

void CreateText(Text* text, const char* filename, size_t sortmode);

/**
 * 
 * \brief Appends the text to a file.
 * 
 * @param[in] text - text structure.
 * @param[in] filename - name of file.
 * @param[in] sortmode - sort mode.
 * 
 */

void AppendText(Text* text, const char* filename);

/**
 * 
 * \brief Frees memory space.
 * 
 * @param[in] text - text structure.
 * @param[in] filename - name of file.
 * @param[in] sortmode - sort mode.
 * 
 */

void DestroyText(Text* text);

/**
 * 
 * \brief Validates the file name.
 * 
 * @param[in] filename - name of file.
 * 
 * \return file correction code.
 * 
 */

size_t CheckFile (const char* filename);

/**
 * 
 * \brief Gets the length of the file.
 * 
 * @param[in] filename - name of file.
 * @param[out] length - size of file.
 * 
 * \return size of file.
 * 
 */

size_t getSize(const char* filename);

/**
 * 
 * \brief Gets the amount of lines.
 * 
 * @param[in] filename - name of file.
 * @param[out] lines - amount of lines.
 * 
 * \return size of file.
 * 
 */

size_t countLines(const Text* text);

/**
 * 
 * \brief Buffer parser.
 * 
 * @param[in] text - text structure.
 * @param[in] filename - name of file.
 * @param[out] buffer - parsed buffer.
 * 
 * \return buffer.
 * 
 */

char* parseBuf(Text* text, const char* filename);

/**
 * 
 * \brief  Gets an array of line pointers.
 * 
 * @param[in] filename - name of file.
 * @param[out] length - size of file.
 * 
 * \return size of file.
 * 
 */

char* const* getLinePointers(Text *text);

/**
 * 
 * \brief Gets the string of the line.
 * 
 * @param[in] text - text structure.
 * @param[in] numLine - the number of line that needs to be return.
 * @param[out] string - line string.
 * 
 * \return string.
 * 
 */

char* getLine(Text* text, size_t numLine);

/**
 * 
 * \brief Gets lots of line structures.
 * 
 * @param[in] text - text structure.
 * @param[out] struct Line.   
 * 
 * \return string.
 * 
 */

struct Line* getLines(Text* text);

/**
 * 
 * \brief Frees up memory space.
 * 
 * @param[in] text - text structure.
 * 
 */

/**
 * 
 * \brief Sorts the text with the chosen sort mode.
 * 
 * @param[in] text - text structure.
 * @param[in] sortmode - sort mode.
 * 
 */

void generalSort(Text* text, size_t sortmode);

/**
 * 
 * \brief Void Quick Sort realisation.
 * 
 * @param[in] array - pointer.
 * @param[in] start - index of first element.
 * @param[in] end - index of last element.
 * @param[in] elemSize - size of array element.
 * @param[in] compareFunc - compare function.
 * 
 */

void quickSort(void* array, int start, int end, size_t elemSize, compareFunc_t compareFunc);

/**
 * 
 * \brief Auxiliary function for qsort.
 * 
 * @param[in] array - pointer.
 * @param[in] start - index of first element.
 * @param[in] end - index of last element.
 * @param[in] elemSize - size of array element.
 * @param[in] compareFunc - compare function.
 * 
 * \return value of pivot. 
 * 
 */


int partition(void* array, int left, int right, size_t elemSize, compareFunc_t compareFunc);

/**
 * 
 * \brief Bubble Sort realisation.
 * 
 * @param[in] array - pointer.
 * @param[in] lines - amount of lines.
 * @param[in] elemSize - size of array element.
 * @param[in] compareFunc - compare function.
 * 
 */

void bubbleSort(void* array, size_t lines, const size_t elemSize, compareFunc_t compareFunc);

/**
 * 
 * \brief Forward string comparer.
 * 
 * @param[in] a - pointer.
 * @param[in] b - pointer.
 * 
 * \return value < 0 if lexicographically a < b, value = 0 if a = b, value a > b if lexicographically a > b.
 * 
 */


int compareStringForw(const void* a, const void* b);

/**
 * 
 * \brief Backward string comparer.
 * 
 * @param[in] a - pointer.
 * @param[in] b - pointer.
 * 
 * \return Same as @ref compareStringForw but compares from end to start.
 * 
 */

int compareStringBack(const void* a, const void* b);

int StringIsEmpty(const Line* line);

#endif
