#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <string.h>
#include <arpa/inet.h>
#include <ctype.h>
#include <unistd.h>


int main(int argc, char** argv)
{
    int rt, sock;
    struct sockaddr_in sin, csin;
    socklen_t socklen = sizeof(csin);
    int len;
    char buf[1024];
    char ip[32];

    sock = socket(AF_INET, SOCK_DGRAM, 0);
    if(sock == -1) {
        perror("socket");
        return 1;
    }

    memset(&sin, 0, sizeof(sin));
    sin.sin_family = AF_INET;
    sin.sin_port = htons(8899);
    sin.sin_addr.s_addr = INADDR_ANY;
    rt = bind(sock, (struct sockaddr*)&sin, sizeof(sin));
    if(sock == -1) {
        perror("bind");
        return 1;
    }

    puts("dup server staring");
    while(1) {
        memset(&csin, 0, sizeof(csin));
        memset(buf, 0, sizeof(buf));
        memset(ip, 0, sizeof(ip));

        len = recvfrom(sock, buf, sizeof(buf), 0, (struct sockaddr*)&csin, &socklen);

        printf("client:%s:%d  say:%s\n", inet_ntop(csin.sin_family, &csin.sin_addr, ip, sizeof(ip)), ntohs(csin.sin_port), buf);

        for(int i =0 ; i < len; i++) {
            buf[i] = toupper(buf[i]);
        }

        rt = sendto(sock, buf, len, 0, (struct sockaddr*)&csin, socklen);
        if( rt != len) {
            printf("sendto un complete\n");
            return 1;
        }

    }

    close(sock);

    return 0;
}