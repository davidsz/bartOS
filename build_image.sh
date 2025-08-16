#!/bin/bash

ARGS="--bootloader [custom|grub] --kernel [elf|bin]"
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

if [[ -z "$BOOTLOADER" || -z "$KERNEL_FORMAT" ]]; then
    echo "Error - All arguments are mandatory: $ARGS"
fi

make clean
mkdir -p $BUILDDIR

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

if [[ "$BOOTLOADER" == "custom" ]]; then
    make bootloader.bin
    rm -rf "$HD_IMG"
    dd if=$BUILDDIR/bootloader/bootloader.bin >> "$HD_IMG"
    dd if=$KERNEL_FILE >> "$HD_IMG"
    dd if=/dev/zero bs=512 count=100 >> "$HD_IMG"
    echo "Image is done: $HD_IMG"

elif [[ "$BOOTLOADER" == "grub" ]]; then
    mkdir -p $BUILDDIR/iso/boot/grub
    cat <<EOF > $BUILDDIR/iso/boot/grub/grub.cfg
set timeout=0
set default=0

menuentry "BartOS" {
    multiboot /boot/kernel.bin
    boot
}
EOF
    cp $KERNEL_FILE $BUILDDIR/iso/boot/kernel.bin
    grub-mkrescue -o $CD_IMG $BUILDDIR/iso/
    echo "Image is done: $CD_IMG"

else
    echo "Error - Invalid argument."
    usage
fi
