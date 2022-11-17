#include <stdio.h>
#include <signal.h>
#include <errno.h>
#include <unistd.h>

int main(int argc, char** argv)
{

    sigset_t sigset;

    int save_errno = errno;

    int rt = sigprocmask(0, NULL, &sigset);
    if(rt == -1) {
        perror("sigprocmask");
        return 1;
    }

    if( sigismember(&sigset, SIGINT) ) {
        puts("SIGINT");
    }
    if( sigismember(&sigset, SIGQUIT) ) {
        puts("SIGQUIT");
    }
    if( sigismember(&sigset, SIGUSR1) ) {
        puts("SIGUSR1");
    }
    if( sigismember(&sigset, SIGALRM) ) {
        puts("SIGALRM");
    }

    sigaddset(&sigset, SIGINT);
    rt = sigprocmask(SIG_BLOCK, &sigset, NULL);
    if(rt == -1) {
        perror("sigprocmask");
        return 1;
    }

    puts("pause()");
    pause();
    puts("end");

    return 0;
}