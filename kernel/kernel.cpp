#include "kernel.h"

#include "arch/i386/gdt/gdt.h"
#include "arch/i386/idt/idt.h"
#include "arch/i386/io/io.h"
#include "arch/i386/paging/paging.h"
#include "arch/i386/tty.h"
#include "disk/disk.h"
#include "disk/disk_streamer.h"
#include "drivers/ata/ata.h"
#include "fs/file.h"
#include "fs/path_parser.h"
#include "libc/stdlib.h"
#include "libc/string.h"
#include "mm/heap/kheap.h"

extern "C" void div_zero();

namespace ARCH = arch::i386;

namespace {

ARCH::paging::paging_chunk* kernel_paging_chunk;

ARCH::gdt::gdt_t gdt_real[GDT_TOTAL_SEGMENTS];
ARCH::gdt::structured_gdt_t structured_gdt[GDT_TOTAL_SEGMENTS] = {
    {.base = 0x00, .limit = 0x0, .access = 0x0},         // NULL segment
    {.base = 0x00, .limit = 0xffffffff, .access = 0x9a}, // Kernel code segment (executable)
    {.base = 0x00, .limit = 0xffffffff, .access = 0x92}};

} // anonymous namespace

void panic(const char* message) {
    ARCH::terminal_set_colour(4); // red
    ARCH::terminal_write(message);
    while (true) {
    }
}

void kernel_main() {
    ARCH::terminal_initialise();
    ARCH::terminal_set_colour(10); // NOLINT(readability-magic-numbers,cppcoreguidelines-avoid-magic-numbers)
    ARCH::terminal_write("Welcome to N00bOS\n");

    // load the global descriptor table
    memset(gdt_real, 0x00, sizeof(gdt_real));
    //ARCH::gdt::structured_gdt_to_gdt(static_cast<ARCH::gdt::structured_gdt_t*>(structured_gdt), static_cast<ARCH::gdt::gdt_t*>(gdt_real), GDT_TOTAL_SEGMENTS);
    auto* gdt_ptr            = &gdt_real[0];
    auto* structured_gdt_ptr = &structured_gdt[0];
    ARCH::gdt::structured_gdt_to_gdt(structured_gdt_ptr, gdt_ptr, GDT_TOTAL_SEGMENTS);
    gdt_load(gdt_ptr, sizeof(gdt_real));

    ARCH::idt::idt_init();

    mm::heap::kheap_init();

    fs::init();

    kernel_paging_chunk = ARCH::paging::paging_new(ARCH::paging::PAGING_IS_WRITABLE | ARCH::paging::PAGING_IS_PRESENT | ARCH::paging::PAGING_ACCESS_FROM_ALL);
    ARCH::paging::paging_switch(kernel_paging_chunk);
    //auto* ptr = (char*)kzalloc(4096);                                                                                                                     // NOLINT
    //paging_set(kernel_paging_chunk->directory_entry, (void*)0x1000, (PAGING_ENTRY)ptr | PAGING_ACCESS_FROM_ALL | PAGING_IS_PRESENT | PAGING_IS_WRITABLE); // NOLINT
    enable_paging();

    disk::search_and_initialize();

    disk::streamer::disk_stream* ds = disk::streamer::new_stream(0);

    char buf[514];
    disk::streamer::seek(ds, 0x1ff);                        // 500
    disk::streamer::read(ds, static_cast<void*>(buf), 514); // 550

    auto fd = fs::fopen("0:/my_file.txt", "r");
    if (fd >= 0) {
        ARCH::terminal_write("We opened a root file!\n");
        char buf[12];
        fs::fread(buf, 11, 1, fd);
        buf[11] = '\0';
        ARCH::terminal_write("Content:");
        ARCH::terminal_write(buf);
        ARCH::terminal_write("\n");
        // fstat
        fs::file_stat stat;
        fs::fstat(fd, &stat);
        char fsizebuf[3];
        ARCH::terminal_write("Stat: ");
        ARCH::terminal_write(itoa(static_cast<int>(stat.filesize), fsizebuf, 10));
        ARCH::terminal_write("\n");
        char fflagsbuf[3];
        ARCH::terminal_write(itoa(static_cast<int>(stat.flags), fflagsbuf, 10));
        ARCH::terminal_write("\n");
        // close
        if (fs::fclose(fd) == 0) {
            ARCH::terminal_write("Successful close of root file\n");
        }
    }
    auto fd2 = fs::fopen("0:/my_dir/my_file2.txt", "r");
    if (fd2 >= 0) {
        ARCH::terminal_write("We opened a directory file!\n");
        fs::fseek(fd2, 3, fs::SEEK_SET);
        char buf[10];
        fs::fread(buf, 9, 1, fd2);
        buf[9] = '\0';
        ARCH::terminal_write("Content:");
        ARCH::terminal_write(buf);
        ARCH::terminal_write("\n");
        // now seek current
        fs::fseek(fd2, 1, fs::SEEK_CURRENT);
        char buf2[8];
        fs::fread(buf2, 8, 1, fd2);
        buf2[7] = '\0';
        ARCH::terminal_write("Content:");
        ARCH::terminal_write(buf2);
        ARCH::terminal_write("\n");
        // close
        if (fs::fclose(fd2) == 0) {
            ARCH::terminal_write("Successful close of directory file\n");
        }
    }

    enable_interrupts();

    //panic("Testing panic!");
}