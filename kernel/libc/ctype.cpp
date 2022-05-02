#include "ctype.h"

int isdigit(int ch) {
    auto c = static_cast<unsigned char>(ch);
    int result = (c >= 0x30 && c <= 0x39) ? 1 : 0;
    return result;
}