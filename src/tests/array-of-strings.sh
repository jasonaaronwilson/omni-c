#!/bin/bash
readonly DIR="$(dirname "${BASH_SOURCE[0]}")"
exec $DIR/../tools/generate-header-file-test.sh $DIR/array-of-strings.oc