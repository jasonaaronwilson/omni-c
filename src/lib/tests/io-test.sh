#!/bin/bash

readonly DIR="$(dirname "${BASH_SOURCE[0]}")"

SUFFIX=$$

$DIR/build-and-run.sh io-test.c io-test /tmp/file-a.${SUFFIX}
STATUS=$?

# cleanup
rm -f /tmp/file-a.${SUFFIX}

exit ${STATUS}
