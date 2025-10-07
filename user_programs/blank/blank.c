#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(int, char **)
{
    char *ptr = (char *)malloc(20);
    strcpy(ptr, "Blank - 1\n");
    puts(ptr);
    free(ptr);

    exec("0:/ass.elf");

    puts("Blank - 2\n");
/*
    puts("> ");
    char buf[1024];
    readline(buf, sizeof(buf), true);
    puts("\n- ");
    puts(buf);
*/
    while(1);
    return 0;
}
