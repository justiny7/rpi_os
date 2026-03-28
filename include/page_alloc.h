#ifndef PAGE_ALLOC_H
#define PAGE_ALLOC_H

#include "page.h"
#include "phys_mem.h"

void page_alloc_init(PhysMem* pmem);
void* page_alloc(uint8_t order);
void page_free(void* addr, uint8_t order);

void* page_addr(Page* page);
Page* page_get(void* vaddr);

#endif

