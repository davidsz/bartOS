#include "config.h"
#include "console/console.h"
#include "core/constructors.h"
#include "core/destructors.h"
#include "core/gdt.h"
#include "core/idt.h"

// Lib
#include "block_allocator.h"
#include "heap.h"

// Check if the compiler thinks you are targeting the wrong operating system.
#if defined(__linux__)
#error "You are not using a cross-compiler, you will most certainly run into trouble"
#endif

// Work only for the 32-bit ix86 targets
#if !defined(__i386__)
#error "This kernel needs to be compiled with a ix86-elf compiler"
#endif

// Dummy __dso_handle implementation for freestanding environments
extern "C" void *__dso_handle;
void *__dso_handle = nullptr;

// Will handle heap allocations
static BlockAllocator s_allocator(HEAP_BLOCK_SIZE);

// extern "C": Using C-style linking keeps the function name unmodified;
// otherwise C++ extends the name and we can't call it from assembly.
// (called by kernel.s)
extern "C" int kernel_main()
{
    // Call constructors of global objects.
    core::call_constructors();

    // Global Descriptor Table
    core::setup_gdt();

    // Interrupt Descriptor Table
    core::setup_idt();
    // Interrupts were potentionally disabled by the bootloader
    // since the beginning of protected mode. It's safe to enable them again.
    core::enable_interrupts();

    console::set_color(console::Color::DarkGrey, console::Color::LightGrey);
    console::clear();
    console::print("bartOS raises\n\n");

    // Initialize heap and support allocation methods
    int ret = s_allocator.Initialize((void *)HEAP_ADDRESS, HEAP_SIZE_BYTES);
    if (ret != Status::ALL_OK)
        console::print("Failed to initialize the heap allocator.\n");
    set_heap_allocator(&s_allocator);

    // TODO: Implement an exit condition
    while (true);

    // Call destructors of global objects
    __cxa_finalize(__dso_handle);

    return 0;
}
