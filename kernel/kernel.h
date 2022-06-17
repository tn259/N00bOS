#pragma once

extern "C" {
void panic(const char* message);
void kernel_main();
}

template <typename T>
void* ERROR(T value) {
    return reinterpret_cast<void*>(value);
}
template <typename T>
int ERROR_I(T* value) {
    return reinterpret_cast<int>(value);
}
template <typename T>
int ISERR(T* value) {
    return (reinterpret_cast<int>(value) < 0);
}