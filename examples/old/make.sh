#!/bin/bash

cd /home/jawilson/src/omni-c/examples/
export ARMYKNIFE_FATAL_ERROR_SLEEP_SECONDS=30
export ARMYKNIFE_LIB_LOG_LEVEL=info

../src/omni-c generate-header-file --output-file=ordering-bug.oc.out.c ordering-bug.oc
cat ordering-bug.oc.out.c
gcc ordering-bug.oc.out.c

