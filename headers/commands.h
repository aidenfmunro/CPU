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
      else if (isArgRam && isArgReg)
        {
          PUSH(*(elem_t*)spu->RAM[size_t(spu->regs[registerNum])]);
        }
      else if (isArgIm && isArgReg && isArgRam)
        {
          PUSH(*(elem_t*)spu->RAM[size_t(spu->regs[registerNum]) + (size_t)value]);
        }
      else if (isArgIm)
        {
          PUSH(value);
        }
    })

DEF_COMMAND(POP, 2, 1,
    {
      if (isArgIm && isArgReg)
        {
          spu->regs[registerNum] = POP() + value;
        }
      else if (isArgReg && isArgRam)
        {
          *(elem_t*)spu->RAM[size_t(spu->regs[registerNum])] = POP();
        }
      else if (isArgReg && isArgRam && isArgIm)
        {
          *(elem_t*)spu->RAM[size_t(spu->regs[registerNum]) + size_t(value)] = POP();
        }
      else if (isArgRam && isArgIm)
        {
          *(elem_t*)spu->RAM[size_t(value)] = POP();
        }
      else if (isArgReg)
        { 
          spu->regs[registerNum] = POP();
        }
    })

DEF_COMMAND(IN, 3, 0,  
    {
      scanf("%lg", &value);
      PUSH(value);

      if (isArgReg)
        {
          spu->regs[registerNum] = POP();
        }
      else if (isArgRam)
        {
          *(elem_t*)spu->RAM[size_t(value)] = POP();
        }
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
        PUSH(pow(POP() / POP(), -1));   // TODO: check for zero      
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

DEF_COMMAND(JMP, 13, 1,
    {
      *curPosition = labelAddress;
    })

DEF_COMMAND(JNE, 14, 1,
    {
      elem_t b = POP();
      elem_t a = POP();

      if (!doubleCompare(a, b))
        {
          *curPosition = labelAddress;
        }
    })

DEF_COMMAND(JE, 15, 1,
    {
      elem_t b = POP();
      elem_t a = POP();

      if (doubleCompare(a, b)) // double values comparisson 
        {
          *curPosition = labelAddress;
        }
    })

DEF_COMMAND(JBE, 16, 1,
    {
      elem_t b = POP();
      elem_t a = POP();

      if (a <= b)
        {
          *curPosition = labelAddress;
        }
    })

DEF_COMMAND(JB, 17, 1,
    {
      elem_t b = POP();
      elem_t a = POP();

      if (a < b)
        {
          *curPosition = labelAddress;
        }
    })

DEF_COMMAND(JAE, 18, 1,
    {
      elem_t b = POP();
      elem_t a = POP();

      if (a >= b)
        {
          *curPosition = labelAddress;
        }
    })

DEF_COMMAND(JA, 19, 1,
    {
      elem_t b = POP();
      elem_t a = POP();

      if (a > b)
        {
          *curPosition = labelAddress;
        }
    })

DEF_COMMAND(CALL, 20, 1,
    {
      PUSHC(*curPosition);
      
      *curPosition = labelAddress;
    })

DEF_COMMAND(RET, 21, 0,
    {
      *curPosition = size_t(POPC());
    })