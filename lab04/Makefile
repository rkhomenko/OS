LAB_BUILD_DIR=$(BUILD_DIR)/lab04
INCLUDE_DIR=./include
BINARY=$(LAB_BUILD_DIR)/lab04

CFLAGS=-c -std=gnu99 -Wall -Wextra -pedantic-errors -I$(INCLUDE_DIR)
LDFLAGS=

ifeq ($(BUILD_TYPE), release)
	CFLAGS:=$(CFLAGS) -O2
endif

ifeq ($(BUILD_TYPE), debug)
	CFLAGS:=$(CFLAGS) -g -Og
endif

SOURCES=src/main.c src/text_editor.c
OBJECTS=$(patsubst src/%.c, $(LAB_BUILD_DIR)/%.o,$(SOURCES))

$(BINARY): dir $(OBJECTS)
	$(CC) $(LDFLAGS) $(OBJECTS) -o $(BINARY)

$(OBJECTS): $(LAB_BUILD_DIR)/%.o: src/%.c
	$(CC) $(CFLAGS) $< -o $@

dir:
	@mkdir -p "$(LAB_BUILD_DIR)"

clean:
	@rm -f $(OBJECTS) $(BINARY)
	@make BUILD_DIR=$(BUILD_DIR) --directory=report clean

test: $(BINARY)
	@bash ./run_tests.sh $(BINARY)

report:
	@make BUILD_DIR=$(BUILD_DIR) --directory=report

.PHONY: clean test report
