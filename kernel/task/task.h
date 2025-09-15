#ifndef TASK_TASK_H
#define TASK_TASK_H

#include <cstdint>

namespace task {

struct Registers {
    uint32_t edi;
    uint32_t esi;
    uint32_t ebp;
    uint32_t ebx;
    uint32_t edx;
    uint32_t ecx;
    uint32_t eax;
    uint32_t ip;
    uint32_t cs;
    uint32_t flags;
    uint32_t esp;
    uint32_t ss;
};

struct Task {
    ~Task();

    // The page directory of the task
    uint32_t *page_directory = 0;

    // The registers of the task when the task is not running
    Registers registers;

    // The next task in the linked list
    Task *next = 0;

    // Previous task in the linked list
    Task *prev = 0;
};

Task *new_task();
Task *current_task();
Task *next_task();

}; // namespace task

#endif // TASK_TASK_H
