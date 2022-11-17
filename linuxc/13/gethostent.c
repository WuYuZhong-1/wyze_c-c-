#include <stdio.h>
#include <netdb.h>      //网络数据库
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>


static void printhost(struct hostent *ent)
{
    char addr[INET_ADDRSTRLEN];

    printf("hostname: %s\n", ent->h_name);
    for(char** aliases = ent->h_aliases; *aliases; ++aliases) {
        printf("aliases name: %s\n" , *aliases);
    }

    if( ent->h_addrtype == AF_INET) {
        puts("address type: AF_INET");
    }
    else {
        puts("Not an IPv4 address.");
    }

    printf("address length: %d\n", ent->h_length);
    
    for(char** addr_list = ent->h_addr_list;  *addr_list; ++addr_list) {
        printf("address: %s\n", inet_ntop(ent->h_addrtype, *addr_list,addr, INET6_ADDRSTRLEN));
    }

}

int main(int argc, char** argv)
{
    struct hostent  *ent;
    char hostname[128];

    gethostname(hostname, sizeof(hostname));
    printf("%s\n", hostname);
    // int rt = sethostname("wyze", strlen("wyze"));    //临时更改主机名
    // if(rt == -1) {
    //     perror("sethostname");
    // }

    sethostent(1);
    while( (ent = gethostent()) != NULL) {
        printhost(ent);
    }

    // 测试 sethostent(1) 是否打开一个文件
    // int sfd = socket(AF_INET, SOCK_STREAM, 0);
    // printf("socket fd = %d\n", sfd);
    
    endhostent();

    return 0;
}