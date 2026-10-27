#include "malloc.h"

static struct page *pages = NULL;

struct page *get_pages(void)
{
    return pages;
}

static struct page *init_page(void *memory, size_t size)
{
    size_t page_meta_size = alignment(sizeof(struct page));
    size_t block_meta_size = alignment(sizeof(struct block));

    struct page *page = memory;
    page->size = size;

    char *start = memory;
    void *first = start + page_meta_size;
    page->first = first;

    struct block *block = page->first;
    block->size = size - page_meta_size - block_meta_size;
    block->next = NULL;
    block->prev = NULL;
    block->free = 1;

    page->next = pages;
    pages = page;

    return page;
}

static size_t get_mapping_size(size_t size) // For big allocation
{
    long p_size = sysconf(_SC_PAGESIZE);

    if (p_size <= 0)
    {
        return 0;
    }

    size_t page_size = p_size;
    size_t aligned_size = alignment(size);
    size_t page_meta_size = alignment(sizeof(struct page));
    size_t block_meta_size = alignment(sizeof(struct block));

    if (!aligned_size || !page_meta_size || !block_meta_size)
    {
        return 0;
    }

    size_t total;

    if (__builtin_add_overflow(aligned_size, page_meta_size, &total)
        || __builtin_add_overflow(total, block_meta_size, &total))
    {
        return 0;
    }

    size_t remainder = total % page_size;

    if (remainder != 0
        && __builtin_add_overflow(total, page_size - remainder, &total))
    {
        return 0;
    }

    return total;
}

struct page *create_page(size_t size)
{
    size_t mapping_size = get_mapping_size(size);

    if (mapping_size == 0)
    {
        return NULL;
    }

    void *memory = mmap(NULL, mapping_size, PROT_READ | PROT_WRITE,
                        MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);

    if (memory == MAP_FAILED)
    {
        return NULL;
    }

    return init_page(memory, mapping_size);
}

/*
struct page *create_page(void)
{
    long p_size = sysconf(_SC_PAGESIZE);

    if (p_size <= 0)
    {
        return NULL;
    }

    size_t page_size = p_size;

    size_t page_meta_size = alignment(sizeof(struct page));
    size_t block_meta_size = alignment(sizeof(struct block));

    if (page_meta_size == 0 || block_meta_size == 0
        || page_meta_size > page_size
        || block_meta_size > page_size - page_meta_size)
    {
        return NULL;
    }

    void *memory = mmap(NULL, page_size, PROT_READ | PROT_WRITE,
                        MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);

    if (memory == MAP_FAILED)
    {
        return NULL;
    }

    return init_page(memory, page_size);
} */

void release_empty_pages(void)
{
    struct page **current = &pages;

    while (*current)
    {
        struct page *page = *current;
        struct block *block = page->first;
        size_t meta =
            alignment(sizeof(struct page)) + alignment(sizeof(struct block));

        if (block->free && !block->next && block->size == page->size - meta)
        {
            struct page *next = page->next;

            if (munmap(page, page->size) == 0)
            {
                *current = next;
                continue;
            }
        }
        current = &page->next;
    }
}
