#include "process.h"
#include "config.h"
#include "disk/file.h"
#include "heap.h"
#include "log.h"
#include "paging/paging.h"
#include "status.h"

namespace task {

static Process *s_currentProcess = nullptr;
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

int Process::LoadBinary(const String &filename)
{
    int fd = core::fopen(filename.c_str(), "r");
    if (!fd) {
        log::error("Process::LoadBinary: Failed to open file %s\n", filename.c_str());
        return Status::E_INVALID_ARGUMENT;
    }

    filesystem::FileStat stat;
    if (int res = core::fstat(fd, &stat)) {
        log::error("Process::LoadBinary: Failed to stat file %s\n", filename.c_str());
        core::fclose(fd);
        return res;
    }

    void *program_data = malloc(stat.size);
    if (!program_data) {
        log::error("Process::LoadBinary: Failed to allocate memory for %s\n", filename.c_str());
        core::fclose(fd);
        return Status::E_NO_MEMORY;
    }

    if (core::fread(program_data, stat.size, 1, fd) != 1) {
        log::error("Process::LoadBinary: Failed to read file %s\n", filename.c_str());
        free(program_data);
        core::fclose(fd);
        return Status::E_IO;
    }

    m_ptr = program_data;
    m_size = stat.size;
    return Status::ALL_OK;
}

int Process::LoadData(const String &filename)
{
    return LoadBinary(filename);
}

int Process::MapBinary()
{
    if (!m_task || !m_ptr || !m_size) {
        log::error("Process::MapBinary: Invalid process\n");
        return Status::E_INVALID_ARGUMENT;
    }

    void *end_address = paging::align((void *)((uint32_t)m_ptr + m_size));
    uint8_t flags = paging::IS_PRESENT | paging::ACCESS_FROM_ALL | paging::IS_WRITEABLE;
    paging::map_from_to(m_task->page_directory, (void *)PROGRAM_VIRTUAL_ADDRESS, m_ptr, end_address, flags);
    return Status::ALL_OK;
}

int Process::MapMemory()
{
    return MapBinary();
}

int Process::Load(const String &filename)
{
    if (int res = LoadData(filename)) {
        log::error("Process::Load: Failed to load data (%d)\n", res);
        return res;
    }

    void *program_stack = calloc(PROGRAM_STACK_SIZE);
    if (!program_stack) {
        log::error("Process::Load: Failed to allocate memory for stack\n");
        return Status::E_NO_MEMORY;
    }
    m_stack = program_stack;

    m_task = new_task();

    if (int res = MapMemory()) {
        log::error("Process::Load: Failed to map memory (%d)\n", res);
        delete m_task;
        return Status::E_IO;
    }

    m_filename = filename;
    m_id = s_processes.size();
    s_processes.push_back(this);
    return Status::ALL_OK;
}

} // namespace task
