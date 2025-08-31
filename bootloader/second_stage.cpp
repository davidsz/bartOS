#include "elf.h"
#include <stddef.h>
#include <stdint.h>

#define TEST_SECTORS_ADDRESS 0x00000500
#define KERNEL_LOAD_ADDRESS  0x00100000
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

static ELF32_Header *parse_elf_header(uint32_t start_address)
{
    ELF32_Header *ret = nullptr;
    const uint8_t *addr = (const uint8_t *)start_address;
    if (addr[0] == 0x7F && addr[1] == 'E' && addr[2] == 'L' && addr[3] == 'F')
        ret = (ELF32_Header *)start_address;
    return ret;
}

static Multiboot_Header *find_multiboot_header(uint32_t start_address)
{
    Multiboot_Header *ret = nullptr;
    const uint8_t *addr = (const uint8_t *)start_address;
    for (int i = 0; i < 8192 / 4; i++) {
        if (addr[0] == 0x02 && addr[1] == 0xB0 && addr[2] == 0xAD && addr[3] == 0x1B) {
            ret = (Multiboot_Header *)addr;
            break;
        }
        addr += 4;
    }
    return ret;
}

extern "C" void ata_lba_read(uint32_t start_lba, uint32_t num_sectors, uint32_t dest_addr);
extern "C" uint32_t bytes_to_sectors(uint32_t bytes);

extern "C" uint32_t second_stage_cpp(uint32_t kernel_lba)
{
    uint32_t kernel_entry_addr = 0;

    // Load the first sector of the kernel
    ata_lba_read(kernel_lba, 1, TEST_SECTORS_ADDRESS);
    // Determine its format by looking into the loaded sector
    if (ELF32_Header *elf_header = parse_elf_header(TEST_SECTORS_ADDRESS)) {
        ELF32_ProgramHeader *ph = (ELF32_ProgramHeader *)(TEST_SECTORS_ADDRESS + elf_header->e_phoff);
        for (int i = 0; i < elf_header->e_phnum; i++) {
            // Check if it's a loadable section
            if (ph->p_type == 1) {
                memset((void *)(ph->p_vaddr), 0, ph->p_memsz);
                // Don't call bytes_to_sectors embedded in arguments list to avoid mixing call conventions
                int sectors = bytes_to_sectors(ph->p_filesz);
                ata_lba_read(kernel_lba + (ph->p_offset / SECTOR_SIZE), sectors, ph->p_vaddr);
            }
            ph++;
        }
        kernel_entry_addr = elf_header->e_entry;
    } else {
        // Load more to test if the multiboot header can be found in the first 16 sectors (8192 bytes)
        ata_lba_read(kernel_lba + 1, 15, TEST_SECTORS_ADDRESS + SECTOR_SIZE);
        if (Multiboot_Header *header = find_multiboot_header(TEST_SECTORS_ADDRESS)) {
            int sectors = bytes_to_sectors(header->load_end_addr - header->load_addr);
            ata_lba_read(kernel_lba, sectors, header->load_addr);
            // Zero out the BSS
            if (header->bss_end_addr > header->load_end_addr)
                memset((void *)header->load_end_addr, 0, header->bss_end_addr - header->load_end_addr);
            kernel_entry_addr = header->entry_addr;
        }
    }

    if (!kernel_entry_addr)
        while (true);

    return kernel_entry_addr;
}
