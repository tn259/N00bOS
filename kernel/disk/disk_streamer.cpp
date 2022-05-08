#include "disk_streamer.h"

#include "mm/heap/kheap.h"

#include "config.h"

namespace disk_streamer {

disk_stream* new_stream(int disk_id) {
    auto* disk = disk::get(disk_id);
    if (disk == nullptr) {
        return nullptr;
    }

    auto* stream = static_cast<disk_stream*>(kzalloc(sizeof(disk_stream)));
    stream->pos = 0;
    stream->disk = disk;

    return stream;
}

int seek(disk_stream* stream, int pos) {
    stream->pos = pos;
    return 0;
}

/**
 * |    sector 0    |    sector 1    |    sector 2    |
 * |     |<---a---->|<-b->|          |                |
 * |     seek pos
 */
int read(disk_stream* stream, void* out, int total) {
    auto sector_size = stream->disk->sector_size;
    auto* buf = static_cast<char*>(kzalloc(sector_size));
    int bytes_read = 0;
    int sectors_read = 0;
    auto* out_char = static_cast<char*>(out);
    while (bytes_read < total) {
        auto sector = (stream->pos + bytes_read) / sector_size;
        auto offset = (stream->pos + bytes_read) % sector_size;
        auto res = disk::read_block(stream->disk, sector, 1, buf);
        if (res < 0) {
            return res;
        }
        // read down 0->offset bytes but not for the first sector read
        if (sectors_read > 0) {
            for (int idx = 0; idx < offset; ++idx) {
                *out_char++ = buf[idx];
                ++bytes_read;
            }
        }
        // read up to the sector boundary or the remainder to total
        auto remainder_to_next_sector = sector_size - offset;
        auto remainder = total - bytes_read;
        auto bytes_to_read = (remainder > remainder_to_next_sector) ? remainder_to_next_sector : remainder;
        for (int idx = 0; idx < bytes_to_read; ++idx) {
            *out_char++ = buf[offset+idx];
            ++bytes_read;
        }
        ++sectors_read;
    }
    stream->pos += total;
    kfree(buf);
    return 0;
}

void close(disk_stream* stream) {
    kfree(stream);
}

} // namespace disk_streamer