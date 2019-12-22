#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>

#define MALLOC_SIZE 1024

class GlobalVar
{
public:
    GlobalVar() {
        volatile char *p = (char *)malloc(MALLOC_SIZE);
        printf("malloctest1: malloc address: 0x%p ~ 0x%p\n", &p[0], &p[MALLOC_SIZE - 1]);
    }
};

GlobalVar globalVar;