#include <sys/mman.h>
#include <stdbool.h>
#include <unistd.h>

#include <sys/mman.h>
#include <stdbool.h>
#include <unistd.h>

bool isPointerValid(void *ptr) 
{
    size_t pageSize = sysconf(_SC_PAGESIZE); // get the page size

    void *base = (void *)((((size_t)ptr) / pageSize) * pageSize); // find the address of the page that contains p

    return msync(base, pageSize, MS_ASYNC) == 0; // call msync, if it returns non-zero, return false 
}