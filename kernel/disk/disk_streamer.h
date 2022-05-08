#pragma once

#include "drivers/ata/ata.h"

namespace disk_streamer {

struct disk_stream
{
    int pos;
    disk::disk* disk;
};

/**
 * @brief Create a new disk stream 
 * 
 * @param disk_id 
 * @return disk_stream* 
 */
disk_stream* new_stream(int disk_id);

/**
 * @brief Set the seek offset to pos
 * 
 * @param stream 
 * @param pos 
 * @return int 
 */
int seek(disk_stream* stream, int pos);

/**
 * @brief Read total bytes into out from disk stream
 * 
 * @param stream 
 * @param out - output buffer to read into
 * @param total - total bytes to read
 * @return int 
 */
int read(disk_stream* stream, void* out, int total);

/**
 * @brief Closes and deallocates the disk_stream
 * 
 * @param stream 
 */
void close(disk_stream* stream);

} // namespace disk_streamer