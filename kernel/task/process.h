#ifndef TASK_PROCESS_H
#define TASK_PROCESS_H

#include "string.h"
#include "keyboard/keyboard.h"
#include <cstdint>

namespace loader {
class Binary;
}

namespace task {

struct Task;

class Process {
public:
    static Process *Get(uint16_t id);
    static Process *Current();
    static void Switch(Process *process);

    int Load(const String &filename);

    keyboard::KeyBuffer *KeyBuffer() { return &m_keybuffer; }
    loader::Binary *Binary() { return m_binary; }

private:
    int MapMemory();

    // The process ID
    uint16_t m_id = -1;

    // The binary file to be loaded
    String m_filename;

    // The main process task
    Task *m_task = 0;

    // The memory (malloc) allocations of the process
    // void *allocations[MAX_PROGRAM_ALLOCATIONS];

    // Each process has its own keyboard buffer
    keyboard::KeyBuffer m_keybuffer;

    // The executable for this process
    loader::Binary *m_binary = 0;

    // The physical pointer to the stack memory
    void *m_stack = 0;
};

}; // namespace task

#endif // TASK_PROCESS_H
