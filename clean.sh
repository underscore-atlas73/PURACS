#!/bin/sh
#set -x
find . -type f \( -iname "*.o" -o -iname "*.elf" -o -iname "*.out" \) -delete
rm -R build-sysroot/
rm -R iso/
rm os.iso
