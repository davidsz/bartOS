#include "kernel_commands.h"
#include "keyboard/keyboard.h"
#include "log.h"
#include "output/console.h"
#include "paging/paging.h"
#include "string.h"
#include "task/process.h"
#include "task/task.h"
#include "vector.h"

namespace core {

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
    uint32_t *task_directory = task->pageDirectory();
    uint32_t old_entry = paging::get_table_entry(task_directory, tmp);
    uint8_t flags = paging::IS_PRESENT | paging::ACCESS_FROM_ALL | paging::IS_WRITEABLE;
    paging::map(task_directory, tmp, tmp, flags);

    // Now strcpy can copy into the shared area in user land
    task->ActivateContext();
    strcpy(tmp, (char *)virt);
    task::Task::ReturnToKernel();

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
    register_kernel_command(KernelCommand::EXEC, kc_exec);
    register_kernel_command(KernelCommand::PRINT, kc_print);
    register_kernel_command(KernelCommand::GETKEY, kc_getkey);
    register_kernel_command(KernelCommand::PUTCHAR, kc_putchar);
    register_kernel_command(KernelCommand::MALLOC, kc_malloc);
    register_kernel_command(KernelCommand::FREE, kc_free);
    register_kernel_command(KernelCommand::EXIT, kc_exit);
    register_kernel_command(KernelCommand::GETARGS, kc_getargs);
}

void *run_kernel_command(uint32_t id, core::Registers *registers)
{
    if (s_kernelCommands.capacity() <= id || s_kernelCommands[id] == nullptr) {
        log::warning("run_kernel_command: command %d not registered\n", id);
        return nullptr;
    }
    return s_kernelCommands[id](registers);
}

void *kc_exec(core::Registers *)
{
    task::Task *current_task = task::Task::Current();
    void *user_space_str_addr = current_task->GetStackItem(0);
    char buf[1024];
    copy_string_from_task(current_task, user_space_str_addr, buf, sizeof(buf));
    String command(buf);
    Vector<String> parts = command.split(' ');
    if (parts.size() == 0)
        return 0;

    task::Process *process = new task::Process;
    if (process->Load(parts[0].c_str()) != Status::ALL_OK) {
        log::warning("kc_exec: Failed to load program (%s)\n", parts[0].c_str());
        return 0;
    }

    for (size_t i = 0; i < parts.size(); i++)
        process->AddArgument(parts[i]);

    task::Process::Switch(process);
    return 0;
}

void *kc_print(core::Registers *)
{
    task::Task *current_task = task::Task::Current();
    // TODO: Getting stack items one-by-one is slow. Implement a batched method!
    void *user_space_msg_addr = current_task->GetStackItem(0);
    char buf[1024];
    copy_string_from_task(current_task, user_space_msg_addr, buf, sizeof(buf));
    console::print(buf);
    return 0;
}

void *kc_getkey(core::Registers *)
{
    char c = keyboard::pop();
    return (void *)((int)c);
}

void *kc_putchar(core::Registers *)
{
    int c = (char)(int)task::Task::Current()->GetStackItem(0);
    if (c == 0x08)
        console::backspace();
    else
        console::print("%c", c);
    return 0;
}

void *kc_malloc(core::Registers *)
{
    task::Task *current_task = task::Task::Current();
    size_t size = (size_t)current_task->GetStackItem(0);
    return current_task->process()->Allocate(size);
}

void *kc_free(core::Registers *)
{
    task::Task *current_task = task::Task::Current();
    void *ptr = current_task->GetStackItem(0);
    current_task->process()->Deallocate(ptr);
    return 0;
}

void *kc_exit(core::Registers *)
{
    task::Task::Current()->process()->Terminate();
    return 0;
}

void *kc_getargs(core::Registers *)
{
    struct arguments_t {
        int argc;
        char **argv;
    };

    task::Task *task = task::Task::Current();
    arguments_t *args_out = (arguments_t *)paging::get_physical_address(task->pageDirectory(), task->GetStackItem(0));

    List<String> arg_list = task->process()->Arguments();
    args_out->argc = arg_list.length();
    args_out->argv = (char **)malloc(sizeof(char *) * args_out->argc);
    int i = 0;
    for (auto it = arg_list.begin(); it != arg_list.end(); it++, i++) {
        args_out->argv[i] = (char *)malloc(sizeof(char) * it->length());
        strcpy(args_out->argv[i], it->c_str());
    }

    return 0;
}

}; // namespace core
