#pragma once

namespace fs {
class filesystem;
} // namespace fs

namespace disk {

enum disk_type {
    REAL_PHYSICAL = 0
};

struct disk {
    disk_type type;
    int sector_size;
    int id;
    fs::filesystem* fs;
    void* fs_private_data;
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

} // namespace disk