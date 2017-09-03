LAB_BUILD_DIR=$(BUILD_DIR)/program2
INCLUDE_DIR=$(LIBRARY_INCLUDE_DIR)
BINARY=$(LAB_BUILD_DIR)/program2

CFLAGS=-c -std=c99 -Wall -Wextra -pedantic-errors -I$(INCLUDE_DIR)
LDFLAGS=-ldl

ifeq ($(BUILD_TYPE), release)
	CFLAGS:=$(CFLAGS) -O2
endif

ifeq ($(BUILD_TYPE), debug)
	CFLAGS:=$(CFLAGS) -g -Og
endif

SOURCES=src/main.c
OBJECTS=$(patsubst src/%.c, $(LAB_BUILD_DIR)/%.o,$(SOURCES))

$(BINARY): dir $(OBJECTS)
	$(CC) $(LDFLAGS) $(OBJECTS) -o $(BINARY)

$(OBJECTS): $(LAB_BUILD_DIR)/%.o: src/%.c
	$(CC) $(CFLAGS) $< -o $@

dir:
	@mkdir -p "$(LAB_BUILD_DIR)"

clean:
	@rm -f $(OBJECTS) $(BINARY)

.PHONY: clean
