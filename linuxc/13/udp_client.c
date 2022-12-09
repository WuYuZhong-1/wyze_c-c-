#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdlib.h>

int main(int argc, char** argv) 
{
    int sock, rt;
    struct sockaddr_in ssin, sin;
    short port = 8899;
    char buf[1024];
    char *data;
    int len;

    if(argc < 2) {
        printf("usage: %s ip [port]", argv[0]);
    }
    else if( argc == 3) {
        port = atoi(argv[2]);
    }

    sock = socket(AF_INET, SOCK_DGRAM, 0);
    if(sock == -1) {
        perror("socket");
        return 1;
    }

    memset(&ssin, 0, sizeof(ssin));
    ssin.sin_family = AF_INET;
    ssin.sin_port = htons(port);
    rt = inet_pton(AF_INET, argv[1], &ssin.sin_addr);
    if(rt == 0) {
        printf(" %s addr is invalid\n", argv[1]);
        return 1;
    }
    else if( rt == -1) {
        perror("inet_pton");
        return 1;
    }

    // memset(&sin, 0, sizeof(sin));
    // sin.sin_family = AF_INET;
    // sin.sin_port = htons(9988);
    // sin.sin_addr.s_addr = INADDR_ANY;
    // rt = bind(sock, (struct sockaddr*)&sin, sizeof(sin));
    // if(rt == -1) {
    //     perror("bind");
    //     return 1;
    // }

    rt = connect(sock, (struct sockaddr*)&ssin, sizeof(ssin));
    if(rt == -1) {
        perror("connect");
        return 1;
    }


    while(1) {
        memset(buf, 0, sizeof(buf));

        data = fgets(buf, sizeof(buf), stdin);
        
        len  = send(sock, data, strlen(data), 0);
        if(len != strlen(data)) {
            printf("send un complete\n");
            perror("send");
            return 1;
        } 

        memset(buf, 0, sizeof(buf));
        len = recv(sock, buf, sizeof(buf), 0);
        printf("server:%s:%d  say:%s\n", argv[1], port, buf);
    }
}