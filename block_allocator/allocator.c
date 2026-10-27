#include "allocator.h"

#include <assert.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <unistd.h>

struct blk_allocator *blka_new(void)
{
    struct blk_allocator *new = malloc(sizeof(struct blk_allocator));

    if (!new)
    {
        return NULL;
    }

    new->meta = NULL;

    return new;
}

struct blk_meta *blka_alloc(struct blk_allocator *blka, size_t size)
{
    long page_size = sysconf(_SC_PAGESIZE);

    if (page_size < 0)
    {
        return NULL;
    }

    size_t max_size = ~(size_t){ 0 }; // Size max for size_t

    if (size > max_size - sizeof(struct blk_meta)) // Overflow
    {
        return NULL;
    }

    size_t needed = size + sizeof(struct blk_meta);
    size_t page_needed = needed / page_size;

    if (needed % page_size != 0)
    {
        page_needed++;
    }

    if (page_needed > max_size / page_needed) // Overflow
    {
        return NULL;
    }

    struct blk_meta *new =
        mmap(NULL, page_needed * page_size, PROT_READ | PROT_WRITE,
             MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);

    if (new == MAP_FAILED)
    {
        return NULL;
    }

    new->next = blka->meta;
    blka->meta = new;
    new->size = page_needed *page_size - sizeof(struct blk_meta);

    return new;
}

void blka_free(struct blk_meta *blk)
{
    munmap(blk, blk->size + sizeof(struct blk_meta));
}

void blka_pop(struct blk_allocator *blka)
{
    if (!blka->meta)
    {
        return;
    }

    struct blk_meta *head = blka->meta;

    blka->meta = head->next;
    blka_free(head);
}

void blka_delete(struct blk_allocator *blka)
{
    while (blka->meta)
    {
        blka_pop(blka);
    }

    free(blka);
}
