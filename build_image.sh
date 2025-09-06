#!/bin/bash
# Terminate on errors
set -e

ARGS="--bootloader [custom|grub] --kernel [elf|bin]"
BOOTLOADER=""
KERNEL_FORMAT=""

BUILDDIR="build"
KERNEL_FILE=""
IMG="$BUILDDIR/bartos.img"

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
    rm -rf "$IMG"
    make bootloader

    # first_stage.bin is already 512 bytes
    dd if=$BUILDDIR/bootloader/first_stage.bin >> "$IMG"

    # Pad second_stage.bin to 512 bytes
    size=$(stat -c%s $BUILDDIR/bootloader/second_stage.bin)
    pad=$(( (512 - size % 512) % 512 ))
    if [ "$pad" -ne 0 ]; then
      dd if=/dev/zero bs=1 count=$pad >> $BUILDDIR/bootloader/second_stage.bin
    fi
    dd if=$BUILDDIR/bootloader/second_stage.bin >> "$IMG"

    # Kernel
    dd if=$KERNEL_FILE >> "$IMG"
    dd if=/dev/zero bs=1048576 count=16 >> "$IMG"
    echo "Image is done: $IMG"

elif [[ "$BOOTLOADER" == "grub" ]]; then
    # Add the GRUB config and the kernel
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

    # An additional file for testing
    mkdir -p $BUILDDIR/iso/home/user/
    cat <<EOF > $BUILDDIR/iso/home/user/file.txt
This is example data for testing.
EOF

    # Compose the image
    grub-mkrescue -o $IMG $BUILDDIR/iso/
    echo "Image is done: $IMG"

else
    echo "Error - Invalid argument."
    usage
fi
