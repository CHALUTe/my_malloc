#ifndef MALLOC_H
#define MALLOC_H

#include <stddef.h>
#include <sys/mman.h>
#include <unistd.h>

//-------------------------------------
//              STRUCT
//-------------------------------------

struct block
{
    size_t size;
    struct block *next;
    struct block *prev;
    int free; // 1 if block is free, 0 if not
};

struct page
{
    size_t size; // total size of all page
    struct page *next;
    struct block *first; // first bloc of the page
};

//-------------------------------------
//            ALIGNMENT
//-------------------------------------

size_t alignment(size_t size);

//-------------------------------------
//              PAGE
//-------------------------------------

struct page *create_page(size_t size);
struct page *get_pages(void);
void release_empty_pages(void);

//-------------------------------------
//             BLOCK
//-------------------------------------

struct block *find_free_block(size_t size);
void split_block(struct block *block, size_t size);
void merge_blocks(struct block *block);
int expand_block(struct block *block, size_t size);

#endif /* ! MALLOC_H */
