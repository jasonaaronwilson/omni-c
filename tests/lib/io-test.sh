#!/bin/bash

readonly DIR="$(dirname "${BASH_SOURCE[0]}")"

SUFFIX=$$

exec ${OMNI_C_ROOT}/tests/lib/build-and-run.sh ${OMNI_C_ROOT}/tests/lib/io-test.c io-test /tmp/file-a.${SUFFIX}
STATUS=$?

# cleanup
rm -f /tmp/file-a.${SUFFIX}

exit ${STATUS}
