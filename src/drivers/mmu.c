#include "mmu.h"

static __attribute__((section(".mmu_table"))) volatile PageTableEntry page_table[NUM_PAGES];

void page_table_init() {
    for (int i = 0; i < NUM_PAGES; i++) {
        volatile PageTableEntry* pte = &page_table[i];
        int is_ram = (i < PERIPHERAL_PAGE);

        // most attributes are 0 by default
        pte->val = 0;

        // (potentially) non-zero attributes
        pte->tag = 0b10;
        pte->B = is_ram;
        pte->C = is_ram;
        pte->XN = !is_ram;
        pte->AP = AP_RW;
        pte->addr = i;
    }
}

