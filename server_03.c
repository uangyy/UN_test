/*
 * 预先分配子进程
 */

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <signal.h>
#include <errno.h>
#include <arpa/inet.h>

#include "test.h"

static int nchildren;
static pid_t *pids;

void sig_int_(int);
pid_t child_make(int, int, int);
void child_main(int, int, int);

int main(int argc, char **argv)
{
    int listenfd, i, len, port = 8765;
    struct sockaddr_in servaddr, cliaddr;

    memset(&servaddr, 0, sizeof servaddr);
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(port);
    servaddr.sin_addr.s_addr = INADDR_ANY;
    len = sizeof servaddr;

    listenfd = socket(AF_INET, SOCK_STREAM, 0);
    bind(listenfd, (struct sockaddr *)&servaddr, len);
    listen(listenfd, 128);

    nchildren = 10;
    pids = calloc(nchildren, sizeof(pid_t));

    for (i = 0; i < nchildren; ++i)
    {
        pids[i] = child_make(i, listenfd, len);
    }

    signal(SIGINT, sig_int_);
    for( ; ; )
        pause();
    return 0;
}

void sig_int_(int sig_int)
{
    int i;
    for (i = 0; i < nchildren; ++i)
    {
        kill(pids[i], SIGTERM);
    }
    while (wait(NULL) > 0)
        ;

    if (errno != ECHILD)
        err_sys("wait error");
    pr_cpu_time();
    exit(0);
}

pid_t child_make(int i, int sockfd, int len)
{
    pid_t pid;
    if ((pid = fork()) > 0)
        return pid;
    child_main(i, sockfd, len);
}

void child_main(int i, int sockfd, int len_)
{
    int connfd, len;
    struct sockaddr_in cliaddr;

    memset(&cliaddr, 0, sizeof cliaddr);
    printf("child %d starting\n", getpid());

    for ( ; ; )
    {
        len = sizeof cliaddr;
        connfd = accept(sockfd, (struct sockaddr *)&cliaddr, &len);

        web_server(connfd);
        close(connfd);
    }
}
