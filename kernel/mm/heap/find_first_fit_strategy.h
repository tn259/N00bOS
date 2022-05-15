#pragma once

#include <stddef.h>
#include <stdint.h>

namespace mm {
namespace heap {

class heap_strategy;
class heap;

namespace find_first_fit {

/**
 * @brief Set the strategy object
 * 
 * @param strategy 
 */
void set_strategy(heap_strategy* strategy);
/**
 * @brief malloc implementation for find first fit algorithm
 * 
 * @param heap_ptr 
 * @param block_size 
 * @return void* 
 */
void* heap_malloc(heap* heap_ptr, size_t block_size);
/**
 * @brief free implementation for find first fit algorithm
 * 
 * @param heap 
 * @param addr 
 */
void heap_free(heap* heap, void* addr);

} // namespace find_first_fit
}  // namespace heap
}  // namespace mm