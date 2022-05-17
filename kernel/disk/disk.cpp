
#include "disk.h"
#include "drivers/ata/ata.h"

#include "fs/file.h"

#include "status.h"
#include "config.h"

#include "libc/string.h"
#include "arch/i386/io/io.h"

namespace disk {

namespace {
disk primary_disk;
}  // anonymous namespace


void search_and_initialize() {
    memset(&primary_disk, 0, sizeof(primary_disk));
    primary_disk.type = REAL_PHYSICAL;
    primary_disk.sector_size = DISK_BLOCK_SIZE;
    primary_disk.fs = fs::resolve(&primary_disk);
}

disk* get(int index) {
    if (index != 0) {
        return nullptr;
    }
    return &primary_disk;
}

int read_block(disk* disk, int lba, int total, void* buf) {
    if (disk != &primary_disk) {
        return -EINVAL;
    }
    return ata::read_sector(lba, total, buf);
}

}  // namespace disk