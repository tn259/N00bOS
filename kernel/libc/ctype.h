#pragma once

/**
 * @brief Tests if character supplied is a numberic digit in the range 0-9
 *
 * ch is assumed to be representable by an unsigned char
 * 
 * @param ch - input character
 * @return int - 0 if true, -1 if false 
 */
int isdigit(int ch);

/**
 * @brief returns the lowercase character value
 *
 * Examples:
 * 'A' -> 'a'
 * 'b' -> 'b'
 *  
 * @param ch - input character
 * @return int - lowercase character
 */
int tolower(int ch);