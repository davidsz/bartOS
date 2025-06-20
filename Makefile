SRCDIR   := src
BUILDDIR := build
ISO      := $(BUILDDIR)/bartos.iso

CC       := g++
CFLAGS   := -m32 -nostdlib -nostdinc -fno-builtin -fno-stack-protector \
            -nostartfiles -nodefaultlibs -Wall -Wextra -Werror -c
LDFLAGS  := -T link.ld -melf_i386
AS       := nasm
ASFLAGS  := -f elf32

LOADER   := $(SRCDIR)/loader.s
SOURCES  := $(wildcard $(SRCDIR)/*.cpp) $(wildcard $(SRCDIR)/**/*.cpp)
OBJECTS  := $(patsubst $(SRCDIR)/%.cpp,$(BUILDDIR)/%.o,$(SOURCES)) \
			$(patsubst $(SRCDIR)/%.s,$(BUILDDIR)/%.o,$(LOADER))

all: $(ISO)

$(ISO): kernel.elf
	cp -R iso $(BUILDDIR)/iso
	mv $(BUILDDIR)/kernel.elf $(BUILDDIR)/iso/boot/kernel.elf
	mkisofs -R                                                  \
		-b boot/grub/stage2_eltorito                            \
		-no-emul-boot                                           \
		-boot-load-size 4                                       \
		-boot-info-table                                        \
		-o $(ISO)                                               \
		$(BUILDDIR)/iso

kernel.elf: $(OBJECTS)
	ld $(LDFLAGS) $(OBJECTS) -o $(BUILDDIR)/kernel.elf

$(BUILDDIR)/%.o: $(SRCDIR)/%.cpp build
	$(CC) $(CFLAGS) $< -o $@

$(BUILDDIR)/%.o: $(SRCDIR)/%.s build
	$(AS) $(ASFLAGS) $< -o $@

build:
	mkdir -p $(BUILDDIR)

clean:
	rm -rf $(BUILDDIR)/*
