#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc, char **argv)
{
    char *ptr = (char *)malloc(20);
    strcpy(ptr, "Blank - begin\n");
    puts(ptr);
    free(ptr);

    for (int i = 0; i < argc; i++)
        printf("--- %d = %s\n", i, argv[i]);

    // exec("0:/ass.elf");

    // char *crash = (char *)0x00;
    // *crash = 0x50;

/*
    puts("> ");
    char buf[1024];
    readline(buf, sizeof(buf), true);
    puts("\n- ");
    puts(buf);
*/

    // while(1);

    puts("Blank - end\n");
    return 0;
}
