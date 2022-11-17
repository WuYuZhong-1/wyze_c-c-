#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

static void sig_usr(int sig)
{
    if( sig  == SIGUSR1) {
        puts("recevice SIGUSR1 signal");
    }
    else if( sig == SIGUSR2) {
        puts("recevice SIGUSR2 signal");
    }
    else if( sig == SIGKILL) {
        puts("recevice SIGKILL signal");
    }
}


int main(int argc, char** argv)
{

    if( signal(SIGUSR1, sig_usr) == SIG_ERR){
        perror("signal");
        return 1;
    }

    if(signal(SIGUSR2, sig_usr) == SIG_ERR){
        perror("signal");
        return 1;
    }

    pid_t cpid = fork();
    if(cpid == -1) {
        perror("fork");
    }
    
    if(cpid == 0) {
        printf("child process id %d\n", getpid());
        for(;;)
            pause();
        return 0;
    }

    printf("parent process id %d\n", getpid());
    int status;
    wait(&status);

    return 0;
}
