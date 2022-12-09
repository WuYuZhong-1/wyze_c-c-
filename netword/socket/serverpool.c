#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <pthread.h>
#include"threadpool.h"

typedef struct SockInfo
{
    struct sockaddr_in addr;
    int fd;
}SockInfo;

typedef struct PoolInfo 
{
    ThreadPool *pool;
    int fd;
} PoolInfo;

void tmain(void *arg) 
{
    SockInfo *info = (SockInfo*)arg;
    //打印端口和IP信息
    char ip[20] = {0};
    int port =  ntohs(info->addr.sin_port);
    printf("IP: %s\n", inet_ntop(AF_INET,(const char *) &info->addr.sin_addr.s_addr, ip, sizeof(ip)));
    printf("Port: %d\n", port);

    int fd = *(int*)arg;
    //先接收数据
    while(1){
        char buf[1024] = {0};
        int ret = recv(info->fd, buf, sizeof(buf), 0);
        if(ret > 0) {
            printf(" port = %d,   client say: %s\n", port,  buf);
            ret = send(info->fd, buf, ret, 0);
        }
        else if(ret == 0) {
            printf("客户端断开连接\n");
            break;
        }
        else {
            perror("revc");
            break;
        }
    }
    close(info->fd);
}


void acceptConn(void *arg)
{
    PoolInfo *poolInfo = (PoolInfo *) arg;
    while(1) {   
        
        //等待连接
        SockInfo *info = (SockInfo*)malloc(sizeof(SockInfo));
        memset(info, 0, sizeof(SockInfo));
        int len = sizeof(struct sockaddr_in);
        info->fd = accept(poolInfo->fd,(struct sockaddr *) &info->addr,  &len);
        if(info->fd  == -1) {
            perror("accept");
            free(info);
            break;
        }

        threadPoolAdd(poolInfo->pool, tmain, info);
    }
    close(poolInfo->fd);
}

int main(int argc, char **argv) 
{
    int ret =0;
    // 创建监听的套接字
    int fd = socket(AF_INET, SOCK_STREAM, 0);
    if(fd == -1) {
        perror("socket ");
        return -1;
    }

    //绑定
    struct sockaddr_in saddr = {
        .sin_family = AF_INET,
        .sin_port = htons(9999),
        .sin_addr.s_addr = INADDR_ANY 
    };

    //saddr.sin_addr.s_addr = inet_pton(AF_INET, "192.168.199.219", &saddr.sin_addr.s_addr) ;

    ret = bind(fd, (struct sockaddr *)&saddr, sizeof(saddr));
    if(ret == -1) {
        perror("bind");
        close(fd);
        return -1;
    }

    //设置监听
    ret = listen(fd, 128);
    if(ret == -1) {
        perror("listen");
        close(fd);
        return -1;
    }

#if 0
    PoolInfo poolInfo = { .pool = NULL, .fd = fd } ;
    poolInfo.pool = threadPoolCreate(3, 20, 100);

    threadPoolAdd(&poolInfo.pool, acceptConn, &poolInfo);

    while(1);

    threadPoolDestroy(poolInfo.pool);
#else
    PoolInfo *poolInfo = (PoolInfo*) malloc(sizeof(PoolInfo));
    poolInfo->fd = fd;
    poolInfo->pool = threadPoolCreate(3, 20, 100);

    threadPoolAdd(poolInfo->pool, acceptConn, poolInfo);
    pthread_exit(NULL);
#endif

    return 0;
}
