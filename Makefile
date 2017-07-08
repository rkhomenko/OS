BUILD_DIR=.

DIRS = $(shell find . -type d -name "lab*")

all:
	@$(foreach dir, $(DIRS), make BUILD_DIR=$(BUILD_DIR) --directory=$(dir))

clean:
	@$(foreach dir, $(DIRS), \
		make BUILD_DIR=$(BUILD_DIR) --directory=$(dir) clean)

test:
	@$(foreach dir, $(DIRS), \
		make BUILD_DIR=$(BUILD_DIR) --directory=$(dir) test)

.PHONY: all clean tests