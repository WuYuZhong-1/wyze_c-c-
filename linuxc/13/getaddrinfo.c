#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <arpa/inet.h>


int main(int argc, char** argv)
{
    struct addrinfo hint, *res;
    int rt;
    struct sockaddr_in *ipv4;
    struct sockaddr_in6 *ipv6;
    char buf[512];

    memset(&hint, 0, sizeof(hint));
    hint.ai_family = AF_UNSPEC;
    hint.ai_socktype = SOCK_STREAM;
    hint.ai_flags = AI_ALL;
    hint.ai_protocol = IPPROTO_TCP;

    rt = getaddrinfo("www.baidu.com", "https", &hint, &res);
    if(rt != 0) {
        printf("getaddrinfo: %s\n", gai_strerror(rt));
        return 1;
    }

    for(struct addrinfo *r = res; r != NULL; r = r->ai_next) {
        memset(buf, 0, sizeof(buf));
        switch(r->ai_family) {
            case AF_INET:
                ipv4 = (struct sockaddr_in*)r->ai_addr;
                inet_ntop(r->ai_family, &ipv4->sin_addr, buf, sizeof(buf));
                break;
            case AF_INET6:
                ipv6 = (struct sockaddr_in6*)r->ai_addr;
                inet_ntop(r->ai_family, &ipv6->sin6_addr, buf, sizeof(buf));
                break;
            default:
                printf(" no expect type %d\n", r->ai_family);
                return 1;
        }
        printf("[IPv%d] %s\n", r->ai_family==AF_INET?4:6, buf);
    }


    freeaddrinfo(res);
    return 0;
}