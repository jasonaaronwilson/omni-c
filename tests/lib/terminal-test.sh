#!/bin/bash
readonly OMNI_CC_PATH="${BUILD_DIR}/bin/omni-c"
readonly LIB_PATH="${BUILD_DIR}/bin/lib.oar"
exec ${OMNI_CC_PATH} test ${LIB_PATH} ${OMNI_C_ROOT}/tests/lib/terminal-test.c
