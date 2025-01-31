#!/bin/bash

cpp ${OMNI_C_ROOT}/src/global-includes.c > ${BUILD_DIR}/tmp/system-includes.cpp-out.c
exec ${BUILD_DIR}/bin/omni-c generate-header-file ${BUILD_DIR}/tmp/system-includes.cpp-out.c
