#ifndef CORE_KERNEL_COMMANDS_H
#define CORE_KERNEL_COMMANDS_H

#include "registers.h"

namespace core {

enum KernelCommand {
    EXEC = 0,
    PRINT = 1,
    GETKEY = 2,
    PUTCHAR = 3,
    MALLOC = 4,
    FREE = 5,
    EXIT = 6,
    GETARGS = 7,
};

typedef void *(*KERNEL_COMMAND)(core::Registers *registers);
void *kc_exec(core::Registers *);
void *kc_print(core::Registers *);
void *kc_getkey(core::Registers *);
void *kc_putchar(core::Registers *);
void *kc_malloc(core::Registers *);
void *kc_free(core::Registers *);
void *kc_exit(core::Registers *);
void *kc_getargs(core::Registers *);

void register_all_kernel_commands();
void register_kernel_command(uint32_t command_id, KERNEL_COMMAND command);
void *run_kernel_command(uint32_t command, core::Registers *registers);

}; // namespace core

#endif // CORE_KERNEL_COMMANDS_H
