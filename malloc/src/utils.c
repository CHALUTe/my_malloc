#include <stddef.h>

#include "malloc.h"

size_t alignment(size_t size)
{
    size_t align = sizeof(long double);
    size_t result;

    if (__builtin_add_overflow(size, align - 1, &result))
    {
        return 0;
    }

    return result & ~(align - 1);
}
