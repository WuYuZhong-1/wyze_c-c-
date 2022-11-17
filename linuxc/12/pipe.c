#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>
#include <signal.h>

int main(int argc, char** argv)
{
    pid_t pid;
    int pfd[2];
    int rt;

    if(signal(SIGPIPE, SIG_IGN) == SIG_ERR) {
        perror("signal");
        return 1;
    }

    rt = pipe(pfd);
    if(rt == -1) {
        perror("pipe");
        return 1;
    }

    pid = fork();
    if(pid == -1) {
        perror("fork");
        return 1;
    }

    if( pid == 0) {
        //child
        close(pfd[0]);
        char str[] = "hello world";
        do {
            int len = write(pfd[1], str, strlen(str));
            if(len == -1 && errno == EPIPE) {
                puts("read fd is colse");
                break;
            }
            usleep(1000);
        }while(1);

        close(pfd[1]);
    }
    else {
        //parent
        close(pfd[1]);
        char buf[1024] = {0};
        int count = 2;
        do {
            int len = read(pfd[0], buf, sizeof(buf));
            if(len <= 0) {
                printf("len == %d\n", len);
                break;
            }

            buf[len] = '\n';
            buf[len+1] = '\0';
            write(STDIN_FILENO, buf, strlen(buf));
        } while (count--);
        
        close(pfd[0]);

        waitpid(pid, NULL, 0);
    }

    return 0;
}