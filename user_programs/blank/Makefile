all:
	mkdir -p $(BUILDDIR)
	nasm -f elf ./blank.s -o $(BUILDDIR)/blank.o
	i686-elf-gcc -g -T ./link.ld -o  $(BUILDDIR)/blank.bin -ffreestanding -O0 -nostdlib -fpic -g  $(BUILDDIR)/blank.o

clean:
	rm -rf $(BUILDDIR)/blank.o
