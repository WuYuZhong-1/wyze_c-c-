#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <grp.h>
#include <unistd.h>

int main(int argc, char** argv)
{
    struct group gp;
    struct group *res = NULL;
    const size_t bufsize = 1024;
    char * buf = malloc(bufsize);

    int rt = getgrgid_r(getgid(), &gp, buf, bufsize, &res);
    if(res == NULL) {
        if(rt == 0) {
            printf("not found group id = %d\n", getgid());
        }
        else {
            perror("getgrgid:");
        }
        return 1;
    } 

    printf("gr_name = %s\n", gp.gr_name);
    printf("gr_passwd = %s\n", gp.gr_passwd);
    printf("gr_gid = %d\n", gp.gr_gid);
    for(int i = 0; gp.gr_mem[i] != NULL; ++i) {
        printf("gr_mem[%d] = %s\n", i, gp.gr_mem[i]);
    }

    return 0;
}