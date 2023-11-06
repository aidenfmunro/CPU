#include <stdio.h>
#include <string.h>
#include <time.h>
#include "stackfuncs.h"

ErrorCode createStack(Stack* stack ON_DEBUG(, char* name))
{  
    ON_DEBUG(stack->leftCanary  = LEFT_STRUCT_CANARY);
    ON_DEBUG(stack->name        = name);
             stack->size        = 0;
             stack->capacity    = DEFAULT_CAPACITY; 
             stack->data        = (elem_t*)calloc(1 ON_DEBUG(+ 2), sizeof(canary_t));
             if (stack->data == NULL)
                 return NO_MEMORY;
    ON_DEBUG(stack->hash        = hashAiden32(stack));
    ON_DEBUG(stack->rightCanary = RIGHT_STRUCT_CANARY); 
    
    ON_DEBUG(placeCanary(stack, 0, LEFT_DATA_CANARY));

    ON_DEBUG(stack->data = (elem_t*)((char*)stack->data + sizeof(canary_t)));

    ON_DEBUG(placeCanary(stack, stack->capacity, RIGHT_DATA_CANARY));

    ON_DEBUG(stack->hash = hashAiden32(stack));

    ASSERTHARD(stack); 

    return OK;
}

ErrorCode placeCanary(Stack* stack, size_t place, canary_t canary)
{
    *(canary_t*)((char*)stack->data + place * sizeof(elem_t)) = canary;

    ON_DEBUG(stack->hash = hashAiden32(stack));

    return OK;
}

canary_t* getCanaryRightPtr(const Stack* stack)
{
    return (canary_t*)(stack->data + stack->capacity);
}

canary_t* getCanaryLeftPtr(const Stack* stack)
{
    return (canary_t*)((char*)stack->data - sizeof(canary_t));
}

ErrorCode reallocStack(Stack* stack)
{
    elem_t* temp = (elem_t*)realloc((char*)stack->data ON_DEBUG(- sizeof(canary_t)),  \
    ((stack->capacity) * sizeof(elem_t) ON_DEBUG(+ sizeof(canary_t) * 2))); 

    if (temp == NULL)
        return NO_MEMORY; 

    stack->data = temp;
    stack->data = (elem_t*)((char*)stack->data ON_DEBUG(+ sizeof(canary_t))); 

    ON_DEBUG(placeCanary(stack, stack->capacity, RIGHT_DATA_CANARY));

    ASSERTHARD(stack);

    ON_DEBUG(stack->hash = hashAiden32(stack));

    ASSERTHARD(stack);

    return OK;
}

ErrorCode Push(Stack* stack, elem_t value)
{
    ASSERTHARD(stack);
    
    if (stack->capacity <= stack->size + 1)
      {
        stack->capacity *= 2;

        ON_DEBUG(stack->hash = hashAiden32(stack));

        reallocStack(stack);

        poisonFill(stack);
      }

    stack->data[stack->size++] = value;

    ON_DEBUG(stack->hash = hashAiden32(stack));

    ASSERTHARD(stack);

    return OK;    
}

elem_t Pop(Stack* stack)
{
    ASSERTHARD(stack);

    if (stack->size == 0) return 0;

    if (stack->capacity >= 2 * (stack->size + 1))
      {
        stack->capacity /= 2;

        ON_DEBUG(stack->hash = hashAiden32(stack));

        reallocStack(stack);
      }

    elem_t peek = stack->data[stack->size - 1];

    stack->data[stack->size--] = POISON;

    ON_DEBUG(stack->hash = hashAiden32(stack));

    ASSERTHARD(stack);

    return peek;
}

ErrorCode poisonFill(Stack* stack)
{
    ASSERTHARD(stack);
    
    for(size_t i = stack->size; i < stack->capacity; i++)
        stack->data[i] = POISON;

    ON_DEBUG(stack->hash = hashAiden32(stack));

    ASSERTHARD(stack);

    return OK;
}

ErrorCode DestroyStack(Stack* stack)
{
    if (stack->data == NULL)
      {
        printf("stack is already deleted!\n");
        return STACK_DELETED;
      }
    else
      {
        stack->size = 0;

        ON_DEBUG(stack->hash = hashAiden32(stack));

        poisonFill(stack);

        stack->capacity = 0;

        free((elem_t*)((char*)stack->data ON_DEBUG(- sizeof(canary_t))));

        stack->data = NULL;

        ON_DEBUG(stack->hash = 0);
        ON_DEBUG(stack->name = "NONE");
      }
    
    return OK;
}

ErrorCode PrintStack(const Stack* stack)
{
    ON_DEBUG(printf("\tleft data canary = %llx\n", *getCanaryLeftPtr(stack)));

    for (size_t i = 0; i < stack->capacity; i++)
    if (stack->data[i] == POISON)
        printf("\t\t ! [%lu] %" FORMAT " (POISON!)\n", i, stack->data[i]);
    else
        printf("\t\t   [%lu] %" FORMAT "\n", i, stack->data[i]);
    
    ON_DEBUG(printf("\tright data canary = %llx\n", *getCanaryRightPtr(stack)));

    return OK;
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

        default:
            break;
        }

    return "UNKNOWN";

    #undef CODE_    
}

ErrorCode stackDump(const Stack* stack, const char* filename, const int lineNum, const char* functionName)
{
    FILE* fp = fopen(logfilename, "w+");

    if (fp == NULL)
        return UNABLE_TO_OPEN_FILE;

    LOG(fp, "This log file was made at: %s\n", getTime());

    int error = stackVerify(stack);

    LOG(fp, "Stack " NAME " [%p], ERROR #%d (%s), in file %s, line %d, function: %s\n"
                "{\n"
        ON_DEBUG("\tleft struct canary = 0x%llx\n")
                 "\tsize = %lu\n"
                 "\tcapacity = %lu\n"
        ON_DEBUG("\tright struct canary = 0x%llx\n")
                 "\n",

        ON_DEBUG(stack->name,) stack, error, stackStrError(error), filename, lineNum, functionName
        ON_DEBUG(, stack->leftCanary), 
                   stack->size,
                   stack->capacity
        ON_DEBUG(, stack->rightCanary));

        LOG(fp, "\tdata[%p]:\n"
                    "\n" 
        ON_DEBUG(   "\tleft data canary = 0x%llx\n"), 
        stack->data ON_DEBUG(, *getCanaryLeftPtr(stack)));

        for (size_t i = 0; i < stack->capacity; i++)
          if (isnan(stack->data[i]))
            LOG(fp, "\t\t ! [%lu] %" FORMAT " (POISON!)\n", i, stack->data[i]);
          else
            LOG(fp, "\t\t   [%lu] %" FORMAT "\n", i, stack->data[i]);
    
        ON_DEBUG(LOG(fp, "\tright data canary = 0x%llx\n", *getCanaryRightPtr(stack)));
                 LOG(fp, "}\n");

    fclose(fp);

    return OK;
}

const char* getTime(void)
{
    time_t t;
    time(&t);
    return ctime(&t);
}

#define CHECK_ERROR(EXPRESSION, ERROR, errorSum)        \
    if (EXPRESSION)                                     \
      {                                                 \
        errorSum += ERROR;                              \
      }                

ErrorCode stackVerify(const Stack* stack)
{
    int error = 0;

    CHECK_ERROR(stack == NULL, NULLPTR_STACK, error);

    CHECK_ERROR(stack->data == NULL, NULLPTR_DATA, error);

    CHECK_ERROR(stack->size > stack->capacity, SIZE_BIGGER_CAPACITY, error);

    ON_DEBUG(CHECK_ERROR(*getCanaryLeftPtr(stack) != LEFT_DATA_CANARY, LCANARY_DATA_CHANGED, error));

    ON_DEBUG(CHECK_ERROR(*getCanaryRightPtr(stack) != RIGHT_DATA_CANARY, RCANARY_DATA_CHANGED, error));

    ON_DEBUG(CHECK_ERROR(stack->leftCanary != LEFT_STRUCT_CANARY, LCANARY_STRUCT_CHANGED, error));

    ON_DEBUG(CHECK_ERROR(stack->rightCanary != RIGHT_STRUCT_CANARY, RCANARY_STRUCT_CHANGED, error));

    CHECK_ERROR(stack->capacity > MAX_STACK_SIZE, MAX_CAPACITY_OVERFLOW, error);

    CHECK_ERROR(stack->capacity < DEFAULT_CAPACITY, CAPACITY_LESS_DEFAULT, error);

    ON_DEBUG(CHECK_ERROR(stack->hash != hashAiden32(stack), HASH_CHANGED, error));

    return error; 
}

#undef CHECK_ERROR

int hashAiden32(const Stack* stack)
{
    int hashSum = 1;
    
    for (size_t i = 0; i < stack->capacity * sizeof(elem_t); i++)
      {
        hashSum = (hashSum + *((char*)stack->data + i)) % MOD_AIDEN;
      }

    hashSum += stack->size % MOD_AIDEN;
    hashSum += stack->capacity % MOD_AIDEN;
    
    return hashSum;
}   