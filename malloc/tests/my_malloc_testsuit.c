#include "my_malloc_testsuit.h"

#include <stdlib.h>

#include "../src/malloc.h"

void *my_malloc(size_t size)
{
    if (size == 0)
    {
        size = 1;
    }

    size_t aligned_size = alignment(size);

    if (aligned_size == 0)
    {
        return NULL;
    }

    struct block *block = find_free_block(aligned_size);

    if (!block)
    {
        struct page *new_page = create_page(aligned_size);

        if (!new_page)
        {
            return NULL;
        }

        block = new_page->first;
    }
    if (block->size < aligned_size)
    {
        return NULL;
    }

    split_block(block, aligned_size);
    block->free = 0;

    size_t meta_size = alignment(sizeof(struct block));
    void *memory = block;
    char *start = memory;

    return start + meta_size;
}

void my_free(void *ptr)
{
    if (!ptr)
        return;

    size_t meta_size = alignment(sizeof(struct block));

    char *start = ptr;
    void *address = start - meta_size;
    struct block *block = address;

    block->free = 1;
    merge_blocks(block);
    release_empty_pages();
}

void *my_calloc(size_t nmemb, size_t size)
{
    size_t total;

    if (__builtin_mul_overflow(nmemb, size, &total))
    {
        return NULL;
    }

    void *ptr = my_malloc(total);

    if (!ptr)
    {
        return NULL;
    }

    unsigned char *bytes = ptr;

    for (size_t i = 0; i < total; i++)
    {
        bytes[i] = 0;
    }

    return ptr;
}

void *my_realloc(void *ptr, size_t size)
{
    if (!ptr)
    {
        return my_malloc(size);
    }

    if (size == 0)
    {
        my_free(ptr);
        return NULL;
    }

    size_t meta_size = alignment(sizeof(struct block));
    char *start = ptr;
    void *address = start - meta_size;
    struct block *block = address;

    if (size <= block->size)
    {
        return ptr;
    }

    size_t aligned_size = alignment(size);

    if (aligned_size == 0)
    {
        return NULL;
    }

    size_t old_size = block->size;

    if (expand_block(block, aligned_size))
    {
        split_block(block, aligned_size);
        return ptr;
    }

    void *new_ptr = my_malloc(size);

    if (!new_ptr)
    {
        return NULL;
    }

    unsigned char *src = ptr;
    unsigned char *dst = new_ptr;

    for (size_t i = 0; i < old_size; i++)
    {
        dst[i] = src[i];
    }

    my_free(ptr);
    return new_ptr;
}
