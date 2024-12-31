#!/bin/bash

# This is a fragile script. It assumes our "cwd" is omni-c/src/

C_FILE=$1
EXE_FILE=$2

export ARMYKNIFE_LIB_LOG_LEVEL=WARN

readonly DIR="$(dirname "${BASH_SOURCE[0]}")"
readonly OMNI_C_PATH="${OMNI_C_EXECUTABLE:-$DIR/../../build/bin/omni-c}"

echo "Using ${OMNI_C_PATH} to build ${EXE_FILE}"

file_content=$(cat $DIR/../Makefile.sources)

# Extract the SRC_C variable definition using sed
src_c_definition=$(echo "$file_content" | sed -n '/^SRC_C :=/,/^$/p')

# Process the SRC_C variable to create a space-separated list of files
# (Gemni 2.0 Wrote this... I think we can simplify this in the future...)
src_files=$(echo "$src_c_definition" | tr -d '\\' | tr '\n' ' ' | sed 's/SRC_C :=//;s/^[[:space:]]*//;s/[[:space:]]*$//')

modified_src_files=""
for file in $src_files; do
  modified_src_files="$modified_src_files $DIR/../$file"
done


echo "Library source files are:"
echo $src_files

src_files="$modified_src_files $DIR/$C_FILE"

echo "All source files are:"
echo $src_files

echo `pwd`
${OMNI_C_PATH} build --binary-output-file=$DIR/../../build/bin/$2 --c-output-file=$DIR/../../build/bin/$1  $src_files
exec $DIR/../../build/bin/$2

exit 0

shift
shift

echo ${C_FILE}

../../build/build/bin/omni-c build ${C_FILE}

# In omni-c mode and with flag, --use-statement-parser=true, we always
# add parens around all binary operators, even in cases where it isn't
# necessary like inside of an if conditional so
# -Wno-parentheses-equality turns off some frequent warnings that
# appear. Other options would be clang format or to special case this
# with a little work in the C code generator.
#
# -Wno-return-stack-address avoids a warning in random_state() which
# is potentially something sub-optimal and easy enough to get rid of
# I'm just being lazy for a bit.

CC_FLAGS="-g -rdynamic -Wno-parentheses-equality"

AT_LEAST_ONE_COMPILER=0

# tcc doesn't understand "no return" and gives spurious warnings

for CC in gcc clang tcc ; do
    echo
    if which ${CC} >/dev/null ; then
        AT_LEAST_ONE_COMPILER=1
        echo "-- Bulding and running ${C_FILE} with ${CC} --"
        ${CC} ${CC_FLAGS} ${C_FILE} -o build/${EXE_FILE}
        if [[ $? != 0 ]] ; then
            echo "-- failed to compile test case with " ${CC}
            exit 1
        fi
        ./build/${EXE_FILE} $*
        if [[ $? != 0 ]] ; then
            echo "-- the built binary build/${EXE_FILE} returned non-zero return code"
            exit 1
        fi
    else
        echo "-- (${CC} does not appears to be installed). Skipping. --"
    fi
done

if [[ ${AT_LEAST_ONE_COMPILER} == 0 ]] ; then
    echo "ERROR: A C compiler was not found so this test was not run!"
    exit 1
fi

exit 0
