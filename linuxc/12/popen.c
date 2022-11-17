#include <stdio.h>

int main(int argc, char** argv)
{
    char buf[1024] = {0};
    int len ;

    if(argc != 2) {
        printf("usage: %s cmd\n", argv[0]);
        return 0;
    }

    FILE *pfd = popen(argv[1], "r");
    if(pfd == NULL) {
        perror("popen");
        return 1;
    }

    len = fread(buf, 1, sizeof(buf), pfd);
    printf("len = %d\n%s\n", len, buf);
    
    pclose(pfd);

    return 0;
}