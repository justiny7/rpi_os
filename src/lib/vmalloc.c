#include "vmalloc.h"
#include "page_alloc.h"
#include "kmem.h"
#include "lib.h"

#include <stddef.h>

static VMStruct* vm_list;

void vmalloc_init() {
    vm_list = NULL;
}

static uint32_t find_free_range(uint32_t size) {
    uint32_t candidate = VMALLOC_BASE;

    for (VMStruct* cur = vm_list; cur; cur = cur->next) {
        if (candidate + size <= cur->addr) {
            return candidate;
        }
        candidate = cur->addr + cur->size;
    }

    return (candidate + size <= VMALLOC_END ? candidate : 0);
}

void* vmalloc(uint32_t size) {
    if (!size) return NULL;

    size = PAGE_ALIGN_UP(size);

    uint32_t va = find_free_range(size);
    if (!va) return NULL;

    uint32_t num_pages = size / PAGE_SIZE;
    for (uint32_t i = 0; i < num_pages; i++) {
        Page* page = page_alloc(0);
        if (!page) {
            // undo what we mapped so far
            for (uint32_t j = 0; j < i; j++) {
                uint32_t paddr = unmap_page_4k(l1_page_table, va + j * PAGE_SIZE);
                page_free(page_get_p(paddr), 0);
            }
            return NULL;
        }

        map_page_4k(l1_page_table, va + i * PAGE_SIZE, page_paddr(page), 0);
    }

    VMStruct* vm = kmalloc(sizeof(VMStruct));
    vm->addr = va;
    vm->size = size;

    // insert sorted by address
    VMStruct** pp = &vm_list;
    for (; *pp && (*pp)->addr < va; pp = &(*pp)->next);
    vm->next = *pp;
    *pp = vm;

    return (void*) va;
}

void vfree(void* addr) {
    if (!addr || !vmalloc_addr(addr)) return;

    uint32_t va = (uint32_t) addr;

    VMStruct** pp = &vm_list;
    for (; *pp && (*pp)->addr != va; pp = &(*pp)->next);

    if (!*pp) return;

    VMStruct* vm = *pp;
    *pp = vm->next;

    uint32_t num_pages = vm->size / PAGE_SIZE;
    for (uint32_t i = 0; i < num_pages; i++) {
        uint32_t paddr = unmap_page_4k(l1_page_table, va + i * PAGE_SIZE);
        if (paddr) {
            Page* page = page_get_p(paddr);
            page_free(page, 0);
        }
    }

    kfree(vm);
}

bool vmalloc_addr(void* ptr) {
    uintptr_t p = (uintptr_t) ptr;
    return (p >= VMALLOC_BASE && p < VMALLOC_END);
}
