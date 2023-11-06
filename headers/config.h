#ifndef CONFIG_H
#define CONFIG_H

#define DOUBLE_T
#define logfilename "log.txt"

// TODO: Should all the info below be in config file?
typedef int ErrorCode;

typedef char byte;

const int NULLPTR = 0;

enum ERRORS
{
    OK                  = 0,
    NO_MEMORY           = 3,
    STACK_DELETED       = 5,
    UNABLE_TO_OPEN_FILE = 7,
    UNKNOWN_ASM_COMMAND = 9,
    INVALID_PTR         = 11,
    INCORRECT_SYNTAX    = 13,
    REPEATING_LABEL     = 15,
    SYNTAX_ERROR        = 17,
    NON_EXIST_REGISTER  = 19,
    NON_EXIST_LABEL     = 21,
    ZERO_DIVISION       = 23
};

const char ARG_FORMAT_IMMED = (1 << 5);

const char ARG_FORMAT_REG = (1 << 6);

const unsigned char ARG_FORMAT_RAM = (1 << 7);

const char ARG_FORMAT_CMD = 0x1F;

#endif
