# ======================================================================
#
# This Makefile is invoked indirectly from OMNI_C_ROOT
#
# ======================================================================

.PHONY: all test test-suite selftest

# ----------------------------------------------------------------------
# Aliases
# ----------------------------------------------------------------------

all: test 

# ----------------------------------------------------------------------
# Variable Definitions
# ----------------------------------------------------------------------

# --- Variables Passed from Top-Level Makefile ---

# OMNI_C_ROOT: Root directory of the omni-c project.
# BUILD_DIR:   Directory for build artifacts (absolute path).
ifndef OMNI_C_ROOT
  $(error OMNI_C_ROOT is not defined)
endif

ifndef BUILD_DIR
  $(error BUILD_DIR is not defined)
endif

# --- Internal Variables ---

export ARMYKNIFE_LIB_LOG_LEVEL = WARN

# Import
include ${OMNI_C_ROOT}/src/lib/Makefile.sources

C_ARMYKNIFE_SRC_C := $(addprefix lib/, $(SRC_C))

$(shell mkdir -p ${BUILD_DIR}/bin ${BUILD_DIR}/gen-files ${BUILD_DIR}/tests ${BUILD_DIR}/tests/statements ${BUILD_DIR}/tests/expressions ${BUILD_DIR}/tmp)

STABLE_BINARY_PATH := ${BUILD_DIR}/bin/omni-c-stable
OMNI_C := ${BUILD_DIR}/bin/omni-c

WARN_FLAGS := -Wall -Werror -Wno-error=format -Wno-format -Wno-error=return-local-addr
CC_FLAGS := -g -rdynamic ${WARN_FLAGS}
CC_FLAGS_OPT := ${CC_FLAGS} -O3 

# TODO(jawilson): fixup these tests
# lib/tests/value-hashtable-test.sh
# lib/tests/logger-test.sh

LIB_TESTS := \
	${OMNI_C_ROOT}/tests/lib/buffer-test.sh \
	${OMNI_C_ROOT}/tests/lib/cdl-printer-test.sh \
	${OMNI_C_ROOT}/tests/lib/flag-test.sh \
	${OMNI_C_ROOT}/tests/lib/io-test.sh \
	${OMNI_C_ROOT}/tests/lib/random-test.sh \
	${OMNI_C_ROOT}/tests/lib/splitjoin-test.sh \
	${OMNI_C_ROOT}/tests/lib/string-alist-test.sh \
	${OMNI_C_ROOT}/tests/lib/string-hashtable-test.sh \
	${OMNI_C_ROOT}/tests/lib/string-tokenizer-test.sh \
	${OMNI_C_ROOT}/tests/lib/string-tree-test.sh \
	${OMNI_C_ROOT}/tests/lib/string-util-test.sh \
	${OMNI_C_ROOT}/tests/lib/sub-process-test.sh \
	${OMNI_C_ROOT}/tests/lib/terminal-test.sh \
	${OMNI_C_ROOT}/tests/lib/uint64-test.sh \
	${OMNI_C_ROOT}/tests/lib/value-alist-test.sh \
	${OMNI_C_ROOT}/tests/lib/value-array-test.sh \
	${OMNI_C_ROOT}/tests/lib/value-tree-test.sh

TESTS := $(wildcard ${OMNI_C_ROOT}/tests/*.c ${OMNI_C_ROOT}/tests/expressions/*.e ${OMNI_C_ROOT}/tests/expressions/*.expr ${OMNI_C_ROOT}/tests/statements/*.stmt)
TESTS += $(LIB_TESTS)

SUITE_TESTS := $(shell grep -v '^#' test-suite.txt)

# ----------------------------------------------------------------------
# Target Definitions
# ----------------------------------------------------------------------

test: ${BUILD_DIR}/bin/omni-c
	${OMNI_C_ROOT}/tools/run-tests.lua ${TESTS}

selftest: ${BUILD_DIR}/bin/self
	OMNI_C_EXECUTABLE=${BUILD_DIR}/bin/self ${OMNI_C_ROOT}/tools/run-tests.lua ${TESTS}

test-suite: ${BUILD_DIR}/bin/omni-c
	tools/run-tests.lua ${SUITE_TESTS}
