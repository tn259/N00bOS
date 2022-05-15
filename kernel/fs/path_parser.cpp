#include "path_parser.h"

#include "mm/heap/kheap.h"

#include "libc/ctype.h"
#include "libc/string.h"
#include "libc/stdlib.h"

namespace fs {

namespace {

/**
 * @brief validates that the absolute path is of the form "[0-9]:/.*"
 * 
 * @param path 
 * @return true
 * @return false 
 */
bool validate_path(const char* path) {
    if (strlen(path) >= 3) {
        char drive_num = path[0];
        char colon = path[1];
        char slash = path[2];
        return (isdigit(drive_num) == 0 && colon == ':' && slash == '/');
    }
    return false;
}

/**
 * @brief Get the drive number from the absolute path
 * 
 * @param path 
 * @return int - -1 if invalid
 */
int get_drive(const char* path) {
    auto* drive_str = static_cast<char*>(mm::heap::kzalloc(2));
    drive_str[0] = path[0];
    auto drive = atoi(drive_str);
    mm::heap::kfree(drive_str);
    return drive;
}

/**
 * @brief Create a path_root
 * 
 * @param drive_number 
 * @return path_root* 
 */
path_root* create_root(int drive_number) {
    auto* root = static_cast<path_root*>(mm::heap::kzalloc(sizeof(path_root)));
    root->drive_number = drive_number;
    root->path = nullptr;
    return root;
}

/**
 * @brief Get the path part from path
 * 
 * @param path  e.g. /a/b/c/
 * @return char* e.g. 'a'
 */
char* get_path_part_str(const char** path) {
    // find the path part length
    const char* path_ptr = *path;
    const auto** tmp_path = path;
    int idx = 0;
    while (**tmp_path != '/' && **tmp_path != '\0') {
        *tmp_path += 1;
        ++idx;
    }
    if (idx == 0) {
        return nullptr;
    }
    // allocate string and set
    auto* path_part_str = static_cast<char*>(mm::heap::kzalloc(idx+1)); // +1 for the null terminator
    idx = 0;
    while (*path_ptr != '\0' && *path_ptr != '/') {
        path_part_str[idx] = *path_ptr;
        ++idx;
        ++path_ptr;
    }
    // skip past the '/'
    if (*path_ptr == '/') {
        path_ptr += 1;
    }
    *path = path_ptr;
    return path_part_str;
}

/**
 * @brief Appends a new path_part onto the previous part 
 * 
 * @param previous_part 
 * @param path - points to the path_part string to be parsed
 * @return path_part* - the parsed path part
 */
path_part* parse_path_part(path_part* previous_part, const char** path) {
    auto* path_part_str = get_path_part_str(path);
    // allocate new path part
    auto* current_part = static_cast<path_part*>(mm::heap::kzalloc(sizeof(path_part)));
    current_part->part_name = path_part_str;
    current_part->next = nullptr;
    // set prev next to current
    if (previous_part != nullptr) {
        previous_part->next = current_part;
    }
    return current_part;
}

}  // namespace anonymous

path_root* parse(const char* path) {
    if (strlen(path) > MAX_PATH_LENGTH) {
        return nullptr;
    }

    auto drive_number = get_drive(path);
    if (drive_number < 0) {
        return nullptr;
    }

    auto* root = create_root(drive_number);

    path += 3; // e.g. skip over '0:/'

    const auto* tmp_path = path;
    path_part* part = parse_path_part(nullptr, &tmp_path);
    root->path = part; // set first part
    while (*tmp_path != '\0') {
        part = parse_path_part(part, &tmp_path);
    }
    return root;
}

void free(path_root* root) {
    auto* part = root->path;
    while (part != nullptr) {
        auto* next = part->next;
        mm::heap::kfree(part->part_name);
        mm::heap::kfree(part);
        part = next;
    }
    mm::heap::kfree(root);
}

}  // namespace fs