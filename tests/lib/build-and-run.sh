#!/bin/bash

C_FILE=$1
EXE_FILE=$2
shift
shift

readonly OMNI_CC_PATH="${BUILD_DIR}/bin/omni-c"
readonly LIB_PATH="${BUILD_DIR}/bin/lib.oar"

##
## Now we can invoke omni-c to build the output binary
##

## By default we use tcc since it fast. If we were to tie ourselves to
## just on C compiler, clang or gcc

OMNI_C_TEST_COMPILER="${OMNI_C_TEST_COMPILER:-tcc}"

export ARMYKNIFE_LIB_LOG_LEVEL=WARN
${OMNI_CC_PATH} build \
    --use-statement-parser=true \
    --c-output-file=${BUILD_DIR}/bin/$(basename ${C_FILE}) \
    --binary-output-file=${BUILD_DIR}/bin/${EXE_FILE} \
    --c-compiler=${OMNI_C_TEST_COMPILER} \
    ${LIB_PATH} ${C_FILE}

if [[ $? != 0 ]] ; then
    echo "FAIL: ${OMNI_CC_PATH} returned non zero exit code"
    exit 1
fi

##
## Finally we can run the produced binary
##

exec ${BUILD_DIR}/bin/${EXE_FILE} $*
