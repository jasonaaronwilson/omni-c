#!/bin/bash

## First just do this test the normal way.
./build-and-run.sh logger-test.c logger-test

if [ $? -ne 0 ]; then
    echo "  (since logger-test failed, we aren't testing writing to a file)"
    exit 1
fi

echo ""
echo ">>> logger-test passed but now we want to try writing to a file <<<"
echo ""

LOGGER_FILE=/tmp/armyknife-logger-test.$$
export ARMYKNIFE_LIB_LOG_FILE=${LOGGER_FILE}
./build-and-run.sh logger-test.c logger-test

RESULT=$?
cat ${LOGGER_FILE}

export ARMYKNIFE_LIB_LOG_LEVEL=5
export ARMYKNIFE_LIB_LOG_FILE=/tmp/armyknife-logger-test.$$.FATAL

./build-and-run.sh logger-test.c logger-test
RESULT=$?
exit ${RESULT}


