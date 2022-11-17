#include <stdio.h>
#include <pwd.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>

int main(int argc, char** argv)
{

    struct passwd pw;
    struct passwd *res;
    size_t buffszie;
    char* buf;

    buffszie = sysconf(_SC_GETPW_R_SIZE_MAX);
    if(buffszie == -1) 
        buffszie = 16384;
    printf("buffsize = %ld\n", buffszie);

    buf = malloc(buffszie);

    int rt = getpwuid_r(getuid(), &pw, buf, buffszie, &res);
    if(res == NULL) {
        if(rt == 0) 
            printf("not found\n");
        else 
            perror("getpwuid_r()");
        return 1;
    }

    printf("buffsize addr = %p\n", buf);
    printf("pw_name addr = %p\n", pw.pw_name);
    printf("pw_passwd addr = %p\n", pw.pw_passwd);
    printf("pw_name: %s\n", pw.pw_name);
    printf("pw_passwd: %s\n", pw.pw_passwd);
    printf("pw_uid: %d\n", pw.pw_uid);
    printf("pw_gid: %d\n", pw.pw_gid);
    printf("pw_gecos: %s\n", pw.pw_gecos);
    printf("pw_dir: %s\n", pw.pw_dir);
    printf("pw_shell: %s\n", pw.pw_shell);

    free(buf);
    return 0;
}