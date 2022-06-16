#pragma once

#include <stddef.h>
#include <stdint.h>

namespace mm::heap {

/**
 * @brief Initialises the kernel heap 
 */
void kheap_init();
/**
 * @brief Dynamically allocate and return size bytes of data
 * 
 * @param size - num bytes to allocate
 * @return void* - address of memory allocated
 */
void* kmalloc(size_t size);
/**
 * @brief kmalloc but initialize all bytes to zero
 * 
 * @param size - num bytes to allocate
 * @return void* - address of memory allocated
 */
void* kzalloc(size_t size);
/**
 * @brief return memory to heap - frees for further use
 * 
 * @param ptr - address of memory to be freed originally returned by an allocation
 */
void kfree(void* ptr);

} // namespace mm::heap