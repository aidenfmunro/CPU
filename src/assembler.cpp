#include <stdio.h>
#include <string.h>
#include <math.h>
#include "assembler.h"
#include "config.h"
#include "textfuncs.h"
#include "stackfuncs.h"
#include "utils.h"
#include "enum.h"

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
    double immed; // TODO: immedArg, immedValue?

    ErrorCode error;
};

// TODO: Are u going to change labels?
bool labelIsInitialized(Labels* labels, const char* labelName);

// same as above
size_t findLabel(Labels* labels, const char* labelName);

RegNum getRegisterNum(const char symbol);

// TODO: runNum?
ArgRes parseArgument(FILE* listingFile, char* argument, size_t* curPosition, byte* bytecode, Labels* labels, size_t runNum);

/*
  TODO: In many functions you pass the same data
  almost the same data that characterizes state of your
  assembly process (labels, runNum, position, bytecode)

  May be connect it to reduce this cumbersome lists of args?
*/
ErrorCode proccessLabel(char* curLine, Labels* labels, size_t* curPosition);

/*
  TODO: Divide into files.

  Now your assembler.cpp file looks heterogeneous.
  It has concrete subsystems (e.g. label system),
  but they are described in the same one big file.

  It would be less difficult to develop,
  if you explicitly divide it on subsystems.
*/
ErrorCode proccessLine(Text* assemblyText, FILE* listingFile, byte* bytecode, size_t index, size_t* curPosition, Labels* lables /* TODO: labels */, size_t runNum);

ErrorCode parseRam(char** argument, ArgRes* arg, FILE* listingFile);

ErrorCode parseReg(char** argument, ArgRes* arg, size_t runNum, FILE* listingFile);

ErrorCode parseImmed(char** argument, ArgRes* arg);

ErrorCode parseLabel(char** argument, ArgRes* arg, Labels* labels, size_t runNum);

/*
  TODO: If you have long list of args it's better
  to format them in way that they fit the screen.

  70-80 columns is a limit.

  E.g. you can do like I below
*/
ErrorCode parseImmedOrLabel(char** argument,
                            ArgRes* arg,
                            Labels* labels,
                            size_t runNum,
                            FILE* listingFile);

ErrorCode emitCommand(int num, int argc, byte* byteode, ArgRes* arg, size_t* curPosition, size_t runNum);

/*

  TODO:
  1) Logging is about recording history of states, events.
  But here your file will consist of only one record.
  So that doesn't seem like logging.

  May be REPORT_ERROR or smth?

  2)  Imagine that your compiler would write errors into file.
      It's not convinient. For developer it's better to output this info
      in terminal. You can remain opportunity to write it to file, but
      make it configurable - e.g. with command line flag.

  3) Too little information about error. Check how g++ reports erros.

  4) What is the purpose of using macros here?
  Why can't you do the same with functions?
*/
#define COMPILE_LOG(error) myOpen("log_compile.txt", "w", log_compile);                         \
                    fprintf(log_compile, "Error code [%d] in line: %d\n", error, index + 1);    \
                    myClose(log_compile);

/*
  TODO: Why macros?
  + Check comment about state of assembly process
  [it's above proccessLabel() function declaration]

  Here you clean resources that were captured by this process.
*/
#define FREE_EVERYTHING DestroyText(&assemblyText); free(bytecode); free(labels.label)

/*
  TODO: Rename / refactor

  WriteListing should be good at writing listing.
  
  This macros doesn't write anything as I see.
  It only manages execution of code, like shortcut for conditional statement.

  It's more like your ON_DEBUG() from Stack.

  In uses of this macros line "fprintf(listingFile, ...)" always repeats.
  It's copypaste.
*/
#define WRITE_LISTING(...) if (runNum == 2) __VA_ARGS__

// TODO: Why need this??
#define RETURN_ERROR(error)                                                                                         \
do                                                                                                                  \
{                                                                                                                   \
    __typeof__(error) _error = error;                                                                               \
    if (_error != 0)                                                                                                \
        return _error;                                                                                              \
} while (0)

ErrorCode Compile(const char* filename, const char* listingFileName)
{
    Text assemblyText = {};

    CreateText(&assemblyText, filename, NONE);
    
    size_t numLines = assemblyText.numLines;

    byte* bytecode = (byte*)calloc(numLines, sizeof(elem_t) * 2); // TODO: calloc return macro/function check

    Labels labels = {.count = 0};

    // TODO: Is there need in dynamic memory allocation?
    labels.label = (Label*)calloc(MAX_LABEL_AMOUNT, sizeof(Label));

    // TODO: Check pointer right after calling calloc
    CheckPointerValidation(bytecode); // TODO: mycalloc to check for return value

    myOpen(listingFileName, "w", listingFile);

    size_t curPosition = 0;

    int error = OK;

    for (size_t runNum = 1; runNum < 3; runNum++)
      {
        curPosition = 0;

        // TODO: Split into lines
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
                COMPILE_LOG(error);
                RETURN_ERROR(error); // TODO: Still don't get why need this
              }
          }
      }  
    
    myOpen("code.bin", "wb", codebin);

    /*
      TODO: Does your code really have such size?
    
      push 1
      push 2
      add

      It's size is not 6, but 5
    */
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

        // TODO: Why not strncpy?
        memcpy(labels->label[labels->count++].name, labelName, strlen(labelName));

        return OK;
      }

    return REPEATING_LABEL;    
}

size_t findLabel(Labels* labels, const char* labelName) // TODO: change size_t to a typedef
{
    for (size_t i = 0; i < labels->count; i++)
      {
        /*
          TODO: Recompute length of label every time you make search
          is waste of CPU. Why not remember it when add it to array?
        */
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

    // TODO: Put variable declarations next to using
    char* labelPtr = strchr(curLine, int(':'));

    // same above
    int error = SYNTAX_ERROR;

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

    char labelName[MAX_LABELNAME_LENGTH] = "";

    if (sscanf(curLine, "%4s%ln", command, &commandLength) != 1)
        return INCORRECT_SYNTAX;

    #define DEF_COMMAND(name, num, argc, code)                                                                              \
      if (strcasecmp(#name, command) == 0)                                                                                  \
        {                                                                                                                   \
          WRITE_LISTING(fprintf(listingFile, "%5ld %5s[0x%016lX] %4s", *curPosition, "", *curPosition, command));           \
          ArgRes arg = parseArgument(listingFile, curLine + commandLength + 1,                                              \
                                                                  curPosition,                                              \
                                                                     bytecode,                                              \
                                                                       labels,                                              \
                                                                      runNum);                                              \
          error = emitCommand(CMD_ ## name, argc, bytecode, &arg, curPosition, runNum);                                     \
          WRITE_LISTING(fprintf(listingFile, "\n"));                                                                        \
          return OK;                                                                                                        \
        }                                                                                                                   \
        
    #include "commands.h"

    #undef DEF_COMMAND

    return INCORRECT_SYNTAX; // TODO: emit code function, commands.h make a getarg function in spu    
}

ErrorCode emitCommand(int num, int argc, byte* bytecode, ArgRes* arg, size_t* curPosition, size_t runNum)
{
    *(bytecode + *curPosition) |= num;  

    if (argc)                                                                                                         
      {                                                                                                               
          RETURN_ERROR(arg->error);                                                                                     
        *(bytecode + *curPosition) |= arg->argType;                                                                    
      }                                                                                                               
    *curPosition += sizeof(char);                                                                                     
    if (argc)                                                                                                         
      {                                                                                                               
        if (arg->argType & ARG_FORMAT_REG)                                                                             
          {                                                                                                           
            memcpy(bytecode + *curPosition, &arg->regNum, sizeof(char));                                               
            *curPosition += sizeof(char);                                                                             
          }                                                                                                           
        if (arg->argType & ARG_FORMAT_IMMED)                                                                           
          {                                                                                                             
            memcpy(bytecode + *curPosition, &arg->immed, sizeof(double));                                              
            *curPosition += sizeof(double);                                                                           
          }                                                                                                           
      }                                                                                                               
    
    return OK;                                                
}


#define RETURN_ERROR_ARG(arg) if (arg.error != 0) return arg; 

ArgRes parseArgument(FILE* listingFile, char* argument, size_t* curPosition, byte* bytecode, Labels* labels, size_t runNum)
{
    ArgRes arg = {};

    arg.argType = 0;

    arg.error = parseRam(&argument, &arg, listingFile);

    arg.error = parseReg(&argument, &arg, runNum, listingFile);
    RETURN_ERROR_ARG(arg);

    arg.error = parseImmedOrLabel(&argument, &arg, labels, runNum, listingFile);
    RETURN_ERROR_ARG(arg);
    
    return arg;
}

ErrorCode parseRam(char** argument, ArgRes* arg, FILE* listingFile)
{
    char* openBracketPtr  = strchr(*argument, '[');
    char* closeBracketPtr = strchr(*argument, ']');

    if (openBracketPtr && closeBracketPtr)
      {
        *argument = openBracketPtr + 1;
        arg->argType |= ARG_FORMAT_RAM;
      }
    if (!(openBracketPtr || closeBracketPtr))
      {
        return SYNTAX_ERROR; 
      }
    
    return OK;
}

ErrorCode parseReg(char** argument, ArgRes* arg, size_t runNum, FILE* listingFile)
{
    char regArg    = 0;

    int countChars = 0;

    if (sscanf(*argument, "r%c%n", &regArg, &countChars) == 1 && *(*argument + countChars) == 'x')
      {
        if ((*argument)[countChars] == 'x')
          {
           char regNum = getRegisterNum(regArg);

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
        {WRITE_LISTING(fprintf(listingFile, "%5sr%cx", "", arg->regNum));}
      else 
        {WRITE_LISTING(fprintf(listingFile, "%5s---", ""));}
    
    return OK;
}

ErrorCode parseImmedOrLabel(char** argument, ArgRes* arg, Labels* labels, size_t runNum, FILE* listingFile)
{
    ErrorCode temperror = parseImmed(argument, arg);

    if ((arg->argType & ARG_FORMAT_IMMED) == 0 && (arg->argType & ARG_FORMAT_REG) == 0)  
        parseLabel(argument, arg, labels, runNum);
    else
        arg->error = temperror;
    
    char* plusPtr = strchr(*argument, '+');

    if (plusPtr)
        *argument = plusPtr + 1;
    
    WRITE_LISTING(fprintf(listingFile, "%5s%lg", "", arg->immed));
    
    return arg->error;
}

ErrorCode parseImmed(char** argument, ArgRes* arg)
{
    elem_t value = POISON;

    int check = 0;

    if (sscanf(*argument, "%lg%n", &value, &check) == 1)
      {
        if (check != 0 && !isnan(value))
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

ErrorCode parseLabel(char** argument, ArgRes* arg, Labels* labels, size_t runNum)
{
    int check = 0;

    char labelName[MAX_LABELNAME_LENGTH] = "";

        sscanf(*argument, "%s%n", labelName, &check);

        size_t labelAddress = findLabel(labels, labelName);

        if (check != 0) // if no labels still add argument!!!
          {
            arg->argType |= ARG_FORMAT_IMMED; 
            
            if (labelAddress != LABEL_NOT_FOUND)
                memcpy(&arg->immed, &labelAddress, sizeof(size_t));
            if(labelAddress == LABEL_NOT_FOUND && runNum == 2)
                return NON_EXIST_LABEL;
          }    

    return OK;
}

RegNum getRegisterNum(const char symbol)
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