#include "idt.h"

#include <stddef.h>

#include "tty.h" // just for printing

/**
 * https://wiki.osdev.org/Interrupt_Descriptor_Table
 **/

idt_descriptor idt_descs[NOOBOS_TOTAL_INTERRUPTS];
idtr_descriptor idtr_desc;

namespace {
// TODO(tn259) put this into a "libc"
void* memset(void* ptr, char c, size_t size) {
    char* char_ptr = static_cast<char*>(ptr);
    for (size_t idx = 0; idx < size; ++idx) {
        char_ptr[idx] = c;
    }
    return ptr;
}
} // namespace

extern "C" {
void idt_load(idtr_descriptor* ptr);
}

void idt_zero() {
    terminal_write("DIVIDE BY 0 ERROR\n");
}

void idt_set(int interrupt_number, void* address) {
    idt_descriptor* desc = &idt_descs[interrupt_number];                     // NOLINT(cppcoreguidelines-pro-bounds-constant-array-index)
    desc->offset_1       = reinterpret_cast<uint32_t>(address) & 0x0000FFFF; // NOLINT (cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)
    desc->selector       = KERNEL_CODE_SELECTOR;
    desc->zero           = 0;
    // P = 1 ; DPL = 3; S = 0 ; Type = 32 bit interrupt gate; 0b11101110
    desc->type_attributes = 0xEE;                                      // NOLINT (cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)
    desc->offset_2        = reinterpret_cast<uint32_t>(address) >> 16; // NOLINT (cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)
}

void idt_init() {
    memset(static_cast<void*>(idt_descs), 0, sizeof(idt_descs));
    idtr_desc.limit = sizeof(idt_descs) - 1;
    idtr_desc.base  = reinterpret_cast<uint32_t>(&idt_descs[0]);

    idt_set(0, reinterpret_cast<void*>(idt_zero)); // example divide by zero

    // Load interrupt descriptor table
    idt_load(&idtr_desc);
}
