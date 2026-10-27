
#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>

#include "my_malloc_testsuit.h"

//-------------------------------------------------------------
//                          MALLOC
//-------------------------------------------------------------

// Compare a simple allocation with the standard malloc
void test_my_malloc(void)
{
    unsigned char *original = malloc(64);
    unsigned char *mine = my_malloc(64);

    assert(original);
    assert(mine);

    // Write identical values into both allocations
    for (size_t i = 0; i < 64; i++)
    {
        original[i] = 42;
        mine[i] = 42;
    }

    // Check that the allocated memory contains the same data
    for (size_t i = 0; i < 64; i++)
    {
        assert(original[i] == mine[i]);
    }

    free(original);
}

// Check that multiple allocations do not overlap
void test_multiple_allocations(void)
{
    unsigned char *a = my_malloc(64);
    unsigned char *b = my_malloc(64);

    assert(a);
    assert(b);
    assert(a != b);

    // Store different values in each allocation
    for (size_t i = 0; i < 64; i++)
    {
        a[i] = 42;
        b[i] = 84;
    }

    // Verify that writing to one block does not affect the other
    for (size_t i = 0; i < 64; i++)
    {
        assert(a[i] == 42);
        assert(b[i] == 84);
    }
}

// Check allocations spanning multiple memory pages
void test_multiple_pages(void)
{
    unsigned char *ptrs[100];

    // Allocate enough blocks to require multiple pages
    for (size_t i = 0; i < 100; i++)
    {
        ptrs[i] = my_malloc(64);
        assert(ptrs[i] != NULL);
        ptrs[i][0] = 42;
    }

    // Verify that all allocations remain accessible
    for (size_t i = 0; i < 100; i++)
    {
        assert(ptrs[i][0] == 42);
    }
}

// Check an allocation larger than one system page
void test_large_allocation(void)
{
    size_t size = 10000;
    unsigned char *ptr = my_malloc(size);

    assert(ptr != NULL);

    // Write to every byte of the allocated region
    for (size_t i = 0; i < size; i++)
    {
        ptr[i] = 42;
    }

    // Verify that the entire region is writable
    for (size_t i = 0; i < size; i++)
    {
        assert(ptr[i] == 42);
    }
}

// Check a large allocation of one MiB
void test_very_large_allocation(void)
{
    size_t size = 1024 * 1024;
    unsigned char *ptr = my_malloc(size);

    assert(ptr != NULL);

    // Fill the entire allocation
    for (size_t i = 0; i < size; i++)
    {
        ptr[i] = 84;
    }

    // Verify that all stored values are preserved
    for (size_t i = 0; i < size; i++)
    {
        assert(ptr[i] == 84);
    }
}

// Check several large allocations at the same time
void test_multiple_large_allocations(void)
{
    unsigned char *a = my_malloc(10000);
    unsigned char *b = my_malloc(20000);
    unsigned char *c = my_malloc(50000);

    assert(a && b && c);

    // Each allocation must have a distinct address
    assert(a != b && a != c && b != c);

    // Fill each allocation with a different value
    for (size_t i = 0; i < 10000; i++)
    {
        a[i] = 10;
    }

    for (size_t i = 0; i < 20000; i++)
    {
        b[i] = 20;
    }

    for (size_t i = 0; i < 50000; i++)
    {
        c[i] = 30;
    }

    // Check the last byte of each allocation
    assert(a[9999] == 10);
    assert(b[19999] == 20);
    assert(c[49999] == 30);
}

// Check different sizes and long double alignment
void test_different_sizes(void)
{
    size_t sizes[] = { 1,    8,    16,   17,   32,    100,  500,
                       1000, 4096, 4097, 8192, 10000, 65536 };

    for (size_t i = 0; i < sizeof(sizes) / sizeof(sizes[0]); i++)
    {
        unsigned char *ptr = my_malloc(sizes[i]);
        assert(ptr != NULL);

        // Convert the address to an integer to verify alignment
        assert((uintptr_t)ptr % sizeof(long double) == 0);

        // Write and read every byte for the requested size
        for (size_t j = 0; j < sizes[i]; j++)
        {
            ptr[j] = 42;
        }

        for (size_t j = 0; j < sizes[i]; j++)
        {
            assert(ptr[j] == 42);
        }
    }
}

// Check zero-size allocations and integer overflow
void test_edge_cases(void)
{
    // A zero-size allocation returns a freeable pointer
    void *ptr = my_malloc(0);
    assert(ptr != NULL);
    my_free(ptr);

    // SIZE_MAX cannot be safely aligned and allocated
    assert(my_malloc(SIZE_MAX) == NULL);
}

//-------------------------------------------------------------
//                           FREE
//-------------------------------------------------------------

// Check that freeing NULL has no effect
void test_free_null(void)
{
    my_free(NULL);
}

// Check that a freed block can be reused
void test_free_block(void)
{
    unsigned char *a = my_malloc(64);
    unsigned char *b = my_malloc(64);

    assert(a && b);

    // Free the first block while the second remains allocated
    my_free(a);

    // First-Fit should reuse the freed block
    unsigned char *c = my_malloc(64);
    assert(c == a);

    my_free(b);
    my_free(c);
}

// Check that adjacent free blocks are merged
void test_free_merge(void)
{
    unsigned char *a = my_malloc(64);
    unsigned char *b = my_malloc(64);
    unsigned char *c = my_malloc(64);

    assert(a && b && c);

    // Free two neighboring blocks so they can be merged
    my_free(a);
    my_free(b);

    // The merged space should be large enough for 128 bytes
    unsigned char *d = my_malloc(128);
    assert(d == a);

    my_free(c);
    my_free(d);
}

// Check that a large allocation can be freed and allocated again
void test_free_large(void)
{
    unsigned char *ptr = my_malloc(10000);
    assert(ptr);

    // Free the region, which may be released with munmap
    my_free(ptr);

    // Allocate a new large region and verify it is writable
    unsigned char *again = my_malloc(10000);
    assert(again);
    again[9999] = 42;

    my_free(again);
}

//-------------------------------------------------------------
//                          CALLOC
//-------------------------------------------------------------

// Check that calloc initializes all requested bytes to zero
void test_my_calloc(void)
{
    size_t count = 100;
    unsigned char *ptr = my_calloc(count, sizeof(unsigned char));

    assert(ptr);

    // Every allocated byte must initially be zero
    for (size_t i = 0; i < count; i++)
    {
        assert(ptr[i] == 0);
    }

    my_free(ptr);
}

// Check multiplication overflow protection in calloc
void test_calloc_overflow(void)
{
    // SIZE_MAX multiplied by 2 exceeds the size_t range
    assert(my_calloc(SIZE_MAX, 2) == NULL);
}

//-------------------------------------------------------------
//                         REALLOC
//-------------------------------------------------------------

// Check that realloc(NULL, size) behaves like malloc(size)
void test_realloc_null(void)
{
    unsigned char *ptr = my_realloc(NULL, 64);
    assert(ptr);
    my_free(ptr);
}

// Check that realloc preserves data when growing an allocation
void test_realloc_grow(void)
{
    unsigned char *ptr = my_malloc(64);
    assert(ptr);

    // Initialize the original allocation
    for (size_t i = 0; i < 64; i++)
    {
        ptr[i] = 42;
    }

    // Grow the allocation from 64 to 128 bytes
    unsigned char *new_ptr = my_realloc(ptr, 128);
    assert(new_ptr);

    // The original 64 bytes must remain unchanged
    for (size_t i = 0; i < 64; i++)
    {
        assert(new_ptr[i] == 42);
    }

    // Verify that the newly available space is writable
    new_ptr[127] = 84;
    my_free(new_ptr);
}

// Check that shrinking an allocation preserves its contents
void test_realloc_shrink(void)
{
    unsigned char *ptr = my_malloc(128);
    assert(ptr);

    ptr[0] = 42;
    unsigned char *new_ptr = my_realloc(ptr, 64);

    // Our implementation keeps the same address when shrinking
    assert(new_ptr == ptr);
    assert(new_ptr[0] == 42);

    my_free(new_ptr);
}

// Check that realloc(ptr, 0) frees the allocation
void test_realloc_zero(void)
{
    unsigned char *ptr = my_malloc(64);
    assert(ptr);

    // Our implementation frees the block and returns NULL
    assert(my_realloc(ptr, 0) == NULL);
}

// Check in-place expansion using an adjacent free block
void test_realloc_expand(void)
{
    unsigned char *a = my_malloc(64);
    unsigned char *b = my_malloc(128);
    unsigned char *c = my_malloc(64);

    assert(a && b && c);

    a[0] = 42;

    // Free the block immediately after a
    my_free(b);

    // Expand a into the adjacent free space without moving it
    unsigned char *new_a = my_realloc(a, 160);

    assert(new_a == a);
    assert(new_a[0] == 42);

    my_free(new_a);
    my_free(c);
}

int main(void)
{
    // malloc
    test_my_malloc();
    test_multiple_allocations();
    test_multiple_pages();
    test_large_allocation();
    test_very_large_allocation();
    test_multiple_large_allocations();
    test_different_sizes();
    test_edge_cases();

    // free
    test_free_null();
    test_free_block();
    test_free_merge();
    test_free_large();

    // calloc
    test_my_calloc();
    test_calloc_overflow();

    // realloc
    test_realloc_null();
    test_realloc_grow();
    test_realloc_shrink();
    test_realloc_zero();
    test_realloc_expand();

    return 0;
}
