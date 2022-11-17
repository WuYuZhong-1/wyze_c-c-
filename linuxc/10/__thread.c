#include <stdio.h>
#include <pthread.h>
#include <errno.h>
#include <stdlib.h>

#define handle_error_en(rt, msg)    \
        do { errno = rt; perror(msg); exit(EXIT_FAILURE); } while(0)

static __thread int count = 0;

static void* tmain(void *arg)
{
    printf("tmain count = %d\n", count);
    count = 2;
    printf("tmain count = %d\n", count);
}

int main(int argc, char** argv) 
{
    pthread_t tid;
    int rt;

    printf("main count = %d\n", count);
    count = 1;

    rt = pthread_create(&tid, NULL, &tmain, NULL);
    if(rt != 0)
        handle_error_en(rt, "pthread_create");
    
    pthread_join(tid, NULL);
    printf("main count = %d\n", count);

    return 0;
}