#pragma once

#include <stddef.h>
#include <stdint.h>

void kheap_init();
void* kmalloc(size_t size);
void* kzalloc(size_t size);
void kfree(void* ptr);
bool kheap_assert_all_free();