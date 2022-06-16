#include "file.h"

#include "arch/i386/tty.h"
#include "config.h"
#include "disk/disk.h"
#include "fat/fat16.h"
#include "kernel.h"
#include "libc/string.h"
#include "mm/heap/kheap.h"
#include "path_parser.h"
#include "status.h"

namespace fs {

namespace {

filesystem* filesystems[FS_MAX_FILESYSTEMS];
file_descriptor* file_descriptors[FS_MAX_FILE_DESCRIPTORS];

/**
 * @brief Loads OS supported filesystems into the filesystems table 
 */
void load() {
    memset(static_cast<void*>(filesystems), 0, sizeof(filesystems));
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
file_descriptor* get_file_descriptor(int fd) { // NOLINT(clang-diagnostic-unused-function)
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
            auto* descriptor      = static_cast<file_descriptor*>(mm::heap::kzalloc(sizeof(file_descriptor)));
            descriptor->index     = idx;
            file_descriptors[idx] = descriptor;
            *out_file_descriptor  = descriptor;
            return 0;
        }
    }
    return -ENOMEM;
}

/**
 * @brief Convert string representation of file mode to FILEMODE value
 * 
 * @param str 
 * @return FILE_MODE 
 */
FILE_MODE str_2_filemode(const char* str) {
    if (strncmp(str, "r", 1) == 0) {
        return FILE_MODE_READ;
    }
    if (strncmp(str, "w", 1) == 0) {
        return FILE_MODE_WRITE;
    }
    if (strncmp(str, "a", 1) == 0) {
        return FILE_MODE_APPEND;
    }
    return FILE_MODE_INVALID;
}

} // anonymous namespace

void init() {
    memset(static_cast<void*>(file_descriptors), 0, sizeof(file_descriptor));
    load();
}

int fopen(const char* filename, const char* mode) {
    auto* root_path = parse(filename);
    // valid root path parsed?
    if (!root_path) {
        return -EINVAL;
    }
    if (!root_path->path) {
        return -EINVAL;
    }

    auto* disk = disk::get(root_path->drive_number);
    // disk exists?
    if (!disk) {
        return -EIO;
    }
    if (!disk->fs) {
        return -EIO;
    }

    auto filemode = str_2_filemode(mode);
    if (filemode == FILE_MODE_INVALID) {
        return -EINVAL;
    }

    // now open on the filesystem
    auto* descriptor_private_data = disk->fs->open(disk, root_path->path, filemode);
    if (ISERR(descriptor_private_data)) {
        return ERROR_I(descriptor_private_data);
    }
    // setup filedescriptor for the file
    file_descriptor* descriptor = nullptr;
    int result                  = 0;
    if ((result = new_file_descriptor(&descriptor)) < 0) {
        return result;
    }
    descriptor->d            = disk;
    descriptor->fs           = disk->fs;
    descriptor->private_data = descriptor_private_data;
    return descriptor->index;
}

void insert_filesystem(filesystem* fs) {
    filesystem** fs_ptr = nullptr;
    fs_ptr              = get_free_filesystem();
    if (fs_ptr == nullptr) {
        arch::i386::terminal_write("Error: insert_filesystem\n");
        while (true) {
        }
    }
    arch::i386::terminal_write(static_cast<char*>(fs->name));
    *fs_ptr = fs;
}

filesystem* resolve(disk::disk* d) {
    filesystem* fs = nullptr;
    for (auto& filesystem : filesystems) {
        if (filesystem != nullptr && filesystem->resolve(d) == 0) {
            fs = filesystem;
            break;
        }
    }
    return fs;
}

} // namespace fs