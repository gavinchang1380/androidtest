#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <dlfcn.h>

#define ENUM(x) (sizeof(x) / (sizeof(x[0])))
#define CMD_LINE_SIZE 128
#define MALLOC_SIZE 1024

static void func_overflow(void)
{
    volatile char *p = (char *)malloc(MALLOC_SIZE);
    printf("malloc address: %p ~ %p\n", &p[0], &p[MALLOC_SIZE - 1]);
    for (int i = 0; i < 100; ++i) {
        p[MALLOC_SIZE + i] = 0xA5;
    }

    free((void *)p);
}

static void func_underflow(void)
{
    volatile char *p = (char *)malloc(MALLOC_SIZE);
    printf("malloc address: %p ~ %p\n", &p[0], &p[MALLOC_SIZE - 1]);
    for (int i = 0; i < 100; ++i) {
        p[-1 - i] = 0xA5;
    }

    free((void *)p);
}

static void func_leak(void)
{
    char *p = (char *)malloc(MALLOC_SIZE);
    printf("malloc address: %p ~ %p\n", &p[0], &p[MALLOC_SIZE - 1]);
}

static char *ptr;
static void func_malloc(void)
{
    ptr = (char *)malloc(MALLOC_SIZE);
    printf("malloc address: %p ~ %p\n", &ptr[0], &ptr[MALLOC_SIZE - 1]);
}

static void func_free(void)
{
    free(ptr);
    printf("free address: %p\n", ptr);
}

static void func_user_after_free(void)
{
    volatile char *p = (char *)malloc(MALLOC_SIZE);
    printf("malloc address: %p ~ %p\n", &p[0], &p[MALLOC_SIZE - 1]);
    free((void *)p);

    for (int i = 0; i < 100; ++i) {
        p[i] = 0xA5;
    }
}

static void func_leak_on_dlopen(void)
{
    void *handle;
    handle = dlopen("libmalloctest1.so", RTLD_NOW);
    if (!handle) {
        printf("dlopen failed\n");
        return;
    }
}

static void func_leak_on_dlopen_sym(void)
{
    void *handle;
    handle = dlopen("libmalloctest2.so", RTLD_NOW);
    if (!handle) {
        printf("dlopen failed\n");
    }

    void (*sym)(void) = (void (*)())dlsym(handle, "do_something");
    if (!sym) {
        printf("dlsym failed\n");
    }

    sym();
}

extern "C" void do_something(void);
static void func_leak_on_so_sym(void)
{
    do_something();
}


static void func_exit(void)
{
    exit(0);
}

const struct {
    const char *cmdline;
    void (*func)(void);
} cmd_table[] = {
    {"overflow", func_overflow},
    {"underflow", func_underflow},
    {"leak", func_leak},
    {"malloc", func_malloc},
    {"free", func_free},
    {"user_after_free", func_user_after_free},
    {"leak_on_dlopen", func_leak_on_dlopen},
    {"leak_on_dlopen_sym", func_leak_on_dlopen_sym},
    {"leak_on_so_sym", func_leak_on_so_sym},
    {"exit", func_exit},
};

static bool get_cmd_line(char *cmdline, size_t len)
{
    printf("Please choose a command:\n");
    for (size_t index = 0; index < ENUM(cmd_table); ++index) {
        printf("%zu - %s\n", index + 1, cmd_table[index].cmdline);
    }
    printf("Your choose:\n");
    return fgets(cmdline, len, stdin) != nullptr;
}

static void choose_func(const char *cmdline)
{
    size_t index;
    index = (size_t)atoi(cmdline);
    if (index == 0 || index >= ENUM(cmd_table) + 1) {
        printf("Invalid command.\n");
        return;
    }

    printf("func: %s\n", cmd_table[index - 1].cmdline);
    cmd_table[index - 1].func();
}

int main()
{
    printf("pid is %d\n", getpid());

    char cmdline[CMD_LINE_SIZE];
    while (get_cmd_line(cmdline, sizeof(cmdline))) {
        choose_func(cmdline);
    }

    return 0;
}
