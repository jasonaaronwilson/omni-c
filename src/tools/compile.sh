#!/bin/bash

readonly DIR="$(dirname "${BASH_SOURCE[0]}")"
readonly OMNI_C_PATH="${OMNI_C_EXECUTABLE:-./build/bin/omni-c}"

TARGET_C_FILE=/tmp/$(basename "$1")
TARGET_BINARY=/tmp/$(basename "$1").bin
TARGET_OUTPUT=/tmp/$(basename "$1").bin.out
shift

$OMNI_C_PATH generate-library --c-output-file=${TARGET_C_FILE} --use-statement-parser=true $*
if [ $? -ne 0 ] ; then
  echo "FAIL: generate-library failed to generate ${TARGET_C_FILE}" >&2
  exit 1
fi
gcc -g -o ${TARGET_BINARY} -I${DIR}/../ ${TARGET_C_FILE}
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

