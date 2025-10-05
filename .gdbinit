add-symbol-file ./build/kernel_debug.o 0x100000
target remote | qemu-system-i386 -hda ./build/bartos.img -S -gdb stdio
