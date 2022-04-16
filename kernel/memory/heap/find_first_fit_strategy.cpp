#include "find_first_fit_strategy.h"

#include <stddef.h>

#include "config.h"
#include "heap.h"
#include "heap_utilities.h"
#include "memory.h"
#include "status.h"

namespace find_first_fit {

namespace {

int find_first_fit_start_block(heap* heap, size_t block_size) {
    // starting from the start address find the first contiguous block big enough
    for (size_t idx = 0; idx < heap->table->total_entries;) {
        size_t contiguous_block_count = 0;
        size_t idx_range              = idx;
        for (; idx_range < heap->table->total_entries; ++idx_range) {
            if (get_entry_type(heap->table->entries[idx_range]) == HEAP_BLOCK_TABLE_ENTRY_FREE) {
                ++contiguous_block_count;
            } else {
                break;
            }
        }
        if (contiguous_block_count * HEAP_BLOCK_SIZE >= block_size) {
            return idx;
        }
        idx = idx_range + 1;
    }
    return -ENOMEM;
}

} // anonymous namespace

void set_strategy(heap_strategy* strategy) {
    strategy->malloc = &heap_malloc;
    strategy->free   = &heap_free;
}

void* heap_malloc(heap* heap, size_t block_size) {
    // align data block requested and find corresponding start entry block
    auto aligned_size    = align_to_upper(block_size);
    auto first_block_idx = find_first_fit_start_block(heap, aligned_size);

    // First zero out data
    void* data_start = block_idx_2_block_address(heap, first_block_idx);
    memset(data_start, 0x00, aligned_size);

    // Mark blocks as allocated
    size_t num_blocks                         = aligned_size / HEAP_BLOCK_SIZE;
    size_t final_block_idx                    = first_block_idx + num_blocks - 1;
    HEAP_BLOCK_TABLE_ENTRY* table_start_entry = heap->table->entries;
    table_start_entry[first_block_idx]        = HEAP_BLOCK_IS_FIRST;
    for (size_t idx = first_block_idx; idx <= final_block_idx; ++idx) {
        HEAP_BLOCK_TABLE_ENTRY* entry = heap->table->entries + idx;
        *entry                        = HEAP_BLOCK_TABLE_ENTRY_TAKEN;
        if (idx != final_block_idx) {
            *entry |= HEAP_BLOCK_HAS_NEXT;
        }
    }

    return data_start;
}

void heap_free(heap* heap, void* addr) {
    // mark all blocks as free starting from heap_ptr for this allocation
    auto block_idx = block_address_2_block_idx(heap, addr);
    for (size_t idx = block_idx; idx < heap->table->total_entries; ++idx) {
        HEAP_BLOCK_TABLE_ENTRY block = heap->table->entries[idx];
        heap->table->entries[idx]    = HEAP_BLOCK_TABLE_ENTRY_FREE;
        bool has_next                = block & HEAP_BLOCK_HAS_NEXT;
        if (!has_next) {
            break;
        }
    }
}

} // namespace find_first_fit
