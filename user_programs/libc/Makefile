SRC_DIR       := src

AS            := nasm
ASFLAGS       := -f elf32
CC            := i686-elf-gcc
CFLAGS        := -c -O0 -ffreestanding -Wall -Wextra -Werror -fno-builtin -nostdlib -nostdinc -nostartfiles -nodefaultlibs -fno-exceptions
INCLUDES      := -I$(BUILDDIR)/include
LD            := i686-elf-ld

ASSES    := $(shell find $(SRC_DIR) -name '*.s')
SOURCES  := $(shell find $(SRC_DIR) -name '*.c')
OBJECTS  := $(patsubst $(SRC_DIR)/%.s,$(BUILDDIR)/%_s.o,$(ASSES)) \
		    $(patsubst $(SRC_DIR)/%.c,$(BUILDDIR)/%_c.o,$(SOURCES))

all: $(OBJECTS)
	i686-elf-ld -m elf_i386 -relocatable $(OBJECTS) -o $(BUILDDIR)/libc.elf

$(BUILDDIR)/%_c.o: $(SRC_DIR)/%.c include_headers
	mkdir -p $(@D)
	$(CC) $(CFLAGS) $(INCLUDES) $< -o $@

$(BUILDDIR)/%_s.o: $(SRC_DIR)/%.s include_headers
	mkdir -p $(@D)
	$(AS) $(ASFLAGS) $(INCLUDES) $< -o $@

include_headers:
	mkdir -p $(BUILDDIR)/include
	cp -r include $(BUILDDIR)/

clean:
	rm -rf $(BUILDDIR)
