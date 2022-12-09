#ifndef _SOCKET_H_
#define _SOCKET_H_

#include <arpa/inet.h>

//创建套接字
int createSocket();

//绑定和监听
int setListen(int fd, unsigned short port);

//等待连接
int acceptConn(int fd,  struct sockaddr_in *addr);

//连接主机
int connectToHost(int fd, const char *ip, unsigned short port);

//发送数据
int sendMsg(int fd,  const char *msg, int len);

//接收数据
int recvMsg(int fd, char **msg);

//关闭套接字
int closeSocket(int fd);



#endif // !_SOCKET_H_