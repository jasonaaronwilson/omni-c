#!/bin/bash
readonly DIR="$(dirname "${BASH_SOURCE[0]}")"
exec $DIR/build-and-run.sh cdl-printer-test.c cdl-printer-test
