CC := clang
CFLAGS := -std=c99 -lm
DEBUG := -g -fsanitize=address -Wall -Wextra -Wpedantic

SOURCES := $(wildcard src/*.c)
TARGETS := $(patsubst src/%.c, %, $(SOURCES))
################################################################################
debug: CFLAGS += $(DEBUG)
debug: all

all: $(addprefix bin/, $(TARGETS))

bin/%: src/%.c | bin
	$(CC) $(CFLAGS) -o $@ $<

bin:
	mkdir -p bin

clean:
	rm $(addprefix bin/, $(TARGETS))

.PHONY: all clean
