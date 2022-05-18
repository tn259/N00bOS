#include "file.h"
#include "fat/fat16.h"
#include "disk/disk.h"

#include "libc/string.h"
#include "config.h"
#include "status.h"

#include "mm/heap/kheap.h"

#include "arch/i386/tty.h"

namespace fs {

namespace {

filesystem* filesystems[FS_MAX_FILESYSTEMS];
file_descriptor* file_descriptors[FS_MAX_FILE_DESCRIPTORS];

/**
 * @brief Loads OS supported filesystems into the filesystems table 
 */
void load() {
    memset(filesystems, 0, sizeof(filesystems));
    insert_filesystem(fat::fat16_init());
}

/**
 * @brief Get the first filesystem that is free
 * 
 * @return filesystem** pointer to the array entry that is the first free fs
 */
filesystem** get_free_filesystem() {
    for (auto& fs : filesystems) {
        if (fs == nullptr) {
            return &fs;
        }
    }
    return nullptr;
}

/**
 * @brief Get the file descriptor indexed by fd
 * 
 * @param fd 
 * @return file_descriptor* 
 */
file_descriptor* get_file_descriptor(int fd) {
    if (fd < 0 || fd >= FS_MAX_FILE_DESCRIPTORS) {
        return nullptr;
    }
    return file_descriptors[fd];
}

/**
 * @brief Creates a new fd and returns it
 * 
 * @param out_file_descriptor - the new file_descriptor structure
 * @return int - fd number which matches the index into the fd array
 */
int new_file_descriptor(file_descriptor** out_file_descriptor) {
    for (int idx = 0; idx < FS_MAX_FILE_DESCRIPTORS; ++idx) {
        if (file_descriptors[idx] == nullptr) {
            auto* descriptor = static_cast<file_descriptor*>(mm::heap::kzalloc(sizeof(file_descriptor)));
            descriptor->index = idx;
            file_descriptors[idx] = descriptor;
            *out_file_descriptor = descriptor;
            return 0;
        }
    }
    return -ENOMEM;
}

}  // anonymous namespace

void init() {
    memset(file_descriptors, 0, sizeof(file_descriptor));
    load();
}

int fopen(const char* filename, const char* mode) {
    return -EIO;
}

void insert_filesystem(filesystem* fs) {
    filesystem** fs_ptr;
    fs_ptr = get_free_filesystem();
    if (fs_ptr == nullptr) {
        arch::i386::terminal_write("Error: insert_filesystem\n");
        while (1) {}
    }
    arch::i386::terminal_write(fs->name);
    *fs_ptr = fs;
}

filesystem* resolve(disk::disk* d) {
    filesystem* fs;
    for (auto& filesystem : filesystems) {
        if (filesystem != nullptr && filesystem->resolve(d) == 0) {
           fs = filesystem;
           break; 
        }
    }
    return fs;
}

}  // namespace fs