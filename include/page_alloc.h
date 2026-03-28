#ifndef PAGE_ALLOC_H
#define PAGE_ALLOC_H

#include "phys_mem.h"
#include "linked_list.h"

// max 4MB sections (2^10 * sizeof(Page))
#define MAX_PAGE_ORDER 11

typedef struct Page Page;

struct Page {
    union {
        LList ll;
        struct {
            LList ll;
            void* free_list;
            void* cache;
            uint32_t used;
        } slab;
    };
    uint8_t used;
    uint8_t order;
    uint32_t flags;
};

void page_alloc_init(PhysMem* pmem);
void* page_alloc(uint32_t order);
void page_free(void* addr, uint32_t order);

void* page_addr(Page* page);
Page* page_get(void* vaddr);

#endif

