#include "kernel.h"

#include "arch/i386/idt.h"
#include "arch/i386/io.h"
#include "memory/heap/kheap.h"
#include "tty.h"

extern "C" void div_zero();

void kernel_main() {
    terminal_initialise();
    terminal_set_colour(10); // NOLINT(readability-magic-numbers,cppcoreguidelines-avoid-magic-numbers)
    terminal_write("Welcome to N00bOS\n");
    idt_init();
    enable_interrupts();
    kheap_init();

    // test heap
    if (kheap_assert_all_free()) terminal_write("Pre kmalloc - ALL FREE\n");
    char* c = (char*)kmalloc(sizeof(char[5000])); // should allocate two blocks
    if (!kheap_assert_all_free()) terminal_write("Post kmalloc - c is allocated\n");
    *c = 'C';
    kfree(c);
    if (kheap_assert_all_free()) terminal_write("Post kfree - NOTHING ALLOCATED\n");
}
