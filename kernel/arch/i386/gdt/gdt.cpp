#include "gdt.h"

#include "kernel.h" // panic

namespace arch::i386::gdt {

/**
     * @brief populate a gdt entry from a structured gdt entry
     * 
     * @param target_entry - gdt entry as uint8_t to be populated 
     * @param source - structured_gdt to populate from
     */
void encode_gdt_entry(uint8_t* target_entry, const structured_gdt_t& source) {
    /*struct gdt_t {
        uint16_t segment;
        uint16_t base_low;
        uint8_t base_middle;
        uint8_t access;
        uint8_t high_flags;
        uint8_t base_high;
    };*/
    auto& segment0    = target_entry[0];
    auto& segment1    = target_entry[1];
    auto& base_low0   = target_entry[2];
    auto& base_low1   = target_entry[3];
    auto& base_middle = target_entry[4];
    auto& access      = target_entry[5];
    auto& high_flags  = target_entry[6];
    auto& base_high   = target_entry[7];

    // the segment limit is a uint16_t
    // This check is to ensure that if we look to be in 4KB granulrity mode, then we are according to the limit.
    if ((source.limit > UINT16_MAX) && (source.limit & 0xfff) != 0xfff) {
        panic("encode_gdt_entry - invalid segment limit for 4KB granularity");
    }

    // https://en.wikipedia.org/wiki/Global_Descriptor_Table
    // See the format diagram in the link above.
    // The semgment limit in red is 20 Bytes
    // If we are addressing more than UINT16_MAX then we are using 4K page granularity up to an addressable limit of 4GB
    // In this case we use the upper 8 bits of the limit which is shifted back to the left by 12 when loaded.
    high_flags = 0x40; // Granularity bit originally clear
    auto limit = source.limit;
    if (limit > UINT16_MAX) {
        limit      = limit >> 12;
        high_flags = 0xc0; // Sets the granularity bit
    }

    // encode segment limit
    segment0 = limit & 0xff;
    segment1 = (limit >> 8) & 0xff;
    high_flags |= (limit >> 16) & 0x0f;

    // encode base
    base_low0   = source.base & 0xff;
    base_low1   = (source.base >> 8) & 0xff;
    base_middle = (source.base >> 16) & 0xff;
    base_high   = (source.base >> 24) & 0xff;

    // encode type
    access = source.access;
}

void structured_gdt_to_gdt(structured_gdt_t* structured_gdt, gdt_t* gdt, int total_entries) {
    for (int entry = 0; entry < total_entries; ++entry) {
        encode_gdt_entry(reinterpret_cast<uint8_t*>(&gdt[entry]), structured_gdt[entry]);
    }
}
} // namespace arch::i386::gdt