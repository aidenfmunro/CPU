#ifndef COMMANDS_H
#define COMMANDS_H

DEF_COMMAND(PUSH, 1, isArgIm, isArgReg,
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

DEF_COMMAND(POP, 2, isArgIm, isArgReg,
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
/*
DEF_COMMAND(IN, 3, false, false,  
    {

    })
*/

DEF_COMMAND(ADD, 4, false, false,
    {
        PUSH(POP + POP);
    })

DEF_COMMAND(SUB, 5, false, false,
    {
        PUSH(0 - POP + POP);        
    })

DEF_COMMAND(MUL, 6, false, false,
    {
        PUSH(POP * POP);        
    })
    
DEF_COMMAND(DIV, 7, false, false,
    {
        PUSH(pow(POP / POP, -1));        
    })
/*
DEF_COMMAND(SQRT, 8, false, false
    {

    })
*/

/*
DEF_COMMAND(SIN, 9)
DEF_COMMAND(COS, 10)
DEF_COMMAND(OUT, 11)
*/

DEF_COMMAND(HLT, 12, false, false, 
    {
       return EXIT_CODE; 
    })

#endif