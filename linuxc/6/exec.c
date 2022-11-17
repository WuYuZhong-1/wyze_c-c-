#include <unistd.h>
#include <sys/types.h>
#include <stdio.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <signal.h>

char* env_init[] = {"USER=wyze", "PATH=/tmp", NULL};

int main(int argc, char** argv)
{
    pid_t cpid;
    int rt;

    // if(argc < 2) {
    //     printf("usage: %s execfi\n", argv[0]);
    //     return 0;
    // }

    cpid = vfork();
    if(cpid == -1) {
        perror("vfork:");
        return 1;
    }

    if( cpid == 0) {
        rt = execle("/home/wuyz/learn/wyze_c-c-/linuxc/6/myecho", "myecho", "lili", "123456", NULL, env_init);
        if(rt == -1) {
            perror("exevle");
        }
        _exit(1);
    }

    rt = waitpid(cpid, NULL, 0);
    if(rt == -1) {
        perror("waitpid");
        exit(1);
    }

    cpid = vfork();
    if(cpid == -1) {
        perror("vfork");
        return 1;
    }

    if(cpid == 0) {
        rt = execlp("echo","echo", "lili", "123456", NULL);
        if(rt == -1) {
            perror("execlp");
        }
        _exit(2);
    }

    rt = waitpid(0, NULL, 0);
    if(rt == -1) {
        perror("waitpid");
        return 1;
    }

    return 0;
}