###
### Top-level Makefile for omni-c
###
### A "developer" or early adoper on linux will *eventually* just type
### this:
###
### git clone https://github.com/jasonaaronwilson/omni-c.git && \
###   cd omni-c && ./configure && make && make test && make install
###
### We are kind of close... I might have to put the Boehm collector
### into a "third-party" library directory to make this work.

# Check if config.mk (and output from the ./configure) script exists
# and bail with a nice erroor messasge if it doesn't.

ifneq ("$(wildcard build/config.mk)","")
  include build/config.mk
else
define CONFIG_ERROR_MESSAGE

----------------------------------------------------------------------
Error: 'build/config.mk' not found.

The most common reason for this is that you cloned a source repo but
never ran './configure' script in the root directory.

(Less common reasons would be an accidental deletion of either the
build directory symlink or the target build-dir.)

'./configure' is usually sufficent unless you are a package maintainer
where you'll probably want to use the --build-dir option to cleanly
build outside of the source tree.

For help with available options, use:

  ./configure --help

Please see the README.md file for more detailed instructions.
----------------------------------------------------------------------

endef
  $(error $(CONFIG_ERROR_MESSAGE))
endif

# Default target
.PHONY: all
all:
	@echo "Building omni-c..."
	@$(MAKE) -C src OMNI_C_ROOT=$(OMNI_C_ROOT) BUILD_DIR=$(BUILD_DIR) IS_MAC_OS=$(IS_MAC_OS) BREW_GC_PREFIX=$(BREW_GC_PREFIX)
	@$(MAKE) -C src self OMNI_C_ROOT=$(OMNI_C_ROOT) BUILD_DIR=$(BUILD_DIR) IS_MAC_OS=$(IS_MAC_OS) BREW_GC_PREFIX=$(BREW_GC_PREFIX)

# Clean target
.PHONY: clean
clean:
	@echo "Cleaning build artifacts..."
	@$(MAKE) -C src clean OMNI_C_ROOT=$(OMNI_C_ROOT) BUILD_DIR=$(BUILD_DIR)

# Examples target
.PHONY: examples
examples: all
	@echo "Building examples in src/lib/examples..."
	@$(MAKE) -C src/lib/examples all OMNI_C_ROOT=$(OMNI_C_ROOT) BUILD_DIR=$(BUILD_DIR)


# Tags target
.PHONY: tags
tags:
	@echo "Creating tags file..."
	@$(MAKE) -C src tags OMNI_C_ROOT=$(OMNI_C_ROOT) BUILD_DIR=$(BUILD_DIR)

# Format target
.PHONY: format
format: all
	@echo "Formatting src..."
	@$(MAKE) -C src format OMNI_C_ROOT=$(OMNI_C_ROOT) BUILD_DIR=$(BUILD_DIR)
	@echo "Formatting tests..."
	@$(MAKE) -C tests format OMNI_C_ROOT=$(OMNI_C_ROOT) BUILD_DIR=$(BUILD_DIR)

# Test target
.PHONY: test
test: all
	@echo "Running tests..."
	@$(MAKE) -C tests all OMNI_C_ROOT=$(OMNI_C_ROOT) BUILD_DIR=$(BUILD_DIR)

# Help target
.PHONY: help
help:
	@echo "Available targets:"
	@echo "  all     - Build omni-c (default)"
	@echo "  clean   - Clean build artifacts"
	@echo "  format  - Format source files and tests"
	@echo "  tags    - make a tags file"
	@echo "  test    - Build and run tests"
	@echo "  help    - Display this help message"

# --- Phony targets ---

.PHONY: $(PHONY)
