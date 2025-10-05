#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void append(char *str, int *pos, const char *appendix)
{
    strcpy(str + *pos, appendix);
    *pos += strlen(appendix);
}

char *resolve_command(char *cmd)
{
    const char *token = strtok(cmd, ' ');
    if (!token)
        return 0;
    char *out = (char *)malloc(sizeof(char) * 1024);
    int pos = 0;
    append(out, &pos, "0:/");
    append(out, &pos, token);
    append(out, &pos, ".elf");

    token = strtok(NULL, ' ');
    while (token) {
        append(out, &pos, " ");
        append(out, &pos, token);
        token = strtok(NULL, ' ');
    }

    return out;
}

int main(int, char **)
{
    puts("WEEshell (bartOS 0.1)\n");

    while (true) {
        puts("> ");
        char buf[1024];
        readline(buf, sizeof(buf), true);
        puts("\n");
        char *command = resolve_command(buf);
        if (!command)
            continue;
        printf("Resolved command: %s\n", command);
        free(command);
    }

    return 0;
}
