#include "fat16.h"
#include "file.h"

#include "disk/disk.h"
#include "disk/disk_streamer.h"

#include "mm/heap/kheap.h"

#include "libc/string.h"

#include "status.h"
#include "config.h"

#include "kernel.h"

#include <stdint.h>

// https://www.cs.fsu.edu/~cop4610t/assignments/project3/spec/fatspec.pdf

namespace fs {
namespace fat {

void* fat16_open(disk::disk* d, path_part* path, FILE_MODE mode);
int fat16_resolve(disk::disk* d);

namespace {

const constexpr uint8_t FAT16_SIGNATURE = 0x29;
const constexpr uint8_t FAT16_FAT_ENTRY_SIZE = 0x02;

// https://en.wikipedia.org/wiki/Design_of_the_FAT_file_system
const constexpr uint16_t FAT16_FAT_RESERVED_CLUSTER_START = 0xFFF0;
const constexpr uint16_t FAT16_FAT_RESERVED_CLUSTER_END = 0xFFF6;
const constexpr uint16_t FAT16_FAT_BAD_SECTOR = 0xFFF7;
const constexpr uint16_t FAT16_FAT_LAST_CLUSTER_IN_FILE_START = 0xFFF8;
const constexpr uint16_t FAT16_FAT_LAST_CLUSTER_IN_FILE_END = 0xFFFF;
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
        fat_directory_item* directory_item;
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
    // streams the root directory region
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
 * @brief Closes members of private data
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
    // This delimits where the directory items end.
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

int get_fat_entry(disk::disk* d, int cluster) {
    auto* private_data = static_cast<fat_private*>(d->fs_private_data);
    auto* fat_stream = private_data->fat_read_stream;
    if (fat_stream == nullptr) {
        return -1;
    }

    // first FAT sector starts after the reserved sectors
    auto first_fat_sector = private_data->header.primary_header.reserved_sectors;
    auto fat_table_pos = first_fat_sector * d->sector_size;
    int res = -1;
    // TODO(tn259) this seek pos does not make sense
    res = disk::streamer::seek(fat_stream, fat_table_pos * (cluster * FAT16_FAT_ENTRY_SIZE));
    if (res < 0) {
        return res;
    }
    // fat entry size is 2 bytes
    uint16_t fat_entry = 0;
    res = disk::streamer::read(fat_stream, &fat_entry, sizeof(fat_entry));
    if (res < 0) {
        return res;
    }

    return fat_entry;
}

/**
 * @brief Gets the first cluster index for the fat directory item
 * 
 * @param directory_item - the fat directory item 
 * @return uint32_t - the first cluster index
 */
uint32_t get_first_cluster(fat_directory_item* directory_item) {
    return (directory_item->hi_16bits_first_cluster << 16) | directory_item->lo_16bits_first_cluster;
}

/**
 * @brief Gets the cluster index corresponsing to the the byte offset from the starting cluster index
 * 
 * @param d - disk
 * @param starting_cluster - cluster to start from 
 * @param offset - byte offset into cluster data section
 * @return int - the cluster
 */
int get_cluster(disk::disk* d, int starting_cluster, int offset) {
    auto* private_data = static_cast<fat_private*>(d->fs_private_data);
    auto cluster_size_bytes = private_data->header.primary_header.sectors_per_cluster * d->sector_size;
    auto cluster = starting_cluster;
    auto cluster_offset = offset / cluster_size_bytes;
    for (int idx = 0; idx < cluster_offset; ++idx) {
        // TODO(tn259) Find the first cluster that is not used?
        auto fat_entry = get_fat_entry(d, cluster);
        if (fat_entry < 0) {
            return fat_entry;
        }
        if (FAT16_FAT_LAST_CLUSTER_IN_FILE_START <= fat_entry && fat_entry <= FAT16_FAT_LAST_CLUSTER_IN_FILE_END) {
            return -EIO;
        }
        if (fat_entry == FAT16_FAT_BAD_SECTOR) {
            return -EIO;
        }
        if (FAT16_FAT_RESERVED_CLUSTER_START <= fat_entry && fat_entry <= FAT16_FAT_RESERVED_CLUSTER_END) {
            return -EIO;
        }
        if (fat_entry == FAT16_UNUSED) {
            return -EIO;
        }
        cluster = fat_entry;
    }
    return cluster;
}

/**
 * @brief Gets a sector index from a cluster index
 * 
 * @param private_data
 * @param cluster - the cluster index
 * @return int - the sector index
 */
int cluster_to_sector(fat_private* private_data, int cluster)
{
    // TODO(tn259) why is it cluster - 2?
    return private_data->root_directory.sector_pos_end + ((cluster - 2) * private_data->header.primary_header.sectors_per_cluster);
}

/**
 * @brief Read total_bytes from offset into starting_cluster using the disk_stream
 * 
 * @param d - the disk
 * @param stream - streamer instance to read from
 * @param starting_cluster - the starting cluster to read from
 * @param offset - the offset in bytes into the cluster
 * @param total_bytes - number of bytes to read
 * @param output - output buffer to read into
 * @return int - == 0 on success
 */
int read_internal_from_stream(disk::disk* d, disk::streamer::disk_stream* stream, int starting_cluster, int offset, int total_bytes, void* output) {
    auto* private_data = static_cast<fat_private*>(d->fs_private_data);
    auto cluster_size_bytes = private_data->header.primary_header.sectors_per_cluster * d->sector_size;

    int bytes_read = 0;
    int res = 0;
    while (bytes_read < total_bytes) {
        auto cluster = get_cluster(d, starting_cluster, offset);
        if (cluster < 0) {
            return cluster;
        }
        auto offset_from_cluster = offset % cluster_size_bytes;

        auto starting_sector = cluster_to_sector(private_data, cluster);
        auto starting_pos = (starting_sector * d->sector_size) * offset_from_cluster;

        auto bytes_remaining = total_bytes - bytes_read;
        auto total_to_read = bytes_remaining > cluster_size_bytes ? cluster_size_bytes : bytes_remaining;
        if ((res = disk::streamer::seek(stream, starting_pos)) < 0) {
            return res;
        }
        if ((res = disk::streamer::read(stream, output, total_to_read)) < 0) {
            return res;
        }
        bytes_read += total_to_read;
    }
    return res;
}

/**
 * @brief Read total_bytes from offset into starting_cluster
 * 
 * @param d - the disk
 * @param starting_cluster - the starting cluster to read from
 * @param offset - the offset in bytes into the cluster
 * @param total_bytes - number of bytes to read
 * @param output - output buffer to read into
 * @return int - == 0 on success
 */
int read_internal(disk::disk* d, int starting_cluster, int offset, int total_bytes, void* output) {
    auto* private_data = static_cast<fat_private*>(d->fs_private_data);
    auto* cluster_stream = private_data->cluster_read_stream;
    return read_internal_from_stream(d, cluster_stream, starting_cluster, offset, total_bytes, output);
}

/**
 * @brief Deallocates the fat_directory provided
 * 
 * @param directory 
 */
void directory_free(fat_directory* directory) {
    if (directory != nullptr) {
        if (directory->item != nullptr) {
            mm::heap::kfree(directory->item);
        }
        mm::heap::kfree(directory);
    }
}

/**
 * @brief Deallocates the fat_item provided
 * 
 * @param item 
 */
void item_free(fat_item* item) {
    switch (item->type) {
        case FAT_ITEM_TYPE_DIRECTORY:
            directory_free(item->directory);
            break;
        case FAT_ITEM_TYPE_FILE:
            mm::heap::kfree(item->directory_item);
            break;
        default:
            break;
    }
    mm::heap::kfree(item);
}

/**
 * @brief Loads/Deserializes a directory from disk 
 * 
 * @param d - the disk
 * @param directory_item - fat_directory_item representation of the directory
 * @return fat_directory* - the fat_directory representation of the directory
 */
fat_directory* load_directory(disk::disk* d, fat_directory_item* directory_item) {
    if (!(directory_item->attribute & FAT_FILE_SUBDIRECTORY)) {
        return nullptr;
    }
    auto* private_data = static_cast<fat_private*>(d->fs_private_data);
    auto* directory = static_cast<fat_directory*>(mm::heap::kzalloc(sizeof(fat_directory)));
    if (directory == nullptr) {
        return nullptr;
    }
    
    auto cluster = get_first_cluster(directory_item);
    auto sector = cluster_to_sector(private_data, cluster);
    auto total_items = get_directory_total_items(d, sector);
    directory->total = total_items;
    auto directory_size = directory->total * sizeof(fat_directory_item);
    directory->item = static_cast<fat_directory_item*>(mm::heap::kzalloc(directory_size));
    if (directory->item == nullptr) {
        directory_free(directory);
        return nullptr;
    }

    if (read_internal(d, cluster, 0x00, directory_size, directory->item) < 0) {
        directory_free(directory);
        return nullptr;
    }
    return directory; 
}

/**
 * @brief Allocates and returns a clone of directory_item
 * 
 * @param directory_item 
 * @param size 
 * @return fat_directory_item* 
 */
fat_directory_item* clone_directory_item(fat_directory_item* directory_item, size_t size) {
    if (size < sizeof(fat_directory_item)) {
        return nullptr;
    }
    auto* copy = static_cast<fat_directory_item*>(mm::heap::kzalloc(size));
    if (copy == nullptr) {
        return nullptr;
    }
    memcpy(copy, directory_item, size);
    return copy;
}

/**
 * @brief Creates a new fat_item from the fat_directory_item
 * 
 * @param d - used to load in subdirectory if directory_item is a directory
 * @param directory_item - the directory_item
 * @return fat_item* - the new fat_item
 */
fat_item* new_item(disk::disk* d, fat_directory_item* directory_item) {
    auto* item = static_cast<fat_item*>(mm::heap::kzalloc(sizeof(fat_item)));
    if (item == nullptr) {
        return nullptr;
    }

    if (directory_item->attribute & FAT_FILE_SUBDIRECTORY) {
        // this is a directory
        item->directory = load_directory(d, directory_item);
        item->type = FAT_ITEM_TYPE_DIRECTORY;
        return item; // TODO(tn259) - should we be breaking here?
    }
    // this is a file
    item->type = FAT_ITEM_TYPE_FILE;
    item->directory_item = clone_directory_item(directory_item, sizeof(fat_directory_item));
    return item;
}

/**
 * @brief Copy null terminated a string from in to out truncating at any first space encountered
 * 
 * @param out - pointer to out buffer
 * @param in - in buffer
 */
void get_space_terminated_string(char** out, const char* in) {
    while (*in != 0x00 && *in != 0x20) {
        **out = *in;
        ++*out;
        ++in;
    }
    // truncate ending space with a null
    if (*in == 0x20) {
        **out = 0x00;
    }
}

/**
 * @brief Get the filename from the FAT filename field.
 *        This should be filename + '.' + ext where filename and ext are the FAT item attributes 
 * 
 * @param directory_item - the FAT item
 * @param out - output
 * @param max_length - max length of output
 */
void get_filename(fat_directory_item* directory_item, char* out, int max_length) {
    memset(out, 0x00, max_length);
    char* out_tmp = out;
    get_space_terminated_string(&out_tmp, reinterpret_cast<const char*>(directory_item->filename));
    if (directory_item->ext[0] != 0x00 && directory_item->ext[0] != 0x20) {
        *out_tmp++ = '.';
        get_space_terminated_string(&out_tmp, reinterpret_cast<const char*>(directory_item->ext));
    }
}

/**
 * @brief Tries to find the item 'name' in the directory 'directory'
 * 
 * @param d - the disk
 * @param directory - the directory
 * @param name - the name of the item
 * @return fat_item* - the deserialized located item
 */
fat_item* find_item_in_directory(disk::disk* d, fat_directory* directory, const char* name) {
    fat_item* item = nullptr;
    char filename[FS_MAX_PATH_SIZE];
    for (int item_idx = 0; item_idx < directory->total; ++item_idx) {
        get_filename(&directory->item[item_idx], filename, sizeof(filename));
        if (strcasecmp(filename, name) == 0) {
            item = new_item(d, &directory->item[item_idx]);
            break;
        }
    }
    return item;
}

/**
 * @brief Get and deserialize the fat item from disk identified by a path
 *
 * We traverse the path parts until we can deserialize leaf fat_item
 *  
 * @param d - the disk
 * @param part - the path part identifier
 * @return fat_item* - the deserialized fat item
 */
fat_item* get_item(disk::disk* d, path_part* part) {
    auto* private_data = static_cast<fat_private*>(d->fs_private_data);
    // first get the root directory item
    auto* root_item = find_item_in_directory(d, &private_data->root_directory, part->part_name);
    if (root_item == nullptr) {
        return nullptr;
    }

    auto* current_item = root_item;
    auto* next_part = part->next;
    while (next_part != nullptr) {
        if (current_item->type != FAT_ITEM_TYPE_DIRECTORY) {
            // e.g. we were searching for c.txt in /a/b/c.txt but a or b turned out not to be directories to traverse
            current_item = nullptr;
            return current_item;
        }
        auto* tmp_item = find_item_in_directory(d, current_item->directory, next_part->part_name);
        // TODO(tn259) deallocate everything if null?
        item_free(current_item);
        current_item = tmp_item;
        next_part = next_part->next;
    }
    // current item is now a file
    // e.g. c.txt in /a/b/c.txt
    return current_item; 
}

}  // namespace anonymous

filesystem* fat16_init() {
    strcpy(fat16_fs.name, "FAT16");
    return &fat16_fs;
}

void* fat16_open(disk::disk* d, path_part* path, FILE_MODE mode) {
    // TODO(tn259) implement file system writing
    if (mode != FILE_MODE_READ) {
        return ERROR(-EROFS);
    }

    auto* descriptor = static_cast<fat_item_descriptor*>(mm::heap::kzalloc(sizeof(fat_item_descriptor)));
    if (descriptor == nullptr) {
        return ERROR(-ENOMEM);
    }

    descriptor->item = get_item(d, path);
    if (descriptor->item == nullptr) {
        return ERROR(-EIO);
    }

    descriptor->pos = 0;
    return descriptor;
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
    if (result < 0 && private_data != nullptr) {
        close_fat_private(private_data);
        mm::heap::kfree(private_data);
        d->fs = nullptr;
        d->fs_private_data = nullptr;
    }
    return result;
}

}  // namespace fat
}  // namespace fs