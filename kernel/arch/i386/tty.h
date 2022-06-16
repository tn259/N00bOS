#pragma once

#include <stdint.h>

namespace arch::i386 {

void terminal_initialise();
void terminal_write_char(char character);
void terminal_write(const char* string);
void terminal_set_colour(uint8_t colour);

} // namespace arch::i386