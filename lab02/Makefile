LAB01_BUILD_DIR=$(BUILD_DIR)/lab01
INCLUDE_DIR=./include
BINARY=$(LAB01_BUILD_DIR)/lab01

CFLAGS=-c -std=c99 -Wall -Wextra -pedantic-errors -I$(INCLUDE_DIR)

ifeq ($(BUILD_TYPE), release)
	CFLAGS:=$(CFLAGS) -O2
endif

ifeq ($(BUILD_TYPE), debug)
	CFLAGS:=$(CFLAGS) -ggdb3 -O1
endif

SOURCES=src/main.c src/vector.c
OBJECTS=$(patsubst src/%.c, $(LAB01_BUILD_DIR)/%.o,$(SOURCES))

$(BINARY): dir $(OBJECTS)
	$(CC) $(OBJECTS) -o $(BINARY)

$(OBJECTS): $(LAB01_BUILD_DIR)/%.o: src/%.c
	$(CC) $(CFLAGS) $< -o $@

dir:
	@mkdir -p "$(LAB01_BUILD_DIR)"

clean:
	@rm -f $(OBJECTS) $(BINARY)

.PHONY: clean