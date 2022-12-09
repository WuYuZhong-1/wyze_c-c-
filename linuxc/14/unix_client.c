#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <errno.h>

int main(int argc, char** argv)
{
    int ufd, rt;
    struct sockaddr_un uaddr;
    char pidbuf[16], data[1024], sdata[1024];
    int len;

    ufd = socket(AF_UNIX, SOCK_STREAM, 0);
    if(ufd == -1) {
        perror("socket");
        return 1;
    }

    
    memset(&uaddr, 0, sizeof(uaddr));
    uaddr.sun_family = AF_UNIX;
    //strncpy(uaddr.sun_path, "./unix_file", strlen("./unix_file"));
    strncpy(uaddr.sun_path, "/tmp/wyze_unix_addr", strlen("/tmp/wyze_unix_addr"));

    rt = connect(ufd, (struct sockaddr*)&uaddr, sizeof(uaddr));
    if(rt == -1) {
        perror("connect");
        return 1;
    }

    memset(pidbuf, 0, sizeof(pidbuf));
    snprintf(pidbuf, sizeof(pidbuf), "%d: ", getpid());

    while(1) {
        memset(data, 0, sizeof(data));
        memset(sdata, 0, sizeof(sdata));
        fgets(data, sizeof(data), stdin);
        if(data[strlen(data)] == '\n');
            data[strlen(data)] = '\0';
        
        snprintf(sdata, sizeof(sdata), "%s%s\n", pidbuf, data);
        write(ufd, sdata, strlen(sdata));
    }
}
