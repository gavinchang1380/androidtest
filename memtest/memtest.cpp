#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>

volatile char global_var_temp1[16];
volatile char global_var[1024];
volatile char global_var_temp2[16];

int main()
{
    volatile char local_var_temp1[16];
    volatile char local_var[1024];
    volatile char local_var_temp2[16];

    printf("pid is %d\n", getpid());
    printf("The global_var_temp1 is from 0x%p to 0x%p\n", &global_var_temp1[0], &global_var_temp1[15]);
    printf("The global_var is from 0x%p to 0x%p\n", &global_var[0], &global_var[1023]);
    printf("The global_var_temp2 is from 0x%p to 0x%p\n", &global_var_temp2[0], &global_var_temp2[15]);

    global_var[-1] = 0x5A;
    global_var[1024] = 0xA5;

    getchar();

    return 0;
}