#!/bin/bash

readonly OMNI_C_PATH="${BUILD_DIR}/bin/omni-c"

TARGET_C_FILE=${BUILD_DIR}/tmp/$(basename "$1")
TARGET_BINARY=${BUILD_DIR}/tmp/$(basename "$1").bin
TARGET_OUTPUT=${BUILD_DIR}/tmp/$(basename "$1").bin.out
shift

${OMNI_C_PATH} generate-library --c-output-file=${TARGET_C_FILE} --use-statement-parser=true $*
if [ $? -ne 0 ] ; then
  echo "FAIL: generate-library failed to generate ${TARGET_C_FILE}" >&2
  exit 1
fi
# -I${DIR}/../ 
gcc -g -o ${TARGET_BINARY} ${TARGET_C_FILE}
if [ $? -ne 0 ] ; then
  echo "FAIL: gcc failed compiling ${TARGET_C_FILE}" >&2
  exit 1
fi
${TARGET_BINARY} 2>&1 >${TARGET_OUTPUT}
if [ $? -ne 0 ] ; then
  echo "FAIL: ${TARGET_C_FILE} returned non-zero exit code" >&2
  exit 1
fi
exit 0

