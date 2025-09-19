#ifndef TASK_TASK_H
#define TASK_TASK_H

#include <cstdint>

namespace task {

struct Registers {
    uint32_t edi = 0;
    uint32_t esi = 0;
    uint32_t ebp = 0;
    uint32_t ebx = 0;
    uint32_t edx = 0;
    uint32_t ecx = 0;
    uint32_t eax = 0;
    uint32_t ip = 0;
    uint32_t cs = 0;
    uint32_t flags = 0;
    uint32_t esp = 0;
    uint32_t ss = 0;
};

// TODO: Implement as a well-formatted class
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

extern "C" void restore_task(Registers *reg);
extern "C" void restore_general_purpose_registers(Registers *reg);
extern "C" void change_data_segment(uint16_t selector);

void switch_to(Task *task);
void return_to_current_task();
void run_first();

}; // namespace task

#endif // TASK_TASK_H
