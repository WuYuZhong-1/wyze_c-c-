#include <stdio.h>
#include <unistd.h>

int main(int argc, char** argv)
{

    int pfd[2];

    int rt = pipe(pfd);
    if(rt != 0) {
        perror("pipe");
        return 1;
    }

    long pipesize = fpathconf(pfd[1], _PC_PIPE_BUF);
    printf("pipe write size = %ld\n", pipesize);

    // long namesize = pathconf("/", _PC_NAME_MAX);
    long namesize = fpathconf(0, _PC_NAME_MAX);
    printf("name size = %ld\n", namesize);

    long inputsize = fpathconf(0, _PC_MAX_INPUT);
    printf("input size = %ld\n", inputsize);

    return 0;
}