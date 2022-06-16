#pragma once

#include <stddef.h>
#include <stdint.h>

/**
 * Simple bitmap heap model
 * 
 * heap ---> heap_table -------------------> entries
 *   |           |                           | has_next? | is_first? |  |  |  |  |  | is_taken? |
 *   |           |-------> total_entries     |
 *   |
 *   |-----> data
 *           |
 *           |
 *           |
 *           |
 *            ...
 * 
 * Each heap_table entry accounts for a heap block in memory
 * total_entries = total_heap_size / heap_block_size
 * 
 * Algorithms supported:
 * - Find first fit
 */

namespace mm::heap {

static const constexpr uint8_t HEAP_BLOCK_TABLE_ENTRY_TAKEN = 0x01;
static const constexpr uint8_t HEAP_BLOCK_TABLE_ENTRY_FREE  = 0x00;
static const constexpr uint8_t HEAP_BLOCK_HAS_NEXT          = 0b10000000;
static const constexpr uint8_t HEAP_BLOCK_IS_FIRST          = 0b01000000;

using HEAP_BLOCK_TABLE_ENTRY = unsigned char;

struct heap_table {
    HEAP_BLOCK_TABLE_ENTRY* entries;
    size_t total_entries;
};
struct heap {
    heap_table* table;
    void* start_address;
};

using heap_create = int (*)(heap* heap_ptr, void* start_ptr, void* end_ptr, heap_table* table);
using heap_malloc = void* (*)(heap* heap_ptr, size_t block_size);
using heap_free   = void (*)(heap* heap_ptr, void* block_ptr);

struct heap_strategy {
    heap_create create;
    heap_malloc malloc;
    heap_free free;
};

int default_heap_create(heap* heap_ptr, void* start_ptr, void* end_ptr, heap_table* table);

} // namespace mm::heap