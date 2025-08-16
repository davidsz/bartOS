echo "kernel.bin:"
grub-file --is-x86-multiboot build/kernel.bin && echo "Valid multiboot kernel"  || echo "Invalid as multiboot"
echo "kernel.elf:"
grub-file --is-x86-multiboot build/kernel.elf && echo "Valid multiboot kernel"  || echo "Invalid as multiboot"
