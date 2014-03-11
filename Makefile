
CC     ?= cc
SRC     = $(wildcard src/*.c)
SRC    += $(wildcard deps/*/*.c)
TESTS   = $(wildcard test/*.c)
CFLAGS  = -std=c99 -Wall -Isrc -Ideps
CFLAGS += -DDEFAULT_REPO_OWNER=\"clibs\"
CFLAGS += -DDEFAULT_REPO_VERSION=\"master\"
LDFLAGS = -lcurl

test: $(TESTS)

$(TESTS):
	@$(CC) $(CFLAGS) -o $(basename $@) $@ $(SRC) $(LDFLAGS)
	@./$(basename $@)

example: example.c $(SRC)
	$(CC) $(CFLAGS) $(LDFLAGS) $^ -o $@

clean:
	$(foreach t, $(TESTS), rm -f $(basename $(t));)
	rm -f example

.PHONY: test $(TESTS) clean
