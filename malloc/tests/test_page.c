
#include <assert.h>

#include "../src/malloc.h"

// Test page creation, metadata initialization and page linking
int main(void)
{
    // Create a page with enough space for 64 bytes
    struct page *page1 = create_page(64);
    assert(page1);

    // Retrieve the system page size
    long system_page_size = sysconf(_SC_PAGESIZE);
    assert(system_page_size > 0);

    size_t page_size = system_page_size;
    size_t page_meta_size = alignment(sizeof(struct page));
    size_t block_meta_size = alignment(sizeof(struct block));

    // Small allocations should require only one system page
    assert(page1->size == page_size);
    assert(page1->first);

    // The first block must start after the aligned page metadata
    char *start = (void *)page1;
    void *expected_first = start + page_meta_size;
    assert(page1->first == expected_first);

    // Verify the initial block metadata
    struct block *block = page1->first;
    assert(block->free == 1);
    assert(!block->next);
    assert(!block->prev);

    // The free block occupies all remaining space in the page
    assert(block->size == page_size - page_meta_size - block_meta_size);

    // Create another page and verify the linked list
    struct page *page2 = create_page(64);
    assert(page2);
    assert(page2 != page1);

    // New pages are inserted at the head of the list
    assert(page2->next == page1);

    // Release the memory mappings created during the test
    assert(munmap(page2, page2->size) == 0);
    assert(munmap(page1, page1->size) == 0);

    return 0;
}
