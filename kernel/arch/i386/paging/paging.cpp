#include "paging.h"

#include "mm/heap/kheap.h"
#include "status.h"

namespace arch::i386::paging {

namespace {

PAGING_ENTRY* current_directory;

/**
 * @brief Is an address on a PAGE boundary ? 
 * 
 * @param address the address
 * @return true 
 * @return false 
 */
bool paging_is_aligned(void* address) {
    return (reinterpret_cast<PAGING_ENTRY>(address) % PAGING_PAGE_SIZE) == 0;
}

/**
 * Page directory is a page table of 1024 entries: 
 * 
 * PD -->  [0] -> PDE1 address 0
 *         [1] -> PDE2 address == 0 + 1*(Size of entire memory addressable by one Page table) == 1 * PAGING_TOTAL_ENTRIES_PER_TABLE * PAGING_PAGE_SIZE
 *         [2] -> PDE3 address == 0 + 2*(Size of entire memory addressable by one Page table) == 2 * PAGING_TOTAL_ENTRIES_PER_TABLE * PAGING_PAGE_SIZE
 *         ...
 * 
 * To get the PD idx we simply divide by PAGING_TOTAL_ENTRIES_PER_TABLE * PAGING_PAGE_SIZE
 * 
 * To get the PT idx we need to know the offset into the PT which why we do % (PAGING_TOTAL_ENTRIES_PER_TABLE * PAGING_PAGE_SIZE)
 * Then we can / PAGING_PAGE_SIZE to get the table idx.
 *
 * @brief Obtains the directory and table idxs from a virtual address within the paging chunks address space
 * 
 * @param virtual_address 
 * @param directory_idx 
 * @param table_idx 
 * @return int - 0 on success 
 */
int paging_get_idxs(void* virtual_address, PAGING_ENTRY* directory_idx, PAGING_ENTRY* table_idx) {
    if (!paging_is_aligned(virtual_address)) {
        return -EINVAL;
    }
    *directory_idx = (reinterpret_cast<PAGING_ENTRY>(virtual_address) / (PAGING_TOTAL_ENTRIES_PER_TABLE * PAGING_PAGE_SIZE));
    *table_idx     = (reinterpret_cast<PAGING_ENTRY>(virtual_address) % (PAGING_TOTAL_ENTRIES_PER_TABLE * PAGING_PAGE_SIZE) / PAGING_PAGE_SIZE);
    return 0;
}

int paging_map(const PAGING_ENTRY* directory, uint8_t* virtual_address, uint8_t* physical_address, int total_pages, int flags) {
    int result = 0;
    for (int page_idx = 0; page_idx < total_pages; ++page_idx) {
        auto entry = reinterpret_cast<PAGING_ENTRY>(physical_address) | flags;
        result = paging_set(directory, virtual_address, entry);
        if (result != 0) {
            break;
        }
        virtual_address += PAGING_PAGE_SIZE;
        physical_address += PAGING_PAGE_SIZE;
    }
    return result;
}

} // anonymous namespace

paging_chunk* paging_new(uint8_t flags) {
    auto* directory = static_cast<PAGING_ENTRY*>(mm::heap::kzalloc(sizeof(PAGING_ENTRY) * PAGING_TOTAL_ENTRIES_PER_TABLE));
    // Point every directory entry to a page table
    size_t offset = 0; // start address at which the directory points to
    for (size_t dir_idx = 0; dir_idx < PAGING_TOTAL_ENTRIES_PER_TABLE; ++dir_idx) {
        auto* table = static_cast<PAGING_ENTRY*>(mm::heap::kzalloc(sizeof(PAGING_ENTRY) * PAGING_TOTAL_ENTRIES_PER_TABLE));
        for (size_t table_idx = 0; table_idx < PAGING_TOTAL_ENTRIES_PER_TABLE; ++table_idx) {
            table[table_idx] = (offset + (table_idx * PAGING_PAGE_SIZE)) | flags;
        }
        offset += PAGING_TOTAL_ENTRIES_PER_TABLE * PAGING_PAGE_SIZE; // increment by size of entire data in one page table
        directory[dir_idx] = reinterpret_cast<PAGING_ENTRY>(table) | flags | PAGING_IS_WRITABLE;
    }

    auto* chunk            = static_cast<paging_chunk*>(mm::heap::kzalloc(sizeof(paging_chunk)));
    chunk->directory_entry = directory;
    return chunk;
}

void paging_free(paging_chunk* paging) {
    for (size_t idx = 0; idx < PAGING_TOTAL_ENTRIES_PER_TABLE; ++idx) {
        auto dir_entry = paging->directory_entry[idx];
        auto* table = reinterpret_cast<PAGING_ENTRY*>(dir_entry & PAGING_ADDRESS_MASK);
        mm::heap::kfree(table);
    }
    mm::heap::kfree(paging->directory_entry);
    mm::heap::kfree(paging);
}


void paging_switch(paging_chunk* chunk) {
    paging_load_directory(chunk->directory_entry);
    current_directory = chunk->directory_entry;
}

int paging_set(const PAGING_ENTRY* directory, void* virtual_address, PAGING_ENTRY value) {
    PAGING_ENTRY directory_idx = 0;
    PAGING_ENTRY table_idx     = 0;

    int result = 0;
    if ((result = paging_get_idxs(virtual_address, &directory_idx, &table_idx)) < 0) {
        return result;
    }

    PAGING_ENTRY table_entry = directory[directory_idx];
    auto* table              = reinterpret_cast<PAGING_ENTRY*>(table_entry & PAGING_ADDRESS_MASK);
    table[table_idx]         = value;
    return 0;
}

int paging_map(const PAGING_ENTRY* directory, void* virtual_address, void* physical_address, void* physical_address_end, int flags) {
    if (!paging_is_aligned(virtual_address)) {
        return -EINVAL;
    }
    if (!paging_is_aligned(physical_address)) {
        return -EINVAL;
    }
    if (!paging_is_aligned(physical_address_end)) {
        return -EINVAL;
    }

    if (static_cast<PAGING_ENTRY*>(physical_address_end) < static_cast<PAGING_ENTRY*>(physical_address)) {
        return -EINVAL;
    }

    auto total_bytes = static_cast<PAGING_ENTRY*>(physical_address_end) - static_cast<PAGING_ENTRY*>(physical_address);
    auto total_pages = total_bytes / PAGING_PAGE_SIZE;

    return paging_map(directory, static_cast<uint8_t*>(virtual_address), static_cast<uint8_t*>(physical_address), total_pages, flags);
}

void* paging_align(void* address) {
    auto address_entry = reinterpret_cast<PAGING_ENTRY>(address);
    if ((address_entry % PAGING_PAGE_SIZE) != 0) {
        auto ptr = static_cast<uint8_t*>(address);
        ptr += (address_entry - (address_entry % PAGING_PAGE_SIZE) + PAGING_PAGE_SIZE);
        return static_cast<void*>(ptr);
    }
    return address;
}

} // namespace arch::i386::paging