#pragma once // NOLINT(llvm-header-guard)

#include <stdint.h> // NOLINT(hicpp-deprecated-headers,modernize-deprecated-headers)

enum vga_colour {
    VGA_COLOUR_BLACK         = 0,
    VGA_COLOUR_BLUE          = 1,
    VGA_COLOUR_GREEN         = 2,
    VGA_COLOUR_CYAN          = 3,
    VGA_COLOUR_RED           = 4,
    VGA_COLOUR_MAGENTA       = 5,
    VGA_COLOUR_BROWN         = 6,
    VGA_COLOUR_LIGHT_GREY    = 7,
    VGA_COLOUR_DARK_GREY     = 8,
    VGA_COLOUR_LIGHT_BLUE    = 9,
    VGA_COLOUR_LIGHT_GREEN   = 10,
    VGA_COLOUR_LIGHT_CYAN    = 11,
    VGA_COLOUR_LIGHT_RED     = 12,
    VGA_COLOUR_LIGHT_MAGENTA = 13,
    VGA_COLOUR_LIGHT_BROWN   = 14,
    VGA_COLOUR_WHITE         = 15
};

static inline uint8_t vga_entry_colour(enum vga_colour fg, enum vga_colour bg) { // NOLINT(modernize-use-trailing-return-type,clang-diagnostic-unused-function)
    return fg | (bg << 4);                                                       // NOLINT(hicpp-signed-bitwise,cppcoreguidlines-avoid-magic-numbers)
}

static inline uint16_t vga_entry(unsigned char character, enum vga_colour colour) { // NOLINT(modernize-use-trailing-return-type,clang-diagnostic-unused-function)
    return static_cast<uint16_t>(character) | static_cast<uint16_t>(colour << 8);  // NOLINT(hicpp-signed-bitwise,cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)
}
