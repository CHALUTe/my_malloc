#include "my_recycler.h"

#include <stdlib.h>

struct recycler *recycler_create(size_t block_size, size_t total_size)
{
    if (block_size == 0 || total_size == 0
        || block_size % sizeof(long double) != 0
        || total_size % block_size != 0)
    {
        return NULL;
    }

    struct recycler *new = malloc(sizeof(struct recycler));

    if (!new)
    {
        return NULL;
    }

    new->block_size = block_size;
    new->capacity = total_size / block_size;
    new->chunk = malloc(total_size);

    if (!new->chunk)
    {
        free(new);
        return NULL;
    }

    new->free = new->chunk;
    struct free_list *flist = new->chunk;
    char *ptr = new->chunk;

    for (size_t i = 0; i < new->capacity - 1; i++)
    {
        void *address = ptr + (i + 1) * block_size;
        struct free_list *next = address;
        flist->next = next;
        flist = next;
    }

    flist->next = NULL;

    return new;
}

void recycler_destroy(struct recycler *r)
{
    if (!r)
    {
        return;
    }

    free(r->chunk);
    free(r);
}

void *recycler_allocate(struct recycler *r)
{
    if (!r || !r->free)
    {
        return NULL;
    }

    struct free_list *head = r->free;

    r->free = head->next;

    return head;
}

void recycler_free(struct recycler *r, void *block)
{
    if (!r || !block)
    {
        return;
    }

    struct free_list *head = block;
    head->next = r->free;
    r->free = head;
}
