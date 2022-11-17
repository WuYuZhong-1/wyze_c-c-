#include <stdio.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <unistd.h>

int main(int argc, char** argv)
{
    char cwd[512];
    char *pcwd;
    int sem;
    int rt;
    key_t key;

    pcwd = getcwd(cwd, sizeof(cwd));
    key = ftok(pcwd, 12345);

    sem = semget(key, 1, IPC_CREAT | IPC_EXCL | 0600);
    if(sem == -1) {
        perror("semget");
        return 1;
    }

    
    struct semun arg;


    sleep(15);

    rt = semctl(sem, 0, IPC_RMID);
    if(rt == -1) {
        perror("semctl");
    }
    return 0;
}