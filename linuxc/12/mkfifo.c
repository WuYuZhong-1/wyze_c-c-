#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>

int main(int argc, char** argv)
{
    pid_t pid;
    int rt;
    int ffd;

    if( access("./fifo1", F_OK | R_OK) != 0) {
        // int rt = mknod("./fifo1", __S_IFIFO| 0660,0);
        int rt = mkfifo("./fifo1", 0660);
        if(rt == -1) {
            perror("mknod");
            return 1;
        }
    }
    
    rt = fork();
    if( rt == -1) {
        perror("fork");
        return 1;
    }

    if( rt == 0) {
        //child
        sleep(5);
        puts("child open fifo");
        // ffd = open("./fifo1", O_RDWR);
        ffd = open("./fifo1", O_RDONLY);
        if(ffd == -1) {
            perror("open");
            return 1;
        }
        puts("child open success");
    }
    else {
        //parent
        // sleep(5);
        puts("parent open fifo");
        // ffd = open("./fifo1", O_RDWR);
        ffd = open("./fifo", O_WRONLY);
        if( ffd == -1) {
            perror("open");
            return 1;
        }
        puts("parent open success");
    }

    close(ffd);
    return 0;
}