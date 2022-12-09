#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include "socket.h"
#include <pthread.h>

struct SockInfo
{
    struct  sockaddr_in addr;
    int fd;
};

void *tmain( void *arg )
{
    struct SockInfo *info = (struct SockInfo *)arg;
    char ip[32] = {0};
    printf("client  ip = %s \t\t port = %d\n", inet_ntop(AF_INET, &info->addr.sin_addr.s_addr, ip, sizeof(ip)), ntohs(info->addr.sin_port));

    while (1){
        char *buf;
        int ret = recvMsg(info->fd, &buf);
        if( ret > 0) {
            printf("data: %s\n", buf);
            free(buf);
        }
        else {
            break;
        }
        usleep(100000);
    }
    
    closeSocket(info->fd);
    free(info);
    pthread_exit(NULL);
}

int main(int argc, char **argv)
{
    int fd = createSocket();
    if(fd == -1)
        return -1;
    
    int ret = setListen(fd, 12345);
    if(ret == -1){
        closeSocket(fd);
        return -1;
    }

    while(1) {
        struct SockInfo *paddr = (struct SockInfo *)malloc(sizeof(struct SockInfo));
        if(paddr == NULL) 
            break;
        memset(paddr, 0, sizeof(struct SockInfo));
        paddr->fd  = acceptConn(fd, &paddr->addr);
        if(paddr->fd== -1){
            free(paddr);
            break;
        }
        pthread_t tid;
        pthread_create(&tid, NULL, tmain, paddr);
        pthread_detach(tid);
    }

    closeSocket(fd);
    return 0;
}