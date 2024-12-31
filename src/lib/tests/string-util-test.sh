#!/bin/bash
readonly DIR="$(dirname "${BASH_SOURCE[0]}")"
exec $DIR/build-and-run.sh string-util-test.c string-util-test

