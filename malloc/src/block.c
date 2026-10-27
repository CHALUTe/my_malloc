#include "malloc.h"

struct block *find_free_block(size_t size)
{
    struct page *page = get_pages();

    while (page)
    {
        struct block *block = page->first;

        while (block)
        {
            if (block->size >= size && block->free == 1)
            {
                return block;
            }

            block = block->next;
        }

        page = page->next;
    }

    return NULL;
}

void split_block(struct block *block, size_t size)
{
    if (!block)
    {
        return;
    }

    size_t aligned_size = alignment(size);
    size_t meta_size = alignment(sizeof(struct block));

    if (aligned_size == 0 || aligned_size > block->size)
    {
        return;
    }

    size_t remaining = block->size - aligned_size;

    if (remaining < meta_size || remaining - meta_size < sizeof(long double))
    {
        return;
    }

    void *memory = block;
    char *start = memory;
    start += aligned_size + meta_size;
    void *address = start;
    struct block *new_block = address;

    new_block->size = remaining - meta_size;
    new_block->next = block->next;
    new_block->prev = block;
    new_block->free = 1;

    block->size = aligned_size;
    block->next = new_block;

    if (new_block->next)
    {
        new_block->next->prev = new_block;
    }
}

//----------------------------------------------
//                    Merge
//----------------------------------------------

static void merge_next(struct block *block)
{
    struct block *next = block->next;

    if (!next || !next->free)
        return;

    size_t meta_size = alignment(sizeof(struct block));

    block->size += meta_size + next->size;
    block->next = next->next;

    if (block->next)
        block->next->prev = block;
}

void merge_blocks(struct block *block)
{
    if (!block || !block->free)
        return;

    while (block->next && block->next->free)
        merge_next(block);

    if (block->prev && block->prev->free)
    {
        block = block->prev;

        while (block->next && block->next->free)
            merge_next(block);
    }
}

//----------------------------------------------
//              EXPAND BLOCK
//----------------------------------------------

int expand_block(struct block *block, size_t size) // for realloc
{
    if (!block)
    {
        return 0;
    }

    while (block->size < size && block->next && block->next->free)
    {
        merge_next(block);
    }

    return block->size >= size;
}
