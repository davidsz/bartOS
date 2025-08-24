#include "elf.h"
#include <stddef.h>
#include <stdint.h>

#include "print.h"

#define KERNEL_LOAD_ADDRESS 0x00100000
#define SECTOR_SIZE 512

struct Multiboot_Header {
    uint32_t magic;
    uint32_t flags;
    uint32_t checksum;
    uint32_t header_addr;
    uint32_t load_addr;
    uint32_t load_end_addr;
    uint32_t bss_end_addr;
    uint32_t entry_addr;
} __attribute__((packed));

static void *memset(void *ptr, uint8_t byte, size_t size)
{
    for (size_t i = 0; i < size; i++)
        ((uint8_t *)ptr)[i] = byte;
    return ptr;
}

static ELF32_Header *parse_elf_header()
{
    // print("Parsing ELF header\n");
    ELF32_Header *ret = nullptr;
    const uint8_t *addr = (const uint8_t *)KERNEL_LOAD_ADDRESS;
    print("--- %p, %p, %p, %p\n", addr[0], addr[1], addr[2], addr[3]);
    if (addr[0] == 0x7F && addr[1] == 'E' && addr[2] == 'L' && addr[3] == 'F')
        ret = (ELF32_Header *)KERNEL_LOAD_ADDRESS;
    return ret;
}

static Multiboot_Header *find_multiboot_header()
{
    // print("Parsing Multiboot header\n");
    Multiboot_Header *ret = nullptr;
    const uint8_t *addr = (const uint8_t *)KERNEL_LOAD_ADDRESS;
    print("--- %p, %p, %p, %p\n", addr[0], addr[1], addr[2], addr[3]);
    for (int i = 0; i < 8192 / 4; i++) {
        if (addr[0] == 0x02 && addr[1] == 0xB0 && addr[2] == 0xAD && addr[3] == 0x1B) {
            ret = (Multiboot_Header *)addr;
            break;
        }
        addr += 4;
    }
    return ret;
}

using EntryPointFunc = void(*)();
extern "C" void jump_to_address(uint32_t address);

extern "C" void ata_lba_read(uint32_t start_lba, uint32_t num_sectors, uint32_t dest_addr);
extern "C" uint32_t bytes_to_sectors(uint32_t bytes);

typedef void (*constructor)();
extern "C" constructor start_ctors;
extern "C" constructor end_ctors;

extern "C" uint32_t second_stage_cpp(uint32_t stage2_byte_size, uint32_t kernel_lba)
{
    for (constructor *i = &start_ctors; i != &end_ctors; i++)
        (*i)();

    set_color(Color::DarkGrey, Color::LightGrey);
    clear();
    int ss = (stage2_byte_size + (SECTOR_SIZE - 1)) / SECTOR_SIZE;
    print("stage2_byte_size = %d, sector size = %d\n", stage2_byte_size, ss);
    print("kernel_lba = %d\n", kernel_lba);

    // Load the first sector of the kernel
    ata_lba_read(kernel_lba, 1, KERNEL_LOAD_ADDRESS);

    // Determine its format by looking into the loaded sector
    if (ELF32_Header *elf_header = parse_elf_header()) {
        print("ELF kernel found, number of segments: %d\n", elf_header->e_phnum);
        ELF32_ProgramHeader *ph = (ELF32_ProgramHeader *)(KERNEL_LOAD_ADDRESS + elf_header->e_phoff);
        for (int i = 0; i < elf_header->e_phnum; i++) {
            // Check if it's a loadable section
            if (ph->p_type == 1) {
                memset((void *)(KERNEL_LOAD_ADDRESS + ph->p_vaddr), 0, ph->p_memsz);
                print("Loadig segment with sector size: %d\n", bytes_to_sectors(ph->p_filesz));
                ata_lba_read(kernel_lba + (ph->p_offset / SECTOR_SIZE), bytes_to_sectors(ph->p_filesz), KERNEL_LOAD_ADDRESS + ph->p_vaddr);
            }
            ph++;
        }
        // Jump to ELF entry point
        // print("Jumping to kernel entry point: %p\n", elf_header->e_entry);
        // ((EntryPointFunc)elf_header->e_entry)();
        // void *kernel_entry = (void *)elf_header->e_entry;
        // goto *kernel_entry;
        // __asm__ volatile ("jmp *%0" : : "r"(elf_header->e_entry));
        return elf_header->e_entry;
        // print("ELF processing finished\n");
    } else {
        print("Testing for raw multiboot kernel\n");
        // Validate if its multiboot compatible
        // The multiboot header should be somewhere in the first 8192 bytes (16 sectors)
        ata_lba_read(kernel_lba, 16, KERNEL_LOAD_ADDRESS);
        if (Multiboot_Header *header = find_multiboot_header()) {
            print("Multiboot header found\n");
            print("load_addr: %p\n", header->load_addr);
            print("load_end_addr: %p\n", header->load_end_addr);
            print("Bytes to load: %d\n", header->load_end_addr - header->load_addr);
            print("Sectors to load: %d\n", bytes_to_sectors(header->load_end_addr - header->load_addr));
            ata_lba_read(kernel_lba, bytes_to_sectors(header->load_end_addr - header->load_addr), KERNEL_LOAD_ADDRESS);
            if (header->bss_end_addr > header->load_end_addr) {
                print("bss_end_addr: %p\n", header->bss_end_addr);
                print("Bytes to clear: %d\n", header->bss_end_addr - header->load_end_addr);
                memset((void *)(header->load_end_addr), 0, header->bss_end_addr - header->load_end_addr);
            }

            // print("Jumping to kernel entry point: %p\n", header->entry_addr);
            ((EntryPointFunc)header->entry_addr)();
            // jump_to_address(header->entry_addr);
            // return header->entry_addr;
            // print("Multiboot processing finished\n");
        } else {
            print("Multiboot header NOT found\n");
        }
    }

    // TODO:
    // - Collect boot information and ensure the proper machine state
    //     - https://www.gnu.org/software/grub/manual/multiboot/multiboot.html
    // - Jump to the kernel entry point

    while (true);
    return 0;
}
