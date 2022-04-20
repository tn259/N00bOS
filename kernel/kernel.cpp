#include "kernel.h"

#include "arch/i386/idt.h"
#include "arch/i386/io.h"
#include "arch/i386/paging/paging.h"
#include "memory/heap/kheap.h"
#include "tty.h"

extern "C" void div_zero();

namespace {
paging_chunk* kernel_paging_chunk;
} // anonymous namespace

void kernel_main() {
    terminal_initialise();
    terminal_set_colour(10); // NOLINT(readability-magic-numbers,cppcoreguidelines-avoid-magic-numbers)
    terminal_write("Welcome to N00bOS\n");

    idt_init();

    kheap_init();

    kernel_paging_chunk = paging_new(PAGING_IS_WRITABLE | PAGING_IS_PRESENT | PAGING_ACCESS_FROM_ALL);
    paging_switch(kernel_paging_chunk);
    enable_paging();
    enable_interrupts();

    // test heap
    if (kheap_assert_all_free()) {
        terminal_write("Pre kmalloc - ALL FREE\n");
    }
    // we should allocate two blocks here
    char* c = static_cast<char*>(kmalloc(sizeof(char[5000]))); // NOLINT(readability-magic-numbers,cppcoreguidelines-avoid-magic-numbers)
    if (!kheap_assert_all_free()) {
        terminal_write("Post kmalloc - c is allocated\n");
    }
    *c = 'C';
    kfree(c);
    if (kheap_assert_all_free()) {
        terminal_write("Post kfree - NOTHING ALLOCATED\n");
    }
}
