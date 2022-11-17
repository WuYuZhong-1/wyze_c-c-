#include <stdio.h>
#include <sys/times.h>
#include <stdlib.h>
#include <unistd.h>

static long g_clcktck ;

static void pr_times(clock_t real, struct tms* start, struct tms* end)
{
    printf("real: %7.2f\n", real /(double) g_clcktck);
    printf("sys: %7.2f\n", (end->tms_stime - start->tms_stime) /(double) g_clcktck);
    printf("user: %7.2f\n", (end->tms_utime - start->tms_utime) /(double) g_clcktck);
    printf("child sys: %7.2f\n", (end->tms_cstime - start->tms_cstime) /(double) g_clcktck);
    printf("child sys: %7.2f\n", (end->tms_cutime - start->tms_cutime) /(double) g_clcktck);
}

static void do_cmd(char *cmd)
{
    struct tms tmsstart, tmsend;
    clock_t start, end;
    int status;

    do {
        printf("\ncommand: %s\n", cmd);
        if( (start = times(&tmsstart)) == -1) {
            perror("times");
            break;
        }

        if( (status = system(cmd)) == -1) {
            perror("system");
            break;
        }

        if( (end = times(&tmsend)) == -1) {
            perror("times");
            break;
        }

        printf("start = %ld\n", start);
        printf("end = %ld\n", end);
        pr_times(end-start, &tmsstart, &tmsend);

    }while(0);

    return;
}


int main(int argc, char** argv)
{
    int i;

    setbuf(stdout, NULL);
    g_clcktck =  sysconf(_SC_CLK_TCK);
    printf("g_cllktck = %ld", g_clcktck);

    for(i = 1; i < argc; ++i)
        do_cmd(argv[i]);

    exit(0);

    return 0;
}