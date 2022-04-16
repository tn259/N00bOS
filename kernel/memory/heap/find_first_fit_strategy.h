#pragma once

#include <stddef.h>
#include <stdint.h>

class heap_strategy;
class heap;

namespace find_first_fit {
void set_strategy(heap_strategy* strategy);
void* heap_malloc(heap* heap_ptr, size_t block_size);
void heap_free(heap* heap, void* addr);
} // namespace find_first_fit