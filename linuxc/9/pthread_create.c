#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include  <sys/syscall.h>


void *tmain(void *arg)
{
    printf("sub thread id %ld\n", pthread_self());
    printf("sub thread pid %d\n", getpid());
    printf("sub tthread tid %ld\n", syscall(SYS_gettid)); 
    sleep(60);
}

int main(int argc, char** argv)
{
    pthread_t tid;
    void *status;

    int rt = pthread_create(&tid, NULL, &tmain, NULL);
    if(rt != 0) {
        puts("pthread_create error");
        return 1;
    }

    printf("main thread id %ld\n", pthread_self());
    printf("main thread pid %d\n", getpid());
    printf("main tid %ld\n", syscall(SYS_gettid));

    sleep(3);
    pthread_cancel(tid);        //PTHREAD_CANCELED;

    pthread_join(tid, &status);
    puts("--------------");
    printf("%d\n", ((int)status));
    

    return 0;
}