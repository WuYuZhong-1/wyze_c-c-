#include <stdio.h>
#include <arpa/inet.h>
#include <netdb.h>

extern int h_errno;

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
    struct hostent ent, *res;
    char buf[1024];
    struct in_addr addr;
    int herrno;
    int rt;

    if(argc < 2 ) {
        printf("usage: %s ipaddr\n", argv[0]);
        return 1;
    }

    rt = inet_pton(AF_INET, argv[1], &addr);
    if(rt == 0) {
        printf("address is invalid: %s\n", argv[1]);
        return 1;
    }
    else if( rt == -1) {
        perror("inet_pton");
        return 1;
    }

    // gethostbyaddr_r(&addr, sizeof(addr), AF_INET, &ent,
    //                 buf, sizeof(buf), &res, &herrno);
    res = gethostbyaddr(&addr, sizeof(addr), AF_INET);
    if(res == NULL) {
        // printf("gethostbyaddr_r error: %s\n", hstrerror(herrno));

        printf("gethostbyaddr error: %s\n", hstrerror(h_errno));
        return 1;
    }

    printhost(res);

    return 0;
}