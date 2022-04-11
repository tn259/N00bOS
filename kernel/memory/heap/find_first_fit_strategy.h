#pragma once

#include <stdint.h>
#include <stddef.h>

class heap_strategy;
class heap;

namespace find_first_fit {
    void set_strategy(heap_strategy* strategy);
    void* heap_malloc(heap* heap_ptr, size_t block_size);
    void heap_free(heap* heap_ptr, void* block_ptr);
}