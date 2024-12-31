#!/bin/bash
readonly DIR="$(dirname "${BASH_SOURCE[0]}")"
exec $DIR/build-and-run.sh value-tree-test.c value-tree-test

