#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>


void first(void)
{
    puts("first");
}

void second(void)
{
    puts("second");
}

int main(int argc, char** arg)
{
    long a = sysconf(_SC_ATEXIT_MAX);
    printf("_SC_ATEXIT_MAX = %ld\n", a);

    int rt = atexit(&first);
    if(rt != 0) {
        printf("cannot set exit function\n");
        exit(EXIT_FAILURE);
    }

    atexit(&second);

    printf("process end\n");
    
    // return  0;
    return 2;
    _exit(0);
}