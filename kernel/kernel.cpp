#include "tty.h"
#include "kernel.h"

void kernel_main() {
    terminal_initialise();
    terminal_set_colour(10);
    terminal_write("Welcome to N00bOS!\n");
} 
