#include "process.h"
#include "vm.h"
#include "kmem.h"
#include "page_alloc.h"
#include "lib.h"
#include "fat.h"
#include "emmc.h"
#include "armv6_asm.h"

#include <stddef.h>

static uint32_t cur_pid = 1;
Process* current_process;

Process* proc_create(const char* filename, uint32_t entry_point) {
    Process* p = (Process*) kmalloc(sizeof(Process));
    memset(p, 0, sizeof(Process));

    p->pid = cur_pid++;
    p->state = PROC_READY;

    Page* l1_pt = page_alloc(2);
    p->l1_pt_paddr = page_paddr(l1_pt);

    volatile uint32_t* l1_pt_vaddr = (volatile uint32_t*) page_vaddr(l1_pt);
    memset((void*) l1_pt_vaddr, 0, L1_NUM_PAGES * sizeof(uint32_t));

    for (int i = (KERNEL_VBASE >> 20); i < L1_NUM_PAGES; i++) {
        l1_pt_vaddr[i] = l1_page_table[i];
    }
    
    p->heap_start = PROC_HEAP_START;
    p->prog_break = PROC_HEAP_START;

    // map stack
    Page* stack = page_alloc(0);
    memset(page_vaddr(stack), 0, PAGE_SIZE);
    map_page_4k(l1_pt_vaddr, PROC_STACK_START, page_paddr(stack), 1);
    p->context.sp = PROC_STACK_START + PAGE_SIZE;

    // map code (1 page for now)
    uint8_t* code_data;
    uint32_t filesize;
    fat_readfile(filename, &code_data, &filesize);
    assert(filesize <= PAGE_SIZE, "Only doing files smaller than a page");

    Page* code = page_alloc(0);
    memset(page_vaddr(code), 0, PAGE_SIZE);
    memcpy(page_vaddr(code), code_data, filesize);
    kfree(code_data);
    map_page_4k(l1_pt_vaddr, PROC_CODE_START, page_paddr(code), 1);

    p->context.pc = entry_point;
    p->context.cpsr = USER_MODE;

    return p;
}
