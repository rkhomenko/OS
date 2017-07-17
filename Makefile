BUILD_DIR=.

DIRS:=$(sort $(wildcard lab*))

# foreach with DIRS doesn't work for more than one dir! \
	I don't know why.
all:
	@$(MAKE) BUILD_DIR=$(BUILD_DIR) --directory=lab02
	@$(MAKE) BUILD_DIR=$(BUILD_DIR) --directory=lab03

clean:
	@$(MAKE) BUILD_DIR=$(BUILD_DIR) --directory=lab02 clean
	@$(MAKE) BUILD_DIR=$(BUILD_DIR) --directory=lab03 clean

test:
	@$(MAKE) BUILD_DIR=$(BUILD_DIR) --directory=lab02 test
	@$(MAKE) BUILD_DIR=$(BUILD_DIR) --directory=lab03 test

report:
	@$(MAKE) BUILD_DIR=$(BUILD_DIR) --directory=lab02 report
	@$(MAKE) BUILD_DIR=$(BUILD_DIR) --directory=lab03 report

.PHONY: all $(DIRS) clean test report
