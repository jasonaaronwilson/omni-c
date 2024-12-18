#!/bin/bash

readonly SHOW_OUTPUT="false"
readonly DIR="$(dirname "${BASH_SOURCE[0]}")"
readonly OMNI_C_PATH="$DIR/../build/bin/omni-c"
readonly FILE=$1
TARGET_FILE=/tmp/$(basename "$1").h
TARGET_BINARY=/tmp/$(basename "$1").o

if [ ! -x "$OMNI_C_PATH" ]; then 
    echo "ERROR: 'omni-c' not found or not executable at '$OMNI_C_PATH'." >&2
    echo "Please ensure it is built and available." >&2
    exit 1
fi

# export ARMYKNIFE_FATAL_ERROR_SLEEP_SECONDS=30
export ARMYKNIFE_LIB_LOG_LEVEL=info

echo "=====> Invoking omni-c..."
"$OMNI_C_PATH" generate-header-file "--c-output-file=${TARGET_FILE}" --use-statement-parser=true ${FILE}
if [ $? -ne 0 ] ; then
  echo "FAIL: generate-header-file failed to generate output file for ${FILE}" >&2
  exit 1
fi

if [ "$SHOW_OUTPUT" = "true" ]; then
  echo "=====> The generated output is:"
  cat ${TARGET_FILE}
fi

echo "=====> Invoking gcc on the generated output..."

# -pedantic -- ISO doesn't like "empty" translations units
gcc -c -Wall -Werror -std=c99 -o ${TARGET_BINARY} ${TARGET_FILE}
if [ $? -ne 0 ] ; then
  echo "FAIL: gcc failed on the output ${TARGET_FILE} for ${FILE}" >&2
  exit 1
else
  echo "SUCCESS!"
fi

