CC = gcc

CPPFLAGS = -D_DEFAULT_SOURCE
CFLAGS = -std=c99 -pedantic -Werror -Wall -Wextra -Wvla -fvisibility=hidden -fPIC
LDFLAGS = -shared -Wl,--no-undefined

TARGET = libmalloc.so
SRC = src/malloc.c src/block.c src/page.c src/utils.c
OBJ = $(SRC:.c=.o)

TESTFLAGS = $(CPPFLAGS) $(CFLAGS) -g

.PHONY: all library check clean debug check_library

all: library

library: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(LDFLAGS) -o $@ $^

src/%.o: src/%.c src/malloc.h
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

tests/test_page: tests/test_page.c src/page.c src/utils.c
	$(CC) $(TESTFLAGS) $^ -o $@

tests/test_alignment: tests/test_alignment.c src/utils.c
	$(CC) $(TESTFLAGS) $^ -o $@

tests/test_block: tests/test_block.c src/block.c src/page.c src/utils.c
	$(CC) $(TESTFLAGS) $^ -o $@

tests/test_split_block: tests/test_split_block.c src/block.c src/page.c src/utils.c
	$(CC) $(TESTFLAGS) $^ -o $@

tests/test_malloc: tests/test_malloc.c tests/my_malloc_testsuit.c src/block.c src/page.c src/utils.c
	$(CC) $(TESTFLAGS) -fno-builtin $^ -o $@

tests/test_library: tests/test_library.c
	$(CC) $(TESTFLAGS) $^ -o $@

debug:
	$(MAKE) clean
	$(MAKE) library CFLAGS="$(CFLAGS) -g -O0"

check: tests/test_page tests/test_alignment tests/test_block tests/test_split_block tests/test_malloc library tests/test_library
	./tests/test_alignment
	./tests/test_page
	./tests/test_block
	./tests/test_split_block
	./tests/test_malloc
	LD_PRELOAD="$(CURDIR)/$(TARGET)" ./tests/test_library

clean:
	$(RM) $(OBJ) $(TARGET) tests/test_page tests/test_alignment tests/test_block tests/test_split_block tests/test_malloc tests/test_library
