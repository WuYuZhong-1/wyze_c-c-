#include "socket.h"
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

int createSocket()
{
    int fd = socket(AF_INET, SOCK_STREAM, 0);
    if(fd == -1) {
        perror(" socket");
    }
    return fd;
}


int setListen(int fd, unsigned short port)
{
    struct sockaddr_in addr = {
        .sin_family = AF_INET,
        .sin_port = htons(port)
    };
    inet_pton(AF_INET, "192.168.1.125", &addr.sin_addr.s_addr);

    int ret = bind(fd, (struct sockaddr *) &addr, sizeof(addr));
    if(ret == -1){
        perror("bind");
        return -1;
    }

    ret = listen(fd, 128);
    if(ret == -1) {
        perror("listen");
    }
    return ret;
}

int acceptConn(int fd,  struct sockaddr_in *addr)
{
    int len = sizeof(struct sockaddr_in);
    socklen_t *lenAddr = &len;

    if(addr == NULL) 
        lenAddr =  NULL;

    int ret = accept(fd, (struct sockaddr *)addr, lenAddr);
    if(ret == -1) {
        perror("accept");
    }
    return ret;
}


int connectToHost(int fd, const char *ip, unsigned short port)
{
    struct sockaddr_in addr  = {
        .sin_family = AF_INET,
        .sin_port = htons(port)
    };

     inet_pton(AF_INET, ip, &addr.sin_addr.s_addr);

    int ret = connect(fd, (struct sockaddr *)&addr, sizeof(struct sockaddr_in));
    if(ret == -1) {
        perror("connect");
    }
    return ret;
}

static int writen(int fd, char *msg, int size) 
{
    char *buf = msg;
    int count = size;

    while(count > 0) {
        int len = send(fd, buf, count , 0);
        if(len == -1){
            perror("send");
            return -1;
        }
        buf += len;
        count -= len;
    }

    return size;
}

int sendMsg(int fd,  const char *msg, int len)
{
    if(fd < 0 || msg == NULL || len < 0) 
        return -1;
    
    char *buf = (char*)malloc(len + sizeof(int));
    if(buf == NULL) 
        return -1;
    memset(buf, 0, len + sizeof(int));

    *(int *)buf = htonl(len);
    memcpy(buf + sizeof(int), msg, len);

    int ret = writen(fd, buf, len + sizeof(int));
    return ret;
}

static int readn(int fd, char  *msg, int size)
{
    char *buf = msg;
    int count = size;
    while( count > 0) {
        int len = recv(fd, buf, count, 0 );
        if( len == -1) {
            perror("recv");
            return -1;
        }
        else if( len == 0) {
            return size - count;
        }
        buf += len;
        count -= len;
    }

    return size;
}

int recvMsg(int fd, char **msg)
{
    if(msg == NULL || fd < 0)
        return -1;

    int readLen =0;
    int ret = readn(fd, (char *)&readLen, sizeof(int));
    if(ret != 4) {
        return -1;
    }
    readLen = ntohl(readLen);

    char *data = (char *)malloc(readLen +1);
    if(data == NULL) {  
        return -1;
    }
    memset(data, 0, readLen+1);

    ret = readn(fd, data, readLen);
    if(ret != readLen) {
        printf("readn error\n");
        free(data);
        return -1;
    }
    
    *msg = data;
    return ret;
}

int closeSocket(int fd)
{
    int ret = close(fd);
    if(ret == -1)   {
        perror("close");
    }
    return ret;
}