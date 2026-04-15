#ifndef VMA_H
#define VMA_H

#include <stdint.h>
#include <stdbool.h>

typedef struct vm_area {
    uint32_t vm_start;
    uint32_t vm_end;
    uint32_t flags;
    struct vm_area* next;
} VMArea;

enum {
    MAP_SHARED      = 0x01,
    MAP_PRIVATE     = 0x02,
    MAP_FIXED       = 0x10,
    MAP_ANONYMOUS   = 0x20,
};

bool vma_check_collision(VMArea* head, uint32_t start, uint32_t n);
uint32_t vma_find_gap(VMArea* head, uint32_t n);
void vma_insert(VMArea** head, VMArea* vma);
void vma_remove(VMArea** head, uint32_t start, uint32_t n);

#endif
