#include <sys/types.h>
#include <sys/wait.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

void pr_exit(int status)
{
    if(  WIFEXITED(status) ) {
        printf("exit status = %d\n", WEXITSTATUS(status));
    }
    else if( WIFSIGNALED(status) ) {
        printf("signal number = %d\n", WTERMSIG(status));
    }
    else if( WIFSTOPPED(status) ) {
        printf("child stop signal number = %d\n", WSTOPSIG(status));
    }
}


int main(int argc, char** argv)
{
    pid_t pid;
    int status;

    {   //测试正常退出
        if( (pid = fork()) < 0) {
            perror("fork");
            return 1;
        }
        else if( pid == 0)
            exit(8);            //子进程正常退出

        //父进程继续执行
        if( wait(&status) != pid ) {
            perror("wait");
            return 1;
        }
        pr_exit(status);
    }

    {   //测试异常退出
        if( (pid = fork()) < 0){
            perror("fork");
            return 1;
        }
        else if( pid == 0)
            abort();        //异常终止进程

        //父进程继续执行
        if( wait(&status) != pid) {
            perror("wait");
            return 1;
        }
        pr_exit(status);
    }

    {   //测试子进程也等待
        if( (pid = fork()) < 0) {
            perror("fork");
            return 1;
        }
        else if( pid == 0)
            status /= 0;
        
        int rt;
        if( (rt = wait(&status)) != pid) {
            printf("child rt = %d\n",rt);
            perror("wait");
            return 1;
        }
        pr_exit(status);

    }

    return 0;
}