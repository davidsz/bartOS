SRC_DIR       := .

CC            := i686-elf-gcc
INCLUDES      := $(BUILDDIR)/../libc/include
CFLAGS        := -I$(INCLUDES) -std=gnu99 -nostdlib -c

SOURCES  := $(shell find $(SRC_DIR) -name '*.c')
OBJECTS  := $(patsubst $(SRC_DIR)/%.c,$(BUILDDIR)/%_c.o,$(SOURCES))

all: $(OBJECTS)
	mkdir -p $(BUILDDIR)
	i686-elf-gcc -g -T ./link.ld -o $(BUILDDIR)/blank.elf -ffreestanding -O0 -nostdlib -fpic -g $(OBJECTS) $(BUILDDIR)/../libc/libc.elf

$(BUILDDIR)/%_c.o: $(SRC_DIR)/%.c
	mkdir -p $(@D)
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm -rf $(BUILDDIR)
