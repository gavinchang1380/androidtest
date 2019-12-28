#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>

#define MEMORY_SIZE 1024

extern "C" void do_something(void)
{
    volatile char *p = (char *)malloc(MEMORY_SIZE);
    printf("memtest3: heap size: %zu, address: %p ~ %p\n", (size_t)MEMORY_SIZE, &p[0], &p[MEMORY_SIZE - 1]);
}
