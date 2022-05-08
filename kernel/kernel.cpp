#include "kernel.h"

#include "arch/i386/idt.h"
#include "arch/i386/io.h"
#include "arch/i386/paging/paging.h"
#include "drivers/ata/ata.h"
#include "disk/disk_streamer.h"
#include "mm/heap/kheap.h"
#include "fs/path_parser.h" 
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

    disk_streamer::disk_stream* ds = disk_streamer::new_stream(0);

    char buf[514];
    disk_streamer::seek(ds, 0x1ff); // 500
    disk_streamer::read(ds, buf, 514); // 550

    enable_interrupts();
}