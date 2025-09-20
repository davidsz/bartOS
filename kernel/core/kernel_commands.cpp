#include "kernel_commands.h"
#include "log.h"
#include "vector.h"

namespace core {

// FIXME: Global constructors are called before initializing the heap
// so Vector's capacity needs to be reserved manually before using it
static Vector<KERNEL_COMMAND> s_kernelCommands;

void register_kernel_command(uint32_t id, KERNEL_COMMAND command)
{
    s_kernelCommands[id] = command;
}

void register_all_kernel_commands()
{
    s_kernelCommands.resize(32);
    register_kernel_command(KernelCommand::SUM, kc_sum);
}

void *run_kernel_command(uint32_t id, core::Registers *registers)
{
    if (s_kernelCommands.capacity() <= id || s_kernelCommands[id] == nullptr) {
        log::warning("run_kernel_command: command %d not registered\n", id);
        return nullptr;
    }
    return s_kernelCommands[id](registers);
}

void *kc_sum(core::Registers *)
{
    log::info("kc_sum: called\n");
    return 0;
}

}; // namespace core
