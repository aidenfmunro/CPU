#ifndef UTILS_H
#define UTILS_H

bool isPointerValid(void *ptr); 

#define CheckPointerValidation(pointer) \
    do                                  \
      {                                 \       
        if (!isPointerValid(&pointer))  \      
          return INVALID_PTR;           \
      } while(0);                           

#endif 