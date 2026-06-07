#ifndef VMALLOC_H
#define VMALLOC_H

#include "vm.h"

#include <stdbool.h>

#define VMALLOC_BASE    KERNEL_VBASE_VMALLOC          // 0xE1000000
#define VMALLOC_SIZE    MiB(128)                      // 0x08000000
#define VMALLOC_END     (VMALLOC_BASE + VMALLOC_SIZE) // 0xE9000000

typedef struct vm_struct {
    uint32_t addr;
    uint32_t size; // in bytes, page-aligned
    struct vm_struct* next;
} VMStruct;

void vmalloc_init();
bool vmalloc_addr(void* ptr);
void* vmalloc(uint32_t size);
void vfree(void* addr);

#endif
