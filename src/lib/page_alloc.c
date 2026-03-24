#include "page_alloc.h"
#include "lib.h"
#include "vm.h"

static Page mem_map[NUM_PAGES];
static Page* free_pages_head;

void page_alloc_init(PhysMem* pmem) {
    uint32_t start = pmem->regions[0].start / PAGE_SIZE;
    uint32_t end = (pmem->regions[0].start + pmem->regions[0].size) / PAGE_SIZE;

    for (uint32_t i = end - 1; i >= start; i--) {
        mem_map[i].flags = 0;
        mem_map[i].next_free_page = free_pages_head;
        free_pages_head = &mem_map[i];
    }
}
void* page_alloc(uint32_t order) {
    assert(order == 0, "Only doing 4KB pages for now");

    Page* page = free_pages_head;
    assert(page, "No more pages to alloc");

    free_pages_head = page->next_free_page;
    return page_addr(page);
}
void page_free(void* vaddr, uint32_t order) {
    assert(order == 0, "Only doing 4KB pages for now");

    Page* page = page_get((uint32_t) vaddr);
    page->next_free_page = free_pages_head;
    free_pages_head = page;
}

void* page_addr(Page* page) {
    return __va((page - mem_map) * PAGE_SIZE);
}
Page* page_get(uint32_t vaddr) {
    assert(vaddr % PAGE_SIZE == 0, "Invalid physical address");
    return &mem_map[__pa(vaddr) / PAGE_SIZE];
}

