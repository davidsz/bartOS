#include <ctype.h>

int isdigit(int c)
{
    return c >= 48 && c <= 57;
}

int tolower(int c)
{
    if (c >= 65 && c <= 90)
        c += 32;
    return c;
}

int tonumericdigit(int c)
{
    return c - 48;
}
