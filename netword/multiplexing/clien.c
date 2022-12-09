#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <errno.h>

int main(int argc, char **argv)
{
    int fd = socket(AF_INET, SOCK_STREAM, 0);
    if(fd == -1) {
        perror("socket");
        return -1;
    }

    struct sockaddr_in addr = {
        .sin_family = AF_INET,
        .sin_port = htons(9854)
    };
    
    inet_pton(AF_INET, "192.168.1.10", &addr.sin_addr.s_addr);
    

    int ret = connect(fd, (const struct sockaddr *)&addr, sizeof(addr));
    if(ret == -1) {
        perror("connect");
        close(fd);
        return -1;
    }

    printf("connect success\n");
    int flag = fcntl(fd, F_GETFL);
    flag |= O_NONBLOCK;
    fcntl(fd, F_SETFL, flag);

    int count =1;
    while( 1) {
        sleep(1);
        char buf[1024];
        memset(buf, 0, sizeof(buf));
        if(count > 10) 
            break;
        sprintf(buf,"hello wuyz ... %d \n",count);
        ret = send(fd, buf, strlen(buf)+1, 0);
        if(ret == -1) {
            perror("send");
            break;
        }
        memset(buf, 0, sizeof(buf));
        int len =0;
        while(1) {
            ret = recv(fd, buf + len, sizeof(buf) -  len, 0);
            if(ret == -1) {
                if(errno == EAGAIN) {
                    printf("server say: %s\n", buf);
                    break;
                }
                perror("recv");
                break;
            }
            else if(ret == 0) {
                break;
            }
            len += ret;
         }
        count++;
    }
    close(fd);
    return 0;
}
