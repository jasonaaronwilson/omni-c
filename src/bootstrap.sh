#!/bin/bash

# Boot-strapping is necessary because we want to use some features of
# omni-c to continue writing omni-c. Eventually this process will
# replace our depencency on
# "https://github.com/jasonaaronwilson/c-single-source-file" since
# omni-c will be able to generate the appropriate header files so you
# don't have to repeat yourself.
#
# A true bootstrapping process normally entails creating a binary that
# can compile itself and using cross-compilation when bringing up a
# new platform. In our case, at least one of our "assembly" languages
# will always be C so we can start with a known good output from
# omni-c to build a bootstrap version with which to continue
# development.

cp -upv --target-directory=. bootstrap-saved/*

echo "Bootstrap process is complete."

exit 0
