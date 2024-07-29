#!/bin/bash
readonly DIR="$(dirname "${BASH_SOURCE[0]}")"
exec $DIR/../tools/generate-header-file-test.sh $DIR/out-of-order-typedef.oc

