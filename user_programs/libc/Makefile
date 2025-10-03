SRC_DIR       := src

AS            := nasm
ASFLAGS       := -f elf32
CC            := i686-elf-g++
CFLAGS        := -c -O0 -ffreestanding -Wall -Wextra -Werror -fno-builtin -nostdlib -nostartfiles -nodefaultlibs -fno-rtti -fno-exceptions
LD            := i686-elf-ld

ASSES    := $(shell find $(SRC_DIR) -name '*.s')
SOURCES  := $(shell find $(SRC_DIR) -name '*.c')
OBJECTS  := $(patsubst $(SRC_DIR)/%.s,$(BUILDDIR)/%_s.o,$(ASSES)) \
		    $(patsubst $(SRC_DIR)/%.c,$(BUILDDIR)/%_c.o,$(SOURCES))

all: $(OBJECTS)
	mkdir -p $(BUILDDIR)/include
	cp -r include $(BUILDDIR)/
	i686-elf-ld -m elf_i386 -relocatable $(OBJECTS) -o $(BUILDDIR)/libc.elf

$(BUILDDIR)/%_cpp.o: $(SRC_DIR)/%.cpp
	mkdir -p $(@D)
	$(CC) $(CFLAGS) $< -o $@

$(BUILDDIR)/%_s.o: $(SRC_DIR)/%.s
	mkdir -p $(@D)
	$(AS) $(ASFLAGS) $< -o $@

clean:
	rm -rf $(BUILDDIR)
