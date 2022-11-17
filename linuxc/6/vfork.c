#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

int g_count = 0;

int main(int argc, char** argv)
{
    int count = 0;

    pid_t pid = vfork();
    if( pid == -1) {
        perror("vfork");
        return 1;
    }
    else if( pid == 0) {
        //子进程
        puts("child running");
        for(int i = 0; i < 10; ++i) {
            ++g_count;
            sleep(1);
        }
        puts("child finish");
        // _exit(0);
        exit(0);
    }
    else {
        puts("parent running");
        printf("count = %d\ng_count = %d\n", count, g_count);
    }

    return 0;
}