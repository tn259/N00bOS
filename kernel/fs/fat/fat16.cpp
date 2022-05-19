#include "fat16.h"
#include "file.h"

#include "disk/disk_streamer.h"

#include "libc/string.h"

#include <stdint.h>

namespace fs {
namespace fat {

void* fat16_open(disk::disk* d, path_part* path, FILE_MODE mode);
int fat16_resolve(disk::disk* d);

namespace {

const constexpr uint8_t FAT16_SIGNATURE = 0x29;
const constexpr uint8_t FAT16_FAT_ENTRY_SIZE = 0x02;
const constexpr uint16_t FAT16_FAT_BAD_SECTOR = 0xFF7;
const constexpr uint8_t FAT16_UNUSED = 0x00;

enum fat_item_type {
    FAT_ITEM_TYPE_DIRECTORY = 0,
    FAT_ITEM_TYPE_FILE
};

// FAT directory bitmask attributes
const constexpr uint8_t FAT_FILE_READ_ONLY = 0x01;
const constexpr uint8_t FAT_FILE_HIDDEN = 0x02;
const constexpr uint8_t FAT_FILE_SYSTEM = 0x04;
const constexpr uint8_t FAT_FILE_VOLUME_LABEL = 0x08;
const constexpr uint8_t FAT_FILE_SUBDIRECTORY = 0x10;
const constexpr uint8_t FAT_FILE_ARCHIVED = 0x20;
const constexpr uint8_t FAT_FILE_DEVICE = 0x40;
const constexpr uint8_t FAT_FILE_RESERVED = 0x80;

struct fat_header_extened {
    uint8_t drive_number;
    uint8_t win_nt_bit;
    uint8_t signature;
    uint32_t volume_id;
    uint8_t volume_id_string[11];
    uint8_t system_id_string[8];
} __attribute__((packed));

struct fat_header {
    uint8_t short_jmp_instruction[3];
    uint8_t oem_identifier[8];
    uint16_t bytes_per_sector;
    uint8_t sectors_per_cluster;
    uint16_t reserved_sectors;
    uint8_t fat_copies;
    uint16_t root_dir_entries;
    uint16_t number_of_sectors;
    uint8_t media_type;
    uint16_t sectors_per_fat;
    uint16_t sectors_per_track;
    uint16_t number_of_heads;
    uint32_t hidden_sectors;
    uint32_t sectors_big;
} __attribute__((packed));

struct fat_h {
    fat_header primary_header;
    // TODO(tn259) is this optional?
    union fat_h_e {
        fat_header_extened extender_header;
    };
};

struct fat_directory_item {
    uint8_t filename[8];
    uint8_t ext[3];
    uint8_t attribute;
    uint8_t reserved;
    uint8_t creation_time_tenths_of_a_second;
    uint16_t creation_time;
    uint16_t creation_date;
    uint16_t last_access;
    uint16_t hi_16bits_first_cluster;
    uint16_t last_modification_time;
    uint16_t last_modification_date;
    uint16_t lo_16bits_first_cluster;
    uint32_t filesize;
} __attribute__((packed));

struct fat_directory {
    fat_directory_item* item;
    int total;
    int sector_pos;
    int sector_pos_end;
};

struct fat_item {
    union {
        fat_directory_item* director_item;
        fat_directory* directory;
    };
    fat_item_type type;
};

struct fat_item_descriptor {
    fat_item* item;
    uint32_t pos;
};

struct fat_private {
    fat_h header;
    fat_directory root_directory;

    // streams the data
    disk::streamer::disk_stream* cluster_read_stream;
    // streams the FAT table itself 
    disk::streamer::disk_stream* fat_read_stream;
    disk::streamer::disk_stream* directory_read_stream;
};

filesystem fat16_fs = {
    .open = fat16_open,
    .resolve = fat16_resolve
};

}  // namespace anonymous

filesystem* fat16_init() {
    strcpy(fat16_fs.name, "FAT16");
    return &fat16_fs;
}

void* fat16_open(disk::disk* d, path_part* path, FILE_MODE mode) {
    return 0;
}

int fat16_resolve(disk::disk* d) {
    return 0;
}


}  // namespace fat
}  // namespace fs