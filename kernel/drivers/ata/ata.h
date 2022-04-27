#pragma once

// TODO(tn259) - Disk interface to be separated into different files later
namespace disk {

enum disk_type {
    REAL_PHYSICAL = 0
};

struct disk {
    disk_type type;
    int sector_size;
};

/**
 * @brief At startup search for and initailise disks on the system
 */
void search_and_initialize();

/**
 * @brief Get the disk associated with index
 * 
 * @param index - 0 for primary, 1 for secondary... etc. 
 * @return disk* - the indexed disk requested
 */
disk* get(int index);

/**
 * @brief Read block from disk
 * 
 * @param disk - the disk to read from
 * @param lba - Logical block address info
 * @param total - total blocks to read
 * @param buf - read data into this buffer 
 * @return int - 0 on success
 */
int read_block(disk* disk, int lba, int total, void* buf);


namespace ata {

/**
 * @brief Read a number of sectors from an ata disk 
 * 
 * @param lba - Logical block addressing information
 * @param total - total blocks to read
 * @param buf - read data into this buffer
 * @return int - 0 on success
 */
int read_sector(int lba, int total, void* buf);

} // namespace ata
} // namespace disk