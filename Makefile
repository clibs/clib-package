
CC ?= cc
VALGRIND ?= valgrind
SRC = $(wildcard src/*.c)
SRC += $(wildcard deps/*/*.c)
OBJS = $(SRC:.c=.o)
TESTS = $(wildcard test/*.c)
CFLAGS = -std=c99 -Wall -Isrc -Ideps
LDFLAGS = -lcurl
VALGRIND_OPTS ?= --leak-check=full

test: $(TESTS)

$(TESTS): $(OBJS)
	@$(CC) $(CFLAGS) -o $(basename $@) $@ $(OBJS) $(LDFLAGS)
	@$(TEST_RUNNER) ./$(basename $@)

grind:
	@TEST_RUNNER="$(VALGRIND) $(VALGRIND_OPTS)" \
		$(MAKE) test

%.o: %.c
	$(CC) $< -c -o $@ $(CFLAGS)

example: example.c $(OBJS)
	$(CC) $(CFLAGS) $(LDFLAGS) $^ -o $@

clean:
	$(foreach t, $(TESTS), rm -f $(basename $(t));)
	rm -f example $(OBJS)

.PHONY: test $(TESTS) clean grind
