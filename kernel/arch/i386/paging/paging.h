#pragma once

#include <stddef.h>
#include <stdint.h>

// https://wiki.osdev.org/Paging#32-bit_Paging_.28Protected_Mode.29

/**
 *  PAGING_ENTRY directory structure for 4096 pages sizes x86 32 bit.
 *  Note that this is different for pages of 4MB size.
 * 
 *  31         12 11    8   7   6   5   4   3   2   1   0
 *  |  ADDRESS   |  AVL   |PS |AVL| A |PCD|PWT|U/S|R/W| P |
 * 
 * AVL: Available
 * PS: Page Size - 0 == 4KB, 1 == 4MB
 * A: Accessed
 * PCD: Cache disable
 * PWT: Write through cache
 * U/S: User/supervisor
 * R/W: Read/write
 * P: Present
 *
 * PAGING_TABLE_ENTRY for x86 32 bit.
 * 
 *  31         12 11     9 8   7   6   5   4   3   2   1   0
 *  |  ADDRESS   |  AVL   |G  |PAT|D  |A  |PCD|PWT|U/S|R/W| P |
 * 
 * AVL: Available
 * G: Global - Tells the processor not to invalidate the TLB entry upon a mov to CR3.
 * PAT: Page attribute table - allows per page fine grained control over how areas of memory are cached.
 * D: Dirty bit - determines if the page has been written to.
 * A: Accessed - Tells us whether a PDE or PTE was read during virtual address translation - cleared by CPU must be reset by OS.
 * PCD: Cache disable
 * PWT: Write through cache - 1 == Write-through, 0 == Write-back
 * U/S: User/supervisor
 * R/W: Read/write
 * 
 * 
 * The paging structure then looks as follows.
 * It maps an entire memory space of 4GB regardless of the RAM on the system.
 * 
 *              Page Directory      Page Tables
 *  CR3 ---->   PDE 1   ---->       PTE 1  ----->   PAGE 1
 *                |                 PTE 2  ----->   PAGE 2
 *                |                 PTE 3
 *                |                 ...
 *              PDE 2   ---->       PTE 1
 *                                  PTE 2
 * 
 */
namespace arch::i386::paging {

using PAGING_ENTRY = uint32_t;

static const constexpr size_t PAGING_TOTAL_ENTRIES_PER_TABLE = 1024;
static const constexpr size_t PAGING_PAGE_SIZE               = 4096; // 4KB pages

static const constexpr uint8_t PAGING_CACHE_DISABLED      = 0b00010000;
static const constexpr uint8_t PAGING_WRITE_THROUGH_CACHE = 0b00001000;
static const constexpr uint8_t PAGING_ACCESS_FROM_ALL     = 0b00000100;
static const constexpr uint8_t PAGING_IS_WRITABLE         = 0b00000010;
static const constexpr uint8_t PAGING_IS_PRESENT          = 0b00000001;

static const constexpr PAGING_ENTRY PAGING_ADDRESS_MASK = 0xfffff000;

struct paging_chunk {
    PAGING_ENTRY* directory_entry;
};

/**
 * @brief Allocates a new 4GB memory space
 * 
 * @param flags  
 * @return paging_chunk - new allocated 4GB memory space
 */
paging_chunk* paging_new(uint8_t flags);

/**
 * @brief Loads a new 4GB memory space 
 * 
 * @param chunk - the 4GB memory space
 */
void paging_switch(paging_chunk* chunk);

/**
 * @brief - Adds a new page table entry into the 4GB memory space
 * 
 * @param directory - Pointer to page directory
 * @param virtual_address - The address of the page table entry from the page directory
 * @param value - The page table entry contents
 * @return 0 on success 
 */
int paging_set(const PAGING_ENTRY* directory, void* virtual_address, PAGING_ENTRY value);

} // namespace arch::i386::paging

extern "C" { // ASM functions
namespace ARCH = arch::i386;
void enable_paging();
void paging_load_directory(ARCH::paging::PAGING_ENTRY* directory);
}
