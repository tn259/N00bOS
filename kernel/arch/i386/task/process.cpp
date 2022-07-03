#include "process.h"

#include <stdint.h>

#include "config.h"
#include "fs/file.h"
#include "kernel.h"
#include "libc/string.h"
#include "mm/heap/kheap.h"
#include "status.h"
#include "task.h"

namespace arch::i386::task {

namespace {

process* current_process = nullptr;
process* processes[MAX_PROCESSES];

void init_process(process* proc) {
    memset(proc, 0x00, sizeof(process));
}

/**
 * @brief Load process as a flat binary exe
 * 
 * @param filename 
 * @param proc 
 * @return int 
 */
int load_binary(const char* filename, process* proc) {
    int result       = 0;
    int close_result = 0;
    fs::file_stat stat;
    void* program_data_ptr = nullptr;
    // Read file
    int fd = fs::fopen(filename, "r");
    if (fd < 0) {
        result = -EIO;
        goto out;
    }
    // Obtain the size
    result = fs::fstat(fd, &stat);
    if (result != 0) {
        goto out;
    }
    // Allocate and read in physical memory
    program_data_ptr = mm::heap::kzalloc(stat.filesize);
    if (program_data_ptr == nullptr) {
        result = -ENOMEM;
        goto out;
    }
    if (fs::fread(program_data_ptr, 1, stat.filesize, fd) != 1) {
        result = -EIO;
        goto out;
    }

    proc->phys_mem = program_data_ptr;
    proc->size     = stat.filesize;

out:
    close_result = fs::fclose(fd);
    if (close_result != 0) {
        result = close_result;
    }
    return result;
}

/**
 * @brief Load process as ELF or binary 
 * 
 * @param filename 
 * @param proc 
 * @return int - 0 on success
 */
int load_data(const char* filename, process* proc) {
    // TODO(tn259) implement other exe file formats i.e. ELF
    int result = 0;
    result     = load_binary(filename, proc);
    return result;
}

/**
 * @brief Creates virtual to physical page mapping for binary format process 
 * 
 * @param proc 
 * @return int - 0 on success
 */
int mmap_binary(process* proc) {
    auto* page_directory = proc->task->page_directory->directory_entry;
    auto* virt           = reinterpret_cast<void*>(PROGRAM_VIRTUAL_ADDRESS);
    auto* phys_ptr       = static_cast<uint8_t*>(proc->phys_mem);
    auto* phys_end       = reinterpret_cast<void*>(paging::paging_align(phys_ptr + proc->size));
    int flags            = paging::PAGING_IS_PRESENT | paging::PAGING_ACCESS_FROM_ALL | paging::PAGING_IS_WRITABLE;
    return paging::paging_map(page_directory, virt, proc->phys_mem, phys_end, flags);
}

/**
 * @brief Creates virtual to physical page mapping for the process
 * 
 * @param process 
 * @return int - 0 on success
 */
int mmap_process(process* proc) {
    // TODO(tn259) implement other exe file formats i.e. ELF
    int result = 0;
    result     = mmap_binary(proc);
    return result;
}

} // anonymous namespace

/**
 * @brief Get the current running process 
 * 
 * @return process* 
 */
process* get_current_process() {
    return current_process;
}

/**
 * @brief Get process by pid
 * 
 * @param pid 
 * @return process* 
 */
process* get_process(int pid) {
    if (pid < 0 || pid >= MAX_PROCESSES) {
        return static_cast<process*>(ERROR(-EINVAL));
    }
    return processes[pid];
}

/**
 * @brief Deallocate process
 * 
 * @param proc 
 */
void free_process(process* proc) {
    if (proc->task != nullptr) {
        free_task(proc->task);
    }
    for (void* allocation : proc->allocations) {
        if (allocation != nullptr) {
            mm::heap::kfree(allocation);
        } else {
            break;
        }
    }
    if (proc->phys_mem != nullptr) {
        mm::heap::kfree(static_cast<void*>(proc->phys_mem));
    }
    if (proc->stack != nullptr) {
        mm::heap::kfree(static_cast<void*>(proc->stack));
    }
    mm::heap::kfree(static_cast<void*>(proc));
}

/**
 * @brief Loads exe filename as a process identified by pid
 * 
 * @param filename 
 * @param proc 
 * @param pid 
 * @return int - 0 on success
 */
int load_process(const char* filename, process** proc, int pid) {
    // Check no process with pid already exisits
    if (get_process(pid) != nullptr) {
        return -EINVAL;
    }

    // init process struct
    auto* tmp_proc = static_cast<process*>(mm::heap::kzalloc(sizeof(process)));
    if (tmp_proc == nullptr) {
        return -ENOMEM;
    }
    init_process(tmp_proc);

    // Load program data from file
    void* program_stack_ptr = nullptr;
    task_t* task            = nullptr;
    int result              = load_data(filename, tmp_proc);
    if (result < 0) {
        goto out;
    }

    // Allocate stack
    program_stack_ptr = mm::heap::kzalloc(PROGRAM_USER_PROGRAM_STACK_SIZE);
    if (program_stack_ptr == nullptr) {
        result = -ENOMEM;
        goto out;
    }

    // Assign filename
    strncpy(static_cast<char*>(tmp_proc->name), filename, sizeof(tmp_proc->name));
    tmp_proc->stack = program_stack_ptr;
    tmp_proc->pid   = pid;

    // New task for process
    task = new_task(tmp_proc);
    if (ERROR_I(task) == 0) {
        result = ERROR_I(task);
        goto out;
    }
    tmp_proc->task = task;

    // map virtual to physical address space of process
    result = mmap_process(tmp_proc);

    *proc          = tmp_proc;
    processes[pid] = tmp_proc;

out:
    if (result < 0) {
        if (tmp_proc != nullptr) {
            free_process(tmp_proc);
        }
    }
    return result;
}

} // namespace arch::i386::task