#include <stdio.h>
#include <unistd.h>

int main(int argc, char** argv)
{
    for(int i = 0; i < argc; ++i) {
        printf("argv[%d] : %s\n", i, argv[i]);
    }

    extern char** __environ;
    for( char** ptr = __environ; *ptr != NULL; ++ptr) {
        puts(*ptr);
    }

    return 0;
}