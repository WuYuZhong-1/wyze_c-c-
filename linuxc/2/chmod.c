#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>


int main(int argc, char** argv)
{
    if(argc < 2) {
        printf("usage %s file \n", argv[0]);
        return 1;
    }

    if(access(argv[1], F_OK) != 0) {
        printf("file: %s not eixt\n", argv[1]);
        return 1;
    }

    struct stat st;
    memset(&st, 0, sizeof(st));
    int rt = stat(argv[1], &st);
    if(rt != 0){
        perror("stat");
        return 1;
    }

    mode_t old = st.st_mode | S_ISUID | S_ISGID;
    rt = chmod(argv[1], old);
    if(rt != 0) {
        perror("chmod");
        return 1;
    }
    printf("sucess\n");

    return 0;   
}