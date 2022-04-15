#include "heap.h"
#include "heap_utilities.h"

#include "memory.h"

#include "status.h"
#include "config.h"


namespace {

bool validate_heap_table(void* start_ptr, void* end_ptr, heap_table* table) {
    size_t table_size = reinterpret_cast<size_t>(end_ptr) - reinterpret_cast<size_t>(start_ptr);
    size_t table_total_blocks = table_size / HEAP_BLOCK_SIZE;

    if (table_total_blocks != table->total_entries) {
        return false;
    }

    return true;
}

} // anonymous namespace

int default_heap_create(heap* heap, void* start_ptr, void* end_ptr, heap_table* table) {
    // Check pointer alignments
    if (!validate_alignment(start_ptr) || !validate_alignment(end_ptr)) {
        return -EINVAL;
    }

    // Init heap memory and start
    memset(heap, 0, sizeof(heap));
    heap->start_address = start_ptr;
    heap->table = table;

    // Validate table
    if (!validate_heap_table(start_ptr, end_ptr, table)) {
        return -EINVAL;
    }

    // Init all table entries to be free
    size_t table_size = sizeof(HEAP_BLOCK_TABLE_ENTRY) * table->total_entries;
    memset(table->entries, HEAP_BLOCK_TABLE_ENTRY_FREE, table_size);

    return 0;
}