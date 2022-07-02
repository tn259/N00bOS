#pragma once

#include "task.h"
#include "config.h"

#include <stdint.h>

namespace arch::i386::task {

struct process {
    uint16_t pid;

    char name[FS_MAX_PATH_SIZE];

    task_t* task;

    // All of the allocations made by the process
    void* allocations[PROGRAM_MAX_ALLOCATIONS];

    // Physical memory of the process memory
    void* phys_mem;
    // Size of memory pointed to by phys_mem
    uint32_t size;

    void* stack;
};

}