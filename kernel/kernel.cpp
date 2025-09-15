#include "config.h"
#include "core/constructors.h"
#include "core/destructors.h"
#include "core/idt.h"
#include "core/seriallogger.h"
#include "disk/disk.h"
#include "disk/filesystem.h"
#include "output/console.h"
#include "output/serial.h"
#include "paging/paging.h"
#include "segmentation/gdt.h"

// Lib
#include "block_allocator.h"
#include "heap.h"
#include "path.h"

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

static core::SerialLogger s_logger;

// Will handle heap allocations
static BlockAllocator s_allocator(HEAP_BLOCK_SIZE);

// Describes the kernel page directory
static paging::Directory s_kernelPageDirectory = 0;

// extern "C": Using C-style linking keeps the function name unmodified;
// otherwise C++ extends the name and we can't call it from assembly.
// (called by kernel.s)
extern "C" int kernel_main(unsigned int multiboot_magic, void *)
{
    // Call constructors of global objects.
    core::call_constructors();

    // Main outputs
    serial::init();
    console::set_color(console::Color::DarkGrey, console::Color::LightGrey);
    console::clear();

    if (multiboot_magic == 0x2BADB002)
        console::print("Loaded by a Multiboot compliant bootloader. (%p)\n\n", multiboot_magic);

    log::set_logger(&s_logger);
    log::info("BartOS supports serial output.\n");

    // Initialize heap and support allocation methods
    if (s_allocator.Initialize((void *)HEAP_ADDRESS, HEAP_SIZE_BYTES) != Status::ALL_OK)
        log::error("Failed to initialize the heap allocator.\n");
    set_heap_allocator(&s_allocator);

    // Global Descriptor Table
    segmentation::setup_gdt();

    // Interrupt Descriptor Table
    core::setup_idt();

    // Setup paging
    s_kernelPageDirectory = paging::new_directory(paging::IS_WRITEABLE | paging::IS_PRESENT | paging::ACCESS_FROM_ALL);
    paging::switch_directory(s_kernelPageDirectory);

#if 0
    char *ptr = (char *)kalloc(4096);
    paging::set_table_entry(s_kernelPageDirectory, (void *)0x1000, (uint32_t)ptr | paging::IS_WRITEABLE | paging::IS_PRESENT | paging::ACCESS_FROM_ALL);
#endif

    paging::enable();

#if 0
    char *ptr2 = (char *)0x1000;
    ptr2[0] = 'A';
    ptr2[1] = 'B';
    console::print("Address of ptr: %p\nData of ptr: %s\n", ptr, ptr);
    console::print("Address of ptr2: %p\nData of ptr2: %s\n", ptr2, ptr2);
    console::print("Mapping %p to %p succeeded.\n", ptr, ptr2);
#endif

    // Interrupts were potentionally disabled by the bootloader
    // since the beginning of protected mode. It's safe to enable them again.
    core::enable_interrupts();

    // Initialize supported file systems - disk initialization will use these
    filesystem::init_all();

    // Initialize disks
    disk::search_and_init_all();

    // Test file handling
    int fd = core::fopen("0:/file.txt", "r");
    if (fd >= 0) {
        console::print("We opened file.txt\n");
        filesystem::FileStat stat;
        core::fstat(fd, &stat);
        console::print("Size: %d\n", stat.size);
        char buf[stat.size + 1];
        console::print("Reading file.txt\n");
        core::fread(buf, stat.size, 1, fd);
        buf[stat.size] = 0x00;
        console::print("Result:\n");
        console::print(buf);
        console::print("\n---\n");
        core::fclose(fd);
    }

    int fd2 = core::fopen("0:/another.txt", "r");
    if (fd2 >= 0) {
        console::print("We opened another.txt\n");
        char buf[14];
        console::print("Reading another.txt\n");
        core::fread(buf, 13, 1, fd2);
        buf[13] = 0x00;
        console::print("Result:\n");
        console::print(buf);
        console::print("\n---\n");
        core::fclose(fd2);
    }

    // TODO: Implement an exit condition
    while (true);

    // Call destructors of global objects
    __cxa_finalize(__dso_handle);

    return 0;
}
