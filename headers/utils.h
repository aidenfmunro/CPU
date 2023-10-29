#ifndef UTILS_H
#define UTILS_H

bool isPointerValid(void *ptr); 

int StringIsEmpty(const Line* line);

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
        if (fread(pointer, elemSize, fileSize, fileVar) == 0) \
          {                           \
            perror("Error");        \
          }

#define myWrite(pointer, elemSize, fileSize, fileVar) \
        if (fwrite(pointer, elemSize, fileSize, fileVar) == 0) \
          {                                                    \
            perror("Error");                                   \
          }                 

#define myClose(fileVar) \
        fclose(fileVar);

bool doubleCompare(double a, double b);



                    
  




#endif 