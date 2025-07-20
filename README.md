# bartOS
My hobby operating system project where I learn low level development.

## Setup Docker environment and build the project
```
docker build --platform=linux/amd64 -t build-bartos:1.0 .

docker run -it --rm --name=build-bartos --platform=linux/amd64--mount=type=bind,source=${PWD},target=/project --workdir=/project build-bartos:1.0 bash

make
```

## Start project in emulator (outside of Docker)

Bochs (configured for Mac):
```
bochs -f bochsrc_mac.txt -q
```
QEMU:
```
qemu-system-x86_64 -cdrom .build/bartos.iso
```
