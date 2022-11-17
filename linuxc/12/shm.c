#include <sys/ipc.h>
#include <sys/shm.h>
#include <stdio.h>
#include <errno.h>
#include <unistd.h>

int main(int argc, char** argv)
{
    int shm, rt;
    key_t key;
    char buf[512] = {0};
    char* pcwd;

    pcwd = getcwd(buf, sizeof(buf));
    key = ftok(pcwd, 1234);
    shm = shmget(key, 4096, IPC_CREAT | IPC_EXCL | 0600);
    if(shm == -1) {
        perror("shmget");
        return 1;
    }

    sleep(12);

    rt = shmctl(shm, IPC_RMID, NULL);
    if(rt == -1) {
        perror("shmctl");
    }

    return 0;
}