#include <stdio.h>
#include <string.h>
#include <math.h>

#include "assembler.h"
#include "config.h"
#include "textfuncs.h"
#include "stackfuncs.h"
#include "utils.h"
#include "enum.h"

//

struct Label
{
    size_t address;
    char name[MAX_LABELNAME_LENGTH];
};

struct Labels
{
    size_t count;
    Label* label;
};

struct ArgRes
{
    char argType;

    char regNum;
    double immed;

    ErrorCode error;
};

//

size_t findLabel            (Labels* labels, const char* labelName);

RegNum getRegisterNum       (const char symbol);

ArgRes parseArgument        (FILE* listingFile, char* argument, Labels* labels, size_t runNum);

ErrorCode proccessLabel     (char* curLine, Labels* labels, size_t* curPosition);

ErrorCode proccessLine      (Text* assemblyText, FILE* listingFile, byte* bytecode, size_t index, size_t* curPosition, Labels* lables, size_t runNum);

ErrorCode parseRam          (char** argument, ArgRes* arg);

ErrorCode parseReg          (char** argument, ArgRes* arg, size_t runNum, FILE* listingFile);

ErrorCode parseImmed        (char** argument, ArgRes* arg);

ErrorCode parseLabel        (char** argument, ArgRes* arg, Labels* labels, size_t runNum);

ErrorCode parseImmedOrLabel (char** argument, ArgRes* arg, Labels* labels, size_t runNum, FILE* listingFile);

ErrorCode emitCommand       (byte num, byte argc, byte* byteode, ArgRes* arg, size_t* curPosition);

#define COMPILE_LOG(error) myOpen("log_compile.txt", "w", log_compile);                          \
                    fprintf(log_compile, "Error code [%u] in line: %ld\n", error, index + 1);    \
                    myClose(log_compile);

#define FREE_EVERYTHING DestroyText(&assemblyText); free(bytecode); free(labels.label)

#define WRITE_LISTING(...) if (runNum == 2) __VA_ARGS__

#define RETURN_ERROR(error)                                                                                         \
do                                                                                                                  \
{                                                                                                                   \
    __typeof__(error) _error = error;                                                                               \
    if (_error != 0)                                                                                                \
        return _error;                                                                                              \
} while (0)

ErrorCode Compile(const char* filename, const char* listingFileName, const char* binfilename)
{
    Text assemblyText = {};

    CreateText(&assemblyText, filename, NONE);
    
    size_t numLines = assemblyText.numLines;

    byte* bytecode = (byte*)calloc(numLines, sizeof(elem_t) * 2); // TODO: calloc return macro/function check

    Labels labels = {.count = 0};

    labels.label = (Label*)calloc(MAX_LABEL_AMOUNT, sizeof(Label));

    CheckPointerValidation(bytecode); // TODO: mycalloc to check for return value

    myOpen(listingFileName, "w", listingFile);

    size_t curPosition = 0;

    int error = OK;

    for (size_t runNum = 1; runNum < 3; runNum++)
    {
        curPosition = 0;

        WRITE_LISTING(fprintf(listingFile, "%s\nAuthor: Aiden Munro\nVersion: 2.5\n\nAll labels:\n\n", getTime())); // TODO: split into two lines

        for (size_t i = 0; i < labels.count; i++)
        {
            WRITE_LISTING(fprintf(listingFile, "\t\t%8s\t0x%08lX\n", labels.label[i].name, labels.label[i].address));
        }

        WRITE_LISTING(fprintf(listingFile, "\n%5s%14s%17s%8s%11s\n",
                                           "Line:", "Address:", "Cmd:", "Reg:", "Value:"));

        for (size_t index = 0; index < numLines; index++)
        {
            error = proccessLine(&assemblyText, listingFile, bytecode, index, &curPosition, &labels, runNum);
            if (error)
            {
                FREE_EVERYTHING;
                COMPILE_LOG(error);
                RETURN_ERROR(error);
            }
        }
    }  
    
    myOpen(binfilename, "wb", codebin);

    myWrite(bytecode, sizeof(byte), numLines * 2 * sizeof(elem_t), codebin);

    myClose(codebin);

    myClose(listingFile);

    FREE_EVERYTHING;

    return error;
}

const size_t LABEL_NOT_FOUND = -1;

ErrorCode proccessLabel(char* curLine, Labels* labels, size_t* curPosition)
{
    CheckPointerValidation(labels);

    char labelName[MAX_LABELNAME_LENGTH] = "";

    sscanf(curLine, "%s", labelName);

    if (findLabel(labels, labelName) == LABEL_NOT_FOUND)
    {
        labels->label[labels->count].address = *curPosition;

        memcpy(labels->label[labels->count++].name, labelName, strlen(labelName));

        return OK;
    }

    return REPEATING_LABEL;    
}

size_t findLabel(Labels* labels, const char* labelName)
{
    for (size_t i = 0; i < labels->count; i++)
    {
        if (strncmp(labelName, labels->label[i].name, strlen(labelName)) == 0)
        {
            return labels->label[i].address;
        }
    }

    return LABEL_NOT_FOUND;
}

ErrorCode proccessLine(Text* assemblyText, FILE* listingFile, byte* bytecode, size_t index, size_t* curPosition, Labels* labels, size_t runNum) // TODO: input const char*
{
    CheckPointerValidation(assemblyText);
    CheckPointerValidation(bytecode);
    CheckPointerValidation(labels);

    char* curLine = assemblyText->lines[index].string;

    if (StringIsEmpty(&assemblyText->lines[index])) // ??? remake to char* 
        return OK;

    char* commentPtr = strchr(curLine, int(';'));

    if (commentPtr) // TODO: seperate to functions
        *commentPtr = 0;

    char* labelPtr = strchr(curLine, int(':'));

    if (labelPtr)
    { 
        *labelPtr = '\0';

        if (runNum == 1)
        {
            proccessLabel(curLine, labels, curPosition);
        }
        
        *labelPtr = ':';

        return OK;
    }  
  
    char command[MAX_COMMAND_LENGTH]     = "";

    size_t commandLength                 = 0;

    int error = SYNTAX_ERROR;

    if (sscanf (curLine, "%4s%n", command, &commandLength) != 1)
        return INCORRECT_SYNTAX;

    #define DEF_COMMAND(name, num, argc, code)                                                                            \
                                                                                                                          \
    if (strcasecmp (#name, command) == 0)                                                                                 \
    {                                                                                                                     \
        WRITE_LISTING(fprintf(listingFile, "%5lu %5s[0x%016lX] %4s", *curPosition, "", *curPosition, command));           \
        ArgRes arg = parseArgument (listingFile, curLine + commandLength + 1,                                             \
                                                                      labels,                                             \
                                                                     runNum);                                             \
        /* RETURN_ERROR(arg.error); */                                                                                          \
        error = emitCommand (CMD_ ## name, argc, bytecode, &arg, curPosition);                                            \
        WRITE_LISTING(fprintf(listingFile, "\n"));                                                                        \
        return OK;                                                                                                        \
    }                                                                                                                     \
        
    #include "commands.h"

    #undef DEF_COMMAND

    return error; // TODO: emit code function, commands.h make a getarg function in spu    
}

ErrorCode emitCommand (byte num, byte argc, byte* bytecode, ArgRes* arg, size_t* curPosition)
{
    *(bytecode + *curPosition) |= num;  

    if (argc)                                                                                                         
    {                                                                                                                                                                                                  
        *(bytecode + *curPosition) |= arg->argType;                                                                    
    }                         

    *curPosition += sizeof(char);  

    if (argc)                                                                                                         
    {                                                                                                               
        if (arg->argType & ARG_FORMAT_REG)                                                                             
        {                                                                                                           
            memcpy (bytecode + *curPosition, &arg->regNum, sizeof(char));                                               
            *curPosition += sizeof(char);                                                                             
        }      

        if (arg->argType & ARG_FORMAT_IMMED)                                                                           
        {                                                                                                             
            memcpy (bytecode + *curPosition, &arg->immed, sizeof(double));                                              
            *curPosition += sizeof(double);                                                                           
        }                                                                                                           
    }                                                                                                               
    
    return OK;                                                
}

#define RETURN_ERROR_ARG(arg) if (arg.error != 0) return arg; 

ArgRes parseArgument (FILE* listingFile, char* argument, Labels* labels, size_t runNum)
{
    ArgRes arg = {.argType = 0};

    arg.error = parseRam (&argument, &arg);

    arg.error = parseReg (&argument, &arg, runNum, listingFile); // RETURN_ERROR_ARG(arg);

    arg.error = parseImmedOrLabel (&argument, &arg, labels, runNum, listingFile); // RETURN_ERROR_ARG(arg);
    
    return arg;
}

#undef RETURN_ERROR_ARG

ErrorCode parseRam (char** argument, ArgRes* arg)
{
    char* openBracketPtr  = strchr (*argument, '[');
    char* closeBracketPtr = strchr (*argument, ']');

    if (openBracketPtr && closeBracketPtr)
    {
        *argument = openBracketPtr + 1;
        arg->argType |= ARG_FORMAT_RAM;
    }

    if (! (openBracketPtr || closeBracketPtr))
    {
        return SYNTAX_ERROR; 
    }
    
    return OK;
}

ErrorCode parseReg (char** argument, ArgRes* arg, size_t runNum, FILE* listingFile)
{
    char regArg    = 0;

    int countChars = 0;

    if (sscanf (*argument, "r%c%n", &regArg, &countChars) == 1)
    {
        if ((*argument)[countChars] == 'x')
        {
            char regNum = getRegisterNum (regArg);

            if (regNum == NON_EXIST_REGISTER) return NON_EXIST_REGISTER;

            arg->argType |= ARG_FORMAT_REG; 
        
            arg->regNum = regNum; 

            *argument += countChars;
        }
        else
        {
            return OK;
        }
    }
    
    if ((arg->argType & ARG_FORMAT_REG) != 0) 
        {WRITE_LISTING(fprintf (listingFile, "%5sr%cx", "", arg->regNum));}
    else 
        {WRITE_LISTING(fprintf (listingFile, "%5s---", ""));}
    
    return OK;
}

ErrorCode parseImmedOrLabel (char** argument, ArgRes* arg, Labels* labels, size_t runNum, FILE* listingFile)
{
    char* plusPtr = strchr (*argument, '+');

    if (plusPtr)
        *argument = plusPtr + 1;

    ErrorCode temperror = parseImmed (argument, arg);

    if ((arg->argType & ARG_FORMAT_IMMED) == 0 && (arg->argType & ARG_FORMAT_REG) == 0)  
        parseLabel (argument, arg, labels, runNum);
    else
        arg->error = temperror;
    
    WRITE_LISTING(fprintf (listingFile, "%5s%lg", "", arg->immed));
    
    return arg->error;
}

ErrorCode parseImmed (char** argument, ArgRes* arg)
{
    elem_t value = POISON;

    int check = 0;

    if (sscanf (*argument, "%lg%n", &value, &check) == 1)
    {
        if (check != 0 && ! isnan (value))
        {
            arg->immed = value;
            arg->argType |= ARG_FORMAT_IMMED; // what if label is the argument
            argument += check;
        }
        else
        {
            return SYNTAX_ERROR; 
        }
    }

    return OK;
}

ErrorCode parseLabel (char** argument, ArgRes* arg, Labels* labels, size_t runNum)
{
    int check = 0;

    char labelName[MAX_LABELNAME_LENGTH] = "";

        sscanf (*argument, "%s%n", labelName, &check);

        size_t labelAddress = findLabel (labels, labelName);

        if (check != 0) // if no labels still add argument!!!
        {
            arg->argType |= ARG_FORMAT_IMMED; 
            
            if (labelAddress != LABEL_NOT_FOUND)
                memcpy (&arg->immed, &labelAddress, sizeof(size_t));
            if (labelAddress == LABEL_NOT_FOUND && runNum == 2)
                return NON_EXIST_LABEL;
        }    

    return OK;
}

RegNum getRegisterNum (const char symbol)
{
    switch (symbol)
    {
        case RAX:
            return 0;

        case RBX:
            return 1;

        case RCX:
            return 2;

        case RDX:
            return 3;
        
        default:
            return NON_EXIST_REGISTER;
    }
}