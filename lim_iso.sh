#!/bin/sh
mkdir -p iso/boot/LIMINE
mkdir -p iso/EFI/BOOT/
cp LIMINE/BOOTX64.EFI iso/EFI/BOOT/
cp LIMINE/BOOTIA32.EFI iso/EFI/BOOT/
cp limine.conf iso/boot/LIMINE/
cp LIMINE/limine-bios.sys iso/boot/LIMINE/
cp LIMINE/limine-bios-cd.bin iso/boot/LIMINE/
cp LIMINE/limine-uefi-cd.bin iso/boot/LIMINE/

cp source/kernel.elf iso/boot/
