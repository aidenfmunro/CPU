#include "disassembler.h"

ErrorCode Disassemble(const char* filenameIn, const char* filenameOut)
{
    myOpen(filenameIn, "rb", codebin);

    size_t fileSize = getSize(filenameIn);

    byte* tempcode = (byte*)calloc(1, fileSize);

    if (!tempcode) return NO_MEMORY;

    const byte* code = tempcode;

    myRead((char*)code, sizeof(byte), fileSize, codebin); // (char*) <- ?

    myClose(codebin);

    myOpen(filenameOut, "w", disasmRes);

    size_t curPos = 0;

    while (curPos < fileSize)                                       
    {                                                             
        switch (code[curPos] & ARG_FORMAT_CMD)                                                                                   
        {                                                                                                                      
            #define DEF_COMMAND(name, num, argc, code)                          \
            case num:                                                           \
            {                                                                   \
                cmd = code[curPos];                                             \
                double immed = 0;                                               \
                char reg = 0;                                                   \
                fprintf(disasmRes, "%4s%4s", #name, "");                        \
                curPos += sizeof(char);                                         \
                if (cmd & ARG_FORMAT_RAM)                                       \
                {                                                               \
                    fprintf(disasmRes, "[");                                    \
                }                                                               \
                if (cmd & ARG_FORMAT_REG)                                       \
                {                                                               \
                    memcpy(&reg, &code[curPos], sizeof(char));                  \
                    fprintf(disasmRes, "r%cx%4s", reg, "");                     \ 
                    curPos += sizeof(char);                                     \   
                }                                                               \
                if (cmd & ARG_FORMAT_IMMED)                                     \
                {                                                               \
                    memcpy(&immed, &code[curPos], sizeof(double));              \
                    fprintf(disasmRes, "%lg", immed);                           \
                    curPos += sizeof(double);                                   \
                }                                                               \
                if (cmd & ARG_FORMAT_RAM)                                       \
                {                                                               \
                    fprintf(disasmRes, "]");                                    \
                }                                                               \
                                                                                \
                fprintf(disasmRes, "\n");                                       \
                break;                                                          \
              }
            
            #include "commands.h"
            
            #undef DEF_COMMAND

            default:
                return SYNTAX_ERROR;
          }
      }

    myClose(disasmRes);
    
    free((char*)code);

    return OK;
}