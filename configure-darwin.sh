#!/bin/bash

[ "$(uname)" = "Darwin" ] || { echo "Error: This script requires macOS (Darwin)." >&2; exit 1; }

### This is the bootstrap phase of the omni-c compiler for unix
### systems.

# Find the directory where this script actually lives
DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" >/dev/null 2>&1 && pwd)"
STABLE="${DIR}/build-dir/bin/omni-c-stable"

echo "======================================================================"
echo "                 Welcome to omni-c unix configuation!"
echo "======================================================================"
echo
echo "We're going to put all of the build output in "
echo
echo "==>    ${DIR}/build-dir/"
echo
mkdir -p "${DIR}/build-dir/bin"
# TODO(jawilson): roci should create this directory itself.
mkdir -p "${DIR}/build-dir/gen-files"

# ----------------------------------------------------------------------
# Boot-strap Compiler Discovery
# ----------------------------------------------------------------------
CC=""
for compiler in gcc clang tcc; do
    if command -v "$compiler" >/dev/null 2>&1; then
        CC="$compiler"
        break
    fi
done

if [ -z "$CC" ]; then
    echo "FAILURE: No suitable C compiler (gcc, clang, or tcc) was found in your PATH."
    exit 1
else
    echo "We've autodetected ${CC} as your *bootstrap* compiler."
    echo "    You can switch the C compiler with the CC environment variable."
    echo
    echo "    Right now this choice doesn't matter that much because omni-c-stable"
    echo "    is compiled without optimizations, warnings, etc."
    echo
fi

# ----------------------------------------------------------------------
# Verify libgc (-lgc) Availability
# ----------------------------------------------------------------------
echo "Checking for the Boehm-Demers-Weiser garbage collector."
echo "int main() { return 0; }" | $CC -I/opt/homebrew/opt/bdw-gc/include -L/opt/homebrew/opt/bdw-gc/lib -x c - -lgc -o /dev/null >/dev/null 2>&1

if [ $? -ne 0 ]; then
    echo "FAILURE: libgc (-lgc) is not installed or cannot be found by ${CC}."
    echo "Please install the Boehm-Demers-Weiser garbage collector development package."
    echo "  - Debian/Ubuntu: sudo apt-get install libgc-dev"
    echo "  - Fedora/RHEL:   sudo dnf install gc-devel"
    echo "  - macOS/Homebrew: brew install bdw-gc"
    exit 1
else
    echo "SUCCESS: libgc appears to be available."
    echo
fi

echo "Just a second, now we are going to build the boot-strap compiler..."
echo "    ${STABLE}"
echo
${CC} -w -o "${STABLE}" -I/opt/homebrew/opt/bdw-gc/include -L/opt/homebrew/opt/bdw-gc/lib "${DIR}/src/omni-c-stable-darwin.c" -lgc

if [ $? -ne 0 ]; then
    echo "FAILURE: Building omni-c-stable failed. This must be resolved to continue."
    exit 1
fi

echo "SUCCESS: building ${STABLE}!"
echo
echo "From now on you can just run: "
echo
echo "==>    BUILD_DIR=${DIR}/build-dir CC=${CC} ${STABLE} roci --script build.roci"
