#pragma once // NOLINT(llvm-header-guard)

// Not setting up C++ freestanding std headers
#include <stddef.h> // NOLINT(hicpp-deprecated-headers,modernize-deprecated-headers)
#include <stdint.h> // NOLINT(hicpp-deprecated-headers,modernize-deprecated-headers)

/**
 * https://wiki.osdev.org/Interrupt_Descriptor_Table
 **/

static const constexpr uint16_t KERNEL_CODE_SELECTOR  = 0x08;
static const constexpr uint16_t KERNEL_DATA_SELECTOR  = 0x10;
static const constexpr size_t NOOBOS_TOTAL_INTERRUPTS = 512;

struct idt_descriptor {
    uint16_t offset_1; // Bits 0 - 15
    uint16_t selector; // Selector/Segment in our GDT (CODE SEG)
    uint8_t zero;      // Unused
    uint8_t type_attributes;
    uint16_t offset_2; // Bits 16 - 31
} __attribute__((packed));

struct idtr_descriptor {
    uint16_t limit; // Size of descriptor table - 1
    uint32_t base;  // start address of descriptor table
} __attribute__((packed));

void idt_init();
