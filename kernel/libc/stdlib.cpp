#include "string.h"

int atoi(const char* str) {
    auto len = strlen(str);
    int result = 0;
    int base_10_multiplier = 1;
    for (size_t idx = 0; idx < len; ++idx) {
        auto char_idx = len-1-idx;
        auto c = str[char_idx];
        // 0x30 is '0'
        result += base_10_multiplier * static_cast<int>(c-0x30);
        base_10_multiplier *= 10;
    }
    return result;
}