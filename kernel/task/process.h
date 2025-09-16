#ifndef TASK_PROCESS_H
#define TASK_PROCESS_H

#include "string.h"
#include "task.h"
#include <cstdint>

namespace task {

struct Process {
    static Process *Get(uint16_t id);
    static Process *Current();

    int Load(const String &filename);

private:
    int LoadData(const String &filename);
    int LoadBinary(const String &filename);
    int MapMemory();
    int MapBinary();

    // The process ID
    uint16_t m_id = -1;

    // The binary file to be loaded
    String m_filename;

    // The main process task
    Task *m_task = 0;

    // The memory (malloc) allocations of the process
    // void *allocations[MAX_PROGRAM_ALLOCATIONS];

    // The physical pointer to the process memory
    void *m_ptr = 0;

    // The size of the data pointed by "ptr"
    uint32_t m_size = 0;

    // The physical pointer to the stack memory
    void *m_stack = 0;
};

}; // namespace task

#endif // TASK_PROCESS_H
