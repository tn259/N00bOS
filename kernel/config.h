#pragma once

#include <stddef.h>
#include <stdint.h>

namespace mm::heap {
enum heap_algorithm {
    find_first = 0,
    unknown
};
} // namespace mm::heap

// HEAP
static const constexpr mm::heap::heap_algorithm HEAP_ALGORITHM = mm::heap::heap_algorithm::find_first;
static const constexpr size_t HEAP_SIZE_BYTES                  = 1024 * 1024 * 100; // 100MB for now
static const constexpr size_t HEAP_BLOCK_SIZE                  = 1024 * 4;          // 4KB for now
static const constexpr size_t HEAP_ADDRESS                     = 0x01000000;
static const constexpr size_t HEAP_TABLE_ADDRESS               = 0x00007E00;
// DISK
static const constexpr size_t DISK_BLOCK_SIZE = 512;
// FS
static const constexpr size_t FS_MAX_FILESYSTEMS   = 512;
static const constexpr int FS_MAX_FILE_DESCRIPTORS = 512;
static const constexpr size_t FS_MAX_PATH_SIZE     = 255;
// GDT
static const constexpr size_t GDT_TOTAL_SEGMENTS = 6;
static const constexpr size_t GDT_KERNEL_CODE_SELECTOR = 0x08;
static const constexpr size_t GDT_KERNEL_DATA_SELECTOR = 0x10;

