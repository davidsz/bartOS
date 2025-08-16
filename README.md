# bartOS
Das ist my hobby operating system project where I learn low level development. It contains a custom simple bootloader to have a glance at the early stage of the boot process of different binary formats, but GRUB is also supported.

## Setup Docker environment for building
```
docker build --platform=linux/amd64 -t build-bartos:1.0 .

docker run -it --rm --name=build-bartos --platform=linux/amd64--mount=type=bind,source=${PWD},target=/project --workdir=/project build-bartos:1.0 bash
```

## Build parts of the project
```
# Build the kernel in raw binary or ELF format
make kernel.bin
make kernel.elf

# Make the bootloader individually
make bootloader.bin
```

## Disc image creator script
Alternative way to build the project and compose the disc image.
```
./build_image.sh --bootloader [custom|grub] --kernel [elf|bin]
```

## Start the project in emulator

QEMU:
```
# HD image
qemu-system-x86_64 -serial stdio -hda ./build/bartos.iso
```
