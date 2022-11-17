#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>

#define DUMMY_PATH      "dummy.txt"

int main(int argc, char** argv)
{
    unlink(DUMMY_PATH);
    if(access(DUMMY_PATH, F_OK) == 0) {
        printf("file: %s is exists\n",DUMMY_PATH);
        return 1;
    }

    int fd = open(DUMMY_PATH, O_WRONLY | O_CREAT | O_TRUNC, 0664);
    if(fd == -1) {
        perror("open:");
        return 1;
    }

    do {
        int rt = lseek(fd, 1024 * 4 * 4, SEEK_SET);
        if(rt == -1) {
            printf("lseek error, rt = %d\n",rt);
            break;
        }

        const char data[] = "hello wyze";
        rt = write(fd, data, sizeof(data));
        printf("realy data len is %d\n", rt);
    }while(0);

    close(fd);
    return 0;
}