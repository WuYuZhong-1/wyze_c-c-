#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <signal.h>
#include <pthread.h>
#include <unistd.h>

#define err_exit(rt, msg)   \
        do { errno = rt; perror(msg); exit(EXIT_FAILURE); } while(0)

static int  quitflag;
static sigset_t mask;

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;;
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;

void *tmain(void* arg) 
{
    int rt, signo;

    sigset_t sigset;
    rt = pthread_sigmask(0,NULL, &sigset);
    if(rt != 0) 
        err_exit(rt, "pthread_sigmask");
    
    if( sigismember(&sigset, SIGQUIT))
        puts("SIGQUIT is mask");
    if( sigismember(&sigset, SIGINT))
        puts("SIGINT is mask");
    if( sigismember(&sigset, SIGUSR1))
        puts("SIGUSR1 is mask");


    for(;;) {
        rt = sigwait(&mask, &signo);
        if(rt != 0) 
            err_exit(rt, "sigwait");
        
        switch(signo) {
            case SIGINT:
                puts("interrupt");
                break;

            case SIGQUIT:
                puts("quit");
                pthread_mutex_lock(&mutex);
                quitflag = 1;
                pthread_mutex_unlock(&mutex);
                pthread_cond_signal(&cond);
                puts("tmain end");
                return 0;
            
            default:
                printf(" unexcepted signal %d\n", signo);
                exit(1); 
        }
    }
}

int main(int argc, char** argv)
{
    int rt;
    sigset_t oldmask;
    pthread_t tid;
    printf("pid %d\n", getpid());
    sigemptyset(&mask);
    sigaddset(&mask, SIGINT);
    sigaddset(&mask, SIGQUIT);
    if( (rt = pthread_sigmask(SIG_BLOCK, &mask, &oldmask)) != 0)
        err_exit(rt, "pthread_sigmask");

    rt = pthread_create(&tid, NULL, &tmain, NULL);
    if( rt != 0) 
        err_exit(rt, "pthread_create");

    pthread_mutex_lock(&mutex);
    while(quitflag == 0)
        pthread_cond_wait(&cond, &mutex);
    pthread_mutex_unlock(&mutex);

    puts("begin end");
    pthread_join(tid, NULL);

    rt = pthread_sigmask(SIG_SETMASK, &oldmask, NULL);
    if(rt != 0) 
        err_exit(rt, "pthread_sigmask");

    puts("main end");
    return 0;
}



