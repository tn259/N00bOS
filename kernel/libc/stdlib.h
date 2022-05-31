#pragma once

/**
 * @brief Converts string to integer where string contains numeric digits
 * 
 * @param str 
 * @return int 
 */
int atoi(const char* str);

/**
 * @brief Converts integer value to a string stored in str according to the base provided
 * 
 * @param value - int value
 * @param str - output string
 * @param base - base to do the conversion
 * @return char* - output string
 */
char* itoa(int value, char* str, int base);