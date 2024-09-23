#!/bin/bash

echo "*** >>> ECHO"
/usr/bin/time -v ./tools/repeat.sh echo

echo "*** >>> OMNI C"
/usr/bin/time -v ./tools/repeat.sh ./build/bin/omni-c

echo "*** >>> PARSE EXPRESSION"
/usr/bin/time -v ./tools/repeat.sh ./build/bin/omni-c parse-expression --expression="tgt += src1 + src2;"

echo "*** >>> java"
/usr/bin/time -v ./tools/repeat.sh java

echo "*** >>> (mock) java"
/usr/bin/time -v ./tools/repeat.sh ./tools/mock-java.sh
