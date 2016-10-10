#include <stddef.h>
size_t readn(int sockfd, char *ptr, size_t n);
void sig_int(int sig_int);
void pr_cpu_time(void);
void err_sys(const char *ptr);
void web_server(int sockfd);
