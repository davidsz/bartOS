SRCDIR   := src
BUILDDIR := build
ISO      := $(BUILDDIR)/bartos.iso

CC       := i686-elf-g++
CFLAGS   := -c -ffreestanding -Wall -Wextra -fno-exceptions -fno-rtti
LD       := i686-elf-g++
LDFLAGS  := -T link.ld -ffreestanding -nostdlib -lgcc
AS       := i686-elf-as
ASFLAGS  :=

ASSES    := $(wildcard $(SRCDIR)/*.s) $(wildcard $(SRCDIR)/**/*.s)
SOURCES  := $(wildcard $(SRCDIR)/*.cpp) $(wildcard $(SRCDIR)/**/*.cpp)
OBJECTS  := $(patsubst $(SRCDIR)/%.s,$(BUILDDIR)/%.o,$(ASSES)) \
			$(patsubst $(SRCDIR)/%.cpp,$(BUILDDIR)/%.o,$(SOURCES))

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
	$(LD) $(LDFLAGS) $(OBJECTS) -o $(BUILDDIR)/kernel.elf

$(BUILDDIR)/%.o: $(SRCDIR)/%.cpp
	mkdir -p $(@D)
	$(CC) $(CFLAGS) $< -o $@

$(BUILDDIR)/%.o: $(SRCDIR)/%.s
	mkdir -p $(@D)
	$(AS) $(ASFLAGS) $< -o $@

clean:
	rm -rf $(BUILDDIR)/*
