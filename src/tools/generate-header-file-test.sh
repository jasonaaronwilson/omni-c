#!/bin/bash

DIR="$(dirname "${BASH_SOURCE[0]}")"
OMNI_C_PATH="$DIR/../omni-c"

if [ ! -x "$OMNI_C_PATH" ]; then 
    echo "ERROR: 'omni-c' not found or not executable at '$OMNI_C_PATH'." >&2
    echo "Please ensure it is built and available." >&2
    exit 1
fi

# export ARMYKNIFE_FATAL_ERROR_SLEEP_SECONDS=30
export ARMYKNIFE_LIB_LOG_LEVEL=info

"$OMNI_C_PATH" generate-header-file --output-file=$1.out.c $1
if [ $? -eq 0 ] ; then
  echo "FAIL: generate-header-file failed to generate output file for $1"  >&2
  exit 1
fi
cat ordering-bug.oc.out.c
gcc ordering-bug.oc.out.c
if [ $? -eq 0 ] ; then
  echo "FAIL: gcc failed on the output for $1.out.c"  >&2
  exit 1
fi

