DEF_COMMAND(PUSH, 1, 1,
    {
      if (isArgIm && isArgReg)
        {
          PUSH(spu->regs[registerNum] + value);
        }
      else if (isArgReg)
        {
          PUSH(spu->regs[registerNum]);
        }
      else
        {
          PUSH(value);
        }
    })

DEF_COMMAND(POP, 2, 1,
    {
      if (isArgIm && isArgReg)
        {
          spu->regs[registerNum] = POP + value;
        }
      else if (isArgReg)
        { 
          spu->regs[registerNum] = POP;
        }
    })

DEF_COMMAND(IN, 3, 0,  
    {
      scanf("%lg", &value);
      PUSH(value);
    })

DEF_COMMAND(ADD, 4, 0,
    {
        PUSH(POP + POP);
    })

DEF_COMMAND(SUB, 5, 0,
    {
        PUSH(0 - POP + POP);        
    })

DEF_COMMAND(MUL, 6, 0,
    {
        PUSH(POP * POP);        
    })
    
DEF_COMMAND(DIV, 7, 0,
    {
        PUSH(pow(POP / POP, -1));   // TODO: check for zero      
    })

DEF_COMMAND(SQRT, 8, 0,
    {
        PUSH(sqrt(POP));
    })

DEF_COMMAND(SIN, 9, 0,
    {
        PUSH(sin(POP));
    })

DEF_COMMAND(COS, 10, 0,
    {
        PUSH(cos(POP));
    })

DEF_COMMAND(OUT, 11, 0,
    {
        printf("result: %lg\n", POP);
    })

DEF_COMMAND(HLT, 12, 0, 
    {
        return EXIT_CODE; 
    })

DEF_JMP(JMP, 13, NULL,
    {
      ;
    })

DEF_JMP(JNE, 14, !=, {;})

DEF_JMP(JE, 15, =, {;})

DEF_JMP(JBE, 16, <=, {;})

DEF_JMP(JB, 17, <, {;})

DEF_JMP(JAE, 18, >=, {;})

DEF_JMP(JA, 19, >, {;})
