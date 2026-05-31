#include "qpu.h"
#include "lib.h"
#include "mmu.h"
#include "mailbox_interface.h"

#define MAX_PTRS_ALLOC 256
static uint32_t handles[MAX_PTRS_ALLOC];
static uint32_t ptrs[MAX_PTRS_ALLOC];

static bool enbl;
void qpu_enable() {
    if (enbl) return;
    assert(!mbox_set_enable_qpu(1), "Failed QPU enable");
    enbl = true;
}
void qpu_disable() {
    if (!enbl) return;
    assert(!mbox_set_enable_qpu(0), "Failed QPU disable");
    enbl = false;
}

uint32_t qpu_init(uint32_t num_bytes) {
    uint32_t i = 0;
    for (; i < MAX_PTRS_ALLOC && ptrs[i]; i++);

    if (i == MAX_PTRS_ALLOC) {
        printk("qpu_init: WARNING - max pointers allocated\n");
        return 0;
    }

    qpu_enable();

    uint32_t handle = mbox_allocate_memory(num_bytes, PAGE_SIZE, MEM_FLAG_L1_NONALLOCATING);
    if (!handle) {
        printk("qpu_init: WARNING - can't allocate memory");
        return 0;
    }

    uint32_t addr = mbox_lock_memory(handle);
    if (!addr) {
        printk("qpu_init: WARNING - can't lock handle, returning");
        mbox_release_memory(handle);
        return 0;
    }

    handles[i] = handle;
    ptrs[i] = addr;

    return addr;
}

void qpu_free(uint32_t addr) {
    uint32_t i = 0;
    for (; i < MAX_PTRS_ALLOC && ptrs[i] != addr; i++);

    if (i == MAX_PTRS_ALLOC) {
        printk("qpu_free: WARNING - can't find address to free\n");
        return;
    }

    uint32_t handle = handles[i];
    mbox_unlock_memory(handle);
    mbox_release_memory(handle);

    handles[i] = ptrs[i] = 0;
}

void qpu_execute(uint32_t num_qpus, uint32_t* mail) {
    mmu_flush_dcache();

    PUT32(V3D_DBCFG, 0); // Disallow IRQ
    PUT32(V3D_DBQITE, 0); // Disable IRQ
    PUT32(V3D_DBQITC, -1); // Resets IRQ flags

    PUT32(V3D_L2CACTL, 1 << 2); // Clear L2 cache
    PUT32(V3D_SLCACTL, -1); // Clear other caches

    PUT32(V3D_SRQCS, (1 << 7) | (1 << 8) | (1 << 16)); // Reset error bit and counts

    for (uint32_t q = 0; q < num_qpus; q++) {
        PUT32(V3D_SRQUA, mail[q * 2]);
        PUT32(V3D_SRQPC, mail[q * 2 + 1]);
    }

    mmu_flush_dcache();
}

void qpu_wait(uint32_t num_qpus) {
    // Busy wait polling
    while (((GET32(V3D_SRQCS) >> 16) & 0xFF) != num_qpus);
}
void qpu_block() {
    // qpu_wait on number of qpu requests made
    qpu_wait((GET32(V3D_SRQCS) >> 8) & 0xFF);
}

