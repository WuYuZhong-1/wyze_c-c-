#include <stdio.h>
#include <signal.h>
#include <unistd.h>


static void sig_int(int sig)
{
    puts("SIGINIT");
}

int main(int argc, char** argv)
{
    sigset_t newmask, oldmask, waitmask;
    
    printf("program start: %d\n", getpid());

    if(signal(SIGINT, &sig_int) == SIG_ERR) {
        perror("signal");
        return 1;
    }

    sigemptyset(&waitmask);
    sigaddset(&waitmask, SIGUSR1);

    sigemptyset(&newmask);
    sigaddset(&newmask, SIGINT);

    if( sigprocmask(SIG_BLOCK, &newmask, &oldmask) == -1) {
        perror("sigprocmask");
        return 1;
    }

    puts("in critical region:");

    if( sigsuspend(&waitmask) != -1) {
        perror("sigsuspend");
        return 1;
    }

    puts("after return from sigsuspend:");

    if( sigprocmask(SIG_SETMASK, &oldmask, NULL) == -1) {
        perror("sigprocmask");
        return 1;
    }    

    puts("program exit");

    return 0;
}