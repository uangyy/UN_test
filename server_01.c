/*
 * 迭代服务器
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

int main(int argc, char **argv)
{
    int listenfd, connfd, port = 8765, len, nbytes;
    struct sockaddr_in servaddr, cliaddr;
    char request[1024], *reply;

    memset(&servaddr, 0, sizeof servaddr);
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(port);
    servaddr.sin_addr.s_addr = INADDR_ANY;

    listenfd = socket(AF_INET, SOCK_STREAM, 0);
    bind(listenfd, (struct sockaddr*)&servaddr, sizeof servaddr);
    listen(listenfd, 128);

    signal(SIGINT, sig_int);

    for ( ; ; )
    {
        memset(&cliaddr, 0, sizeof cliaddr);
        len = sizeof cliaddr;
        connfd = accept(listenfd, (struct sockaddr *)&cliaddr, &len);

        if (connfd <= 0)
        {
            fprintf(stderr, "accept error %s\n", strerror(errno));
            exit(1);
        }
        web_server(connfd);
    }
    return 0;
}
