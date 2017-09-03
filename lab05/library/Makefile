LIBRARY_BUILD_DIR=$(BUILD_DIR)
INCLUDE_DIR=./include

CFLAGS=-c -fPIC -std=c99 -Wall -Wextra -pedantic-errors -I$(INCLUDE_DIR)
LDFLAGS=-shared

ifeq ($(BUILD_TYPE), release)
	CFLAGS:=$(CFLAGS) -O2
endif

ifeq ($(BUILD_TYPE), debug)
	CFLAGS:=$(CFLAGS) -g -Og
endif

SOURCES=src/vector.c
OBJECTS=$(patsubst src/%.c, $(LIBRARY_BUILD_DIR)/%.o,$(SOURCES))

$(LIBRARY): dir $(OBJECTS)
	$(CC) $(LDFLAGS) $(OBJECTS) -o $(LIBRARY)

$(OBJECTS): $(LIBRARY_BUILD_DIR)/%.o: src/%.c
	$(CC) $(CFLAGS) $< -o $@

dir:
	@mkdir -p "$(LIBRARY_BUILD_DIR)"

clean:
	@rm -f $(OBJECTS) $(LIBRARY)

.PHONY: clean
