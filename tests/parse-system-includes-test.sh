#!/bin/bash

# TODO(jawilson): consider using -ansi, -std=c90, -std=c99, -std=c11, etc.
cpp ${OMNI_C_ROOT}/src/global-includes.c > ${BUILD_DIR}/tmp/system-includes.cpp-out.c
exec ${BUILD_DIR}/bin/omni-c generate-header-file ${BUILD_DIR}/tmp/system-includes.cpp-out.c
