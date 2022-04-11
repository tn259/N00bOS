#include "memory.h"

void* memset(void* ptr, int c, size_t size) {
    char* char_ptr = static_cast<char*>(ptr);
    for (size_t i = 0; i < size; ++i) {
        *(char_ptr+i) = static_cast<char>(c);
    }
    return ptr;
}