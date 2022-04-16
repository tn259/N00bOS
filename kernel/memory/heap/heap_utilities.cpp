#include "heap_utilities.h"

#include "config.h"

bool validate_alignment(void* addr) {
    return (reinterpret_cast<size_t>(addr) % HEAP_BLOCK_SIZE) == 0;
}

size_t align_to_upper(size_t size) {
    auto remainder = size % HEAP_BLOCK_SIZE;
    auto leftover  = HEAP_BLOCK_SIZE - remainder;
    return size + leftover;
}

void* block_idx_2_block_address(heap* heap, size_t block_idx) {
    auto data_offset = block_idx * HEAP_BLOCK_SIZE;
    return reinterpret_cast<void*>(reinterpret_cast<size_t>(heap->start_address) + data_offset);
}

size_t block_address_2_block_idx(heap* heap, void* addr) {
    auto data_offset = reinterpret_cast<size_t>(addr) - reinterpret_cast<size_t>(heap->start_address);
    return data_offset / HEAP_BLOCK_SIZE;
}

uint8_t get_entry_type(HEAP_BLOCK_TABLE_ENTRY entry) {
    return (entry & 0x0f); // NOLINT(readability-magic-numbers,cppcoreguidelines-avoid-magic-numbers)
}