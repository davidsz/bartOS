#include "process.h"
#include "config.h"
#include "heap.h"
#include "loader/binary.h"
#include "loader/elf_file.h"
#include "loader/loader.h"
#include "log.h"
#include "paging/paging.h"
#include "status.h"
#include "task.h"
#include <cstdint>

namespace task {

static Process *s_currentProcess = nullptr;
// TODO: Implement a Map container
static Vector<Process *> s_processes;

Process *Process::Current()
{
    return s_currentProcess;
}

Process *Process::Get(uint16_t id)
{
    if (id >= s_processes.size()) {
        log::error("get_process: Process id %d does not exist\n", id);
        return nullptr;
    }
    return s_processes[id];
}

void Process::Switch(Process *process)
{
    s_currentProcess = process;
    log::info("Process::Switch: Switched to process %d\n", s_currentProcess->m_id);
    task::switch_to(s_currentProcess->m_task);
    task::restore_task(&s_currentProcess->m_task->registers);
}

void Process::SwitchToAny()
{
    for (size_t i = 0; i < s_processes.size(); i++) {
        if (s_processes[i] && !s_processes[i]->m_terminated) {
            Switch(s_processes[i]);
            return;
        }
    }
    log::error("Process::SwitchToAny: Couldn't find any process\n");
}

int Process::MapMemory()
{
    int res = Status::ALL_OK;
    switch (m_binary->format()) {
    case loader::Binary::RawBinary:
        res = loader::MapRawBinary(m_task->page_directory, m_binary);
        break;
    case loader::Binary::Elf:
        res = loader::MapElfBinary(m_task->page_directory, (loader::ELF_File *)m_binary);
        break;
    default:
        log::error("Process::MapMemory: Invalid binary format\n");
        return Status::E_IO;
    }

    // Map the stack
    void *end_address = paging::align((void *)((uint32_t)m_stack + PROGRAM_STACK_SIZE));
    uint8_t flags = paging::IS_PRESENT | paging::ACCESS_FROM_ALL | paging::IS_WRITEABLE;
    paging::map_from_to(m_task->page_directory, (void *)PROGRAM_VIRTUAL_STACK_ADDRESS_END, m_stack, end_address, flags);
    return res;
}

int Process::Load(const String &filename)
{
    log::info("Process::Load - 1\n");
    m_binary = loader::LoadFile(filename);
    if (!m_binary) {
        log::error("Process::Load: Failed to load file (%s)\n", filename);
        return Status::E_IO;
    }

    log::info("Process::Load - 2\n");
    void *program_stack = calloc(PROGRAM_STACK_SIZE);
    if (!program_stack) {
        log::error("Process::Load: Failed to allocate memory for stack\n");
        return Status::E_NO_MEMORY;
    }
    m_stack = program_stack;

    log::info("Process::Load - 3\n");
    m_task = new_task(this);

    log::info("Process::Load - 4\n");
    if (int res = MapMemory()) {
        log::error("Process::Load: Failed to map memory (%d)\n", res);
        delete m_task;
        return Status::E_IO;
    }

    m_filename = filename;
    m_id = s_processes.size();
    m_terminated = false;
    s_processes.push_back(this);
    return Status::ALL_OK;
}

void Process::AddArgument(const String &argument)
{
    m_arguments.push_back(argument);
}

void *Process::Allocate(size_t size)
{
    void *ptr = malloc(size);
    if (!ptr) {
        log::warning("Process::Allocate: Failed to allocate memory (%d bytes)\n", size);
        return 0;
    }

    void *end_address = paging::align((void *)((uint32_t)ptr + size));
    int flags = paging::IS_WRITEABLE | paging::IS_PRESENT | paging::ACCESS_FROM_ALL;
    paging::map_from_to(m_task->page_directory, ptr, ptr, end_address, flags);

    m_allocations.push_back({
        .ptr = ptr,
        .size = size
    });
    return ptr;
}

void Process::Deallocate(void *ptr)
{
    for (auto it = m_allocations.begin(); it != m_allocations.end(); it++) {
        if (it->ptr == ptr) {
            void *end_address = paging::align((void *)((uint32_t)it->ptr + it->size));
            paging::map_from_to(m_task->page_directory, it->ptr, it->ptr, end_address, 0x00);
            free(it->ptr);
            m_allocations.erase(it);
            return;
        }
    }
}

void Process::Terminate()
{
    for (auto it = m_allocations.begin(); it != m_allocations.end(); it++)
        Deallocate(it->ptr);

    delete m_binary;
    m_binary = 0;

    free(m_stack);
    m_stack = 0;

    delete m_task;
    m_task = 0;

    m_terminated = true;

    if (this == s_currentProcess)
        SwitchToAny();
}

} // namespace task
