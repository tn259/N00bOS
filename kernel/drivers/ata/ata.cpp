#include "ata.h"

#include <stdint.h>

#include "arch/i386/io/io.h"
#include "config.h"
#include "libc/string.h"
#include "status.h"

namespace ata {

// IO PORT CONSTANTS
static const constexpr uint16_t DATA_REGISTER          = 0x1F0;
static const constexpr uint16_t SECTOR_COUNT_REGISTER  = 0x1F2;
static const constexpr uint16_t LBA_LOW_REGISTER       = 0x1F3;
static const constexpr uint16_t LBA_MID_REGISTER       = 0x1F4;
static const constexpr uint16_t LBA_HIGH_REGISTER      = 0x1F5;
static const constexpr uint16_t DRIVE_HEAD_REGISTER    = 0x1F6;
static const constexpr uint16_t READ_STATUS_REGISTER   = 0x1F7;
static const constexpr uint16_t WRITE_COMMAND_REGISTER = 0x1F7;

// DRIVE CONSTANTS
static const constexpr uint8_t MASTER_DRIVE = 0xE0;
static const constexpr uint8_t SLAVE_DRIVE  = 0xF0; // NOLINT(clang-diagnostic-unused-const-variable)

// COMMANDS
static const constexpr uint8_t READ_WITH_RETRY = 0x20;

// https://wiki.osdev.org/ATA_PIO_Mode
// https://wiki.osdev.org/ATA_Command_Matrix
int read_sector(int lba, int total, void* buf) {
    // upper 8bits to the HD controller
    outb(DRIVE_HEAD_REGISTER, (lba >> 24) | MASTER_DRIVE);          // E0 is to select the master drive
    outb(SECTOR_COUNT_REGISTER, total);                             // total sectors to read
    outb(LBA_LOW_REGISTER, static_cast<unsigned char>(lba & 0xFF)); // Lowest 8 bits
    outb(LBA_MID_REGISTER, static_cast<unsigned char>(lba >> 8));   // Lowest 8 bits
    outb(LBA_HIGH_REGISTER, static_cast<unsigned char>(lba >> 16)); // 2nd 8 bits
    outb(WRITE_COMMAND_REGISTER, READ_WITH_RETRY);                  // read with retry command

    // 16 bit ptr
    auto* ptr = static_cast<uint16_t*>(buf);
    for (int sector_idx = 0; sector_idx < total; ++sector_idx) {
        // wait for buffer to be ready
        // TODO(tn259) use interrupt for this once multitasking implemented
        unsigned char c = 0;
        c               = insb(READ_STATUS_REGISTER);
        while (!(c & 0x08)) {
            c = insb(READ_STATUS_REGISTER);
        }
        // Copy from Hard Disk to buf
        for (int word_idx = 0; word_idx < 256; ++word_idx) {
            *ptr = insw(DATA_REGISTER);
            ++ptr;
        }
    }
    return 0;
}

} // namespace ata