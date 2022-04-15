#pragma once

#include "heap.h"

#include <stddef.h>
#include <stdint.h>

bool validate_alignment(void* addr);
size_t align_to_upper(size_t size);
void* block_idx_2_block_address(heap* heap, size_t entry);
size_t block_address_2_block_idx(heap* heap, void* addr);
uint8_t get_entry_type(HEAP_BLOCK_TABLE_ENTRY entry);