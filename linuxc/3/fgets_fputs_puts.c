#include <stdio.h>
#include <string.h>

int main(int argc, char** argv)
{
    char buf[1024] = {0};
    char* pbuf = fgets(buf, sizeof(buf), stdin);
    printf("buf addr = %p\n", buf);
    printf("pbuf addr = %p\n", pbuf);
    int len = strlen(pbuf);
    if( buf[len-1] == '\n') {
        printf("have \\n\n");
        buf[len-1] = '\0'; 
    }

    fputs(buf, stdout);
    printf("------\n");
    puts(buf);

    return 0;
}