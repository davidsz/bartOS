symbol-file ./build/kernel.sym
target remote | qemu-system-i386 -hda ./build/bartos.img -S -gdb stdio
