#define DOUBLE_T
#define logfilename "log.txt"


typedef int ErrorCode;

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
    NON_EXIST_REGISTER  = '0'
};
