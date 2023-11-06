#ifndef STACKFUNCS_H
#define STACKFUNCS_H

#include <stdlib.h>
#include <math.h>
#include <stdint.h>
#include <limits.h>
#include "config.h"

typedef unsigned long long canary_t;

const int MOD_AIDEN = 64;

const int MAX_STACK_SIZE = 4000 * 8;

const canary_t LEFT_STRUCT_CANARY  = 0xDEADAAAA;
const canary_t RIGHT_STRUCT_CANARY = 0xDEADBBBB;

const canary_t LEFT_DATA_CANARY    = 0xDEADEEEE;
const canary_t RIGHT_DATA_CANARY   = 0xDEADFFFF;

#define LOG fprintf

#ifdef DEBUG
    #define ON_DEBUG(...) \
        __VA_ARGS__
    #define NAME "(%s)"
#else
    #define ON_DEBUG(...)
    #define NAME
#endif

#ifdef INT_T
    #define FORMAT "d"
    typedef int elem_t;
    const elem_t POISON = INT_MAX;
    const int DEFAULT_CAPACITY = 2;
#endif

#ifdef FLOAT_T
    #define FORMAT "f"
    typedef float elem_t;
    const elem_t POISON = NAN;
    const int DEFAULT_CAPACITY = 2;
#endif

#ifdef DOUBLE_T
    #define FORMAT "lg"
    typedef double elem_t;
    const elem_t POISON = NAN;
    const int DEFAULT_CAPACITY = 1;
#endif

#ifdef CHAR_T
    #define FORMAT "c"
    typedef char elem_t;
    const elem_t POISON = '!';
    const int DEFAULT_CAPACITY = 8;
#endif

#ifdef STRING_T
    #define FORMAT "s"
    typedef char* elem_t;
    const elem_t POISON = "POISON";
#endif

#define ASSERTSOFT(EXPRESSION, ERROR_CODE)              \
    if (!(EXPRESSION))                                  \
      {                                                 \
        fprintf (stderr,"%s, failed at %s:%d\n",        \
         #ERROR_CODE, __FILE__, __LINE__);              \
        exit(ERROR_CODE);                               \
      }

#define ASSERTHARD(stack)                               \
    do                                                  \
      {                                                 \
        int error = stackVerify(stack);                 \
        if (error)                                      \
          {                                             \
            DUMP(stack);                                \
            exit(error);                                \
          }                                             \
      } while(0);
                                                       

#define DUMP(stack)                                     \
    do                                                  \
      {                                                 \
        stackDump(stack, __FILE__, __LINE__, __func__); \
      } while(0); 

#define CHECK_ERROR(EXPRESSION, ERROR, errorSum)        \
    if (EXPRESSION)                                     \
      {                                                 \
        errorSum += ERROR;                              \
      }
                

#define CreateStack(stack)                              \
    do                                                  \
      {                                                 \
        createStack(&stack ON_DEBUG(, #stack));         \
      } while(0);                                       \

enum STACK_ERRORS
{
    NULLPTR_STACK           = (1 << 0), 
    NULLPTR_DATA            = (1 << 1),
    SIZE_BIGGER_CAPACITY    = (1 << 2), 
    LCANARY_DATA_CHANGED    = (1 << 3),
    RCANARY_DATA_CHANGED    = (1 << 4),
    LCANARY_STRUCT_CHANGED  = (1 << 5),
    RCANARY_STRUCT_CHANGED  = (1 << 6),
    CANARY_SIZE_CHANGED     = (1 << 7),
    MAX_CAPACITY_OVERFLOW   = (1 << 8),
    CAPACITY_LESS_DEFAULT   = (1 << 9),
    HASH_CHANGED            = (1 << 10),
    STACK_EMPTY             = (1 << 11)
};

struct Stack
{
    ON_DEBUG(canary_t leftCanary);

    ON_DEBUG(char* name);
    elem_t* data;
    size_t capacity;
    size_t size;
    ON_DEBUG(unsigned int hash);

    ON_DEBUG(canary_t rightCanary);

};

ErrorCode createStack(Stack* stk ON_DEBUG(, char* name));

elem_t* initData(void);

ErrorCode reallocStack(Stack* stk);

ErrorCode placeCanary(Stack* stk, size_t place, canary_t canary);

ErrorCode poisonFill(Stack* stk);

ErrorCode DestroyStack(Stack* stk);

ErrorCode Push(Stack* stk, elem_t value);

elem_t Pop(Stack* stk);

ErrorCode PrintStack(const Stack* stk);

const char* getTime(void);

const char* stackStrError (const int code);

ErrorCode stackVerify(const Stack* stk);

ErrorCode stackDump(const Stack* stk, const char* filename, const int lineNum, const char* functionName);

canary_t* getCanaryRightPtr(const Stack* stack);

canary_t* getCanaryLeftPtr(const Stack* stack);

int hashAiden32(const Stack* stack);

#endif
