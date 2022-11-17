#include <unistd.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

int main(int argc, char** argv)
{
    if(argc < 2) {
        printf("usage: %s file\n", argv[0]);
        return 1;
    }

    int fd = open(argv[1], O_RDWR);
    if( fd == -1) {
        perror("open");
        return -1;
    }

    unlink(argv[1]);
    sleep(20);
    close(fd);
    
    return 0;
}