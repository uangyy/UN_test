/*
 * 每个现成处理请求
 * */

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <signal.h>
#include <errno.h>
#include <arpa/inet.h>
#include <pthread.h>

#include "test.h"

typedef struct{
    pthread_t tid;
    int count;
} Thread;

static Thread *threads;

void thread_make(int, int);

void thread_main(void *);

int main(int argc, char **argv)
{
    int listenfd, connfd, len, port = 8765, nthread = 10, i;
    struct sockaddr_in servaddr, cliaddr;

    memset(&servaddr, 0, sizeof servaddr);
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(port);
    servaddr.sin_addr.s_addr = INADDR_ANY;

    listenfd = socket(AF_INET, SOCK_STREAM, 0);
    bind(listenfd, (struct sockaddr *)&servaddr, sizeof servaddr);
    listen(listenfd, 128);

    threads = calloc(sizeof(Thread), nthread);

    for (i = 0; i < nthread; ++i)
        thread_make(i, listenfd);

    for ( ; ; )
        pause();
    return 0;
}

void thread_make(int i, int listenfd)
{
    pthread_create(&threads[i], NULL, thread_main, (void *)listenfd);
    return;
}

void thread_main(void *args)
{
    int listenfd = (int)args, len, connfd;
    struct sockaddr_in cliaddr;
    len = sizeof cliaddr;
    for ( ; ; )
    {
        if ((connfd = accept(listenfd, (struct sockaddr *)&cliaddr, &len)) <= 0)
            err_sys("accept error");

        web_server(connfd);

        close(connfd);
    }
}
