
#include <assert.h>
#include <stdint.h>

#include "../src/malloc.h"

int main(void)
{
    size_t a = sizeof(long double);

    assert(alignment(0) == 0);
    assert(alignment(1) == a);
    assert(alignment(a) == a);
    assert(alignment(a + 1) == 2 * a);
    assert(alignment(2 * a) == 2 * a);
    assert(alignment(2 * a - 1) == 2 * a);

    assert(alignment(SIZE_MAX) == 0);

    return 0;
}
