#ifndef PAGE_ALLOC_H
#define PAGE_ALLOC_H

#include "phys_mem.h"
#include "linked_list.h"

typedef struct Page Page;

struct Page {
    union {
        Page* next_free_page;
        struct {
            LList ll;
            void* free_list;
            void* cache;
            uint32_t used;
        } slab;
    };
    uint32_t flags;
};

void page_alloc_init(PhysMem* pmem);
void* page_alloc(uint32_t order);
void page_free(void* addr, uint32_t order);

void* page_addr(Page* page);
Page* page_get(uint32_t paddr);

#endif
