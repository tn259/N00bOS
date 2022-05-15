#include "ata.h"

#include "status.h"
#include "config.h"

#include "libc/string.h"
#include "arch/i386/io/io.h"

namespace ata {

// https://wiki.osdev.org/ATA_PIO_Mode
// https://wiki.osdev.org/ATA_Command_Matrix
int read_sector(int lba, int total, void* buf) {
    // upper 8bits to the HD controller
    outb(0x1F6, (lba >> 24) | 0xE0); // E0 is to select the master drive
    outb(0x1F2, total); // total sectors to read
    outb(0x1F3, static_cast<unsigned char>(lba & 0xFF)); // Lowest 8 bits
    outb(0x1F4, static_cast<unsigned char>(lba >> 8)); // Lowest 8 bits
    outb(0x1F5, static_cast<unsigned char>(lba >> 16)); // 2nd 8 bits
    outb(0x1F7, 0x20); // read with retry command

    // 16 bit ptr
    unsigned short* ptr = (unsigned short*)(buf);
    for (int sector_idx = 0; sector_idx < total; ++sector_idx) {
        // wait for buffer to be ready
        // TODO(tn259) use interrupt for this once multitasking implemented
        char c = 0;
        c = insb(0x1F7);
        while (!(c & 0x08)) {
            c = insb(0x1F7);
        }
        // Copy from Hard Disk to buf
        for (int word_idx = 0; word_idx < 256; ++word_idx) {
            *ptr = insw(0x1F0);
            ++ptr;
        }
    }
    return 0;
}

} // namespace ata