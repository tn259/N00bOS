#include "kernel.h"

#include "arch/i386/idt.h"
#include "arch/i386/io.h"
#include "arch/i386/paging/paging.h"
#include "drivers/ata/ata.h"
#include "mm/heap/kheap.h"
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
    //auto* ptr = (char*)kzalloc(4096);                                                                                                                     // NOLINT
    //paging_set(kernel_paging_chunk->directory_entry, (void*)0x1000, (PAGING_ENTRY)ptr | PAGING_ACCESS_FROM_ALL | PAGING_IS_PRESENT | PAGING_IS_WRITABLE); // NOLINT
    enable_paging();

    disk::search_and_initialize();
    disk::disk* d = disk::get(0);

    char buf[512];
    disk::read_block(d, 0, 1, &buf[0]);

    enable_interrupts();
}