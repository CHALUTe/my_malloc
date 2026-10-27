#ifndef MY_MALLOC_TESTSUITE_H
#define MY_MALLOC_TESTSUITE_H

#include <stddef.h>

// Custom allocator
void *my_malloc(size_t size);
void my_free(void *ptr);
void *my_calloc(size_t nmemb, size_t size);
void *my_realloc(void *ptr, size_t size);

// Test functions
void test_my_malloc(void);
void test_my_free(void);
void test_my_calloc(void);
void test_my_realloc(void);

#endif /* ! MY_MALLOC_TESTSUITE_H */
