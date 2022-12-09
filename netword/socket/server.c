#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <pthread.h>

typedef struct SockInfo
{
    struct sockaddr_in addr;
    int fd;
}SockInfo;

SockInfo infos[512];
pthread_mutex_t mutex_infos;

void *tmain(void *arg) 
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
    pthread_mutex_lock(&mutex_infos);
    info->fd = -1;
    pthread_mutex_unlock(&mutex_infos);
    //free(info);
    pthread_exit(NULL);
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
    int flag = fcntl(fd, F_GETFL);
    if( !(flag & O_NONBLOCK))
       fcntl(fd, F_SETFL, flag | O_NONBLOCK);	

    //绑定
    struct sockaddr_in saddr = {
        .sin_family = AF_INET,
        .sin_port = htons(9999),
        .sin_addr.s_addr = INADDR_ANY 
    };

    //saddr.sin_addr.s_addr = inet_pton(AF_INET, "192.168.1.125", &saddr.sin_addr.s_addr) ;

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

    //这里这么做的目的是减少系统资源的申请，提高效率
    int max = sizeof(infos) / sizeof(SockInfo);
    for(int i = 0; i < max; i++) {
        memset(&infos[i], 0, sizeof(SockInfo));
        infos->fd = -1;
    }

    ret = pthread_mutex_init(&mutex_infos, NULL);
    if(ret != 0) {
        perror("pthread_mutex_init");
        close(fd);
        return -1;
    }

    while(1) {   
        
        //等待连接
        // SockInfo *info = (SockInfo*)malloc(sizeof(SockInfo));
        // memset(info, 0, sizeof(SockInfo));
        int i = 0;
        pthread_mutex_lock(&mutex_infos);
        for(; i < max; i++) {
            if(infos[i].fd == -1 )
                break;
        }
        pthread_mutex_unlock(&mutex_infos);

        int len = sizeof(struct sockaddr_in);
        infos[i].fd = accept(fd,(struct sockaddr *) &infos[i].addr,  &len);
        if(infos[i].fd  == -1) {
            perror("accept");
            //free(info);
            break;
        }

        //创建子线程
        pthread_t tid;
        ret = pthread_create(&tid, NULL, tmain,&infos[i]);
        //住线程和子线程分离
        pthread_detach(tid);

    }
    
    close(fd);
    return 0;
}
