#include "stdio.h"
#include "stdlib.h"
#include "cstring.h"

int main(int, char **)
{
    char *ptr = (char *)malloc(20);
    strcpy(ptr, "WEEEeeeEEEEeeeEEE!");
    print(ptr);
    free(ptr);

    while(1);

    return 0;
}
