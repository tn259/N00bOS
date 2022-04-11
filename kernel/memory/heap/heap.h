#pragma once

#include <stdint.h>
#include <stddef.h>

static const constexpr uint8_t HEAP_BLOCK_TABLE_ENTRY_TAKEN = 0x01;
static const constexpr uint8_t HEAP_BLOCK_TABLE_ENTRY_FREE = 0x00;
static const constexpr uint8_t HEAP_BLOCK_HAS_NEXT = 0b10000000;
static const constexpr uint8_t HEAP_BLOCK_IS_FIRST = 0b01000000;

using HEAP_BLOCK_TABLE_ENTRY = unsigned char;

struct heap_table {
    HEAP_BLOCK_TABLE_ENTRY* entries;
    size_t total_entries;
};
struct heap {
    heap_table* table;
    void* start_address;
};

typedef int (*heap_create)(heap* heap_ptr, void* start_ptr, void* end_ptr, heap_table* table);
typedef void* (*heap_malloc)(heap* heap_ptr, size_t block_size);
typedef void (*heap_free)(heap* heap_ptr, void* block_ptr);

struct heap_strategy {
    heap_create create;
    heap_malloc malloc;
    heap_free free;
};

int default_heap_create(heap* heap_ptr, void* start_ptr, void* end_ptr, heap_table* table);