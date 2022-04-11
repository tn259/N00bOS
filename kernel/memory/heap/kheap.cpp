#include "kheap.h"
#include "heap.h"
#include "heap_strategy_factory.h"
#include "config.h"

namespace {
heap kernel_heap;
heap_table kernel_heap_table;
heap_strategy strategy;
}

void kheap_init() {
    // Set heap_table
    auto total_table_entries = HEAP_SIZE_BYTES / HEAP_BLOCK_SIZE;
    kernel_heap_table.entries = reinterpret_cast<HEAP_BLOCK_TABLE_ENTRY*>(HEAP_TABLE_ADDRESS);
    kernel_heap_table.total_entries = total_table_entries;

    // Set end ptr of heap
    void* end_ptr = reinterpret_cast<void*>(HEAP_ADDRESS + HEAP_SIZE_BYTES);

    // Set strategy
    heap_strategy_factory strategy_factory;
    strategy_factory.make_strategy(HEAP_ALGORITHM, &strategy);

    // Create
    auto create_result = strategy.create(&kernel_heap, reinterpret_cast<void*>(HEAP_ADDRESS), end_ptr, &kernel_heap_table);
    if (create_result < 0) {
        // TODO(tn259) error
    }
}

void* kmalloc(size_t size) {
    return strategy.malloc(&kernel_heap, size);
}

void kfree(void* ptr) {
    strategy.free(&kernel_heap, ptr);
}