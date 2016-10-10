#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <signal.h>

#include "test.h"

void usage();

int main(int argc, char **argv)
{
    int nchilds, nloops, nbytes, port, i, j, connfd, n;
    const char *ptr;
    struct sockaddr_in servaddr;
    char request[1024], reply[10240];
    pid_t pid;

    if (argc != 6)
        usage();
    ptr = argv[1];
    port = atoi(argv[2]);
    nchilds = atoi(argv[3]);
    nloops = atoi(argv[4]);
    nbytes = atoi(argv[5]);
    fprintf(stderr, "ip: %s\nport: %d\nnchilds: %d\nnloops: %d\nnbytes: %d\n", ptr, port, nchilds, nloops, nbytes);
    snprintf(request, sizeof request, "%d\n", nbytes);

    memset(&servaddr, 0, sizeof servaddr);
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(port);
    inet_pton(AF_INET, ptr, &servaddr.sin_addr);

    for (i = 0; i < nchilds; ++i)
    {
        if ((pid = fork()) == 0)
        {
            for (j = 0; j < nloops; ++j)
            {
                connfd = socket(AF_INET, SOCK_STREAM, 0);
                connect(connfd, (struct sockaddr *)&servaddr, sizeof servaddr);
                write(connfd, request, strlen(request));

                if ((n = readn(connfd, reply, nbytes)) != nbytes)
                {
                    fprintf(stderr, "can't read nbytes from server\n");
                    exit(1);
                }
                //reply[n] = 0;
                //fprintf(stderr, "%s\n", reply);

                close(connfd);
            }
        }
    }

    while (wait(NULL) > 0)
        ;

    if (errno != ECHILD)
    {
        fprintf(stderr, "exe error\n");
        exit(1);
    }

    return 0;
}

void usage()
{
    fprintf(stderr, "need 5 args: ip, port, childs, loops, nbytes\n");
    exit(1);
}
