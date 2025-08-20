BUILDDIR      := build
KERNEL_SRC    := kernel
KERNEL_BUILD  := $(BUILDDIR)/$(KERNEL_SRC)
BOOT_SRC      := bootloader
BOOT_BUILD    := $(BUILDDIR)/$(BOOT_SRC)
LIB_SRC       := lib
LIB_BUILD     := $(BUILDDIR)/$(LIB_SRC)
LIB_INCLUDE   := $(LIB_BUILD)/include
LIB_TARGET    := $(BUILDDIR)/lib.o

AS       := nasm
ASFLAGS  := -g -f elf32
CC       := i686-elf-g++
CFLAGS   := -c -g -O0 -ffreestanding -Wall -Wextra -Werror -fno-builtin -nostdlib -nostartfiles -nodefaultlibs -fno-rtti -fno-exceptions
CC_INCL  := -I$(KERNEL_SRC) -I$(LIB_INCLUDE)
LD       := i686-elf-ld

all: kernel.elf kernel.bin bootloader

# Lib
LIB_SOURCES := $(shell find $(LIB_SRC) -name '*.cpp')
LIB_OBJECTS := $(patsubst $(LIB_SRC)/%.cpp,$(LIB_BUILD)/%_cpp.o,$(LIB_SOURCES))

lib: $(LIB_TARGET)

$(LIB_TARGET): $(LIB_OBJECTS)
	$(LD) -r $(LIB_OBJECTS) -o $(LIB_TARGET)

$(LIB_BUILD)/%_cpp.o: $(LIB_SRC)/%.cpp lib_headers
	$(CC) $(CFLAGS) -I$(LIB_INCLUDE) $< -o $@

lib_headers:
	mkdir -p $(LIB_INCLUDE)
	cp -r $(LIB_SRC)/include/* $(LIB_INCLUDE)

# Bootloader
bootloader: first_stage.bin second_stage.bin

first_stage.bin: $(BOOT_SRC)/first_stage.s
	mkdir -p $(BOOT_BUILD)
	$(AS) -f bin $(BOOT_SRC)/first_stage.s -o $(BOOT_BUILD)/first_stage.bin

second_stage.bin: $(BOOT_SRC)/second_stage.s $(BOOT_SRC)/second_stage.cpp
	mkdir -p $(BOOT_BUILD)
	$(AS) $(ASFLAGS) $(BOOT_SRC)/second_stage.s -o $(BOOT_BUILD)/second_stage_s.o
	$(CC) $(CFLAGS) $(BOOT_SRC)/second_stage.cpp -o $(BOOT_BUILD)/second_stage_cpp.o
	$(LD) -T $(BOOT_SRC)/second_stage.ld $(BOOT_BUILD)/second_stage_s.o $(BOOT_BUILD)/second_stage_cpp.o -o $(BOOT_BUILD)/second_stage.bin

# Kernel
KERNEL_ASSES    := $(shell find $(KERNEL_SRC) -name '*.s')
KERNEL_SOURCES  := $(shell find $(KERNEL_SRC) -name '*.cpp')
KERNEL_OBJECTS  := $(patsubst $(KERNEL_SRC)/%.s,$(KERNEL_BUILD)/%_s.o,$(KERNEL_ASSES)) \
				   $(patsubst $(KERNEL_SRC)/%.cpp,$(KERNEL_BUILD)/%_cpp.o,$(KERNEL_SOURCES))

kernel.elf: $(KERNEL_OBJECTS)
	$(LD) -T link_elf.ld $(KERNEL_OBJECTS) $(LIB_TARGET) -o $(BUILDDIR)/kernel.elf

kernel.bin: $(KERNEL_OBJECTS)
	$(LD) -T link_bin.ld $(KERNEL_OBJECTS) $(LIB_TARGET) -o $(BUILDDIR)/kernel.bin

$(KERNEL_BUILD)/%_cpp.o: $(KERNEL_SRC)/%.cpp $(LIB_TARGET)
	mkdir -p $(@D)
	$(CC) $(CFLAGS) $(CC_INCL) $< -o $@

$(KERNEL_BUILD)/%_s.o: $(KERNEL_SRC)/%.s
	mkdir -p $(@D)
	$(AS) $(ASFLAGS) $< -o $@

clean:
	rm -rf $(BUILDDIR)/*
