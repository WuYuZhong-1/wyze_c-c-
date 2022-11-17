#include <stdio.h>
#include <unistd.h>
#include <sys/file.h>


int main(int argc, char** argv)
{
    int rt, fd;

    fd = open("./tmp.txt", O_RDWR | O_CREAT, 0660);
    if(fd == -1) {
        perror("open");
        return 1;
    }

    rt = flock(fd, LOCK_EX);
    printf("get lock  ex rt = %d\n", rt);

    sleep(10);

    rt = flock(fd, LOCK_SH);
    printf("get lock sh rt = %d\n", rt);
    sleep(5);

    puts("process end");
    rt = flock(fd, LOCK_UN);
    printf("get unlock rt = %d\n", rt);

    close(fd);

    return 0;
}