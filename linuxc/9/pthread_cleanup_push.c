#include <stdio.h>
#include <pthread.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>

#define handle_error_en(en, msg)    \
        do { errno = en; perror(msg); exit(EXIT_FAILURE); } while(0);

static int done = 0;
static int cleanup_pop_arg = 0;
static int cnt = 0;

static void cleanup_handler(void *arg)
{
    printf("Called clean-up handler\n");
    cnt = 0;
}

static void *tmain(void *arg)
{
    time_t start, curr;
    
    puts("new thread started");

    pthread_cleanup_push(&cleanup_handler, NULL);
    curr = start = time(NULL);

    while(!done){
        pthread_testcancel();
        if(curr < time(NULL)) {
            curr = time(NULL);
            printf("cnt = %d\n", cnt);
            cnt++;
        }
    }
    puts("------1--------");
    pthread_cleanup_pop(cleanup_pop_arg);
    puts("------2--------");
    return NULL;
}


int main(int argc, char** argv)
{
    pthread_t thr;
    int rt;
    void *res;

    rt = pthread_create(&thr, NULL, &tmain, NULL);
    if(rt != 0)
        handle_error_en(rt, "pthread_create");
    
    sleep(2);

    if(argc > 1) {
        if(argc > 2) {
            cleanup_pop_arg = atoi(argv[2]);
        }
        done = 1;
    }
    else {
        puts("Canceling thread");
        rt = pthread_cancel(thr);
        if(rt != 0) 
            handle_error_en(rt, "pthread_cancel");
    }

    rt = pthread_join(thr, &res);
    if(rt != 0)
        handle_error_en(rt, "pthread_join");

    if( res == PTHREAD_CANCELED)
        printf("thread was canceled; cnt = %d\n", cnt);
    else 
        printf("thread terminated normally; cont = %d\n", cnt);
        
    return 0;
}