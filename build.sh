#!/bin/sh
COMPILER_DEFINES="-D__is_libk"
SYSROOT_FLAG="-I ../build-sysroot/usr/include/"
COMPILER_FLAGS="-ffreestanding -Wall -Wextra -mno-red-zone $SYSROOT_FLAG $COMPILER_DEFINES"
LINKER_FLAGS="-nostdlib"

cd source || exit

pwd
x86_64-elf-gcc -c -x assembler $(find . -path "include" -prune -o -iname "*.asm" -print) -I include/
x86_64-elf-gcc $COMPILER_FLAGS -c $(find . -path "include" -prune -o -iname "*.c" -print) -I include/
x86_64-elf-ld $LINKER_FLAGS -T linker.ld $(find . -name "*.o") -o kernel.elf
