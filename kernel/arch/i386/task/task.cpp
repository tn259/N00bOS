#include "task.h"
#include "paging/paging.h"

#include "mm/heap/kheap.h"

#include "status.h"
#include "config.h"
#include "kernel.h"

#include "libc/string.h"

namespace arch::i386::task {

namespace {

task_t* current = nullptr;
task_t* tail = nullptr;
task_t* head = nullptr;

/**
 * @brief Removes a task from the linked list of tasks 
 * 
 * @param task 
 */
void remove_task(task_t* task) {
    auto* prev = task->prev;
    auto* next = task->next;
    if (prev != nullptr) {
        prev->next = next;
    }
    if (next != nullptr) {
        next->prev = prev;
    }
    if (task == head) {
        head = next;
    }
    if (task == tail) {
        tail = prev;
    }
    if (task == current) {
        current = get_next_task();
    }
    task->prev = nullptr;
    task->next = nullptr;
}

int init_task(task_t* task) {
    memset(task, 0x00, sizeof(task));
    task->page_directory = paging::paging_new(paging::PAGING_IS_PRESENT | paging::PAGING_ACCESS_FROM_ALL);
    if (task->page_directory == nullptr) {
        return -EIO;
    }

    task->reg.ip = PROGRAM_VIRTUAL_ADDRESS;
    task->reg.ss = USER_DATA_SEGMENT;
    task->reg.esp = PROGRAM_VIRTUAL_STACK_ADDRESS_START;

    return 0;
}

}  // anonymous namespace


task_t* current_task() {
    return current;
}

task_t* new_task(process* proc) {
    auto* task = static_cast<task_t*>(mm::heap::kzalloc(sizeof(task_t)));
    if (task == nullptr) {
        return static_cast<task_t*>(ERROR(-ENOMEM));
    }

    int result = init_task(task);
    if (result != 0) {
        free_task(task);
        return static_cast<task_t*>(ERROR(result));
    }

    if (head == nullptr) {
        head = task;
        tail = task;
        return 0;
    }

    tail->next = task;
    task->prev = tail;
    tail = task;

    proc->task = task;

    return task;
}

task_t* get_next_task() {
    // Wrap arround if we are at the end of the list
    if (current->next == nullptr) {
        return head;
    }
    return current->next;
}

void free_task(task_t* task) {
    paging::paging_free(task->page_directory);
    remove_task(task);
    mm::heap::kfree(task);
}


}