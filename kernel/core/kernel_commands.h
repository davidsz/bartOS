#ifndef CORE_KERNEL_COMMANDS_H
#define CORE_KERNEL_COMMANDS_H

#include "registers.h"

namespace core {

enum KernelCommand {
    SUM = 0
};

typedef void *(*KERNEL_COMMAND)(core::Registers *registers);
void *kc_sum(core::Registers *);

void register_all_kernel_commands();
void register_kernel_command(uint32_t command_id, KERNEL_COMMAND command);
void *run_kernel_command(uint32_t command, core::Registers *registers);

}; // namespace core

#endif // CORE_KERNEL_COMMANDS_H
