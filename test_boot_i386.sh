#!/bin/bash

#########################################################
# Small script to verify that the kernel boots correctly
# using gdb
#########################################################

set -xe

# Run and attach gdb then print the first 10 instructions from the start of the kernel
qemu-system-x86_64 -nographic -S -gdb tcp::12345 -hda ./bin/arch/i386/os.bin &>/dev/null &
gdb <<'EOF' > /dev/stdout
add-symbol-file ./build/arch/i386/kernelfull.o 0x100000
break _start
target remote localhost:12345
c
x/10i $pc
EOF

trap 'kill $(jobs -p)' EXIT # kill all background processes on shell exit
