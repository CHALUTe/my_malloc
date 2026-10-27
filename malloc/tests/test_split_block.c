
#include <assert.h>

#include "../src/malloc.h"

static void test_basic_split(void)
{
    struct page *page = create_page(64);
    assert(page);

    struct block *block = page->first;
    size_t original_size = block->size;
    size_t meta_size = alignment(sizeof(struct block));

    split_block(block, 64);

    assert(block->size == alignment(64));
    assert(block->next);

    struct block *new_block = block->next;

    assert(new_block->size == original_size - alignment(64) - meta_size);
    assert(new_block->free == 1);
    assert(new_block->prev == block);
    assert(!new_block->next);

    // Check the physical position of the new block
    void *memory = block;
    char *start = memory;
    void *expected = start + meta_size + alignment(64);
    assert(new_block == expected);
}

static void test_no_split(void)
{
    struct page *page = create_page(64);
    assert(page);

    struct block *block = page->first;
    size_t original_size = block->size;

    // Not enough space for a second block
    split_block(block, original_size);

    assert(block->size == original_size);
    assert(!block->next);

    // Requested size is larger than the block
    split_block(block, original_size + 1);

    assert(block->size == original_size);
    assert(!block->next);

    // NULL should be handled safely
    split_block(NULL, 64);
}

static void test_multiple_splits(void)
{
    struct page *page = create_page(64);
    assert(page);

    struct block *first = page->first;

    split_block(first, 64);
    struct block *second = first->next;
    assert(second);

    split_block(second, 128);
    struct block *third = second->next;
    assert(third);

    assert(first->next == second);
    assert(second->prev == first);
    assert(second->next == third);
    assert(third->prev == second);
    assert(!third->next);

    assert(first->size == alignment(64));
    assert(second->size == alignment(128));
    assert(third->free == 1);
}

static void test_insert_between_blocks(void)
{
    struct page *page = create_page(64);
    assert(page);

    struct block *first = page->first;

    split_block(first, 256);
    struct block *last = first->next;
    assert(last);

    // Split the first block again, inserting a block before last
    split_block(first, 64);

    struct block *middle = first->next;

    assert(middle);
    assert(middle != last);
    assert(middle->next == last);
    assert(middle->prev == first);
    assert(last->prev == middle);
    assert(first->next == middle);

    assert(first->size == alignment(64));
    assert(middle->size
           == 256 - alignment(64) - alignment(sizeof(struct block)));
}

int main(void)
{
    test_basic_split();
    test_no_split();
    test_multiple_splits();
    test_insert_between_blocks();

    return 0;
}
