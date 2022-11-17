#include <stdio.h>
#include <byteswap.h>

int main(int argc, char** argv)
{
    int a = 0x005e005e;
    printf("%02X\n", bswap_32(a));


    return 0;
}