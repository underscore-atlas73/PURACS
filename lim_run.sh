#!/bin/sh
xorriso -as mkisofs -R -r -J -b boot/LIMINE/limine-bios-cd.bin \
        -no-emul-boot -boot-load-size 4 -boot-info-table -hfsplus \
        -apm-block-size 2048 --efi-boot boot/LIMINE/limine-uefi-cd.bin \
        -efi-boot-part --efi-boot-image --protective-msdos-label \
        iso -o os.iso
qemu-system-x86_64 -cdrom os.iso -m 1.5M

