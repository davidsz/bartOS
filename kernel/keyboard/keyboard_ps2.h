#ifndef KEYBOARD_KEYBOARD_PS2_H
#define KEYBOARD_KEYBOARD_PS2_H

#include "keyboard.h"

namespace keyboard {

class Keyboard_PS2 : public IKeyboard {
public:
    Keyboard_PS2(const char *name);
    void Init() override;
};

}; // namespace keyboard

#endif // KEYBOARD_KEYBOARD_PS2_H
