#include "heap.h"
#include "config.h"

#include "memory.h"

namespace {

bool validate_alignment(void* ptr) {
    return (reinterpret_cast<size_t>(ptr) % HEAP_BLOCK_SIZE);
}

int validate_heap_table(void* start_ptr, void* end_ptr, heap_table* table) {
    //size_t table_size = static_cast<size_t>(end_ptr - start_ptr);
    size_t table_size = reinterpret_cast<size_t>(end_ptr) - reinterpret_cast<size_t>(start_ptr);
    size_t table_total_blocks = table_size / HEAP_BLOCK_SIZE;

    int result = 0;
    if (table_total_blocks != table->total_entries) {
        result = -1; // TODO(tn259) specify error codes
    }

    return result;
}

}

int default_heap_create(heap* heap, void* start_ptr, void* end_ptr, heap_table* table) {
    int result = 0;

    // Check pointer alignments
    if (!validate_alignment(start_ptr) || !validate_alignment(end_ptr)) {
        result = -1;
        return result;
    }

    // Init heap memory and start
    memset(heap, 0, sizeof(heap));
    heap->start_address = start_ptr;

    // Validate table
    result = validate_heap_table(start_ptr, end_ptr, table);
    if (!result) {
        return result;
    }

    // Init all table entries to be free
    size_t table_size = sizeof(HEAP_BLOCK_TABLE_ENTRY) * table->total_entries;
    memset(table->entries, HEAP_BLOCK_TABLE_ENTRY_FREE, table_size);

    return result;
}