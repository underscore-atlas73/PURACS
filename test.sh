#!/bin/sh
./clean.sh
./build-env.sh
./build.sh

if [ "$1" = "limine" ]; then
    ./lim_iso.sh
    ./lim_run.sh
fi

if [ "$1" = "grub" ] || [ $# -eq 0 ]; then
    ./iso.sh
    ./run.sh
fi
