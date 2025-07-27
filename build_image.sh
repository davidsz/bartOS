#!/bin/bash

ARGS="--disc [cd|hd] --bootloader [custom|grub] --kernel [elf|bin]"
DISC_FORMAT=""
BOOTLOADER=""
KERNEL_FORMAT=""

BUILDDIR="build"
KERNEL_FILE=""
HD_IMG="$BUILDDIR/bartos.bin"
CD_IMG="$BUILDDIR/bartos.iso"

usage() {
    echo "Usage: $0 $ARGS"
    exit 1
}

while [[ $# -gt 0 ]]; do
    case "$1" in
        --disc)
            DISC_FORMAT="$2"
            shift 2
            ;;
        --bootloader)
            BOOTLOADER="$2"
            shift 2
            ;;
        --kernel)
            KERNEL_FORMAT="$2"
            shift 2
            ;;
        --help)
            usage
            ;;
        *)
            echo "Unknown argument: $1"
            usage
            ;;
    esac
done

if [[ -z "$DISC_FORMAT" || -z "$BOOTLOADER" || -z "$KERNEL_FORMAT" ]]; then
    echo "Error - All arguments are mandatory: $ARGS"
fi

if [[ "$BOOTLOADER" == "grub" && "$DISC_FORMAT" == "hd" ]]; then
    echo "Error - The GRUB on HDD case is not yet implemented / tested."
    exit 1
fi

make clean
mkdir -p $BUILDDIR

if [[ "$BOOTLOADER" == "custom" ]]; then
    make bootloader.bin
elif [[ "$BOOTLOADER" == "grub" ]]; then
    :
else
    echo "Error - Invalid argument."
    usage
fi

if [[ "$KERNEL_FORMAT" == "elf" ]]; then
    make kernel.elf
    KERNEL_FILE="$BUILDDIR/kernel.elf"
elif [[ "$KERNEL_FORMAT" == "bin" ]]; then
    make kernel.bin
    KERNEL_FILE="$BUILDDIR/kernel.bin"
else
    echo "Error - Invalid argument."
    usage
fi

if [[ "$DISC_FORMAT" == "cd" ]]; then
    cp -R iso $BUILDDIR/iso
	cp $KERNEL_FILE $BUILDDIR/iso/boot/kernel.bin
	mkisofs -R                                               \
		-b boot/grub/stage2_eltorito                         \
		-no-emul-boot                                        \
		-boot-load-size 4                                    \
		-boot-info-table                                     \
		-o $CD_IMG                                           \
		$BUILDDIR/iso
	echo "Image is done: $CD_IMG"
elif [[ "$DISC_FORMAT" == "hd" ]]; then
    rm -rf "$HD_IMG"
	dd if=$BUILDDIR/bootloader.bin >> "$HD_IMG"
	dd if=$KERNEL_FILE >> "$HD_IMG"
	dd if=/dev/zero bs=512 count=100 >> "$HD_IMG"
	echo "Image is done: $HD_IMG"
else
    echo "Error - Invalid argument."
    usage
fi
