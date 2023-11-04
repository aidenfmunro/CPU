DEF_COMMAND(PUSH, 1, 1,
    {
      if (isArgIm && isArgReg && isArgRam)
        {
          PUSH(spu->RAM[size_t(spu->regs[registerNum]) + (size_t)value]);
        }
      else if (isArgIm && isArgReg)
        {
          PUSH(spu->regs[registerNum] + value);
        }
      else if (isArgRam && isArgReg)
        {
          PUSH(spu->RAM[size_t(spu->regs[registerNum])]);
        }
      else if (isArgReg)
        {
          PUSH(spu->regs[registerNum]);
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
          spu->RAM[size_t(spu->regs[registerNum])] = POP();
        }
      else if (isArgReg && isArgRam && isArgIm)
        {
          spu->RAM[size_t(spu->regs[registerNum]) + size_t(value)] = POP(); // change to getArg
        }
      else if (isArgRam && isArgIm)
        {
          spu->RAM[size_t(value)] = POP();
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
      // example: in reg
      // if (isArgReg)
      //   {
      //     spu->regs[registerNum] = POP(); // 
      //   }
      // else if (isArgRam)
      //   {
      //     spu->RAM[size_t(value)] = POP();
      //   }
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
          *curPosition = labelAddress;              \
    })

JUMP_COMMAND(JNE, 14, !doubleCompare(a, b))
JUMP_COMMAND(JE, 15, doubleCompare(a, b))
JUMP_COMMAND(JBE, 16, a <= b)
JUMP_COMMAND(JB, 17, a < b)
JUMP_COMMAND(JAE, 18, a >= b)
JUMP_COMMAND(JA, 19, a > b)

DEF_COMMAND(JMP, 13, 1,
    {
      *curPosition = labelAddress;     
    })

DEF_COMMAND(CALL, 20, 1,
    {
      PUSHC(*curPosition);
      
      *curPosition = labelAddress;
    })

DEF_COMMAND(RET, 21, 0,
    {
      *curPosition = POPC();
    })

DEF_COMMAND(MEOW, 22, 0,
    {
      printf("meow :3\n");
    })