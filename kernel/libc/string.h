#pragma once

#include <stddef.h>

/**
 * @brief Compare two memory buffers byte by byte
 * 
 * @param mem1 - first buffer
 * @param mem2 - second buffer
 * @param n - number of bytes to compare
 * @return == 0 if equal, 
 *         >0 if first different unsigned char byte value is greater in mem1
 *         <0 if first different unsigned char byte value is greater in mem2
 */
int memcmp(const void* mem1, const void* mem2, size_t n);

/**
 * @brief Copy n bytes from src to dst 
 * 
 * @param dst - dst buffer
 * @param src - src buffer
 * @param n - number of bytes to copy
 * @return void* - pointer to populated memory
 */
void* memcpy(void* dst, const void* src, size_t n);

/**
 * @brief - Copy n bytes from src to dst even if src and dst overlap
 * 
 * @param dst - dst buffer
 * @param src - src buffer
 * @param n - number of bytes to copy
 * @return void*  - pointer to populated memory
 */
void* memmove(void* dst, const void* src, size_t n);

/**
 * @brief sets n bytes from address to given value
 * 
 * @param mem - address of memory buffer
 * @param value - value to set
 * @param n - number of bytes to set
 * @return void* - address of buffer
 */
void* memset(void* mem, int value, size_t n);

/**
 * @brief - calculate length of null terminated string 
 * 
 * @param str - pointer to first char in string
 * @return size_t - length of string
 */
size_t strlen(const char* str);

/**
 * @brief - copy string contents from src to dst
 * 
 * @param dst - dst output string
 * @param src - src input string
 * @return char* - pointer to start of new dst
 */
char* strcpy(char* dst, const char* src);

/**
 * @brief compare to strings up to a length n or a null terminator - whichever is first
 * 
 * @param mem1 - first string
 * @param mem2 - second string
 * @param n - number of chars to compare
 * @return == 0 if equal, 
 *         >0 if first different unsigned char byte value is greater in mem1
 *         <0 if first different unsigned char byte value is greater in mem2
 */
int strncmp(const char* str1, const char* str2, int n);