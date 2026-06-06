#include "lib.h"
#include "fat.h"
#include "process.h"
#include "vmalloc.h"

#define DEBUG
#include "debug.h"

void main() {
    fat_init();

    printk("vmalloc test\n");
    void* vp = vmalloc(MiB(100));
    printk("addr at: %x\n", (uint32_t) vp);
    vfree(vp);
    printk("done\n");

    Process* p = proc_create("TEST4   ELF");
    // Process* p = proc_create("TEST3   ELF");
    if (!p) {
        panic("can't create process\n");
    }

    current_process = p;

    printk("Dropping into User Mode...\n");
    printk("%x\n", p->context.pc);
    proc_run(&p->context, p->l1_pt_paddr);

    panic("shouldn't reach here\n");
}
