#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "stackfuncs.h"
#include "spu.h"
#include "config.h"
#include "utils.h"
#include "dsl.h"
#include <SDL2/SDL.h>

struct ArgRes
{
    elem_t* storePtr;
    elem_t* value;

    ErrorCode error;
};

const int HIDDEN_REG = 1;

struct SPU
{
    Stack stack;

    Stack calls;

    elem_t regs[AMOUNT_OF_REGISTERS + HIDDEN_REG];

    elem_t RAM[AMOUNT_OF_RAM_SLOTS];
    
    byte* code;

    size_t ip;
};

ErrorCode CreateSPU(SPU* spu, const char* filename);

ErrorCode DestroySPU(SPU* spu);

ErrorCode execCommand(SPU* spu, SDL_Renderer* renderer); 

ErrorCode dumpRAM(SPU* spu, SDL_Renderer* renderer);

ArgRes getArg(SPU* spu, byte command);

ErrorCode RunProgram(const char* filename)
{
    SDL_Init(SDL_INIT_EVERYTHING);

    SDL_Window* window = SDL_CreateWindow(NULL, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 2020, 600, SDL_WINDOW_BORDERLESS);

    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, 0); 

    SDL_Event event;

    bool run = true;

    SPU spu = {};

    CreateSPU(&spu, filename);

    while (run)
    {
        while(SDL_PollEvent(&event))
        {
            switch (event.type)
            {
                case SDL_QUIT:
                {
                    run = false;
                }
            }

            while (execCommand(&spu, renderer) != EXIT_CODE) {;}

        }
    }

    // while (execCommand(&spu) != EXIT_CODE) {;}
    
    DestroySPU(&spu);  

    SDL_DestroyWindow(window);

    SDL_Quit();

    return OK;
}

ErrorCode CreateSPU(SPU* spu, const char* filename)
{
    size_t fileSize = getSize(filename);
    
    byte* tempcode = (byte*)calloc(sizeof(byte), fileSize);

    CheckPointerValidation(tempcode);

    spu->code = tempcode;

    Stack stack = {};

    Stack calls = {};

    CreateStack(calls);

    CreateStack(stack);

    spu->stack = stack;

    spu->calls = calls;
    
    spu->ip = 0;

    myOpen(filename, "rb", codebin);

    myRead(spu->code, sizeof(byte), fileSize, codebin);

    myClose(codebin);

    return OK;
}

ErrorCode DestroySPU(SPU* spu)
{
    DestroyStack(&spu->stack);

    DestroyStack(&spu->calls);

    memset(spu->RAM, 0, AMOUNT_OF_RAM_SLOTS);

    memset(spu->regs, 0, AMOUNT_OF_REGISTERS);

    free(spu->code);

    return OK;
}

ErrorCode dumpRAM(SPU* spu, SDL_Renderer* renderer)
{
    SDL_Rect rect = {.x = 0, .y = 0, .w = 20, .h = 20};

    for (size_t y = 0; y < 30; y++)
    {
        for (size_t x = 0; x < 101; x++)
        {
            rect.x = x * rect.w;
            rect.y = y * rect.h;

            if (doubleCompare(spu->RAM[101 * y + x], 1))
            {
                // printf("#");
                SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255 );

                SDL_RenderFillRect(renderer, &rect);

                SDL_RenderPresent(renderer);
            }
            else
            {
                // printf(".");

                SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255 );

                SDL_RenderFillRect(renderer, &rect);

                SDL_RenderPresent(renderer);

            }
        }

        printf("\n");
    }

    SDL_Delay(0.8);

    // sleep(1.0); // delay
    // printf("\e[1;1H\e[2J"); // clear terminal for console bad
    
    return OK;
}

ErrorCode execCommand(SPU* spu, SDL_Renderer* renderer)
{    
    CheckPointerValidation(spu);
    
    char command = spu->code[spu->ip];
  
    switch (command & ARG_FORMAT_CMD)
    {
        #define DEF_COMMAND(name, number, argc, code)                           \
                                                                                \
        case number:                                                            \
        {                                                                       \
            spu->ip += sizeof(char);                                            \
            ArgRes arg = {};                                                    \
                                                                                \
            if (argc)                                                           \
            {                                                                   \
                arg = getArg(spu, command);                                     \
            }                                                                   \
                                                                                \
            code                                                                \
            break;                                                              \
        }

        #include "commands.h"

        default:
            printf("Unknown command, code: %d, line: %ld\n", command & ARG_FORMAT_CMD, spu->ip);
            return EXIT_CODE;
        
        #undef DEF_COMMAND
    }

    return OK;   
}

ArgRes getArg(SPU* spu, byte command)
{
    ArgRes result = {};

    elem_t tempvalue = 0;

    spu->regs[RHX] = 0;

    if (command & ARG_FORMAT_REG)
    {
        spu->regs[RHX] += spu->regs[int(spu->code[spu->ip])];
        result.storePtr = &spu->regs[int(spu->code[spu->ip])];
        spu->ip += sizeof(char);
    }

    if (command & ARG_FORMAT_IMMED)
    {
        memcpy(&tempvalue, &spu->code[spu->ip], sizeof(double));
        spu->regs[RHX] += tempvalue;
        spu->ip += sizeof(double);
    }

    if (command & ARG_FORMAT_RAM)
    {
        size_t index = (size_t)spu->regs[RHX];
        result.storePtr = &spu->RAM[index];
    }

    result.value = &spu->regs[RHX];

    return result;
}