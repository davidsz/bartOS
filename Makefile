SRCDIR   := src
BUILDDIR := build
ISO      := $(BUILDDIR)/bartos.iso
BOOTSRC  := bootloader
RAWIMG   := $(BUILDDIR)/bartos.bin

CC       := i686-elf-g++
CFLAGS   := -c -ffreestanding -Wall -Wextra -fno-exceptions -fno-rtti
LD       := i686-elf-g++
LDFLAGS  := -T link.ld -ffreestanding -nostdlib -lgcc
AS       := nasm
ASFLAGS  := -f elf32 -g

ASSES    := $(wildcard $(SRCDIR)/*.s) $(wildcard $(SRCDIR)/**/*.s)
SOURCES  := $(wildcard $(SRCDIR)/*.cpp) $(wildcard $(SRCDIR)/**/*.cpp)
OBJECTS  := $(patsubst $(SRCDIR)/%.s,$(BUILDDIR)/%_s.o,$(ASSES)) \
			$(patsubst $(SRCDIR)/%.cpp,$(BUILDDIR)/%_cpp.o,$(SOURCES))

all: $(ISO) $(RAWIMG)
boot-grub: $(ISO)
boot-custom: $(RAWIMG)

$(ISO): kernel.elf
	cp -R iso $(BUILDDIR)/iso
	cp $(BUILDDIR)/kernel.elf $(BUILDDIR)/iso/boot/kernel.elf
	mkisofs -R                                                  \
		-b boot/grub/stage2_eltorito                            \
		-no-emul-boot                                           \
		-boot-load-size 4                                       \
		-boot-info-table                                        \
		-o $(ISO)                                               \
		$(BUILDDIR)/iso

$(RAWIMG): kernel.elf bootloader.bin
	dd if=$(BUILDDIR)/bootloader.bin >> $(RAWIMG)
	dd if=$(BUILDDIR)/kernel.elf >> $(RAWIMG)
	dd if=/dev/zero bs=1048576 count=16 >> $(RAWIMG)

bootloader.bin: $(BOOTSRC)/bootloader.s
	$(AS) -f bin $(BOOTSRC)/bootloader.s -o $(BUILDDIR)/bootloader.bin

kernel.elf: $(OBJECTS)
	$(LD) $(LDFLAGS) $(OBJECTS) -o $(BUILDDIR)/kernel.elf

$(BUILDDIR)/%_cpp.o: $(SRCDIR)/%.cpp
	mkdir -p $(@D)
	$(CC) $(CFLAGS) $< -o $@

$(BUILDDIR)/%_s.o: $(SRCDIR)/%.s
	mkdir -p $(@D)
	$(AS) $(ASFLAGS) $< -o $@

clean:
	rm -rf $(BUILDDIR)/*
