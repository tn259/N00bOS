#pragma once

#include <stdint.h>

namespace arch::i386::gdt {

// See GDT section in arch/i386/boot/boot.asm to see the equivalent for this in asm

/**
 * @brief gdt entry as represented in the x86 processor
 */
struct gdt_t {
    uint16_t segment;
    uint16_t base_low;
    uint8_t base_middle;
    uint8_t access;
    uint8_t high_flags;
    uint8_t base_high;
};

/**
 * @brief gdt entry with fields in logical structure to set easily used by the kernel
 */
struct structured_gdt_t {
    uint32_t base;
    uint32_t limit;
    uint8_t access;
};

/**
 * @brief converts a structured gdt table to a real gdt
 * 
 * @param - structured_gdt
 * @param - gdt
 * @param - entries - the number of entries in the gdt table
 */
void structured_gdt_to_gdt(structured_gdt_t* structured_gdt, gdt_t* gdt, int total_entries);

} // namespace arch::i386::gdt

extern "C" {
/**
 * @brief Loads the global descriptor table gdt into the gdt register
 * 
 * 
 * @param gdt - the gdt
 * @param size - the number of entries in the gdt 
 */
void gdt_load(arch::i386::gdt::gdt_t* gdt, int size);
}