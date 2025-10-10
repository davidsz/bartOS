#include "task.h"
#include "config.h"
#include "kernel.h"
#include "loader/binary.h"
#include "log.h"
#include "paging/paging.h"
#include "process.h"
#include "status.h"

namespace task {

// Assembly functions
extern "C" void restore_and_run_task(core::Registers *reg);
extern "C" void change_data_segment(uint16_t selector);

static List<Task *> s_tasks;
static Task *s_currentTask = 0;

Task *Task::New(Process *process)
{
    Task *task = new Task(process);
    if (!task) {
        log::error("Task::New: Out of memory\n");
        return nullptr;
    }

    s_tasks.push_back(task);
    return task;
}

Task *Task::Current()
{
    if (!s_currentTask) {
        log::error("Task::Current: No current task available\n");
        return nullptr;
    }
    return s_currentTask;
}

Task *Task::Next()
{
    if (s_currentTask == *s_tasks.end())
        return *s_tasks.begin();

    for (auto it = s_tasks.begin(); it != s_tasks.end(); it++) {
        if (*it == s_currentTask)
            return *(++it);
    }

    log::error("Task::Next: Couldn't find next task\n");
    return nullptr;
}

void Task::RunNext()
{
    Task *next = Next();
    if (!next) {
        log::error("Task::SwitchToNext: No next Task available.\n");
        return;
    }
    next->ActivateAndRun();
}

void Task::ReturnToCurrent()
{
    if (!s_currentTask)
        return;
    s_currentTask->ActivateContext();
}

void Task::ReturnToKernel()
{
    change_data_segment(KERNEL_DATA_SELECTOR);
    paging::switch_directory(kernel_page_directory());
}

void Task::SaveCurrentState(core::Registers *frame)
{
    if (!s_currentTask) {
        log::error("Task::SaveCurrentState: No current task available\n");
        return;
    }
    s_currentTask->m_registers = *frame;
}

void Task::ActivateContext()
{
    s_currentTask = this;
    change_data_segment(USER_DATA_SELECTOR);
    paging::switch_directory(m_pageDirectory);
}

void Task::ActivateAndRun()
{
    ActivateContext();
    restore_and_run_task(&m_registers);
}

void *Task::GetStackItem(uint32_t index)
{
    void* result = 0;
    uint32_t *stack_pointer = (uint32_t *)m_registers.esp;

    ActivateContext();
    result = (void *)stack_pointer[index];
    ReturnToKernel();

    return result;
}

Task::Task(Process *process)
{
    // Map the entire 4 Gb address space to itself
    m_pageDirectory = paging::new_directory(paging::IS_PRESENT | paging::ACCESS_FROM_ALL);
    if (!m_pageDirectory) {
        log::error("Task::New: Failed to allocate page directory\n");
        return;
    }

    m_process = process;

    m_registers.ip = (uint32_t)process->Binary()->entryAddress();
    m_registers.ss = USER_DATA_SELECTOR;
    m_registers.cs = USER_CODE_SELECTOR;
    m_registers.esp = PROGRAM_VIRTUAL_STACK_ADDRESS_START;
}

Task::~Task()
{
    paging::free_directory(m_pageDirectory);

    for (auto it = s_tasks.begin(); it != s_tasks.end(); it++) {
        if (this == *it) {
            s_tasks.erase(it);
            if (this == s_currentTask)
                s_currentTask = Task::Next();
            return;
        }
    }
    log::error("Task::~Task: Couldn't find task in list\n");
}

}; // namespace task
