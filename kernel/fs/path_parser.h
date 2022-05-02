#pragma once

#include <stddef.h>

static const constexpr size_t MAX_PATH_LENGTH = 4096;

/**
 * @brief Path part which includes a name and a linked list next element
 * 
 */
struct path_part {
    char* part_name;
    path_part* next;
};

/**
 * @brief Start of absolute path from drive denoted by idx e.g. 0:/a/b/c
 * 
 */
struct path_root {
    int drive_number;
    path_part* path;
};

/**
 * @brief parses path string into path structures above
 * 
 * @param path - string name of path
 * @return path_root* - root of parsed path
 */
path_root* parse(const char* path);

/**
 * @brief deallocates the memory for the parsed path
 */
void free(path_root* root);