#include <unistd.h>
#include <stdio.h>
#include <sys/times.h>
#include <sys/resource.h>
#include <sys/time.h>
#include <sys/types.h>
#include <errno.h>
#include <stdlib.h>

struct timeval g_end;
unsigned long long g_count = 0;

void checktime(char* name)
{
    struct timeval tv;

    gettimeofday(&tv, NULL);
    if( tv.tv_sec >= g_end.tv_sec && tv.tv_usec >= g_end.tv_usec) {
        printf("%s g_count = %lld\n", name, g_count);
        exit(0);
    }
}


int main(int argc, char** argv)
{
    pid_t cpid;
    int adj;
    char *name;

    setbuf(stdout, NULL);
    int nzero = sysconf(_SC_NZERO);
    printf("nzero = %d\n", nzero);

    errno = 0;
    int priority = getpriority(PRIO_USER, 0);
    if(priority == -1 && errno != 0){
        perror("getpriority");
        return 1;
    }
    printf("priority = %d\n", priority);

    errno = 0;
    int n = nice(0);
    if( n == -1 && errno != 0) {
        perror("nice");
        return 1;
    }
    printf("nice = %d\n", n);

    if(argc == 2) 
        adj = strtol(argv[1], NULL, 10);
    printf("adj = %d\n", adj);

    //设置结束的时间点
    gettimeofday(&g_end, NULL);
    g_end.tv_sec += 10;

    cpid = fork();
    if(cpid == -1) {
        perror("fork");
        return 1;
    }

    if( cpid == 0) {
        name = "child";
        int rt = nice(2*adj);
        if(rt == -1 && errno != 0) {
            perror("nice");
            exit(1);
        }
        printf("child nice value is %d\n", nice(0));
    }
    else {
        name = "parent";
        printf("parent nice value is %d\n", nice(0));
    }

    for(;;) {
        if( ++g_count == 0)
            printf("%s counter wrap\n",name);
            checktime(name);
    }

    return 0;
}