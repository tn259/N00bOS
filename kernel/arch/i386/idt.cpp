#include "idt.h"

#include <stddef.h>

#include "io.h"
#include "tty.h" // just for printing

/**
 * https://wiki.osdev.org/Interrupt_Descriptor_Table
 **/

idt_descriptor idt_descs[NOOBOS_TOTAL_INTERRUPTS];
idtr_descriptor idtr_desc;

namespace {
// TODO(tn259): There are equivalents of these in the .asm.
//              Need to figure out how to share the ASM constants -> C/C++ to avoid duplication
const constexpr uint8_t MASTER_PIC_CONTROL_BASE_IO_PORT    = 0x20; // NOLINT(clang-diagnostic-unused-const-variable)
const constexpr uint8_t MASTER_PIC_CONTROL_COMMAND_IO_PORT = MASTER_PIC_CONTROL_BASE_IO_PORT;
const constexpr uint8_t MASTER_PIC_CONTROL_DATA_IO_PORT    = MASTER_PIC_CONTROL_BASE_IO_PORT + 1; // NOLINT(clang-diagnostic-unused-const-variable)

const constexpr uint8_t PIC_END_OF_INTERRUPT_COMMAND = 0x20;

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
void int21h();
void no_interrupt();
}

extern "C" void int21h_handler() {
    terminal_write("Keyboard pressed\n");
    outb(MASTER_PIC_CONTROL_COMMAND_IO_PORT, PIC_END_OF_INTERRUPT_COMMAND); // we are done handling the interrupt
}
extern "C" void no_interrupt_handler() {                                    // for all interrupts not explicitly handled
    outb(MASTER_PIC_CONTROL_COMMAND_IO_PORT, PIC_END_OF_INTERRUPT_COMMAND); // we are done handling the interrupt (End of Interrupt)
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

    for (size_t i = 0; i < NOOBOS_TOTAL_INTERRUPTS; ++i) {
        idt_set(i, reinterpret_cast<void*>(no_interrupt));
    }
    idt_set(0, reinterpret_cast<void*>(idt_zero));  // example divide by zero
    idt_set(0x21, reinterpret_cast<void*>(int21h)); // keyboard interrupt NOLINT (cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)

    // Load interrupt descriptor table
    idt_load(&idtr_desc);
}
