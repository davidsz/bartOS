#ifndef KEYBOARD_KEYBOARD_H
#define KEYBOARD_KEYBOARD_H

#include "config.h"
#include "string.h"

// Virtual Keyboard Layer

namespace keyboard {

// Interface that must be implemented by the keyboard drivers
class IKeyboard {
public:
    explicit IKeyboard(const char *name) : m_name(name) {}
    virtual void Init() = 0;

    String name() const;
private:
    String m_name;
};

// Each Process will have their own KeyBuffer
class KeyBuffer {
public:
    void Push(char c);
    char Pop();
    // TODO: Is this needed?
    void Backspace();
private:
    int m_tail = 0; // Position where we push chars
    int m_head = 0; // Position where we pop from
    char m_buffer[KEYBOARD_BUFFER_SIZE];
};

void init_all();
void push(char c);
char pop();

}; // namespace keyboard

#endif // KEYBOARD_KEYBOARD_H
