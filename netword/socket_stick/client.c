#include <stdio.h>
#include "socket.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h> 

int main(int argc, char **argv) 
{
    int fd = createSocket();
    if(fd == -1)
        return  -1;
    
    int ret = connectToHost(fd, "192.168.1.125", 12345);
    if(ret == -1)
        return -1;

    //通信
    char buf[100] = {0};
    int len;
    int fdFile = open("./english.txt", O_RDONLY);
    if(fdFile == -1) {
        perror("open");
        closeSocket(fd);
        return -1;
    }

    while( (len = read(fdFile, buf, sizeof(buf))) > 0) {

        len = sendMsg(fd, buf, len);
        if( len <= 0) {
            break;
        }
        memset(buf, 0, sizeof(buf));
        usleep(300);
    }
    printf("client disconnected\n");
    sleep(10);
    closeSocket(fd);

    return 0;
}