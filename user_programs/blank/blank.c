#include <string.h>
#include <stdio.h>
#include <stdlib.h>

int main(int, char **)
{
    char *ptr = (char *)malloc(20);
    strcpy(ptr, "WEEEeeeEEEEeeeEEE!\n");
    puts(ptr);
    free(ptr);

    puts("> ");
    char buf[1024];
    readline(buf, sizeof(buf), true);
    puts("\n- ");
    puts(buf);

    while(1);
    return 0;
}
