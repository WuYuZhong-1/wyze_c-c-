#include <netdb.h>
#include <stdio.h>
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
    char buf[1024];
    int buflen = sizeof(buf);
    struct hostent ent, *res;
    int herrno;

    if(argc < 2) {
        printf("usage: %s hostname\n", argv[0]);
    }

    gethostbyname_r(argv[1], &ent, buf, buflen, &res, &herrno);
    if(res == NULL) {
        printf("gethostbyname_r: %s\n", hstrerror(herrno));
        return 1;
    }

    printhost(res);

    return 0;
}