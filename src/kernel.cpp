#include "core/gdt.h"
#include "core/idt.h"
#include "console/console.h"


// Check if the compiler thinks you are targeting the wrong operating system.
#if defined(__linux__)
#error "You are not using a cross-compiler, you will most certainly run into trouble"
#endif

// Work only for the 32-bit ix86 targets
#if !defined(__i386__)
#error "This kernel needs to be compiled with a ix86-elf compiler"
#endif


// Call all constructors of global objects. Without this, static objects
// in the global scope just won't work.
// (start_ctors and end_ctors are defined in the linker script)
typedef void (*constructor)();
extern "C" constructor start_ctors;
extern "C" constructor end_ctors;
void call_constructors()
{
    for (constructor *i = &start_ctors; i != &end_ctors; i++)
        (*i)();
}

// extern "C": Using C-style linking keeps the function name unmodified;
// otherwise C++ extends the name and we can't call it from assembly.
// (called by kernel.s)
extern "C" int kernel_main()
{
    call_constructors();

    core::setup_gdt();
    core::setup_idt();

    console::set_color(console::Color::DarkGrey, console::Color::LightGrey);
    console::clear();
    console::print("bartOS raises\n\n");
    console::print("Technically it's still nothing.\n");

    while (true);

    return 0;
}
