#include <stdio.h>
#include <string.h>
#include <time.h>
#include "stackfuncs.h"

void createStack(Stack* stack ON_DEBUG(, char* name))
{  
    ON_DEBUG(stack->leftCanary  = LEFT_STRUCT_CANARY);
    ON_DEBUG(stack->name        = name);
             stack->size        = 0;
             stack->capacity    = DEFAULT_CAPACITY; // TODO: not in bytes -> DONE
             stack->data        = initData();
    ON_DEBUG(stack->hash        = hashAiden32(stack));
    ON_DEBUG(stack->rightCanary = RIGHT_STRUCT_CANARY); 
    
    ON_DEBUG(placeCanary(stack, 0, LEFT_DATA_CANARY));

    ON_DEBUG(stack->data = (elem_t*)((char*)stack->data + sizeof(canary_t)));

    ON_DEBUG(placeCanary(stack, stack->capacity, RIGHT_DATA_CANARY)); // TODO: for all types -> DONE

    ON_DEBUG(stack->hash = hashAiden32(stack));

    ASSERTHARD(stack); 
}

elem_t* initData(void)
{
    elem_t* temp = (elem_t*)calloc(1 ON_DEBUG(+ 2), sizeof(canary_t));

    while (temp == NULL)
      {
        temp = (elem_t*)calloc(1 ON_DEBUG(+ 2), sizeof(canary_t));
        printf("can't allocate memory for stack");
      }

    return temp;
}

void placeCanary(Stack* stack, size_t place, canary_t canary)
{
    *(canary_t*)((char*)stack->data + place * sizeof(elem_t)) = canary; // TODO: get canary pointer function -> DONE

    ON_DEBUG(stack->hash = hashAiden32(stack));
}

canary_t* getCanaryRightptr(const Stack* stack)
{
    return (canary_t*)(stack->data + stack->capacity);
}

canary_t* getCanaryLeftPtr(const Stack* stack)
{
    return (canary_t*)((char*)stack->data - sizeof(canary_t));
}

void reallocStack(Stack* stack, const int resize)
{
    ASSERTHARD(stack);

    if (resize == EXPAND)
      {
        stack->capacity *= 2;
      }
    else if (resize == COMPRESS)
      {
        stack->capacity /= 2;
      }

    elem_t* temp = (elem_t*)realloc((char*)stack->data ON_DEBUG(- sizeof(canary_t)),  \
    ((stack->capacity) * sizeof(elem_t) ON_DEBUG(+ sizeof(canary_t) * 2))); // TODO: get rid elem_t* -> DONE

    ASSERTSOFT(temp, NULLPTR); 

    stack->data = temp;
    stack->data = (elem_t*)((char*)stack->data ON_DEBUG(+ sizeof(canary_t))); //TODO: changed func -> DONE

    ON_DEBUG(placeCanary(stack, stack->capacity, RIGHT_DATA_CANARY));

    ON_DEBUG(stack->hash = hashAiden32(stack));

    poisonFill(stack); // TODO: call only with realloc -> DONE
}

void Push(Stack* stack, elem_t value)
{
    ASSERTHARD(stack);
    
    if (stack->capacity <= stack->size + 1)
        reallocStack(stack, EXPAND); // TODO: call realloc when needed -> DONE

    stack->data[stack->size++] = value;

    ON_DEBUG(stack->hash = hashAiden32(stack));    
}

elem_t Pop(Stack* stack)
{
    ASSERTHARD(stack);

    if (stack->capacity >= 2 * (stack->size + 1))
        reallocStack(stack, COMPRESS);

    elem_t peek = stack->data[stack->size - 1];

    stack->data[stack->size--] = POISON;

    ON_DEBUG(stack->hash = hashAiden32(stack));

    return peek;
}

void poisonFill(Stack* stack)
{
    ASSERTHARD(stack);
    
    for(size_t i = stack->size; i < stack->capacity; i++)
        stack->data[i] = POISON;

    ON_DEBUG(stack->hash = hashAiden32(stack));

    ASSERTHARD(stack);
}

void DestroyStack(Stack* stack)
{
    if (stack->data == NULL)
        printf("stack is already deleted!\n");
    else
      {
        stack->size = 0;

        ON_DEBUG(stack->hash = hashAiden32(stack));

        poisonFill(stack);  // TODO: fill poison because stays in memory -> DONE

        stack->capacity = 0;

        free((elem_t*)((char*)stack->data ON_DEBUG(- sizeof(canary_t)))); // TODO: DestroyStack more than once, program does not fall -> DONE

        stack->data = NULL;

        ON_DEBUG(stack->hash = 0);
        ON_DEBUG(stack->name = "NONE");
      }
}

void PrintStack(const Stack* stack)
{
    ON_DEBUG(printf("\tleft data canary = %llx\n", *getCanaryLeftPtr(stack)));

    for (size_t i = 0; i < stack->capacity; i++)
    if (stack->data[i] == POISON)
        printf("\t\t ! [%d] %" FORMAT " (POISON!)\n", i, stack->data[i]);
    else
        printf("\t\t   [%d] %" FORMAT "\n", i, stack->data[i]);
    
    ON_DEBUG(printf("\tright data canary = %llx\n", *getCanaryRightptr(stack)));
}

const char* stackStrError (const int code) //TODO: think about its
{
    #define CODE_(code) case code: return #code;

    switch (code)
        {                                       //TODO: change code name???
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

void stackDump(const Stack* stack, const char* filename, const int lineNum, const char* functionName)
{
    FILE* fp = fopen(logfilename, "w+"); // TODO: user config filename -> DONE

    LOG(fp, "This log file was made at: %s\n", getTime()); // TODO: LOG("line: %d", 7); make it look simple -> DONE

    int error = stackVerify(stack);

    LOG(fp, "Stack " NAME " [%p], ERROR #%u (%s), in file %s, line %d, function: %s\n"
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

        LOG(fp, "\tdata[%p]:\n"
                    "\n" 
        ON_DEBUG(   "\tleft data canary = 0x%llx\n"), 
        stack->data ON_DEBUG(, *getCanaryLeftPtr(stack)));

        for (size_t i = 0; i < stack->capacity; i++)
          if (stack->data[i] == POISON)
            LOG(fp, "\t\t ! [%d] %" FORMAT " (POISON!)\n", i, stack->data[i]);
          else
            LOG(fp, "\t\t   [%d] %" FORMAT "\n", i, stack->data[i]);
    
        ON_DEBUG(LOG(fp, "\tright data canary = 0x%llx\n", *getCanaryRightptr(stack)));
                 LOG(fp, "}\n");

    fclose(fp);
}

const char* getTime(void)
{
    time_t t;
    time(&t);
    return ctime(&t);
}

ErrorCode stackVerify(const Stack* stack)
{
    int error = 0;

    CHECK_ERROR(stack == NULL, NULLPTR_STACK, error);

    CHECK_ERROR(stack->data == NULL, NULLPTR_DATA, error);

    CHECK_ERROR(stack->size > stack->capacity, SIZE_BIGGER_CAPACITY, error);

    ON_DEBUG(CHECK_ERROR(*getCanaryLeftPtr(stack) != LEFT_DATA_CANARY, LCANARY_DATA_CHANGED, error));

    ON_DEBUG(CHECK_ERROR(*getCanaryRightptr(stack) != RIGHT_DATA_CANARY, RCANARY_DATA_CHANGED, error));

    ON_DEBUG(CHECK_ERROR(stack->leftCanary != LEFT_STRUCT_CANARY, LCANARY_STRUCT_CHANGED, error));

    ON_DEBUG(CHECK_ERROR(stack->rightCanary != RIGHT_STRUCT_CANARY, RCANARY_STRUCT_CHANGED, error));

    CHECK_ERROR(stack->capacity > MAX_STACK_SIZE, MAX_CAPACITY_OVERFLOW, error);

    CHECK_ERROR(stack->capacity < DEFAULT_CAPACITY, CAPACITY_LESS_DEFAULT, error);

    ON_DEBUG(CHECK_ERROR(stack->hash != hashAiden32(stack), HASH_CHANGED, error));

    return error; 
}

unsigned int hashAiden32(const Stack* stack)
{
    unsigned int hashSum = 1;
    
    for (size_t i = 0; i < stack->capacity * sizeof(elem_t); i++)
      {
        hashSum = (hashSum + *((char*)stack->data + i)) % MOD_AIDEN;
      }

    hashSum += stack->size % MOD_AIDEN;
    hashSum += stack->capacity % MOD_AIDEN;
    
    return hashSum;
}   