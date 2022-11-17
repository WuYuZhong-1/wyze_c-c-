#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <pthread.h>
#include <sys/syscall.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>

#define err_exit(rt, msg)   \
    do { errno = rt; perror(msg); exit(EXIT_FAILURE); } while(0)

void sig_int(int signo)
{
    puts("--------------------");
    printf("tid = %ld  exec SIGINT\n", pthread_self());
    printf("getpid = %d  exec SIGINT\n", getpid());
    printf("syscall = %ld  exec SIGINT\n", syscall(SYS_gettid));
    puts("--------------------");
}


void *tmain(void* arg)
{
    puts("--------------------");
    printf("tid = %ld  exec begin\n", pthread_self());
    printf("getpid = %d  exec begin\n", getpid());
    printf("syscall = %ld  exec begin\n", syscall(SYS_gettid));
    puts("--------------------");
    pause();

    puts("--------------------");
    printf("tid = %ld  exec end\n", pthread_self());
    printf("getpid = %d  exec end\n", getpid());
    printf("syscall = %ld  exec end\n", syscall(SYS_gettid));
    puts("--------------------");
}

int main(int argc, char** argv)
{
    int rt;
    const int thrs = 3;
    pthread_t tid[thrs];


    struct sigaction act;
    memset(&act, 0, sizeof(act));
    act.sa_handler = &sig_int;
    act.sa_flags = 0;
    sigemptyset(&act.sa_mask);
    rt = sigaction(SIGINT, &act, NULL);
    if(rt != 0) {
        perror("sigaction");
        return 0;
    }

    puts("--------------------");
    printf("tid = %ld  exec begin\n", pthread_self());
    printf("getpid = %d  exec begin\n", getpid());
    printf("syscall = %ld  exec begin\n", syscall(SYS_gettid));
    puts("--------------------");

    for(int i = 0; i < thrs; ++i) {
        rt = pthread_create(&tid[i], NULL, &tmain, NULL) ;
        if(rt != 0)
            err_exit(rt, "pthread_create");
        usleep(1000);
    }

    pause();

    puts("--------------------");
    printf("tid = %ld  exec end\n", pthread_self());
    printf("getpid = %d  exec end\n", getpid());
    printf("syscall = %ld  exec end\n", syscall(SYS_gettid));
    puts("--------------------");

    // while(1);

    for(int i = 0; i < thrs; ++i) 
        pthread_join(tid[i], NULL);

    return 0;
}