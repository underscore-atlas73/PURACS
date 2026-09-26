#!/bin/sh
grub-mkrescue -o os.iso iso/ --modules="boot iso9660 multiboot2 normal" # specifying the modules to exactly what we need allows for ridiculous memory constraints (tested as low as 1.9M) 
qemu-system-x86_64 -cdrom os.iso -m 4M
