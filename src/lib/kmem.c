#include "page_alloc.h"
#include "vm.h"
#include "kmem.h"
#include "lib.h"

#include <stddef.h>

// #define DEBUG

static KMemCache kmem_caches[KMEM_NUM_CACHES];
static uint32_t kmem_cache_size[KMEM_NUM_CACHES] = {
    32,
    64,
    128,
    256,
    512,
    1024,
    2048
};

void kmem_init() {
    for (int i = 0; i < KMEM_NUM_CACHES; i++) {
        kmem_caches[i].size = kmem_cache_size[i];
        ll_init(&kmem_caches[i].slabs_free);
        ll_init(&kmem_caches[i].slabs_partial);
        ll_init(&kmem_caches[i].slabs_full);
    }
}

static int find_size(uint32_t size) {
    int idx = -1;
    for (int i = 0; i < KMEM_NUM_CACHES; i++) {
        if (kmem_cache_size[i] >= size) {
            idx = i;
            break;
        }
    }
    return idx;
}
static Page* format_slab(KMemCache* cache) {
    uint32_t size = cache->size;

    uint32_t new_slab_paddr = (uint32_t) page_alloc(0);
    uint32_t new_slab_vaddr = (uint32_t) __va(new_slab_paddr);
    map_page_4k(new_slab_vaddr, new_slab_paddr);

    uint32_t num_objs = PAGE_SIZE / size;

#ifdef DEBUG
    printk("alloc'ed new slab with %d objs\n", num_objs);
    printk("paddr: %x\n", new_slab_paddr);
    printk("vaddr: %x\n", new_slab_vaddr);
#endif

    Page* page = page_get(new_slab_vaddr);
    page->flags |= 1;

    ll_init(&page->slab.ll);
    page->slab.used = 0;
    page->slab.cache = cache;
    page->slab.free_list = (void*) new_slab_vaddr;

    void* cur = page->slab.free_list;
    for (uint32_t i = 1; i < num_objs; i++) {
        *((void**) cur) = (void*) (new_slab_vaddr + i * size);
        cur = *((void**) cur);
    }
    *((void**) cur) = NULL;

    return page;
}
void* kmalloc(uint32_t size) {
    int idx = find_size(size);
    if (idx == -1) {
        return NULL;
    }

    KMemCache* cache = &kmem_caches[idx];

    if (!cache->slabs_partial.next && !cache->slabs_free.next) {
        // get new slab from RAM, add to partial list (presumably we're about to use it)
        Page* new_page = format_slab(cache);
        ll_insert(&new_page->slab.ll, &cache->slabs_partial);
    } else if (!cache->slabs_partial.next) {
        // move free slab to partial list
        ll_insert(ll_remove(cache->slabs_free.next), &cache->slabs_partial);
    }

    LList* partial_ll = cache->slabs_partial.next;
    assert(partial_ll, "No partial slabs available");

    Page* page = container_of(partial_ll, Page, slab.ll);

    void* alloc_chunk = page->slab.free_list;
    page->slab.free_list = *((void**) alloc_chunk);
    page->slab.used++;

    if (!page->slab.free_list) {
        ll_insert(ll_remove(partial_ll), &cache->slabs_full);
    }

    return alloc_chunk;
}
void kfree(void* ptr) {
    if (!ptr) return;

    Page* page = page_get(((uint32_t) ptr) & ~(PAGE_SIZE - 1));
    assert(page->flags & 1, "Page not allocated to slab");
    assert(page->slab.used, "Page is empty");

    KMemCache* cache = page->slab.cache;

    // prepend to free list
    *((void**) ptr) = page->slab.free_list;
    page->slab.free_list = ptr;

    if (page->slab.used-- == (PAGE_SIZE / cache->size)) {
        ll_insert(ll_remove(&page->slab.ll), &cache->slabs_partial);
    } else if (!page->slab.used) {
        ll_insert(ll_remove(&page->slab.ll), &cache->slabs_free);
    }
}
uint32_t kmem_shrink_cache(KMemCache* cache) {
    uint32_t freed = 0;
    while (cache->slabs_free.next) {
        LList* free_ll = ll_remove(cache->slabs_free.next);
        Page* page = container_of(free_ll, Page, slab.ll);

        page->flags &= ~1;

        page_free(page_addr(page), 0);
        freed++;
    }

    return freed;
}
uint32_t kmem_shrink_caches_all() {
    uint32_t freed = 0;
    for (int i = 0; i < KMEM_NUM_CACHES; i++) {
        freed += kmem_shrink_cache(&kmem_caches[i]);
    }
    return freed;
}
