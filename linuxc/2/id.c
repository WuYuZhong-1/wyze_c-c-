#include <stdio.h>
#include <unistd.h>


int main(int argc, char** argv)
{
    printf("uid = %d\n", getuid());
    printf("euid = %d\n", geteuid());
    printf("gid = %d\n", getgid());
    printf("egid = %d\n", getegid());
    return 0;
}