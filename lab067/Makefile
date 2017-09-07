LAB_BUILD_DIR=$(BUILD_DIR)/lab067
INCLUDE_DIR=./include
BINARY=$(LAB_BUILD_DIR)/lab067
LIBZMQ_BUILD_DIR=$(LAB_BUILD_DIR)/libzmq
LIBZMQ=$(LIBZMQ_BUILD_DIRL)/lib/libzmq.so

CURRENT_DIR := $(shell dirname $(realpath $(lastword $(MAKEFILE_LIST))))

CFLAGS=-c -std=c99 -Wall -Wextra -pedantic-errors -I$(INCLUDE_DIR)
LDFLAGS=

ifeq ($(BUILD_TYPE), release)
	CFLAGS:=$(CFLAGS) -O2
endif

ifeq ($(BUILD_TYPE), debug)
	CFLAGS:=$(CFLAGS) -g -Og
endif

SOURCES=src/main.c
OBJECTS=$(patsubst src/%.c, $(LAB_BUILD_DIR)/%.o,$(SOURCES))

$(BINARY): dir $(LIBZMQ) $(OBJECTS)
	$(CC) $(LDFLAGS) $(OBJECTS) -o $(BINARY)

$(OBJECTS): $(LAB_BUILD_DIR)/%.o: src/%.c
	$(CC) $(CFLAGS) $< -o $@

$(LIBZMQ):
	cd "$(LIBZMQ_BUILD_DIR)" && \
	$(CMAKE) -D WITH_PERF_TOOL=OFF \
			-D ZMQ_BUILD_TESTS=OFF \
			-D ENABLE_CPACK=OFF \
			-D CMAKE_BUILD_TYPE=Debug \
			"$(CURRENT_DIR)/libzmq" \
	&& $(MAKE) -j4 && \
	echo $(CURRENT_DIR) && \
	cd "$(CURRENT_DIR)"

dir:
	@mkdir -p "$(LAB_BUILD_DIR)"
	@mkdir -p "$(LIBZMQ_BUILD_DIR)"

clean:
	@rm -f $(OBJECTS) $(BINARY)
	@make BUILD_DIR=$(BUILD_DIR) --directory=report clean

test: $(BINARY)
	@bash ./run_tests.sh $(BINARY)

report:
	@make BUILD_DIR=$(BUILD_DIR) --directory=report

.PHONY: clean test report
