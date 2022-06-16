#pragma once

#include <stdint.h>

extern "C" {
unsigned char insb(uint16_t port);
uint16_t insw(uint16_t port);

void outb(uint16_t port, unsigned char val);
void outw(uint16_t port, uint16_t val);
}
