#ifndef UTILS_H
#define UTILS_H

bool isPointerValid(void *ptr); 

#define CheckPointerValidation(pointer)     \
    do                                      \
      {                                     \       
        if (!isPointerValid(&pointer))      \      
          return INVALID_PTR;               \
      } while(0);                           

#define myOpen(filename, mode, fileVar)     \
        FILE* fileVar = fopen(filename, mode); \
        if (fileVar == NULL)                   \
          {                                 \
            perror("Error");                \
          }                                 \

#define myRead(pointer, elemSize, fileSize, fileVar) \
        size_t num = fread(pointer, elemSize, fileSize, fileVar); \
        if (num == 0) \
          {                           \
            perror("Error");        \
          }                  

#define myClose(fileVar) \
        fclose(fileVar);

                    
  




#endif 