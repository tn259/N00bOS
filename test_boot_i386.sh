#!/bin/bash

#########################################################
# Small script to verify that the kernel boots correctly
# using gdb
#########################################################

set -xe

# Run and attach gdb then confirm we get near to the bottom of kernel_main with no issues
qemu-system-x86_64 -nographic -S -gdb tcp::12345 -hda ./bin/arch/i386/os.bin &>/dev/null &
gdb <<'EOF' > /dev/stdout
add-symbol-file ./build/arch/i386/kernelfull.o 0x100000
break kernel_main
break kernel.cpp:14
target remote localhost:12345
c
x/15i $pc
c
x/15i $pc
si
x/1i $pc
si
x/1i $pc
si
x/1i $pc
si
x/1i $pc
si
x/1i $pc
si
x/1i $pc
si
x/1i $pc
si
x/1i $pc
EOF

trap 'kill $(jobs -p)' EXIT # kill all background processes on shell exit
