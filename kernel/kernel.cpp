#include "kernel.h"

#include "arch/i386/idt.h"
#include "arch/i386/io.h"
#include "tty.h"

extern "C" void div_zero();

void kernel_main() {
    terminal_initialise();
    terminal_set_colour(15); // NOLINT(readability-magic-numbers,cppcoreguidelines-avoid-magic-numbers)
    //terminal_write("T");
    //idt_init();
    //enable_interrupts();
}
