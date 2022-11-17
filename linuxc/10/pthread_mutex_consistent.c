#include <stdio.h>
#include <pthread.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>

#define handle_error_en(err, msg)   \
        do { errno = err; perror(msg); exit(EXIT_FAILURE); } while(0);

pthread_mutex_t g_mutex;

static void* tmain(void *arg)
{
    puts("tmain running, lock");
    pthread_mutex_lock(&g_mutex);
    puts("tmain lock success");

    pthread_exit((void*) 2);
}

int main(int argc, char** argv)
{
    pthread_mutexattr_t mutexattr;
    pthread_t tid;
    void *res;

    pthread_mutexattr_init(&mutexattr);
    pthread_mutexattr_setrobust(&mutexattr, PTHREAD_MUTEX_ROBUST);

    pthread_mutex_init(&g_mutex, &mutexattr);

    pthread_mutexattr_destroy(&mutexattr);

    int rt = pthread_create(&tid, NULL, &tmain, NULL);
    if(rt != 0)
        handle_error_en(rt, "pthread_create");

    sleep(1);
    
    puts("main thread lock robust lock");
    rt = pthread_mutex_lock(&g_mutex);
    if(rt != 0 && rt == EOWNERDEAD) {
        //这里要更改加锁属主
        puts("call pthread_mutex_consistent");
        rt = pthread_mutex_consistent(&g_mutex);
        if(rt != 0)
            handle_error_en(rt, "pthread_mutex_consistent");
    }

    rt = pthread_mutex_unlock(&g_mutex);
    if(rt == 0) {
        puts("pthread_mutex_unlock sucess");
    }
    else {
        puts("pthread_mutex_unlock failure");
    }

    pthread_join(tid, &res);
    pthread_mutex_destroy(&g_mutex);
    printf("thread id %ld, res = %ld\n", tid, (long)res);
    puts("process end");
    
    
    return 0;
}