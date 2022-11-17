#include <stdio.h>
#include <pthread.h>
#include <time.h>
#include <string.h>

int main(int argc, char** argv)
{
    pthread_mutex_t mutex;
    struct timespec ts;
    struct tm t;
    char buf[256] = {0};

    pthread_mutex_init(&mutex, NULL);
    pthread_mutex_lock(&mutex);
    
    clock_gettime(CLOCK_REALTIME, &ts);    
    localtime_r(&ts.tv_sec, &t);
    strftime(buf, sizeof(buf), "%T", &t);
    puts(buf);

    
    ts.tv_sec += 10;
    int rt = pthread_mutex_timedlock(&mutex,  &ts);

    clock_gettime(CLOCK_REALTIME, &ts);
    memset(buf,0, sizeof(buf));
    localtime_r(&ts.tv_sec, &t);
    strftime(buf, sizeof(buf), "%T", &t);
    puts(buf);
    if(rt == 0) {
        puts("lock again");
    }
    else {
        printf("lock error: %s\n", strerror(rt));
    }

    pthread_mutex_destroy(&mutex);

    return 0;
}