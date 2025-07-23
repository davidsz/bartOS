SRCDIR   := src
BUILDDIR := build
# CD-ROM with GRUB included
CD_IMG   := $(BUILDDIR)/bartos.iso
BOOTSRC  := bootloader
# Hard disk image with the custom bootloader
HD_IMG   := $(BUILDDIR)/bartos.bin

AS       := nasm
ASFLAGS  := -g -f elf32
CC       := i686-elf-g++
CFLAGS   := -g -O0 -ffreestanding -fno-builtin -Wall -Wextra -Werror -nostdlib -nostartfiles -nodefaultlibs
LD       := i686-elf-ld
LDFLAGS  := -T link.ld

ASSES    := $(wildcard $(SRCDIR)/*.s) $(wildcard $(SRCDIR)/**/*.s)
SOURCES  := $(wildcard $(SRCDIR)/*.cpp) $(wildcard $(SRCDIR)/**/*.cpp)
OBJECTS  := $(patsubst $(SRCDIR)/%.s,$(BUILDDIR)/%_s.o,$(ASSES)) \
			$(patsubst $(SRCDIR)/%.cpp,$(BUILDDIR)/%_cpp.o,$(SOURCES))

all: $(CD_IMG) $(HD_IMG)
boot-grub: $(CD_IMG)
boot-custom: $(HD_IMG)

$(CD_IMG): kernel.elf
	cp -R iso $(BUILDDIR)/iso
	cp $(BUILDDIR)/kernel.elf $(BUILDDIR)/iso/boot/kernel.elf
	mkisofs -R                                                  \
		-b boot/grub/stage2_eltorito                            \
		-no-emul-boot                                           \
		-boot-load-size 4                                       \
		-boot-info-table                                        \
		-o $(CD_IMG)                                               \
		$(BUILDDIR)/iso

kernel.elf: $(OBJECTS)
	$(LD) $(LDFLAGS) $(OBJECTS) -o $(BUILDDIR)/kernel.elf

$(HD_IMG): kernel.bin bootloader.bin
	rm -rf $(HD_IMG)
	dd if=$(BUILDDIR)/bootloader.bin >> $(HD_IMG)
	dd if=$(BUILDDIR)/kernel.bin >> $(HD_IMG)
	dd if=/dev/zero bs=512 count=100 >> $(HD_IMG)

bootloader.bin: $(BOOTSRC)/bootloader.s
	$(AS) -f bin $(BOOTSRC)/bootloader.s -o $(BUILDDIR)/bootloader.bin

kernel.bin: $(OBJECTS)
	$(LD) -g -relocatable $(OBJECTS) -o $(BUILDDIR)/kernelfull.o
	$(CC) -g -T link_bin.ld $(CFLAGS) $(BUILDDIR)/kernelfull.o -o $(BUILDDIR)/kernel.bin

$(BUILDDIR)/%_cpp.o: $(SRCDIR)/%.cpp
	mkdir -p $(@D)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILDDIR)/%_s.o: $(SRCDIR)/%.s
	mkdir -p $(@D)
	$(AS) $(ASFLAGS) $< -o $@

clean:
	rm -rf $(BUILDDIR)/*
