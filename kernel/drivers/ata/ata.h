#pragma once

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