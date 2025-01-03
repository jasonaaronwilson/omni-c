#!/bin/bash

### OBSOLETE - This simple script has been obsolete for a while. The
### most interesting thing left may be "date +%s%N" which seems to
### offer better resolution than lua. While I maintain that this
### simple treatment of tests as programs that return 0 (SUCCESS) or
### non-zero (FAILURE) is viable and better than nothing,
### omni-c/src/tools/run-tests.lua definitely makes it less painful to
### add news tests and has worked well for quite a while.

### This is a copy of run-tests.sh from c-armyknife-lib. A test is
### simply any executable program (including shell scripts) that
### returns 0 on success and anything else.

start=$(date +%s%N)  # get current timestamp with nanoseconds

success=0
failure=0
failed_tests=""

for arg in "$@"
do
    echo ------------------------------------------------------------------------
    echo "Running: $arg"
    echo ------------------------------------------------------------------------
    
    $arg
    if [ $? -eq 0 ] ; then
        ((success++))
    else
        ((failure++))
        failed_tests="$failed_tests $arg"
        echo "FAIL: non zero exit status for $arg"
    fi
    echo
done

echo ======================================================================

end=$(date +%s%N)    # get another timestamp with nanoseconds
## echo "END" $end
delta_ns=$((end - start))
delta_ms=$((delta_ns / 1000000))

echo "Total test run time: $delta_ms ms"

if [ $success -ne 0 ]; then
    echo "Passed $success tests."
fi

if [ $failure -eq 0 ]; then
    exit 0
else
    echo "Failed $failure tests:"
    echo $failed_tests
    exit 1
fi
