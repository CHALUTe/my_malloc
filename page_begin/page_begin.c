#include "page_begin.h"

void *page_begin(void *ptr, size_t page_size)
{
    size_t offset = (size_t)ptr & (page_size - 1);
    char *p = ptr;
    return p - offset;
}
