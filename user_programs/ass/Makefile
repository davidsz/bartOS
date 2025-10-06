TARGET_NAME   := ass.elf
SRC_DIR       := .

AS            := nasm
ASFLAGS       := -f elf

SOURCES  := $(shell find $(SRC_DIR) -name '*.s')
OBJECTS  := $(patsubst $(SRC_DIR)/%.s,$(BUILDDIR)/%_s.o,$(SOURCES))

all: $(OBJECTS)
	mkdir -p $(BUILDDIR)
	i686-elf-gcc -g -T ./link.ld -o $(BUILDDIR)/$(TARGET_NAME) -ffreestanding -O0 -nostdlib -fpic -g $(OBJECTS)

$(BUILDDIR)/%_s.o: $(SRC_DIR)/%.s
	mkdir -p $(@D)
	$(AS) $(ASFLAGS) $< -o $@

clean:
	rm -rf $(BUILDDIR)
