#include "string.h"

int atoi(const char* str) {
    auto len   = strlen(str);
    int result = 0;
    // TODO(tn259) support for other bases in the string representation
    // TODO(tn259) support for negatives
    int base_10_multiplier = 1;
    for (size_t idx = 0; idx < len; ++idx) {
        auto char_idx = len - 1 - idx;
        auto c        = str[char_idx];
        // 0x30 is '0'
        result += base_10_multiplier * static_cast<int>(c - 0x30);
        base_10_multiplier *= 10;
    }
    return result;
}

char* itoa(int value, char* str, int base) {
    // TODO(tn259) support other bases
    if (base != 10) {
        return str;
    }
    if (value == 0) {
        str[0] = '0';
        return str;
    }
    bool is_negative = value < 0;
    int start_idx    = 0;
    if (is_negative) {
        value     = -value;
        str[0]    = '-';
        start_idx = 1;
    }
    auto populate_idx = start_idx;
    // populate then reverse
    while (value > 0) {
        auto remainder    = value % base;
        str[populate_idx] = static_cast<char>('0' + remainder);
        value /= base;
        ++populate_idx;
    }
    // reverse
    for (int reverse_idx = start_idx; reverse_idx < (start_idx + (populate_idx / 2)); ++reverse_idx) {
        auto tmp                            = str[reverse_idx];
        str[reverse_idx]                    = str[populate_idx - reverse_idx - 1];
        str[populate_idx - reverse_idx - 1] = tmp;
    }
    str[populate_idx] = '\0'; // null-terminate
    return str;
}