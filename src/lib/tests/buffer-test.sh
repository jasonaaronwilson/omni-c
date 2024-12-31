#!/bin/bash
readonly DIR="$(dirname "${BASH_SOURCE[0]}")"
exec $DIR/build-and-run.sh buffer-test.c buffer-test
