#include "ctype.h"

int isdigit(int ch) {
    auto c     = static_cast<unsigned char>(ch);
    int result = (c >= 0x30 && c <= 0x39) ? 1 : 0;
    return result;
}

int tolower(int ch) {
    auto u_ch = static_cast<unsigned char>(ch);
    if (u_ch > 0x40 && u_ch < 0x5B) {
        return static_cast<int>(u_ch + 0x20);
    }
    return ch;
}