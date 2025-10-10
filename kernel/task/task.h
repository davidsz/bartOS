#ifndef TASK_TASK_H
#define TASK_TASK_H

#include "core/registers.h"
#include <cstdint>

namespace task {

class Process;

class Task
{
public:
    static Task *New(Process *);
    static Task *Current();
    static Task *Next();
    static void RunNext();
    static void ReturnToCurrent();
    static void ReturnToKernel();
    static void SaveCurrentState(core::Registers *frame);

    ~Task();

    uint32_t *pageDirectory() { return m_pageDirectory; }
    Process *process() { return m_process; }

    void ActivateContext();
    void ActivateAndRun();
    void *GetStackItem(uint32_t index);

private:
    Task(Process *);

    // The page directory of the task
    uint32_t *m_pageDirectory = 0;

    // The registers of the task when the task is not running
    core::Registers m_registers;

    // Not owning reference for the process
    Process *m_process = 0;
};

}; // namespace task

#endif // TASK_TASK_H
