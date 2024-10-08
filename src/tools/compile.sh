#!/bin/bash

readonly DIR="$(dirname "${BASH_SOURCE[0]}")"

TARGET_C_FILE=/tmp/$(basename "$1")
TARGET_BINARY=/tmp/$(basename "$1").bin
shift

$DIR/../build/bin/omni-c generate-library --output-file=${TARGET_C_FILE} --use-statement-parser=true $*
if [ $? -ne 0 ] ; then
  echo "FAIL: generate-library failed to generate ${TARGET_C_FILE}" >&2
  exit 1
fi
exec gcc -o ${TARGET_BINARY} -I${DIR}/../ ${TARGET_C_FILE}

