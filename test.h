#include <stddef.h>
size_t readn(int sockfd, char *ptr, size_t n);
void sig_int(int sig_int);
static void pr_cpu_time(void);
void err_sys(const char *ptr);
