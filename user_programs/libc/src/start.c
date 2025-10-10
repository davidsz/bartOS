#include <unistd.h>

extern int main(int argc, char **argv);

void c_start()
{
    struct process_arguments main_args;
    get_process_arguments(&main_args);
    main(main_args.argc, main_args.argv);
}
