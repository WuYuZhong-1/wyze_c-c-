#include <unistd.h>
#include <stdio.h>
#include <string.h>


int main(int argc, char** argv) 
{
    char buf[256] = {0};

    if(argc < 2) {
        gethostname(buf, sizeof(buf));
        printf("hostname = %s\n", buf);
    }
    else {
        int rt = sethostname(argv[1], strlen(argv[1]));
        if(rt != 0) {
            perror("sethostname");
        }
    }

    return 0;
}