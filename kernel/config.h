#pragma once

#include <stddef.h>
#include <stdint.h>

namespace mm {
namespace heap {
enum heap_algorithm {
    find_first = 0,
    unknown
};
}
}

// HEAP
static const constexpr mm::heap::heap_algorithm HEAP_ALGORITHM = mm::heap::heap_algorithm::find_first;
static const constexpr size_t HEAP_SIZE_BYTES        = 1024 * 1024 * 100; // 100MB for now
static const constexpr size_t HEAP_BLOCK_SIZE        = 1024 * 4;          // 4KB for now
static const constexpr size_t HEAP_ADDRESS           = 0x01000000;
static const constexpr size_t HEAP_TABLE_ADDRESS     = 0x00007E00;
// DISK
static const constexpr size_t DISK_BLOCK_SIZE        = 512;