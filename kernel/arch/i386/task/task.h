#pragma once

#include <stdint.h>

#include "arch/i386/paging/paging.h"

namespace arch::i386::task {

struct registers {
    uint32_t edi;
    uint32_t esi;
    uint32_t ebp;
    uint32_t ebx;
    uint32_t edx;
    uint32_t ecx;
    uint32_t eax;

    uint32_t ip;
    uint32_t cs;
    uint32_t eflags;
    uint32_t esp;
    uint32_t ss;
};

struct task_t {
    // page directory of the task
    paging::paging_chunk* page_directory;
    // register state when the task is not running
    registers reg;

    task_t* next;
    task_t* prev;
};

struct process;

task_t* new_task(process* proc);
task_t* current_task();
task_t* get_next_task();
void free_task(task_t* task);

} // namespace arch::i386::task