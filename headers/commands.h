DEF_COMMAND(PUSH, 1, 1,
{
    PUSH(*arg.value);
})

DEF_COMMAND(POP, 2, 1,
{
    *arg.place = POP();
})

DEF_COMMAND(IN, 3, 0,  
{
    elem_t value = 0;
    scanf("%lg", &value);
    PUSH(value);
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
        PUSH(a / b);    
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

#define JUMP_COMMAND(name, num, comparison)     \
DEF_COMMAND(name, num, 1,                       \
{                                               \
    elem_t b = POP();                           \
    elem_t a = POP();                           \
                                                \
    if (comparison)                             \
        spu->ip = *(size_t*)arg.value;          \
})

DEF_COMMAND(JMP, 13, 1,
{
    spu->ip = *(size_t*)arg.value;     
})

JUMP_COMMAND(JNE, 14, !doubleCompare(a, b))
JUMP_COMMAND(JE, 15, doubleCompare(a, b))
JUMP_COMMAND(JBE, 16, a <= b)
JUMP_COMMAND(JB, 17, a < b)
JUMP_COMMAND(JAE, 18, a >= b)
JUMP_COMMAND(JA, 19, a > b)

DEF_COMMAND(CALL, 20, 1,
{
    PUSHC(spu->ip);
  
    spu->ip = *(size_t*)arg.value;
})

DEF_COMMAND(RET, 21, 0,
{
    spu->ip = POPC();
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