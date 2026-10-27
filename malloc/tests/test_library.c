
#include <assert.h>
#include <stdint.h>
#include <stdlib.h>

// Test the real malloc, realloc, free and calloc functions
// These functions are replaced by our library using LD_PRELOAD

int main(void)
{
    // Allocate 64 bytes using malloc
    unsigned char *ptr = malloc(64);
    assert(ptr);

    // Initialize the allocated memory
    for (size_t i = 0; i < 64; i++)
    {
        ptr[i] = 42;
    }

    // Grow the allocation to 10000 bytes
    unsigned char *new_ptr = realloc(ptr, 10000);
    assert(new_ptr);

    // Verify that realloc preserved the original data
    for (size_t i = 0; i < 64; i++)
    {
        assert(new_ptr[i] == 42);
    }

    // Release the resized allocation
    free(new_ptr);

    // Allocate 100 bytes initialized to zero
    unsigned char *zero = calloc(100, 1);
    assert(zero);

    // Verify that calloc initialized every byte to zero
    for (size_t i = 0; i < 100; i++)
    {
        assert(zero[i] == 0);
    }

    // Release the calloc allocation
    free(zero);

    return 0;
}
