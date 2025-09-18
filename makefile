CC := gcc
CFLAGS := -std=c99 -lm
SOURCE := mio 
################################################################################
debug: CFLAGS += -g -Wall -Wextra -Wpedantic -fsanitize=address
debug: all

all: $(addprefix bin/, $(SOURCE))

bin/%: src/%.c | bin
	$(CC) $(CFLAGS) -o $@ $<

bin:
	mkdir -p bin

clean: # removes compiled binaries
	rm $(addprefix bin/, $(SOURCE))

.PHONY: all clean
