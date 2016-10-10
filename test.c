#include "test.h"

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <sys/resource.h>

void err_sys(const char *ptr)
{
    fprintf(stderr, "%s\n", ptr);
    exit(1);
}

size_t readn(int sockfd, char *ptr, size_t max)
{
    int nleft = max, n;
    char *curr = ptr;

    while (nleft > 0)
    {
        if ((n = read(sockfd, ptr, nleft)) < 0)
        {
            if (errno == EINTR)
                n = 0;
            else
            {
                return -1;
            }
        }
        else if (n == 0)
            break;

        nleft -= n;
        ptr += n;
    }

    return (max - nleft);
}

void sig_int(int sig_int)
{
    pr_cpu_time();
    exit(0);
}

void pr_cpu_time(void)
{
    double user, sys;
    struct rusage myusage, childusage;

    if (getrusage(RUSAGE_SELF, &myusage) < 0)
        err_sys("getrusage err");
    if (getrusage(RUSAGE_CHILDREN, &childusage) < 0)
        err_sys("getrusage err");

    user = (double) myusage.ru_utime.tv_sec + myusage.ru_utime.tv_usec / 1000000.0;
    user += (double) childusage.ru_utime.tv_sec + childusage.ru_utime.tv_usec / 1000000.0;

    sys = (double) myusage.ru_stime.tv_sec + myusage.ru_stime.tv_usec / 1000000.0;
    sys += (double) childusage.ru_stime.tv_sec + childusage.ru_stime.tv_usec / 1000000.0;

    printf("\nuser time = %g, sys time = %g\n", user, sys);
}
