#include "beware_overflow.h"

void *beware_overflow(void *ptr, size_t nmemb, size_t size)
{
    size_t res;

    if (__builtin_mul_overflow(nmemb, size, &res))
    {
        return NULL;
    }

    char *p = ptr;
    p += res;
    return p;
}
