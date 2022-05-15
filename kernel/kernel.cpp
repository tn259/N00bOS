#include "kernel.h"

#include "arch/i386/tty.h"
#include "arch/i386/idt.h"
#include "arch/i386/io.h"
#include "arch/i386/paging/paging.h"
#include "drivers/ata/ata.h"
#include "disk/disk.h"
#include "disk/disk_streamer.h"
#include "mm/heap/kheap.h"
#include "fs/path_parser.h" 

extern "C" void div_zero();

namespace ARCH = arch::i386;

namespace {
ARCH::paging::paging_chunk* kernel_paging_chunk;
} // anonymous namespace

void kernel_main() {
    ARCH::terminal_initialise();
    ARCH::terminal_set_colour(10); // NOLINT(readability-magic-numbers,cppcoreguidelines-avoid-magic-numbers)
    ARCH::terminal_write("Welcome to N00bOS\n");

    ARCH::idt::idt_init();

    mm::heap::kheap_init();

    kernel_paging_chunk = ARCH::paging::paging_new(PAGING_IS_WRITABLE | PAGING_IS_PRESENT | PAGING_ACCESS_FROM_ALL);
    ARCH::paging::paging_switch(kernel_paging_chunk);
    //auto* ptr = (char*)kzalloc(4096);                                                                                                                     // NOLINT
    //paging_set(kernel_paging_chunk->directory_entry, (void*)0x1000, (PAGING_ENTRY)ptr | PAGING_ACCESS_FROM_ALL | PAGING_IS_PRESENT | PAGING_IS_WRITABLE); // NOLINT
    enable_paging();

    disk::search_and_initialize();

    disk::streamer::disk_stream* ds = disk::streamer::new_stream(0);

    char buf[514];
    disk::streamer::seek(ds, 0x1ff); // 500
    disk::streamer::read(ds, buf, 514); // 550

    enable_interrupts();
}