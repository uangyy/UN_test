/*
 * 每个客户一个子进程
 * */

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <signal.h>
#include <errno.h>
#include <arpa/inet.h>

#include "test.h"

void sig_chld(int sig_chld);

int main(int argc, char **argv)
{
    int listenfd, connfd, len, port = 8765;
    struct sockaddr_in cliaddr, servaddr;
    pid_t pid;

    memset(&servaddr, 0, sizeof servaddr);
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(port);
    servaddr.sin_addr.s_addr = INADDR_ANY;

    listenfd = socket(AF_INET, SOCK_STREAM, 0);
    bind(listenfd, (struct sockaddr *)&servaddr, sizeof servaddr);
    listen(listenfd, 128);

    signal(SIGINT, sig_int);
    signal(SIGCHLD, sig_chld);

    for ( ; ; )
    {
        len = sizeof cliaddr;
        memset(&cliaddr, 0, sizeof cliaddr);
        if ((connfd = accept(listenfd, (struct sockaddr *)&cliaddr, &len)) <= 0)
            err_sys("accept error");

        if ((pid = fork()) == 0)
        {
            close(listenfd);
            web_server(connfd);
            exit(0);
        }
        close(connfd);
    }

    return 0;
}

void sig_chld(int sig_chld)
{
    pid_t pid;
    int status;
    while ((pid = waitpid(-1, &status, WNOHANG)) > 0)
        ;
    return;
}
