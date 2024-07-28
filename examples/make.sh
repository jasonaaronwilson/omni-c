#!/bin/bash

cd /home/jawilson/src/omni-c/examples/
export ARMYKNIFE_FATAL_ERROR_SLEEP_SECONDS=30

../src/omni-c generate-header-file --output-file=ordering-bug.oc.out.c ordering-bug.oc
gcc ordering-bug.oc.out.c

