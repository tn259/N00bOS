#include "vga.h"
#include "tty.h"

#include <stddef.h>
#include <stdint.h>

namespace {

static const constexpr size_t VGA_WIDTH = 80;
static const constexpr size_t VGA_HEIGHT = 25;
static const uint16_t* VGA_MEMORY = reinterpret_cast<uint16_t*>(0xB8000);

static size_t terminal_row;
static size_t terminal_column;
static uint8_t terminal_colour;
static uint16_t* terminal_buffer;

void terminal_write_char_at(char character, size_t row, size_t column) {
    const size_t index = row * VGA_WIDTH + column;
    terminal_buffer[index] = vga_entry(character, static_cast<vga_colour>(terminal_colour));
}

}

void terminal_initialise() {
    terminal_row = 0;
    terminal_column = 0;
    terminal_colour = vga_entry_colour(VGA_COLOUR_LIGHT_GREY, VGA_COLOUR_BLACK);
    terminal_buffer = const_cast<uint16_t*>(VGA_MEMORY);
    for (size_t y = 0; y < VGA_HEIGHT; y++) {
        for (size_t x = 0; x < VGA_WIDTH; x++) {
            terminal_write_char_at(' ', y, x);
        }
    }
}

void terminal_write_char(char character) {
    if (character == '\n') {
        terminal_column = 0;
        ++terminal_row;
        if (terminal_row >= VGA_HEIGHT) {
            terminal_initialise(); // no more space - start again
        }
        return;
    }

    terminal_write_char_at(character, terminal_row, terminal_column);
    ++terminal_column;

    if (terminal_column >= VGA_WIDTH) {
        terminal_column = 0;
        ++terminal_row;
    }
    if (terminal_row >= VGA_HEIGHT) {
        terminal_initialise(); // no more space - start again
    }
}

void terminal_write(const char* string) {
    for (size_t idx = 0; string[idx] != '\0'; ++idx) { // no strlen for now
        terminal_write_char(string[idx]);
    }
}

void terminal_set_colour(uint8_t colour) {
    terminal_colour = colour;
}
