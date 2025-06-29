grub-file --is-x86-multiboot  build/iso/boot/kernel.elf && echo "Valid multiboot kernel"  || echo "Invalid as multiboot"
grub-file --is-x86-multiboot2 build/iso/boot/kernel.elf && echo "Valid multiboot2 kernel" || echo "Invalid as multiboot2"
