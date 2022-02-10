#include "kernel.h"

#include "arch/i386/idt.h"
#include "tty.h"

extern "C" void div_zero();

void kernel_main() {
    terminal_initialise();
    terminal_set_colour(10); // NOLINT(readability-magic-numbers,cppcoreguidelines-avoid-magic-numbers)
    terminal_write("Welcome to N00bOS!\n");
    idt_init();
    div_zero();
}
