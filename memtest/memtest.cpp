#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <dlfcn.h>

#define ENUM(x) (sizeof(x) / (sizeof(x[0])))
#define CMD_LINE_SIZE 128
#define MEMORY_SIZE 1024

static void func_heap_overflow(const char *param)
{
    size_t size = param ? atoi(param) : MEMORY_SIZE;
    volatile char *p = (char *)malloc(size);
    printf("heap size: %zu, address: %p ~ %p\n", size, &p[0], &p[size - 1]);

    p[size] = 0xA5;

    free((void *)p);
}

static void func_heap_underflow(const char *param)
{
    size_t size = param ? atoi(param) : MEMORY_SIZE;
    volatile char *p = (char *)malloc(size);
    printf("heap size: %zu, address: %p ~ %p\n", size, &p[0], &p[size - 1]);

    p[-1] = 0xA5;

    free((void *)p);
}

static void func_heap_leak(const char *param)
{
    size_t size = param ? atoi(param) : MEMORY_SIZE;
    volatile char *p = (char *)malloc(size);
    printf("heap size: %zu, address: %p ~ %p\n", size, &p[0], &p[size - 1]);
}

static char *ptr;
static void func_heap_malloc(const char *param)
{
    size_t size = param ? atoi(param) : MEMORY_SIZE;
    ptr = (char *)malloc(size);
    printf("heap size: %zu, address: %p ~ %p\n", size, &ptr[0], &ptr[size - 1]);
    for (size_t i = 0; i < size; ++i) {
        ptr[i] = 0x5A;
    }
}

static void func_heap_free(const char *param)
{
    (void)param;
    free(ptr);
    printf("free address: %p\n", ptr);
}

static void func_heap_repeat_free(const char *param)
{
    size_t size = param ? atoi(param) : MEMORY_SIZE;
    volatile char *p = (char *)malloc(size);
    printf("heap size: %zu, address: %p ~ %p\n", size, &p[0], &p[size - 1]);

    free((void *)p);
    free((void *)p);
}

static void func_heap_use_after_free(const char *param)
{
    size_t size = param ? atoi(param) : MEMORY_SIZE;
    volatile char *p = (char *)malloc(size);
    printf("heap size: %zu, address: %p ~ %p\n", size, &p[0], &p[size - 1]);
    free((void *)p);

    p[0] = 0xA5;
}

static void func_heap_leak_on_dlopen(const char *param)
{
    (void)param;

    void *handle;
    handle = dlopen("libmemtest1.so", RTLD_NOW);
    if (!handle) {
        printf("dlopen failed: %s\n", dlerror());
        return;
    }
}

static void func_heap_leak_on_dlopen_sym(const char *param)
{
    (void)param;

    void *handle;
    handle = dlopen("libmemtest2.so", RTLD_NOW);
    if (!handle) {
        printf("dlopen failed: %s\n", dlerror());
        return;
    }

    void (*sym)(void) = (void (*)())dlsym(handle, "do_something");
    if (!sym) {
        printf("dlsym failed: %s\n", dlerror());
        return;
    }

    sym();
}

extern "C" void do_something(void);
static void func_heap_leak_on_so_sym(const char *param)
{
    (void)param;
    do_something();
}

static void func_heap_get_info(const char *param)
{
    (void)param;
    struct mallinfo info = mallinfo();

    printf("info: arena = %zu, ordblks = %zu, smblks = %zu, hblks = %zu, hblkhd = %zu, ",
            info.arena,
            info.ordblks,
            info.smblks,
            info.hblkhd,
            info.usmblks);
    printf("usmblks = %zu, fsmblks = %zu, uordblks = %zu, fordblks = %zu, keepcost = %zu\n",
           info.usmblks,
           info.fsmblks,
           info.uordblks,
           info.fordblks,
           info.keepcost);

    malloc_info(0, stdout);
}

static void func_exit(const char *param)
{
    (void)param;
    exit(0);
}

static void func_stack_overflow(const char *param)
{
    size_t size = param ? atoi(param) : MEMORY_SIZE;
    volatile char p[size];
    printf("heap size: %zu, address: %p ~ %p\n", size, &p[0], &p[size - 1]);

    p[size] = 0xA5;
}

static void func_stack_underflow(const char *param)
{
    size_t size = param ? atoi(param) : MEMORY_SIZE;
    volatile char p[size];
    printf("stack size: %zu, address: %p ~ %p\n", size, &p[0], &p[size - 1]);

    p[-1] = 0xA5;
}

static void func_stack_static_overflow(const char *param)
{
    (void)param;
    volatile char p[MEMORY_SIZE];
    printf("heap size: %zu, address: %p ~ %p\n", (size_t)MEMORY_SIZE, &p[0], &p[MEMORY_SIZE - 1]);

    p[MEMORY_SIZE] = 0xA5;
}

static void func_stack_static_underflow(const char *param)
{
    (void)param;
    volatile char p[MEMORY_SIZE];
    printf("heap size: %zu, address: %p ~ %p\n", (size_t)MEMORY_SIZE, &p[0], &p[MEMORY_SIZE - 1]);

    p[-1] = 0xA5;
}

static volatile char *__func_stack_use_after_return__(void)
{
    volatile char p[MEMORY_SIZE];
    printf("stack size: %zu, address: %p ~ %p\n", (size_t)MEMORY_SIZE, &p[0], &p[MEMORY_SIZE - 1]);

    return &p[0];
}

static void func_stack_use_after_return(const char *param)
{
    (void)param;

    volatile char *p = __func_stack_use_after_return__();
    printf("address: %p\n", p);

    *p = 0xA5;
    printf("p = 0x%x\n", *p);
}

static void func_stack_use_after_scope(const char *param)
{
    volatile char *p;
    (void)param;
    {
        char x = 0;
        p = &x;
    }

    *p = 0xA5;
}

static volatile char global_buffer[MEMORY_SIZE];
static void func_global_overflow(const char *param)
{
    (void)param;
    printf("global size: %zu, address: %p ~ %p\n", (size_t)MEMORY_SIZE, &global_buffer[0], &global_buffer[MEMORY_SIZE - 1]);

    global_buffer[MEMORY_SIZE] = 0xA5;
}

static void func_global_underflow(const char *param)
{
    (void)param;
    printf("global size: %zu, address: %p ~ %p\n", (size_t)MEMORY_SIZE, &global_buffer[0], &global_buffer[MEMORY_SIZE - 1]);

    global_buffer[-1] = 0xA5;
}

const struct {
    const char *cmdline;
    void (*func)(const char *param);
} cmd_table[] = {
    {"heap_overflow", func_heap_overflow},
    {"heap_underflow", func_heap_underflow},
    {"heap_leak", func_heap_leak},
    {"heap_malloc", func_heap_malloc},
    {"heap_free", func_heap_free},
    {"heap_repeat_free", func_heap_repeat_free},
    {"heap_use_after_free", func_heap_use_after_free},
    {"heap_leak_on_dlopen", func_heap_leak_on_dlopen},
    {"heap_leak_on_dlopen_sym", func_heap_leak_on_dlopen_sym},
    {"heap_leak_on_so_sym", func_heap_leak_on_so_sym},
    {"heap_get_info", func_heap_get_info},


    {"stack_overflow", func_stack_overflow},
    {"stack_underflow", func_stack_underflow},
    {"stack_static_overflow", func_stack_static_overflow},
    {"stack_static_underflow", func_stack_static_underflow},
    {"stack_use_after_return", func_stack_use_after_return},
    {"stack_use_after_scope", func_stack_use_after_scope},

    {"global_overflow", func_global_overflow},
    {"global_underflow", func_global_underflow},

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
    if (!cmdline) {
        printf("Invalid command.\n");
        return;
    }

    char *param = (char *)strchr(cmdline, ' ');
    if (param) {
        *param = '\0';
        do {
            ++param;
        } while (*param == ' ');
    }

    size_t index;
    index = (size_t)atoi(cmdline);
    if (index == 0 || index >= ENUM(cmd_table) + 1) {
        printf("Invalid command.\n");
        return;
    }

    printf("func: %s\n", cmd_table[index - 1].cmdline);
    cmd_table[index - 1].func(param);
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
