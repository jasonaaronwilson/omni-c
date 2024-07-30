#!/bin/bash

TARGET_C_FILE=$1
shift

../omni-c generate-library --output-file=${TARGET_C_FILE} $*
gcc ${TARGET_C_FILE}

