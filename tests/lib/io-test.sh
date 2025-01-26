#!/bin/bash

SUFFIX=$$

exec ${OMNI_C_ROOT}/tests/lib/build-and-run.sh ${OMNI_C_ROOT}/tests/lib/io-test.c io-test ${BUILD_DIR}/tmp/file-a.${SUFFIX}
STATUS=$?

# cleanup
rm -f ${BUILD_DIR}/tmp/file-a.${SUFFIX}

exit ${STATUS}
