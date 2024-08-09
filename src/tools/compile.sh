#!/bin/bash

readonly DIR="$(dirname "${BASH_SOURCE[0]}")"

TARGET_C_FILE=$1
shift

$DIR/../omni-c generate-library --output-file=${TARGET_C_FILE} $*
gcc -I${DIR}/../ ${TARGET_C_FILE}

