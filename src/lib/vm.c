#include "vm.h"
#include "page_alloc.h"
#include "mmu.h"
#include "lib.h"

#include <stddef.h>

__attribute__((section(".l1_page_table")))
static volatile uint32_t l1_page_table[L1_NUM_PAGES];

void l1_page_table_init() {
    // use physical address bc this is called before MMU turns on
    volatile uint32_t* phys_l1_pt = (volatile uint32_t*) __pa(l1_page_table);

    uint32_t vbase_ram_idx = KERNEL_VBASE_RAM >> 20;
    uint32_t vbase_peri_idx = KERNEL_VBASE_PERI >> 20;

    // map all RAM to high addresses
    for (uint32_t i = 0; i < TOTAL_MEM_MB; i++) {
        // tag | bufferable + cacheable | RW perms | paddr
        phys_l1_pt[vbase_ram_idx + i] =
            (0b10) | (0b11 << 2) | (AP_RW << 10) | (i << 20);
    }

    // map peripherals 
    for (uint32_t i = 0; i < PERI_MEM_MB; i++) {
        // tag | execute-never | RW perms | paddr
        phys_l1_pt[vbase_peri_idx + i] =
            (0b10) | (1 << 4) | (AP_RW << 10) | ((i << 20) + PERI_PBASE);
    }

    // map page 0 for booting
    phys_l1_pt[0] = (0b10) | (0b11 << 2) | (AP_RW << 10);
}

volatile void* l2_page_table_init() {
    static uintptr_t cur_page;
    static uint32_t table_count = 4;

    if (table_count == 4) {
        table_count = 0;
        cur_page = (uintptr_t) page_alloc(0);

        // we can fit 4 L2 tables in a single page
        for (uint32_t i = 0; i < L2_NUM_PAGES * 4; i++) {
            *((volatile uint32_t*) cur_page + i) = 0;
        }
    }

    return (volatile void*) (cur_page + (L2_NUM_PAGES * sizeof(uint32_t) * table_count++));
}

void map_page_4k(uint32_t vaddr, uint32_t paddr) {
    assert((vaddr & 0xFFF) == 0, "vaddr not aligned to 4KB");
    assert((paddr & 0xFFF) == 0, "paddr not aligned to 4KB");

    uint32_t l1_idx = vaddr >> 20;
    uint32_t l2_idx = (vaddr >> 12) & 0xFF;

    volatile uint32_t* l1_pte = &l1_page_table[l1_idx];
    volatile uint32_t* l2_table = NULL;

    if ((*l1_pte & 3) == 0b01) {
        l2_table = (volatile uint32_t*) __va(*l1_pte & 0xFFFFFC00); // this is a coarse PTE, not section
    } else {
        assert(*l1_pte == 0, "L1 section already mapped!\n");

        // allocate new L2 table + map L1 to coarse PTE
        l2_table = l2_page_table_init();
        *l1_pte = __pa(l2_table) | 0b01;
    }

    // physical address | cacheable & bufferable | RW perms | small PTE + XN=0
    l2_table[l2_idx] = (paddr & 0xFFFFF000) |
        (0b11 << 2) | (AP_RW << 4) | (0b10);

    mmu_sync_ptes();
}
