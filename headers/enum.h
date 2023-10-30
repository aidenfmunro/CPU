enum Command
{
    #define DEF_COMMAND(name, num, argc, code) CMD_ ## name = num,

    #include "commands.h"

    #undef DEF_COMMAND
};