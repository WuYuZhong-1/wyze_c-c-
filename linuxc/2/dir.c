#include <sys/types.h>
#include <dirent.h>
#include <stdio.h>
#include <errno.h>

const char* dirtype(unsigned char type) {

    switch(type) {
        case DT_BLK:
            return "block";
        case DT_CHR:
            return "charecter";
        case DT_DIR:
            return "directory";
        case DT_FIFO:
            return "FIFO";
        case DT_LNK:
            return "link";
        case DT_REG:
            return "regular";
        case DT_SOCK:
            return "socket";
        default:
            return "unkonw";
    }

}

int main(int argc, char** argv)
{
    if(argc < 2) {
        printf("usage %s directory\n", argv[0]);
        return 0;
    }

    DIR* dir = opendir(argv[1]);
    if(dir == NULL) {
        perror("opendir");
        return -1;
    }

    printf("--------------------\n");
    errno = 0;
    struct dirent* rent = NULL;
    while( (rent = readdir(dir)) != NULL) {

        printf("name = %s\n", rent->d_name);
        printf("type = %s\n",dirtype(rent->d_type));
        printf("inode = %lu\n", rent->d_ino);
        printf("off_t = %lu\n", rent->d_off);
        printf("d_reclen = %d\n", rent->d_reclen);
        printf("--------------------\n");
        printf("telldir = %ld\n", telldir(dir));
        printf("--------------------\n");
        errno = 0;
    }

    if( rent == NULL) {
        if( errno == 0) {
            printf("readdir end\n");
        }
        else{
            perror("readdir");
        }
    }

    closedir(dir);

    return 0;
}