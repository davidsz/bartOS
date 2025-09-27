#include "keyboard.h"
#include "list.h"
#include "task/process.h"

static List<keyboard::IKeyboard *> s_keyboards;

static void init_keyboard(keyboard::IKeyboard *keyboard)
{
    if (!keyboard)
        return;
    keyboard->Init();
    s_keyboards.push_back(keyboard);
}

namespace keyboard {

void KeyBuffer::Push(char c)
{
    m_buffer[m_tail++ % sizeof(m_buffer)] = c;
}

char KeyBuffer::Pop()
{
    int index = m_head % sizeof(m_buffer);
    char c = m_buffer[index];
    if (c == 0)
        return 0;
    m_buffer[index] = 0;
    m_head++;
    return c;
}

void KeyBuffer::Backspace()
{
    m_buffer[--m_tail % sizeof(m_buffer)] = 0;
}

void init_all()
{
    // TODO: Add keyboards here
    init_keyboard(0);
}

void push(char c)
{
    // We always push keys for the currently active and visible process
    task::Process *current_process = task::Process::Current();
    if (!current_process)
        return;
    current_process->KeyBuffer()->Push(c);
}

char pop()
{
    if (!task::current_task())
        return 0;
    // In case of pop() it's possible that we have to pop from a background task
    task::Process *process = task::current_task()->process;
    return process->KeyBuffer()->Pop();
}

}; // namespace keyboard
