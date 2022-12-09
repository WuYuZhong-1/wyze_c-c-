#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/epoll.h>
#include <arpa/inet.h>
#include "threadpool.h"
#include <fcntl.h>
#include <errno.h>
#include <ctype.h>

struct SockInfo
{
    int fd;
    int epfd;
};

void acceptConn(void *arg)
{
    struct SockInfo *info = (struct SockInfo*) arg;
    int fd = accept(info->fd, NULL, NULL);
    
    //设置非阻塞IO
    int flag = fcntl(fd, F_GETFL);
    flag |= O_NONBLOCK;
    fcntl(fd, F_SETFL, flag);

    struct epoll_event ev = {
        .data.fd = fd, 
        .events = EPOLLIN | EPOLLET
    };
    int ret = epoll_ctl(info->epfd, EPOLL_CTL_ADD, fd, &ev);
    if(ret == -1) { 
        perror("epoll_ctl");
    }

    return ;
}

void communication(void *arg)
{
    struct SockInfo *info = (struct SockInfo *) arg;
    char buf[5];

    while(1) {
        memset(buf, '\0', sizeof(buf));
        int ret = recv(info->fd, buf, sizeof(buf), 0);
        if(ret == -1) {
            if( errno == EAGAIN) {
                break;
            }
            else {
                perror("recv");
                return;
            }
        }
        else if( ret == 0) {
            printf("断开连接..\n");
            break;
        }

        write(STDOUT_FILENO, buf, ret);
        write(STDOUT_FILENO, "\n", 2);
        for(int i =0; i < ret; i++) {
            buf[i] =toupper(buf[i]);
        }
        send(info->fd, buf, ret, 0);
        if(ret == -1) {
            perror("send");
            break;
        }
        else if(ret == 0) { 
            printf("客户端断开连接...\n");
            break;
        }
    }

    return ;
}


int main(int argc, char **argv) 
{
    int lfd = socket(AF_INET, SOCK_STREAM, 0);
    if(lfd == -1) {
        perror("socket");
        return -1;
    }

    struct sockaddr_in addr = {
        .sin_family = AF_INET,
        .sin_port = htons((12345)),
        .sin_addr.s_addr = INADDR_ANY
    };
    int ret = bind(lfd, (struct sockaddr *)&addr, sizeof(addr));
    if(ret == -1) {
        perror("bind");
        close(lfd);
        return -1;
    }

    ret = listen(lfd, 128);
    if(ret == -1) {
        perror("listen");
        close(lfd);
        return -1;
    }

    //创建线程池
    struct ThreadPool *pool =  threadPoolCreate(2, 50, 100);

    //创建epoll 文件描述符
    int epfd = epoll_create(1);
    if(epfd == -1) {
        perror("epoll_create");
        close(lfd);
        return -1;
    }

    struct epoll_event ev = {
        .events = EPOLLIN | EPOLLET,
        .data.fd = lfd
    };
    ret = epoll_ctl(epfd, EPOLL_CTL_ADD, lfd, &ev);

    struct epoll_event event[100] ;
    int max = sizeof(event) / sizeof(event[0]);

    while(1) {
        memset(event, 0, sizeof(event));
        ret = epoll_wait(epfd, event, max, -1);
        if( ret == -1) {
            perror("epoll_wait");
            break;
        }

        for(int i = 0; i < ret ; i++) {
            struct SockInfo *info = (struct SockInfo *) malloc(sizeof(struct SockInfo));
            memset(info, 0, sizeof(struct SockInfo));
            info->fd = event[i].data.fd;
            info->epfd = epfd;
            
            if(info->fd== lfd) {
                threadPoolAdd(pool, acceptConn, info);
            }
            else {
                threadPoolAdd(pool, communication, info);
            }
        }
    }

    threadPoolDestroy(pool);
    close(lfd);
    close(epfd);

    return 0;
}
