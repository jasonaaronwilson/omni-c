#!/bin/bash
readonly DIR="$(dirname "${BASH_SOURCE[0]}")"
exec $DIR/../tools/compile.sh tmp.c $DIR/enum-pointer-in-structure-test.c
