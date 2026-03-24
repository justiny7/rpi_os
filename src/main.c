#include "uart.h"
#include "lib.h"
#include "page_alloc.h"
#include "vm.h"
#include "kmem.h"

#define DEBUG
#include "debug.h"

void main() {
    uint32_t sp;
    asm volatile ("mov %0, sp" :: "r"(sp));
    printk("main sp: %x, %b, %d\n", sp, sp, sp);

    // uint32_t addr = 0x21000000;
    // GET32(addr);

    /*
    void* pg1 = page_alloc(0);
    void* pg2 = page_alloc(0);
    printk("pg1: %d, pg2: %d\n", (uint32_t) pg1, (uint32_t) pg2);

    uint32_t paddr = 0x00100000, vaddr = 0x00200000;
    map_page_4k(paddr, paddr);

    PUT32(paddr, 0xdeafbeef);
    printk("before map: %x\n", GET32(paddr));

    map_page_4k(vaddr, paddr);

    PUT32(vaddr, 0xFEEDFACE);
    printk("after map: %x\n", GET32(paddr));
    */

    // printk("%x\n", GET32(0x00BFF800));
    // printk("%x\n", GET32(0x00C00000));

    const int N = 10;
    void* addr[N];
    for (int i = 0; i < 227325; i++) {
        void* v = kmalloc(2048);

        if (i < N) {
            addr[i] = v;
            printk("%x\n", addr[i]);
        }

        /*
        if (i > 122000) {
            printk("%x\n", v);
            printk("%x\n", __pa(v));
            printk("%d\n", i);
        }
        */
    }
    kfree(addr[0]);
    kfree(addr[1]);
    printk("freeing caches: %d\n", kmem_shrink_caches_all());

    printk("ALLOC\n");
    void* addr1 = kmalloc(32);
    printk("32: %x\n", addr1);

    printk("NO ALLOC\n");
    void* addr2 = kmalloc(32);
    printk("32: %x\n", addr2);

    printk("ALLOC\n");
    void* addr3 = kmalloc(128);
    printk("128: %x\n", addr3);

    printk("NO ALLOC\n");
    void* addr4 = kmalloc(128);
    printk("128: %x\n", addr4);

    for (int i = 1; i < N; i++) {
        kfree(addr[i]);
    }
    printk("freeing caches: %d\n", kmem_shrink_caches_all());

    printk("ALLOC\n");
    void* addr5 = kmalloc(2048);
    printk("2048: %x\n", addr5);

    printk("NO ALLOC\n");
    void* addr6 = kmalloc(2048);
    printk("2048: %x\n", addr6);

    printk("ALLOC\n");
    void* addr7 = kmalloc(2048);
    printk("2048: %x\n", addr7);

    kfree(addr5);
    kfree(addr6);

    printk("NO ALLOC\n");
    void* addr8 = kmalloc(2048);
    printk("2048: %x\n", addr8);

    printk("NO ALLOC\n");
    void* addr9 = kmalloc(2048);
    printk("2048: %x\n", addr9);

    printk("NO ALLOC\n");
    void* addr10 = kmalloc(2048);
    printk("2048: %x\n", addr10);

    printk("ALLOC\n");
    void* addr11 = kmalloc(2048);
    printk("2048: %x\n", addr11);
}
