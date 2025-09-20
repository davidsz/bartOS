#ifndef CORE_REGISTERS_H
#define CORE_REGISTERS_H

#include "memory.h"
#include <cstdint>
#include <stdint.h>

namespace core {

struct Registers {
    uint32_t edi = 0;
    uint32_t esi = 0;
    uint32_t ebp = 0;
    uint32_t reserved = 0; // The pusha command pushes the original esp here
    uint32_t ebx = 0;
    uint32_t edx = 0;
    uint32_t ecx = 0;
    uint32_t eax = 0;
    uint32_t ip = 0;
    uint32_t cs = 0;
    uint32_t flags = 0;
    uint32_t esp = 0;
    uint32_t ss = 0;

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
