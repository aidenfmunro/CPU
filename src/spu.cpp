#include <SDL2/SDL.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "stackfuncs.h"
#include "spu.h"
#include "config.h"
#include "utils.h"
#include "dsl.h"

//

const int HIDDEN_REG = 1;

//

struct ArgRes
{
    elem_t* storePtr;
    elem_t* value;

    ErrorCode error;
};


struct SPU
{
    Stack stack;

    Stack calls;

    elem_t regs[AMOUNT_OF_REGISTERS + HIDDEN_REG];

    elem_t RAM[AMOUNT_OF_RAM_SLOTS];
    
    byte* code;

    size_t ip;
};

//

ErrorCode CreateSPU         (SPU* spu, const char* filename);

ErrorCode DestroySPU        (SPU* spu);

ErrorCode execCommand       (SPU* spu GRAPHICS_ON(, SDL_Renderer* renderer)); 

ErrorCode dumpRAM           (SPU* spu GRAPHICS_ON(, SDL_Renderer* renderer));

ArgRes getArg               (SPU* spu, byte command);

//

ErrorCode RunProgram(const char* filename)
{
    GRAPHICS_ON(SDL_Init(SDL_INIT_EVERYTHING); 

    SDL_Window* window = SDL_CreateWindow(NULL, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 
                                          RECT_WIDTH * RAM_WIDTH, RECT_HEIGHT * RAM_HEIGHT,
                                          SDL_WINDOW_BORDERLESS);

    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, 0); 

    SDL_Event event;

    bool run = true;
    )

    SPU spu = {};

    CreateSPU(&spu, filename);

    GRAPHICS_ON(while (run)
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
    )

            while (execCommand(&spu GRAPHICS_ON(, renderer)) != EXIT_CODE) {;}

    GRAPHICS_ON(
        }
    }
    )
    
    GRAPHICS_ON(

    SDL_DestroyWindow(window);

    SDL_Quit();
    )

    DestroySPU(&spu);  

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

ErrorCode DestroySPU (SPU* spu)
{
    DestroyStack (&spu->stack);

    DestroyStack (&spu->calls);

    memset (spu->RAM, 0, AMOUNT_OF_RAM_SLOTS);

    memset (spu->regs, 0, AMOUNT_OF_REGISTERS);

    free (spu->code);

    return OK;
}

ErrorCode dumpRAM (SPU* spu GRAPHICS_ON(, SDL_Renderer* renderer))
{
    GRAPHICS_ON(SDL_Rect rect = {.x = 0, .y = 0, .w = RECT_WIDTH, .h = RECT_HEIGHT};)

    for (size_t y = 0; y < RAM_HEIGHT; y++)
    {
        for (size_t x = 0; x < RAM_WIDTH; x++)
        {
            GRAPHICS_ON(
            rect.x = x * rect.w;
            rect.y = y * rect.h;
            )

            if (doubleCompare (spu->RAM[RAM_WIDTH * y + x], 1))
            {
                printf("#");

                GRAPHICS_ON(
                SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);

                SDL_RenderFillRect(renderer, &rect);

                SDL_RenderPresent(renderer);
                )
            }
            else
            {
                printf(".");

                GRAPHICS_ON(
                SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);

                SDL_RenderFillRect(renderer, &rect);

                SDL_RenderPresent(renderer);
                )

            }
        }

        printf ("\n");
    }

    GRAPHICS_ON(SDL_Delay(DELAY_FACTOR);)    
    
    return OK;
}

ErrorCode execCommand (SPU* spu GRAPHICS_ON(, SDL_Renderer* renderer))
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
                arg = getArg (spu, command);                                    \
                                                                                \
            }                                                                   \
                                                                                \
            code                                                                \
            break;                                                              \
        }

        #include "commands.h"

        default:
            printf ("Unknown command, code: %d, line: %ld\n", command & ARG_FORMAT_CMD, spu->ip);
            return EXIT_CODE;
        
        #undef DEF_COMMAND


    }

    

    return OK;   
}

ArgRes getArg (SPU* spu, byte command)
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
        memcpy (&tempvalue, &spu->code[spu->ip], sizeof(double));
        spu->regs[RHX] += tempvalue;
        spu->ip += sizeof(double);
    }

    if (command & ARG_FORMAT_RAM)
    {
        size_t index = (size_t)spu->regs[RHX];

        printf("index: %llu\n", index);
        result.storePtr = &spu->RAM[index];

        spu->regs[RHX] = spu->RAM[index];
    }


    result.value = &spu->regs[RHX];

    return result;
}