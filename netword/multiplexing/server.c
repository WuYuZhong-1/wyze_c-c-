#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <sys/select.h>
#include <arpa/inet.h>
#include <ctype.h>

// 高并发使用多线程或是多进程 + IO多路转接(复用)
// select    跨平台，window，linux，mac都可以使用
// poll        linux
// epoll      linux

// select poll   内部是用线性结构实现， select 检测数最大值 1024，poll 没有上限
// epoll 内部使用红黑树， epoll也没有上限

int main(int argc, char **argv) 
{
    int fd = socket(AF_INET, SOCK_STREAM, 0);
    if( fd == -1 )  {
        perror("socket");
        return -1;
    }


    struct sockaddr_in saddr;
    saddr.sin_family = AF_INET;
    saddr.sin_port = htons(12345);
    saddr.sin_addr.s_addr = INADDR_ANY;
    int ret = bind(fd, (struct sockaddr *)&saddr, sizeof(saddr));
    if(ret == -1) {
        perror("bind");
        close(fd);
    }

    ret = listen(fd, 128);
    if(ret == -1) {
        perror ("listen");
        close(fd);
    }

    fd_set readset;
    FD_ZERO(&readset);
    FD_SET(fd, &readset);
    fd_set tmp = readset;
    int maxfd = fd ;

    while(1) {
        printf("maxfd %d\n", maxfd);
        tmp = readset;
        ret = select(maxfd+ 1, &tmp, NULL, NULL, NULL);
        if(ret == -1) {
            perror("select");
            break;
        }
        else if (ret == 0) {
            continue;
        }

        if( FD_ISSET(fd, &tmp)) {
            //存在新的客户端连接
            int cfd = accept(fd,  NULL, NULL);
            if(cfd == -1) {
                perror("accept");
                break;
            }
             printf("cfd = %d\n", cfd);
            FD_CLR(fd, &tmp);
            FD_SET(cfd, &readset);
            maxfd = maxfd > cfd ? maxfd : cfd ;
        }

        for( int i =0; i <= maxfd; i++ ) {
            
            if(FD_ISSET(i, &tmp)) {
                printf("socket %d need read\n", i);
                FD_CLR(i, &tmp);
                //存在可读数据
                char buf[1024] = {0};
                int ret = recv(i, buf, sizeof(buf), 0);
                if(ret == -1) {
                    perror("recv");
                    FD_CLR(i, &readset);
                    close(i);
                    continue;
                }
                else if( ret == 0) {
                    printf("断开连接...\n");
                    FD_CLR(i, &readset);
                    close(i);
                    continue;
                }

                printf("client say: %s\n", buf);
                //将数据转为大写
                for(int i =0; i < ret; i++) {
                    buf[i] = toupper(buf[i]);
                }

                ret = send(i, buf, ret, 0);
                if(ret == -1) {
                    perror("send");
                     FD_CLR(i, &readset);
                    close(i);
                    continue;
                }
                else if(ret == 0) {
                    printf(" 断开连接...\n");
                     FD_CLR(i, &readset);
                    close(i);
                    continue;
                }
                printf("server say: %s\n", buf);
            }

        }

        //算出最小的 maxfd
        int maxtmp = 0;
        for( int i =0; i <= maxfd; i++ ) {
            if(FD_ISSET(i, &readset)) {
                maxtmp = i;
            }
        }
        maxfd = maxtmp ;
        
    }

    close(fd);
    return 0;
}
