#pragma once

#include "config.h"
#include "path_parser.h"

namespace disk {
class disk;
} // namespace disk

namespace fs {

using FILE_SEEK_MODE = unsigned int;
enum file_seek_mode {
    SEEK_SET,
    SEEK_CURRENT,
    SEEK_END
};

using FILE_MODE = unsigned int;
enum file_mode {
    FILE_MODE_READ,
    FILE_MODE_WRITE,
    FILE_MODE_APPEND,
    FILE_MODE_INVALID
};

// function to open a file
using fs_open_function = void* (*)(disk::disk* d, path_part* path, FILE_MODE mode);
// function to read from a file
using fs_read_function = int (*)(disk::disk* d, void* private_data, size_t block_size, size_t num_blocks, char* output);
// function to resolve a filesystem that the os supports against a filesystem that the disk supports
using fs_resolve_function = int (*)(disk::disk* d);

struct filesystem {
    fs_open_function open;
    fs_read_function read;
    fs_resolve_function resolve;

    char name[FS_MAX_PATH_SIZE];
};

struct file_descriptor {
    int index;
    filesystem* fs;

    void* private_data;
    disk::disk* d;
};

void init();
int fopen(const char* filename, const char* mode);
int fread(void* data, size_t block_size, size_t num_blocks, int fd);
void insert_filesystem(filesystem* fs);
filesystem* resolve(disk::disk* disk);

} // namespace fs