#ifndef TASK_TASK_H
#define TASK_TASK_H

#include "core/registers.h"
#include <cstdint>

namespace task {

class Process;

// TODO: Implement as a well-formatted class
struct Task {
    ~Task();

    // The page directory of the task
    uint32_t *page_directory = 0;

    // The registers of the task when the task is not running
    core::Registers registers;

    // Not owning reference for the process
    Process *process = 0;

    // The next task in the linked list
    Task *next = 0;

    // Previous task in the linked list
    Task *prev = 0;
};

Task *new_task(Process *);
Task *current_task();
Task *next_task();

extern "C" void restore_task(core::Registers *reg);
extern "C" void restore_general_purpose_registers(core::Registers *reg);
extern "C" void change_data_segment(uint16_t selector);

void switch_to(Task *task);
void switch_to_next();
void return_to_current_task();
void return_to_kernel();
void run_first();
void save_state(Task *task, core::Registers *frame);
void save_current_state(core::Registers *frame);
void *get_stack_item(Task *task, uint32_t index);

}; // namespace task

#endif // TASK_TASK_H
