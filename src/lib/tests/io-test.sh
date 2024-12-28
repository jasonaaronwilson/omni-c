#!/bin/bash

SUFFIX=$$

./build-and-run.sh io-test.c io-test /tmp/file-a.${SUFFIX}
STATUS=$?

# cleanup
rm -f /tmp/file-a.${SUFFIX}

exit ${STATUS}
