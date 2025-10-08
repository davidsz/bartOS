#ifndef CORE_REGISTERS_H
#define CORE_REGISTERS_H

#include "memory.h"
#include <cstdint>
#include <stdint.h>

namespace core {

struct Registers {
    uint32_t edi = 0;       // + 0
    uint32_t esi = 0;       // + 4
    uint32_t ebp = 0;       // + 8
    uint32_t reserved = 0;  // + 12 (The pusha command pushes the original esp here)
    uint32_t ebx = 0;       // + 16
    uint32_t edx = 0;       // + 20
    uint32_t ecx = 0;       // + 24
    uint32_t eax = 0;       // + 28
    uint32_t ip = 0;        // + 32
    uint32_t cs = 0;        // + 36
    uint32_t flags = 0;     // + 40
    uint32_t esp = 0;       // + 44
    uint32_t ss = 0;        // + 48

    Registers() = default;

    Registers(const Registers &other) {
        memcpy(this, &other, sizeof(Registers));
    }

    Registers &operator=(const Registers &other) {
        if (this != &other)
            memcpy(this, &other, sizeof(Registers));
        return *this;
    }

    Registers(Registers &&) = delete;
    Registers &operator=(Registers &&) = delete;
} __attribute__((packed));

}; // namespace core

#endif // CORE_REGISTERS_H
