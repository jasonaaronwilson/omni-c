#!/bin/bash

readonly SHOW_OUTPUT="false"
readonly DIR="$(dirname "${BASH_SOURCE[0]}")"
readonly OMNI_C_PATH="$DIR/../omni-c"
readonly FILE=$1
readonly OUT_FILE=test-out.c

if [ ! -x "$OMNI_C_PATH" ]; then 
    echo "ERROR: 'omni-c' not found or not executable at '$OMNI_C_PATH'." >&2
    echo "Please ensure it is built and available." >&2
    exit 1
fi

# export ARMYKNIFE_FATAL_ERROR_SLEEP_SECONDS=30
export ARMYKNIFE_LIB_LOG_LEVEL=info

echo "=====> Running omni-c..."
"$OMNI_C_PATH" generate-header-file "--output-file=${OUT_FILE}" ${FILE}
if [ $? -ne 0 ] ; then
  echo "FAIL: generate-header-file failed to generate output file for ${FILE}" >&2
  exit 1
fi

if [ "$SHOW_OUTPUT" = "true" ]; then
  echo "=====> The generated output is:"
  cat ${OUT_FILE}
fi

echo "=====> Running gcc on the generated output..."

# -pedantic -- ISO doesn't like "empty" translations units
gcc -c -fsyntax-only -Wall -std=c99 ${OUT_FILE}
if [ $? -ne 0 ] ; then
  echo "FAIL: gcc failed on the output ${OUT_FILE} for ${FILE}" >&2
  exit 1
else
  echo "SUCCESS!"
fi

