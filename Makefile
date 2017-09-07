BUILD_DIR=.
CMAKE=cmake

DIRS:=$(sort $(wildcard lab*))


# foreach with DIRS doesn't work for more than one dir! \
	I don't know why.
all:
	@$(MAKE) BUILD_DIR=$(BUILD_DIR) BUILD_TYPE=$(BUILD_TYPE) --directory=lab02
	@$(MAKE) BUILD_DIR=$(BUILD_DIR) BUILD_TYPE=$(BUILD_TYPE) --directory=lab03
	@$(MAKE) BUILD_DIR=$(BUILD_DIR) BUILD_TYPE=$(BUILD_TYPE) --directory=lab04
	@$(MAKE) BUILD_DIR=$(BUILD_DIR) BUILD_TYPE=$(BUILD_TYPE) --directory=lab05
	@$(MAKE) BUILD_DIR=$(BUILD_DIR) \
			BUILD_TYPE=$(BUILD_TYPE) \
			CMAKE=$(CMAKE) \
			--directory=lab067

clean:
	@$(MAKE) BUILD_DIR=$(BUILD_DIR) BUILD_TYPE=$(BUILD_TYPE) --directory=lab02 clean
	@$(MAKE) BUILD_DIR=$(BUILD_DIR) BUILD_TYPE=$(BUILD_TYPE) --directory=lab03 clean
	@$(MAKE) BUILD_DIR=$(BUILD_DIR) BUILD_TYPE=$(BUILD_TYPE) --directory=lab04 clean
	@$(MAKE) BUILD_DIR=$(BUILD_DIR) BUILD_TYPE=$(BUILD_TYPE) --directory=lab05 clean
	@$(MAKE) BUILD_DIR=$(BUILD_DIR) BUILD_TYPE=$(BUILD_TYPE) --directory=lab067 clean

test:
	@$(MAKE) BUILD_DIR=$(BUILD_DIR) BUILD_TYPE=$(BUILD_TYPE) --directory=lab02 test
	@$(MAKE) BUILD_DIR=$(BUILD_DIR) BUILD_TYPE=$(BUILD_TYPE) --directory=lab03 test
	@$(MAKE) BUILD_DIR=$(BUILD_DIR) BUILD_TYPE=$(BUILD_TYPE) --directory=lab04 test
	@$(MAKE) BUILD_DIR=$(BUILD_DIR) BUILD_TYPE=$(BUILD_TYPE) --directory=lab05 test
	@$(MAKE) BUILD_DIR=$(BUILD_DIR) BUILD_TYPE=$(BUILD_TYPE) --directory=lab067 test

report:
	@$(MAKE) BUILD_DIR=$(BUILD_DIR) BUILD_TYPE=$(BUILD_TYPE) --directory=lab02 report
	@$(MAKE) BUILD_DIR=$(BUILD_DIR) BUILD_TYPE=$(BUILD_TYPE) --directory=lab03 report
	@$(MAKE) BUILD_DIR=$(BUILD_DIR) BUILD_TYPE=$(BUILD_TYPE) --directory=lab04 report
	@$(MAKE) BUILD_DIR=$(BUILD_DIR) BUILD_TYPE=$(BUILD_TYPE) --directory=lab05 report
	@$(MAKE) BUILD_DIR=$(BUILD_DIR) BUILD_TYPE=$(BUILD_TYPE) --directory=lab067 report

.PHONY: all $(DIRS) clean test report
