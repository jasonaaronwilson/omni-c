#!/bin/bash

CC=gcc
# CC=clang
CC_FLAGS="-Wno-parentheses-equality"

${CC} ${CC_FLAGS} compile-test.c -o build/compile-test

if [[ $? != 0 ]] ; then
    echo "-- ${CC} failed to compile a trivial use case."
    exit 1
fi

./build/compile-test
if [[ $? != 0 ]] ; then
    echo "-- the built binary build/compiler-test returned non-zero return code"
    exit 1
fi

exit 0
