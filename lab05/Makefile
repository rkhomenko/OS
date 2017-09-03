LAB_BUILD_DIR=$(BUILD_DIR)/lab05

LIBRARY_BUILD_DIR=$(LAB_BUILD_DIR)/library
LIBRARY_NAME=vector
LIBRARY=$(LIBRARY_BUILD_DIR)/libvector.so
LIBRARY_INCLUDE_DIR=$(shell dirname $(realpath $(lastword $(MAKEFILE_LIST))))/library/include

lab05: dir
	@make BUILD_DIR=$(LIBRARY_BUILD_DIR) \
			BUILD_TYPE=$(BUILD_TYPE) \
			LIBRARY=$(LIBRARY) \
			--directory=library
	@make BUILD_DIR=$(LAB_BUILD_DIR) \
			BUILD_TYPE=$(BUILD_TYPE) \
			LIBRARY_INCLUDE_DIR=$(LIBRARY_INCLUDE_DIR) \
			LIBRARY_DIR=$(LIBRARY_BUILD_DIR) \
			LIBRARY=$(LIBRARY_NAME) \
			--directory=program1
	@make BUILD_DIR=$(LAB_BUILD_DIR) \
			BUILD_TYPE=$(BUILD_TYPE) \
			LIBRARY_INCLUDE_DIR=$(LIBRARY_INCLUDE_DIR) \
			--directory=program2

dir:
	@mkdir -p "$(LAB_BUILD_DIR)"
	@mkdir -p "$(LIBRARY_BUILD_DIR)"

clean:
	@make BUILD_DIR=$(LIBRARY_BUILD_DIR) BUILD_TYPE=$(BUILD_TYPE) --directory=library clean
	@make BUILD_DIR=$(LAB_BUILD_DIR) BUILD_TYPE=$(BUILD_TYPE) --directory=program1 clean
	@make BUILD_DIR=$(LAB_BUILD_DIR) BUILD_TYPE=$(BUILD_TYPE) --directory=report clean

report:
	@make BUILD_DIR=$(BUILD_DIR) BUILD_TYPE=$(BUILD_TYPE) --directory=report

.PHONY: lab05 clean test report
