#include "kernel_commands.h"
#include "log.h"
#include "output/console.h"
#include "paging/paging.h"
#include "string.h"
#include "task/task.h"
#include "vector.h"

namespace core {

// FIXME: Global constructors are called before initializing the heap
// so Vector's capacity needs to be reserved manually before using it
static Vector<KERNEL_COMMAND> s_kernelCommands;

// TODO: This could be more generic, like copy_bytes_from_task
static void copy_string_from_task(task::Task *task, void *virt, char *buf_out, size_t buf_size)
{
    // This 'tmp' memory will be shared between the kernel and the task
    // We don't use buf_out directly, because it's not an aligned address.
    char *tmp = (char *)calloc(buf_size);
    if (!tmp) {
        log::error("copy_string_from_task: calloc failed\n");
        return;
    }

    // We map the task's tmp address to the kernel's tmp address
    uint32_t *task_directory = task->page_directory;
    uint32_t old_entry = paging::get_table_entry(task_directory, tmp);
    uint8_t flags = paging::IS_PRESENT | paging::ACCESS_FROM_ALL | paging::IS_WRITEABLE;
    paging::map(task_directory, tmp, tmp, flags);

    // Now strcpy can copy into the shared area in user land
    task::switch_to(task);
    strcpy(tmp, (char *)virt);
    task::return_to_kernel();

    // We unmap the shared memory by setting back the old entry
    paging::set_table_entry(task_directory, tmp, old_entry);

    strcpy(buf_out, tmp);
    free(tmp);
}

void register_kernel_command(uint32_t id, KERNEL_COMMAND command)
{
    s_kernelCommands[id] = command;
}

void register_all_kernel_commands()
{
    s_kernelCommands.resize(32);
    register_kernel_command(KernelCommand::SUM, kc_sum);
    register_kernel_command(KernelCommand::PRINT, kc_print);
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
    // TODO: Getting stack items one-by-one is slow. Implement a batched method!
    int v2 = (int)task::get_stack_item(task::current_task(), 1);
    int v1 = (int)task::get_stack_item(task::current_task(), 0);
    return (void *)(v1 + v2);
}

void *kc_print(core::Registers *)
{
    void *user_space_msg_addr = task::get_stack_item(task::current_task(), 0);
    char buf[1024];
    copy_string_from_task(task::current_task(), user_space_msg_addr, buf, sizeof(buf));
    console::print(buf);
    return 0;
}

}; // namespace core
