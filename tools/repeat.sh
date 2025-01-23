#!/bin/bash
for i in {1..100}
do
    "$@" >/dev/null 2>&1
done
