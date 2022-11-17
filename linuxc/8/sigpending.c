#include <stdio.h>
#include <signal.h>
#include <unistd.h>


static void sig_quit(int sig)
{
    puts("sig_quit");
}

int main(int argc, char** argv)
{
    sigset_t sigset;
    sigset_t osigset;

    int rt ;

    if( signal(SIGQUIT, &sig_quit) == SIG_ERR) {
        perror("signal");
        return 1;
    }

    sigemptyset(&sigset);
    sigaddset(&sigset, SIGQUIT);
    rt = sigprocmask(SIG_BLOCK, &sigset, &osigset);
    if(rt == -1) {
        perror("sigprocmask");
        return 1;
    }

    puts("mask signal SIGQUIT success");
    sleep(10);

    //查看有没有未决信号
    rt = sigpending(&sigset);
    if(rt == -1) {
        perror("sigpendig");
        return 1;
    }

    if( sigismember(&sigset, SIGQUIT) ){
        puts("SIGQUIT pending");
    }

    puts("umask signal SIGQUT success");
    rt = sigprocmask(SIG_SETMASK, &osigset, NULL);
    if(rt == -1) {
        perror("sigprocmask");
        return 1;
    }
    puts("umask signal SIGQUT success");
    sleep(5);
    signal(SIGQUIT, SIG_DFL);
    puts("signal default");
    sleep(5);

    return 0;
}