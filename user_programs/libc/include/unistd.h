#ifndef LIBC_UNISTD_H
#define LIBC_UNISTD_H

// This should match with the struct in the corresponding kernel command handler
struct process_arguments
{
    int argc;
    char **argv;
};

void exec(char *command);
void get_process_arguments(struct process_arguments *arguments);

#endif // LIBC_UNISTD_H
