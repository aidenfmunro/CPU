#ifndef DSL_H
#define DSL_H

#define PUSH(value) Push(&spu->stack, value)
#define POP()       Pop(&spu->stack)

#define PUSHC(value) Push(&spu->calls, value) // TODO: Give more clear name
#define POPC()       Pop(&spu->calls)

#endif