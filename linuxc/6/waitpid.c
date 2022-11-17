#include <sys/wait.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>


int main(int argc, char** argv)
{
    pid_t cpid, w;
    int wstatus;

    cpid = fork();
    if( cpid == -1) {
        perror("fork");
        exit(EXIT_FAILURE);
    }

    if( cpid == 0) {
        printf("child PID is %ld\n", (long)getpid());
        if( argc == 1) 
            pause();        //等待暂定
        _exit( atoi(argv[1]) );
    }
    else {
        do {
            w = waitpid(cpid, &wstatus, WUNTRACED | WNOHANG);
            if(w == -1) {
                perror("waitpid");
                exit(EXIT_FAILURE);
            }
            else if( w == 0) {
                printf("waitpid continued\n");
                sleep(2);
                continue;
            }

            if( WIFEXITED(wstatus) ) {
                printf("exited, status = %d\n", WEXITSTATUS(wstatus));
            }
            else if( WIFSIGNALED(wstatus) ) {
                printf("killed by signal %d\n", WTERMSIG(wstatus));
            }
            else if( WIFSTOPPED(wstatus) ) {
                printf("stopped by signal %d\n", WSTOPSIG(wstatus));
            }
            else {
                printf("continued\n");
                sleep(2);
            }
        }while(w == 0 || ( !WIFEXITED(wstatus) && !WIFSIGNALED(wstatus) ));
        exit(EXIT_SUCCESS);
    }
    return 0;
}