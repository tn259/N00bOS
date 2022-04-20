#include "paging.h"

#include "memory/heap/kheap.h"
#include "status.h"

namespace {
PAGING_ENTRY* current_directory;
} // anonymous namespace

paging_chunk* paging_new(uint8_t flags) {
    auto* directory = static_cast<PAGING_ENTRY*>(kzalloc(sizeof(PAGING_ENTRY) * PAGING_TOTAL_ENTRIES_PER_TABLE));
    // Point every directory entry to a page table
    size_t offset = 0; // start address at which the directory points to
    for (size_t dir_idx = 0; dir_idx < PAGING_TOTAL_ENTRIES_PER_TABLE; ++dir_idx) {
        auto* table = static_cast<PAGING_ENTRY*>(kzalloc(sizeof(PAGING_ENTRY) * PAGING_TOTAL_ENTRIES_PER_TABLE));
        for (size_t table_idx = 0; table_idx < PAGING_TOTAL_ENTRIES_PER_TABLE; ++table_idx) {
            table[table_idx] = (offset + (table_idx * PAGING_PAGE_SIZE)) | flags;
        }
        offset += PAGING_TOTAL_ENTRIES_PER_TABLE * PAGING_PAGE_SIZE; // increment by size of entire data in one page table
        directory[dir_idx] = reinterpret_cast<PAGING_ENTRY>(table) | flags | PAGING_IS_WRITABLE;
    }

    auto* chunk            = static_cast<paging_chunk*>(kzalloc(sizeof(paging_chunk)));
    chunk->directory_entry = directory;
    return chunk;
}

void paging_switch(paging_chunk* chunk) {
    paging_load_directory(chunk->directory_entry);
    current_directory = chunk->directory_entry;
}