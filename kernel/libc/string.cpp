#include "string.h"

#include <stddef.h>

#include "ctype.h"

int memcmp(const void* mem1, const void* mem2, size_t n) {
    const auto* ptr1 = static_cast<const unsigned char*>(mem1);
    const auto* ptr2 = static_cast<const unsigned char*>(mem2);
    for (size_t idx = 0; idx < n; ++idx) {
        if (ptr1[idx] > ptr2[idx]) {
            return 1;
        } else if (ptr1[idx] < ptr2[idx]) { // NOLINT(readability-else-after-return)
            return -1;
        }
    }
    return 0;
}

void* memcpy(void* dst, const void* src, size_t n) {
    const auto* src_char = static_cast<const char*>(src);
    auto* dst_char       = static_cast<char*>(dst);
    for (size_t idx = 0; idx < n; ++idx) {
        dst_char[idx] = src_char[idx];
    }
    return dst;
}

/**
 * case 1 - start from end
 * |           |        src
 *       |         |   dst
 * case 2  - start from beginning
 *       |           |     src
 * |           |        dst
 */
void* memmove(void* dst, const void* src, size_t n) {
    const auto* src_char = static_cast<const char*>(src);
    auto* dst_char       = static_cast<char*>(dst);
    if (src_char < dst_char) {
        for (size_t idx = n; idx > 0; --idx) {
            dst_char[idx - 1] = src_char[idx - 1];
        }
    } else {
        for (size_t idx = 0; idx < n; ++idx) {
            dst_char[idx] = src_char[idx];
        }
    }
    return dst;
}

void* memset(void* mem, int value, size_t n) {
    auto* ptr = static_cast<char*>(mem);
    for (size_t idx = 0; idx < n; ++idx) {
        ptr[idx] = static_cast<char>(value);
    }
    return mem;
}

size_t strlen(const char* str) {
    size_t idx = 0;
    while (str[idx] != '\0') {
        ++idx;
    }
    return idx;
}

char* strcpy(char* dst, const char* src) {
    auto* dst_start = static_cast<char*>(dst);
    while (*src != '\0') {
        *dst = *src;
        ++src;
        ++dst;
    }
    *dst = '\0';
    return dst_start;
}

char* strncpy(char* dst, const char* src, size_t n) {
    size_t idx      = 0;
    auto* dst_start = static_cast<char*>(dst);
    while (idx < n && *src != '\0') {
        *dst = *src;
        ++src;
        ++dst;
        ++idx;
    }
    while (idx < n) {
        *dst = '\0';
        ++idx;
    }
    return dst_start;
}

int strncmp(const char* str1, const char* str2, int n) {
    for (int idx = 0; idx < n; ++idx) {
        bool end_str1 = str1[idx] == '\0';
        bool end_str2 = str2[idx] == '\0';
        if (end_str1 && end_str2) {
            return 0;
        }
        if (end_str1) {
            return -1;
        }
        if (end_str2) {
            return 1;
        }
        if (str1[idx] < str2[idx]) {
            return -1;
        }
        if (str2[idx] > str1[idx]) {
            return 1;
        }
    }
    return 0;
}

int strcasecmp(const char* str1, const char* str2) {
    int idx    = 0;
    int result = 0;
    while (true) {
        bool end_str1 = str1[idx] == '\0';
        bool end_str2 = str2[idx] == '\0';
        if (end_str1 && end_str2) {
            result = 0;
            break;
        }
        if (end_str1) {
            result = -1;
            break;
        }
        if (end_str2) {
            result = 1;
            break;
        }
        auto c1 = tolower(str1[idx]);
        auto c2 = tolower(str2[idx]);
        if (c1 < c2) {
            result = -1;
            break;
        }
        if (c2 > c1) {
            result = 1;
            break;
        }
        ++idx;
    }
    return result;
}