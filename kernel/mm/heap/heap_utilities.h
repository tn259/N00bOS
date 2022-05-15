#pragma once

#include <stddef.h>
#include <stdint.h>

#include "heap.h"

namespace mm {
namespace heap {

/**
 * @brief Is this address aligned to a heap block boundary?
 * 
 * @param addr 
 * @return true - if aligned
 * @return false - if not aligned
 */
bool validate_alignment(void* addr);
/**
 * @brief Returns the size aligned to the upper heap block boundary
 * 
 * Example:
 * heap_block_size = 512
 * size = 700
 * aligned_size = 1024
 * 
 * @param size
 * @return size_t - input size aligned to upper block boundary
 */
size_t align_to_upper(size_t size);
/**
 * @brief Get heap data address of block with entry indexed by block_idx in the heap_table entries
 * 
 * @param heap 
 * @param block_idx 
 * @return void* - the heap data block address
 */
void* block_idx_2_block_address(heap* heap, size_t block_idx);
/**
 * @brief Get heap_table block entry index from heap data address
 * 
 * @param heap 
 * @param addr 
 * @return size_t - index of the heap_table block corresponding to the data address
 */
size_t block_address_2_block_idx(heap* heap, void* addr);
/**
 * @brief Get the entry type which encoded in the rightmost nibble of the entry
 * 
 * @param entry 
 * @return uint8_t - masked byte with only the entry type information
 */
uint8_t get_entry_type(HEAP_BLOCK_TABLE_ENTRY entry);

}  // namespace heap
}  // namespace mm