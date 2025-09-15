#include "task.h"
#include "config.h"
#include "log.h"
#include "paging/paging.h"
#include "status.h"

namespace task {

// TODO: Maybe use my custom List implementation?
static Task *s_currentTask = 0;
static Task *s_taskTail = 0;
static Task *s_taskHead = 0;

void remove_from_list(Task *task)
{
    if (task->prev)
        task->prev->next = task->next;

    if (task == s_taskHead)
        s_taskHead = task->next;

    if (task == s_taskTail)
        s_taskTail = task->prev;

    if (task == s_currentTask)
        s_currentTask = next_task();
}

int init_task(Task *task)
{
    // Map the entire 4 Gb address space to itself
    task->page_directory = paging::new_directory(paging::IS_PRESENT | paging::ACCESS_FROM_ALL);
    if (!task->page_directory) {
        log::error("init_task: failed to allocate page directory\n");
        return E_NO_MEMORY;
    }

    task->registers.ip = PROGRAM_VIRTUAL_ADDRESS;
    task->registers.ss = USER_DATA_SELECTOR;
    task->registers.esp = PROGRAM_VIRTUAL_STACK_ADDRESS_START;

    return Status::ALL_OK;
}

Task *new_task()
{
    Task *task = new Task;
    if (!task) {
        log::error("task_new: out of memory\n");
        return nullptr;
    }

    if (init_task(task) != Status::ALL_OK) {
        log::error("task_new: failed to initialize task\n");
        delete task;
        return nullptr;
    }

    if (s_taskHead == 0) {
        s_taskHead = task;
        s_taskTail = task;
        s_currentTask = task;
        return task;
    }

    s_taskTail->next = task;
    task->prev = s_taskTail;
    s_taskTail = task;

    return task;
}

Task *current_task()
{
    return s_currentTask;
}

Task *next_task()
{
    if (!s_currentTask->next)
        return s_taskHead;
    return s_currentTask->next;
}

Task::~Task()
{
    paging::free_directory(page_directory);
    remove_from_list(this);
}

}; // namespace task
