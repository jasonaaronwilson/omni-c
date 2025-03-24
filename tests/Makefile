# ======================================================================
#
# This Makefile is invoked indirectly from OMNI_C_ROOT
#
# ======================================================================

.PHONY: all test test-suite self-test format

# ----------------------------------------------------------------------
# Aliases
# ----------------------------------------------------------------------

all: test 

# ----------------------------------------------------------------------
# Variable Definitions
# ----------------------------------------------------------------------

# --- Variables Passed from Top-Level Makefile ---
ifndef OMNI_C_ROOT
  $(error OMNI_C_ROOT is not defined)
endif

ifndef BUILD_DIR
  $(error BUILD_DIR is not defined)
endif

# --- Internal Variables ---

export ARMYKNIFE_LIB_LOG_LEVEL = WARN

$(shell mkdir -p ${BUILD_DIR}/bin ${BUILD_DIR}/gen-files ${BUILD_DIR}/tests ${BUILD_DIR}/tests/statements ${BUILD_DIR}/tests/expressions ${BUILD_DIR}/tmp)

TESTS := $(wildcard ${OMNI_C_ROOT}/tests/*.c ${OMNI_C_ROOT}/tests/expressions/*.e ${OMNI_C_ROOT}/tests/expressions/*.expr ${OMNI_C_ROOT}/tests/statements/*.stmt)

# TODO(jawilson): fixup these tests
# lib/tests/value-hashtable-test.sh
# lib/tests/logger-test.sh
LIB_TESTS := \
	${OMNI_C_ROOT}/tests/lib/buffer-test.c \
	${OMNI_C_ROOT}/tests/lib/cdl-printer-test.c \
	${OMNI_C_ROOT}/tests/lib/random-test.c \
	${OMNI_C_ROOT}/tests/lib/splitjoin-test.c \
	${OMNI_C_ROOT}/tests/lib/string-alist-test.c \
	${OMNI_C_ROOT}/tests/lib/string-hashtable-test.c \
	${OMNI_C_ROOT}/tests/lib/string-tokenizer-test.c \
	${OMNI_C_ROOT}/tests/lib/string-tree-test.c \
	${OMNI_C_ROOT}/tests/lib/string-util-test.c \
	${OMNI_C_ROOT}/tests/lib/sub-process-test.c \
	${OMNI_C_ROOT}/tests/lib/terminal-test.c \
	${OMNI_C_ROOT}/tests/lib/uint64-test.c \
	${OMNI_C_ROOT}/tests/lib/value-alist-test.c \
	${OMNI_C_ROOT}/tests/lib/value-array-test.c \
	${OMNI_C_ROOT}/tests/lib/value-tree-test.c \
	${OMNI_C_ROOT}/tests/lib/flag-test.sh \
	${OMNI_C_ROOT}/tests/lib/io-test.sh


TESTS += $(LIB_TESTS)

SUITE_TESTS := $(shell grep -v '^#' test-suite.txt)

# ----------------------------------------------------------------------
# Target Definitions
# ----------------------------------------------------------------------

test:
	${OMNI_C_ROOT}/tools/run-tests.lua ${TESTS}

self-test:
	OMNI_C_EXECUTABLE=${BUILD_DIR}/bin/self ${OMNI_C_ROOT}/tools/run-tests.lua ${TESTS}

test-suite:
	${OMNI_C_ROOT}/tools/run-tests.lua ${SUITE_TESTS}

format:
	echo "TODO(jawilson): implement format rule for test makefile"
