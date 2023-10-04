#include <stdio.h>
#include <math.h>
#include <string.h>
#include <time.h>
#include "stackfuncs.h"

void stackInit(stack_t* stack ON_DEBUG(, char* name))
{  
    ON_DEBUG(stack->leftCanary  = LEFT_STRUCT_CANARY);
    ON_DEBUG(stack->name        = name);
             stack->size        = 0;
             stack->capacity    = DEFAULT_CAPACITY;
             stack->data        = (elem_t*)calloc(1 ON_DEBUG(+ 2), sizeof(canary_t));
    ON_DEBUG(stack->hash        = hashAiden32(stack));
    ON_DEBUG(stack->rightCanary = RIGHT_STRUCT_CANARY); 
    
    ON_DEBUG(placeCanary(stack, 0, LEFT_DATA_CANARY));
    ON_DEBUG(placeCanary(stack, 4, RIGHT_DATA_CANARY));

    ON_DEBUG(stack->data = (elem_t*)((char*)stack->data + sizeof(canary_t)));

    ON_DEBUG(stack->hash = hashAiden32(stack));

    ASSERTHARD(stack); 
}

void placeCanary(stack_t* stack, size_t place, canary_t canary)
{
    *(canary_t*)(stack->data + place) = canary;

    ON_DEBUG(stack->hash = hashAiden32(stack));
}

void reallocStack(stack_t* stack)
{
    ASSERTHARD(stack);

    if (stack->capacity < sizeof(elem_t) * (stack->size + 1))
      {
        elem_t* temp = (elem_t*)realloc((elem_t*)((char*)stack->data ON_DEBUG(- sizeof(canary_t))),  \
        (stack->capacity * 2 ON_DEBUG(+ sizeof(canary_t) * 2)));

        ASSERTSOFT(temp, NULLPTR);

        stack->data = temp;
        stack->data = (elem_t*)((char*)stack->data ON_DEBUG(+ sizeof(canary_t)));
        stack->capacity *= 2;
      }
    else if (stack->capacity > 2 * (stack->size + 1) * sizeof(elem_t) && stack->capacity > sizeof(canary_t))
      {
        elem_t* temp = (elem_t*)realloc((elem_t*)((char*)stack->data ON_DEBUG(- sizeof(canary_t))), \
        (stack->capacity / 2 ON_DEBUG(+ sizeof(canary_t) * 2)));

        ASSERTSOFT(temp, NULLPTR);

        stack->data = temp;
        stack->data = (elem_t*)((char*)stack->data ON_DEBUG(+ sizeof(canary_t)));
        stack->capacity /= 2;
      }

    ON_DEBUG(stack->hash = hashAiden32(stack));

    ON_DEBUG(placeCanary(stack, stack->capacity / sizeof(elem_t), RIGHT_DATA_CANARY));  
}

void Push(stack_t* stack, elem_t value)
{
    ASSERTHARD(stack);

    reallocStack(stack);
      
    stack->data[stack->size++] = value;

    ON_DEBUG(stack->hash = hashAiden32(stack));

    poisonFill(stack);
}

elem_t Pop(stack_t* stack)
{
    ASSERTHARD(stack);

    reallocStack(stack);

    elem_t peek = stack->data[stack->size - 1];

    stack->data[stack->size--] = POISON;

    ON_DEBUG(stack->hash = hashAiden32(stack));

    poisonFill(stack);

    return peek;
}

elem_t Peek(const stack_t* stack)
{
    ASSERTHARD(stack);

    return (elem_t)stack->data[stack->size];
}

void poisonFill(stack_t* stack)
{
    ASSERTHARD(stack);

    for(size_t i = stack->size; i < stack->capacity / sizeof(elem_t); i++)
        stack->data[i] = POISON;

    ON_DEBUG(stack->hash = hashAiden32(stack));

    ASSERTHARD(stack);
}

void StackDtor(stack_t* stack)
{
    free((elem_t*)((char*)stack->data ON_DEBUG(- sizeof(canary_t))));
    stack->capacity = 0;
    stack->size = 0;
    ON_DEBUG(stack->hash = 0);
    ON_DEBUG(stack->name = "NONE");
}

void PrintStack(const stack_t* stack)
{
    ON_DEBUG(printf("\tleft data canary = %llx\n", *(elem_t*)((char*)stack->data - sizeof(canary_t))));

    for (size_t i = 0; i < stack->capacity / sizeof(elem_t); i++)
    if (stack->data[i] == POISON)
        printf("\t\t ! [%d] %" FORMAT " (POISON!)\n", i, stack->data[i]);
    else
        printf("\t\t   [%d] %" FORMAT "\n", i, stack->data[i]);
    
    ON_DEBUG(printf("\tright data canary = %llx\n", stack->data[stack->capacity / sizeof(elem_t)]));
}

const char* stackStrError (const int code)
{
    #define CODE_(code) case code: return #code;

    switch (code)
        {
        CODE_ (NULLPTR_STACK)
        CODE_ (NULLPTR_DATA)
        CODE_ (SIZE_BIGGER_CAPACITY)
        CODE_ (LCANARY_DATA_CHANGED)
        CODE_ (RCANARY_DATA_CHANGED)
        CODE_ (LCANARY_STRUCT_CHANGED)
        CODE_ (RCANARY_STRUCT_CHANGED)
        CODE_ (MAX_CAPACITY_OVERFLOW)
        CODE_ (CANARY_SIZE_CHANGED)
        CODE_ (CAPACITY_LESS_DEFAULT)
        CODE_ (HASH_CHANGED)
        }

    return "UNKNOWN";

    #undef CODE_    
}

void stackDump(const stack_t* stack, const char* filename, const int lineNum, const char* functionName)
{
    FILE* fp = fopen("log.txt", "w+");

    fprintf(fp, "This log file was made at: %s\n", getTime());

    int error = stackVerify(stack);

    fprintf(fp, "Stack " NAME " [%p], ERROR #%u (%s), in file %s, line %d, function: %s\n"
                "{\n"
        ON_DEBUG("\tleft struct canary = 0x%llx\n")
                 "\tsize = %llu\n"
                 "\tcapacity = %llu\n"
        ON_DEBUG("\tright struct canary = 0x%llx\n")
                 "\n",

        ON_DEBUG(stack->name,) stack, error, stackStrError(error), filename, lineNum, functionName
        ON_DEBUG(, stack->leftCanary), 
        stack->size,
        stack->capacity
        ON_DEBUG(, stack->rightCanary));

        fprintf(fp, "\tdata[%p]:\n"
                    "\n" 
        ON_DEBUG(   "\tleft data canary = 0x%llx\n"), 
        stack->data ON_DEBUG(, *(canary_t*)((char*)stack->data - sizeof(canary_t))));

    for (size_t i = 0; i < stack->capacity / sizeof(elem_t); i++)
      if (stack->data[i] == POISON)
        fprintf(fp, "\t\t ! [%d] %" FORMAT " (POISON!)\n", i, stack->data[i]);
      else
        fprintf(fp, "\t\t   [%d] %" FORMAT "\n", i, stack->data[i]);
    
    ON_DEBUG(fprintf(fp, "\tright data canary = 0x%llx\n", *(canary_t*)((char*)stack->data + stack->capacity)));
    fprintf(fp, "}\n");

    fclose(fp);
}

const char* getTime(void)
{
    time_t t;
    time(&t);
    return ctime(&t);
}

ErrorCode stackVerify(const stack_t* stack)
{
    int error = 0;

    if (stack == NULL)
        error |= NULLPTR_STACK;
    if (stack->data == NULL)
        error |= NULLPTR_DATA;
    if (stack->size > stack->capacity / sizeof(elem_t))
        error |= SIZE_BIGGER_CAPACITY;
    ON_DEBUG(if (*(canary_t*)((char*)stack->data - sizeof(canary_t)) != LEFT_DATA_CANARY) \
        error |= LCANARY_DATA_CHANGED);
    ON_DEBUG(if (*(canary_t*)((char*)stack->data + stack->capacity) != RIGHT_DATA_CANARY) \
        error |= RCANARY_DATA_CHANGED);
    ON_DEBUG(if (stack->leftCanary != LEFT_STRUCT_CANARY) \
        error |= LCANARY_STRUCT_CHANGED);
    ON_DEBUG(if (stack->rightCanary != RIGHT_STRUCT_CANARY) \
        error |= RCANARY_STRUCT_CHANGED);
    if (stack->capacity > MAX_STACK_SIZE)
        error |= MAX_CAPACITY_OVERFLOW;
    if (stack->capacity < DEFAULT_CAPACITY)
        error |= CAPACITY_LESS_DEFAULT;
    ON_DEBUG(if (stack->hash != hashAiden32(stack)) \
        error |= HASH_CHANGED);

    return error;
}

unsigned int hashAiden32(const stack_t* stack)
{
    unsigned int hashSum = 1;
    
    for (size_t i = 0; i < stack->capacity; i++)
    {
        hashSum = (hashSum + *((char*)stack->data + i)) % MOD_AIDEN;
    }

    hashSum += stack->size % MOD_AIDEN;
    hashSum += stack->capacity % MOD_AIDEN;
    
    return hashSum;
}   