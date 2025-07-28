SRCDIR   := src
BUILDDIR := build
BOOTSRC  := bootloader

AS       := nasm
ASFLAGS  := -g -f elf32
CC       := i686-elf-g++
CFLAGS   := -g -O0 -Isrc -ffreestanding -fno-builtin -Wall -Wextra -Werror -nostdlib -nostartfiles -nodefaultlibs
LD       := i686-elf-ld

ASSES    := $(wildcard $(SRCDIR)/*.s) $(wildcard $(SRCDIR)/**/*.s)
SOURCES  := $(wildcard $(SRCDIR)/*.cpp) $(wildcard $(SRCDIR)/**/*.cpp)
OBJECTS  := $(patsubst $(SRCDIR)/%.s,$(BUILDDIR)/%_s.o,$(ASSES)) \
			$(patsubst $(SRCDIR)/%.cpp,$(BUILDDIR)/%_cpp.o,$(SOURCES))

all: kernel.elf kernel.bin bootloader.bin

kernel.elf: $(OBJECTS)
	$(LD) -T link_elf.ld $(OBJECTS) -o $(BUILDDIR)/kernel.elf

kernel.bin: $(OBJECTS)
	$(LD) -T link_bin.ld $(OBJECTS) -o $(BUILDDIR)/kernel.bin

bootloader.bin: $(BOOTSRC)/bootloader.s
	nasm -f bin $(BOOTSRC)/bootloader.s -o $(BUILDDIR)/bootloader.bin

$(BUILDDIR)/%_cpp.o: $(SRCDIR)/%.cpp
	mkdir -p $(@D)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILDDIR)/%_s.o: $(SRCDIR)/%.s
	mkdir -p $(@D)
	$(AS) $(ASFLAGS) $< -o $@

clean:
	rm -rf $(BUILDDIR)/*
