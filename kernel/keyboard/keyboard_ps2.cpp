#include "keyboard_ps2.h"
#include "core/idt.h"
#include "core/io.h"
#include <stddef.h>
#include <stdint.h>

#define PS2_PORT 0x64
#define PS2_COMMAND_ENABLE_FIRST_PORT 0xAE

#define KEYBOARD_KEY_RELEASED 0b10000000
#define KEYBOARD_INTERRUPT 0x21
#define KEYBOARD_INPUT_PORT 0x60

namespace core {
class Registers;
}

static constexpr uint8_t s_keyboard_scan_set_one[] = {
    0x00, 0x1B, '1', '2', '3', '4', '5',
    '6', '7', '8', '9', '0', '-', '=',
    0x08, '\t', 'Q', 'W', 'E', 'R', 'T',
    'Y', 'U', 'I', 'O', 'P', '[', ']',
    0x0d, 0x00, 'A', 'S', 'D', 'F', 'G',
    'H', 'J', 'K', 'L', ';', '\'', '`',
    0x00, '\\', 'Z', 'X', 'C', 'V', 'B',
    'N', 'M', ',', '.', '/', 0x00, '*',
    0x00, 0x20, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, '7', '8', '9', '-', '4', '5',
    '6', '+', '1', '2', '3', '0', '.'
};

static char scancode_to_char(uint8_t scancode)
{
    if (scancode > sizeof(s_keyboard_scan_set_one) / sizeof(uint8_t))
        return 0;
    return s_keyboard_scan_set_one[scancode];
}

static void handle_interrupt(core::Registers *)
{
    uint8_t scancode = 0;
    scancode = core::inb(KEYBOARD_INPUT_PORT);
    core::inb(KEYBOARD_INPUT_PORT);

    if (scancode & KEYBOARD_KEY_RELEASED)
        return;

    if (char c = scancode_to_char(scancode))
        keyboard::push(c);
}

namespace keyboard {

Keyboard_PS2::Keyboard_PS2(const char *name)
    : IKeyboard(name)
{
}

void Keyboard_PS2::Init()
{
    core::register_interrupt_callback(KEYBOARD_INTERRUPT, handle_interrupt);
    core::outb(PS2_PORT, PS2_COMMAND_ENABLE_FIRST_PORT);
}

}; // namespace keyboard
