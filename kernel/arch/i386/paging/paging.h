#pragma once

#include <stddef.h>
#include <stdint.h>

// https://wiki.osdev.org/Paging#32-bit_Paging_.28Protected_Mode.29

extern "C" {
static const constexpr size_t PAGING_TOTAL_ENTRIES_PER_TABLE = 1024;
static const constexpr size_t PAGING_PAGE_SIZE               = 4096; // 4KB pages

static const constexpr uint8_t PAGING_CACHE_DISABLED      = 0b00010000;
static const constexpr uint8_t PAGING_WRITE_THROUGH_CACHE = 0b00001000;
static const constexpr uint8_t PAGING_ACCESS_FROM_ALL     = 0b00000100;
static const constexpr uint8_t PAGING_IS_WRITABLE         = 0b00000010;
static const constexpr uint8_t PAGING_IS_PRESENT          = 0b00000001;
}

using PAGING_ENTRY = uint32_t;

struct paging_chunk {
    PAGING_ENTRY* directory_entry;
};

extern "C" {
void enable_paging();
void paging_load_directory(PAGING_ENTRY* directory);
}
paging_chunk* paging_new(uint8_t flags);
void paging_switch(paging_chunk* chunk);