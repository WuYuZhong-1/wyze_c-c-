#include <time.h>
#include <sys/time.h>
#include <stdio.h>

int main(int argc, char** argv)
{
    if(argc < 2) {
        printf("usage: %s type(1:time, 2:gettimeofday, 3:clock_gettime(real)\n", argv[0]);
        return 1;
    }

    int type;
    struct tm local;
    struct tm gm;
    struct tm* res;
    sscanf(argv[1],"%d", &type);
    printf("type = %d\n", type);

    switch(type) {
        case 1: {
            time_t t;
            t = time(NULL);
            printf("time = %ld\n", t);
            
            res = localtime_r(&t, &local);
            res = gmtime_r(&t, &gm);
        
            break;
        }
        case 2: {
            struct timeval tv;
            gettimeofday(&tv, NULL);

            printf("tv_sec = %ld\n", tv.tv_sec);
            printf("tv_usec = %ld\n", tv.tv_usec);

            res = localtime_r( (time_t*)&tv.tv_sec, &local);
            res = gmtime_r((time_t*)&tv.tv_sec, &gm);

            break;
        }
        case 3: {
            struct timespec ts;
            clock_getres(CLOCK_REALTIME, &ts);
            printf("clock_getres: tv_sec = %ld\n", ts.tv_sec);
            printf("clock_getres: tv_nsec = %ld\n", ts.tv_nsec);

            clock_gettime(CLOCK_REALTIME, &ts);
            printf("clock_gettime: tv_sec = %ld\n", ts.tv_sec);
            printf("clock_gettime: tv_nsec = %ld\n", ts.tv_nsec);

            res = localtime_r((time_t*)&ts.tv_sec, &local);
            res = gmtime_r((time_t*)&ts.tv_sec, &gm);

            break;
        }
        default:
            break;
    }

    char buf[256] = {0};
    strftime(buf, sizeof(buf), "%A", &local);
    puts(buf);
    strftime(buf, sizeof(buf), "%a", &gm);
    puts(buf);
    strftime(buf, sizeof(buf), "%B", &gm);
    puts(buf);
    strftime(buf, sizeof(buf), "%b", &gm);
    puts(buf);
    strftime(buf, sizeof(buf), "%c", &local);
    puts(buf);
    strftime(buf, sizeof(buf), "%D", &local);
    puts(buf);
    strftime(buf, sizeof(buf), "%F", &local);
    puts(buf);
    strftime(buf, sizeof(buf), "%T", &local);
    puts(buf);

    strftime(buf, sizeof(buf), "%F %T", &local);
    puts(buf);
    struct tm _tm;
    strptime(buf, "%F %T", &_tm);
    time_t ts = mktime(&_tm);
    printf("%ld\n", ts);

 
    return 0;
}