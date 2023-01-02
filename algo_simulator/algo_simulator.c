#include <unistd.h>
#include <signal.h>
#include <time.h>
#include <sys/time.h>
#include <stdio.h>
#include <math.h>
#include <stdlib.h>

int count = 0;
int total_time = 0;
const long TIMES = 294802;

int get_diff_us(struct timeval *before, struct timeval *after)
{
    int time_before = before->tv_sec * 1000000 + before->tv_usec;
    int time_after = after->tv_sec * 1000000 + after->tv_usec;
    return time_after - time_before;
}

double calc_pi(int num)
{
    const long num_steps = TIMES * num;
    double x, sum = 0.0;
    volatile double pi;
    double step = 1.0 / (double)num_steps;
    for (long i = 0; i < num_steps; i++)
    {
        x = (i + 0.5) * step;
        sum = sum + 4.0 / (1.0+x*x);
    }
    pi = step * sum;

    return pi;
}

void handle() 
{
    struct timeval before, after;
    gettimeofday(&before, NULL);
    calc_pi(10);
    gettimeofday(&after, NULL);
    total_time += get_diff_us(&before, &after);

    count++;
}

void handle2()
{
    struct timeval before, after;
    gettimeofday(&before, NULL);
    calc_pi(20);
    gettimeofday(&after, NULL);
    total_time += get_diff_us(&before, &after);

    count++;
}

void do_exit()
{
    printf("process time is %d us, count is %d, %f ms/pre count\n", total_time, count, (float)total_time / count / 1000);
    exit(0);
}

int main(int argc, char *argv[])
{
    signal(SIGUSR1, handle);
    signal(SIGUSR2, handle2);
    signal(SIGINT, do_exit);

    struct sigevent evp;
    struct itimerspec ts;
    timer_t timer;
    timer_t timer2;
    int ret;
    evp.sigev_value.sival_ptr = &timer;
    evp.sigev_notify = SIGEV_SIGNAL;
    evp.sigev_signo = SIGUSR1;
    ret = timer_create(CLOCK_REALTIME, &evp, &timer);
    evp.sigev_signo = SIGUSR2;
    ret = timer_create(CLOCK_REALTIME, &evp, &timer2);

    ts.it_interval.tv_sec = 0;
    ts.it_interval.tv_nsec = 100000000;
    ts.it_value.tv_sec = 0;
    ts.it_value.tv_nsec = 50000000;
    ret = timer_settime(timer, 0, &ts, NULL);
    ts.it_value.tv_nsec = 100000000;
    ret = timer_settime(timer2, 0, &ts, NULL);

    while(1) {
        pause();
    }
}