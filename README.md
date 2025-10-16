# bartOS
Das ist my hobby operating system project where I learn low level development. It contains a custom simple bootloader to have a glance at the early stage of the boot process; and an even more simpler libc implementation to be able to develop C programs on the top of these.

## Setup Docker environment for building
```
# Will build a custom GCC for the project
docker build --platform=linux/amd64 -t build-bartos:1.0 .

# Specifying the platform is needed on systems like Mac with ARM CPU
docker run -it --rm --privileged --name=build-bartos --platform=linux/amd64 --mount=type=bind,source=${PWD},target=/project --workdir=/project build-bartos:1.0 bash
```

## Build parts of the project
```
# Build the kernel in ELF format
make kernel.elf

# Make the bootloader individually
make bootloader
```

## Disc image creator script
Alternative way to build the project and compose the disc image.
```
./build_image.sh --bootloader custom --kernel elf
```

## Start the project in emulator

QEMU:
```
qemu-system-x86_64 -serial stdio -hda ./build/bartos.img
```
