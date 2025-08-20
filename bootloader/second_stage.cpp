#include <stdint.h>

volatile uint16_t* const VGA = (uint16_t*)0xB8000;
const int WIDTH = 80;
const int HEIGHT = 25;

void clear_screen(uint8_t color = 0x07) {
    uint16_t blank = ' ' | (color << 8);
    for (int i = 0; i < WIDTH * HEIGHT; i++) {
        VGA[i] = blank;
    }
}

void put_char(int x, int y, char c, uint8_t color = 0x07) {
    if (x < 0 || x >= WIDTH || y < 0 || y >= HEIGHT) return;
    VGA[y * WIDTH + x] = c | (color << 8);
}

void print_string(int x, int y, const char* str, uint8_t color = 0x07) {
    int cx = x;
    int cy = y;
    while (*str) {
        if (*str == '\n') {
            cx = 0;
            cy++;
        } else {
            put_char(cx, cy, *str, color);
            cx++;
            if (cx >= WIDTH) {
                cx = 0;
                cy++;
            }
        }
        str++;
    }
}

extern "C" int second_stage_cpp(int kernel_lba)
{
    clear_screen();
    print_string(0, 0, "Hello from the second stage!");

    for (int i = 1; i <= kernel_lba; i++)
        print_string(i, i, "x");

    // TODO:
    // - Load the first sector of the kernel
    // - Validate if its multiboot compatible
    // - Determine its format: raw or ELF
    // - ELF: Iterate the program headers and load PT_LOAD segments
    //     - Learn if there is more to do according to the multiboot specs
    // - Raw: Parse the multiboot header, load the segments
    // - Collect boot information and ensure the proper machine state
    //     - https://www.gnu.org/software/grub/manual/multiboot/multiboot.html
    // - Jump to the kernel entry point

    while (true);
    return 0;
}
