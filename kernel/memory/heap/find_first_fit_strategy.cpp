#include "find_first_fit_strategy.h"
#include "heap.h"

#include <stddef.h>

namespace find_first_fit {

void set_strategy(heap_strategy* strategy) {
    strategy->malloc = &heap_malloc;
    strategy->free = &heap_free;
}

void* heap_malloc(heap* heap_ptr, size_t block_size) {
    return 0;
}

void heap_free(heap* heap_ptr, void* block_ptr) {

}

}