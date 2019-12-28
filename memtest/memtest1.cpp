#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>

#define MEMORY_SIZE 1024

class GlobalVar
{
public:
    GlobalVar() {
        volatile char *p = (char *)malloc(MEMORY_SIZE);
        printf("memtest1: heap size: %zu, address: %p ~ %p\n", (size_t)MEMORY_SIZE, &p[0], &p[MEMORY_SIZE - 1]);
    }
};

GlobalVar globalVar;
