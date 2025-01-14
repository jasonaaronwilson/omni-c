#!/bin/bash

C_FILE=$1
EXE_FILE=$2
shift
shift

readonly DIR="$(dirname "${BASH_SOURCE[0]}")"
readonly OMNI_C_PATH="${OMNI_C_EXECUTABLE:-$DIR/../../build/bin/omni-c}"

##
## We need all of the files in "lib" but relative to where the test
## runner is being run from.
##

file_content=$(cat $DIR/../Makefile.sources)

# Extract the SRC_C variable definition using sed
src_c_definition=$(echo "$file_content" | sed -n '/^SRC_C :=/,/^$/p')

# Process the SRC_C variable to create a space-separated list of files
# (Gemni 2.0 Wrote this... I think we can simplify this in the future...)
src_files=$(echo "$src_c_definition" | tr -d '\\' | tr '\n' ' ' | sed 's/SRC_C :=//;s/^[[:space:]]*//;s/[[:space:]]*$//')

modified_src_files=""
for file in ${src_files}; do
  modified_src_files="${modified_src_files} ${DIR}/../${file}"
done

src_files="${modified_src_files} ${DIR}/${C_FILE}"

##
## Now we can invoke omni-c to build the output binary
##

## By default we use tcc since it fast. If we were to tie ourselves to
## just on C compiler, clang or gcc

OMNI_C_TEST_COMPILER:"${OMNI_C_TEST_COMPILER}:-tcc"

export ARMYKNIFE_LIB_LOG_LEVEL=WARN
${OMNI_C_PATH} build \
    --c-output-file=${DIR}/../../build/bin/${C_FILE} \
    --binary-output-file=${DIR}/../../build/bin/${EXE_FILE} \
    --c-compiler=${OMNI_C_TEST_COMPILER} \
    $src_files
if [[ $? != 0 ]] ; then
    echo "FAIL: ${OMNI_C_PATH} returned non zero exit code"
    exit 1
fi

##
## Finally we can run the produced binary
##

exec ${DIR}/../../build/bin/${EXE_FILE} $*
