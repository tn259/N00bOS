#include "fat16.h"
#include "file.h"

#include "disk/disk.h"
#include "disk/disk_streamer.h"

#include "mm/heap/kheap.h"

#include "libc/string.h"

#include "status.h"

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
} __attribute__((packed)); // exists on disk

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
} __attribute__((packed)); // exists on disk

struct fat_h {
    fat_header primary_header;
    // TODO(tn259) is this optional?
    union fat_h_e {
        fat_header_extened extender_header;
    } shared;
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
} __attribute__((packed)); // exists on disk

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

/**
 * @brief Initialises a fat_private instance
 * 
 * @param d - the disk on which the fat instance is being used
 * @param private_data - the private data to be initialized
 */
void init_fat_private(disk::disk* d, fat_private* private_data) {
    memset(private_data, 0, sizeof(private_data));
    private_data->cluster_read_stream = disk::streamer::new_stream(d->id);
    private_data->fat_read_stream = disk::streamer::new_stream(d->id);
    private_data->directory_read_stream = disk::streamer::new_stream(d->id);
}

/**
 * @brief closes members of private data
 * 
 * @param private_data 
 */
void close_fat_private(fat_private* private_data) {
    disk::streamer::close(private_data->cluster_read_stream);
    disk::streamer::close(private_data->fat_read_stream);
    disk::streamer::close(private_data->directory_read_stream);
}

/**
 * @brief Calculates the number of directory items in a directory
 * 
 * @param d - disk to read from
 * @param directory_start_sector - start sector position of where the directory is on disk
 * @return int - >= 0 on success
 */
int get_directory_total_items(disk::disk* d, uint32_t directory_start_sector) {
    int result = 0;

    auto* private_data = static_cast<fat_private*>(d->fs_private_data);

    // seek to the start of the directory
    auto directory_start_pos = directory_start_sector * d->sector_size;
    auto* directory_stream = private_data->directory_read_stream;
    if ((result = disk::streamer::seek(directory_stream, directory_start_pos)) != 0) {
        return result;
    }

    // Loop through the directory items until we find the first one that is not populated.
    // The delimits where the directory items end.
    int num_items = 0;
    fat_directory_item item;
    while (true) {
        if ((result = disk::streamer::read(directory_stream, &item, sizeof(fat_directory_item))) != 0) {
            return result;
        }
        if (item.filename[0] == 0x00) {
            // no filename so not populated
            break;
        }
        if (item.filename[0] == 0xE5) {
            // item is unused so do not count it
            continue;
        }
        ++num_items;
    }
    return num_items;
}

/**
 * @brief Deserializeses the root directory structure from disk
 * 
 * @param d - the disk to read from
 * @param private_data - private fat data
 * @param directory - root directory of fat filesystem to populate
 * @return int - 0 on success
 */
int get_root_directory(disk::disk* d, fat_private* private_data, fat_directory* directory) {
    auto* primary_header = &private_data->header.primary_header;
    // get the root dir sector idx, in other words which sector is it?
    // directories and files i.e. data starts after the FAT metadata
    auto root_dir_sector_pos = (primary_header->fat_copies * primary_header->sectors_per_fat) + primary_header->reserved_sectors;
    auto root_dir_entries = primary_header->root_dir_entries;
    // get root dir size and number of sectors based on the number of entries in the root dir
    auto root_dir_size = root_dir_entries * sizeof(fat_item);
    auto root_dir_total_sectors = root_dir_size / d->sector_size;
    if (root_dir_size % d->sector_size != 0) { // round up
        ++root_dir_total_sectors;
    }

    auto directory_total_items = get_directory_total_items(d, root_dir_sector_pos);
    if (directory_total_items < 0) {
        return directory_total_items;
    }

    auto* directory_as_item = static_cast<fat_directory_item*>(mm::heap::kzalloc(root_dir_size));
    if (directory_as_item == nullptr) {
        return -ENOMEM;
    }

    int result = 0;
    auto* directory_stream = private_data->directory_read_stream;
    if ((result = disk::streamer::seek(directory_stream, root_dir_sector_pos * d->sector_size)) != 0) {
        return result;
    }

    if ((result = disk::streamer::read(directory_stream, &directory_as_item, sizeof(fat_directory_item))) != 0) {
        return result;
    }

    directory->sector_pos = root_dir_sector_pos;
    directory->sector_pos_end = root_dir_sector_pos + (root_dir_total_sectors * d->sector_size);
    directory->item = directory_as_item;
    return 0;
}

}  // namespace anonymous

filesystem* fat16_init() {
    strcpy(fat16_fs.name, "FAT16");
    return &fat16_fs;
}

void* fat16_open(disk::disk* d, path_part* path, FILE_MODE mode) {
    return 0;
}

int fat16_resolve(disk::disk* d) {
    int result = 0;
    // Create private data
    auto* private_data = static_cast<fat_private*>(mm::heap::kzalloc(sizeof(fat_private)));
    init_fat_private(d, private_data);

    d->fs_private_data = private_data;
    d->fs = &fat16_fs;

    // use a streamer to get FAT metadata and data from the disk and populate the FAT16 structures
    auto* stream = static_cast<disk::streamer::disk_stream*>(disk::streamer::new_stream(d->id));
    if (stream == nullptr) {
        result = -ENOMEM;
        goto out;
    }

    // read in header
    if ((result = disk::streamer::read(stream, &private_data->header, sizeof(private_data->header))) != 0) {
        goto out;
    }

    if (private_data->header.shared.extender_header.signature != FAT16_SIGNATURE) {
        // this is not FAT16
        result = -EFSNOTPRESENT;
        goto out;
    }

    // read in root directory
    if ((result = get_root_directory(d, private_data, &private_data->root_directory)) != 0) {
        goto out;
    }

out:
    if (stream) {
        disk::streamer::close(stream);
    }
    if (result < 0 && private_data) {
        close_fat_private(private_data);
        mm::heap::kfree(private_data);
        d->fs = nullptr;
        d->fs_private_data = nullptr;
    }
    return result;
}

}  // namespace fat
}  // namespace fs