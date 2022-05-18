#include "fat16.h"
#include "file.h"

#include "libc/string.h"

namespace fs {
namespace fat {

void* fat16_open(disk::disk* d, path_part* path, FILE_MODE mode);
int fat16_resolve(disk::disk* d);

namespace {

filesystem fat16_fs = {
    .open = fat16_open,
    .resolve = fat16_resolve
};

}  // namespace anonymous

filesystem* fat16_init() {
    strcpy(fat16_fs.name, "FAT16");
    return &fat16_fs;
}

void* fat16_open(disk::disk* d, path_part* path, FILE_MODE mode) {
    return 0;
}

int fat16_resolve(disk::disk* d) {
    return 0;
}


}  // namespace fat
}  // namespace fs