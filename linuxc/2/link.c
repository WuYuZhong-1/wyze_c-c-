#include <unistd.h>
#include <stdio.h>

int main(int argc, char** argv)
{
    if(argc < 3) {
        printf("usage %s oldpath newpath\n", argv[0]);
        return 1;
    }

    int rt = access(argv[1], F_OK);
    if(rt != 0) {
        printf("file: %s not exitst\n", argv[1]);
        return 1;
    }

    unlink(argv[2]);
    rt = link(argv[1], argv[2]);
    if(rt == -1) {
        perror("link");
        return 1;
    }

    printf("success\n");
    return 0;
}