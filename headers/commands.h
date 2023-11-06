/*
    TODO: Make DSL more homogeneous

    DSL is language that users use to create
    new command for your SPU.

    In README you can specify which variables can
    be used by your users. (e.g. arg.value, curPosition)

    There is no sense to give user an information that curPosition
    is in fact is pointer to position (that's strange btw).

    For user it would be nice to write highlevel code like:

    def DEF_COMMAND(AGE_TEST, 404, 1,
        { 
            Elem a = POP();

            if (a < 18)
            {
                Print("Too little for buying alcohol");
            }
            else
            {
                // Move to alcohol buying routine
                Position = ArgValue;
            }
        })

    All in all, I mean there should be clear border of abstraction
    between your spu.cpp code and DSL code.

    DSL should use only restricted subset of SPU variables and functions.

    It helps us to abstract user from internals. Like when you write .dot file
    you do care only about node definitions and connections between them. Any other
    details of implementation of this graph tool are not your business. 
*/

DEF_COMMAND(PUSH, 1, 1,
    {
      PUSH(arg.value);
    })

DEF_COMMAND(POP, 2, 1,
    {
      *arg.place = POP();
    })

DEF_COMMAND(IN, 3, 0,  
    {
      scanf("%lg", &arg.value);
      PUSH(arg.value);
    })

DEF_COMMAND(ADD, 4, 0,
    {
        PUSH(POP() + POP());
    })

DEF_COMMAND(SUB, 5, 0,
    {
        PUSH(0 - POP() + POP());        
    })

DEF_COMMAND(MUL, 6, 0,
    {
        PUSH(POP() * POP());        
    })
    
DEF_COMMAND(DIV, 7, 0,
    {
        elem_t b = POP();
        elem_t a = POP();
        if (!doubleCompare(b, 0))
            PUSH(a / b);   // TODO: check for zero    
        else
          return ZERO_DIVISION;  
    })

DEF_COMMAND(SQRT, 8, 0,
    {
        PUSH(sqrt(POP()));
    })

DEF_COMMAND(SIN, 9, 0,
    {
        PUSH(sin(POP()));
    })

DEF_COMMAND(COS, 10, 0,
    {
        PUSH(cos(POP()));
    })

DEF_COMMAND(OUT, 11, 0,
    {
        printf("result: %lg\n", POP());
    })

DEF_COMMAND(HLT, 12, 0, 
    {
        return EXIT_CODE; 
    })

#define JUMP_COMMAND(name, num, comparison)         \
DEF_COMMAND(name, num, 1,                           \
    {                                               \
      elem_t b = POP();                             \
      elem_t a = POP();                             \
                                                    \
      if (comparison)                               \
          *curPosition = arg.labelAddress;         \
    })

JUMP_COMMAND(JNE, 14, !doubleCompare(a, b))
JUMP_COMMAND(JE, 15, doubleCompare(a, b))
JUMP_COMMAND(JBE, 16, a <= b)
JUMP_COMMAND(JB, 17, a < b)
JUMP_COMMAND(JAE, 18, a >= b)
JUMP_COMMAND(JA, 19, a > b)

// TODO: Why command with number 13 is under 14,15...?
DEF_COMMAND(JMP, 13, 1,
    {
      *curPosition = arg.labelAddress;     
    })

DEF_COMMAND(CALL, 20, 1,
    {
      PUSHC(*curPosition);
      
      *curPosition = arg.labelAddress;
    })

DEF_COMMAND(RET, 21, 0,
    {
      *curPosition = POPC();
    })

DEF_COMMAND(MEOW, 22, 0,
    {
      printf("meow :3\n");
    })

DEF_COMMAND(RND, 23, 0,
    {
      elem_t a = POP();
      PUSH(round(a));
    })

// TODO: adoviy hardcode
DEF_COMMAND(SCRN, 24, 0,
    {
      for (size_t y = 0; y <= 40; y++)
        {
          for (size_t x = 0; x <= 40; x++)
            {
              if (doubleCompare(spu->RAM[40 * y + x], 1))
                {
                  printf("#  ");
                }
              else
                {
                  printf(".  ");
                }
            }
          printf("\n");
          printf("\n");
        }
    })