#include <assert.h>

#include "../src/malloc.h"

int main(void)
{
    // No pages available
    assert(!find_free_block(64));

    // A free block with enough space
    struct page *page1 = create_page(64);
    assert(page1);

    struct block *block1 = page1->first;
    assert(find_free_block(64) == block1);

    // An allocated block
    block1->free = 0;
    assert(!find_free_block(64));

    // A free block that is too small
    block1->free = 1;
    size_t original_size = block1->size;
    block1->size = 32;
    assert(!find_free_block(64));

    // A block with the exact requested size
    block1->size = 64;
    assert(find_free_block(64) == block1);

    // Search through multiple blocks
    struct block block2 = {
        .size = 128, .next = NULL, .prev = block1, .free = 1
    };

    block1->next = &block2;
    block1->free = 0;
    assert(find_free_block(100) == &block2);

    // Search through multiple pages
    block2.free = 0;
    block1->next = NULL;
    block1->size = original_size;
    block1->free = 1;

    struct page *page2 = create_page(64);
    assert(page2);
    assert(page2->next == page1);

    page2->first->free = 0;
    assert(find_free_block(64) == block1);

    // No suitable free block
    block1->free = 0;
    assert(!find_free_block(64));

    return 0;
}
