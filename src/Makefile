# ======================================================================
#
# Just run: "make" from this directory
#
# (Eventually we'll have an "install" target.)
#
# ======================================================================

.PHONY: all clean format tags test test-suite diff cfd how-big cf benchmark omni-c-stable omni-c self selfself selftest

# ----------------------------------------------------------------------
# Aliases
# ----------------------------------------------------------------------

all: build/bin/omni-c

omni-c-stable: build/bin/omni-c-stable

omni-c: build/bin/omni-c

self: build/bin/self

selfself: build/bin/selfself

# ----------------------------------------------------------------------
# Variable Definitions
# ----------------------------------------------------------------------

export ARMYKNIFE_LIB_LOG_LEVEL = WARN

# Import
include lib/Makefile.sources

C_ARMYKNIFE_SRC_C := $(addprefix lib/, $(SRC_C))

# Google Gemini tells me this is a bad idea but I'm trying it out anyways.
BUILD_DIR := $(shell mkdir -p build build/bin build/gen-files build/tests build/tests/statements build/tests/expressions ; echo `pwd`/build)

STABLE_BINARY_PATH := build/bin/omni-c-stable
OMNI_C := build/bin/omni-c

WARN_FLAGS := -Wall -Werror -Wno-error=format -Wno-format -Wno-error=return-local-addr
CC_FLAGS := -g -rdynamic ${WARN_FLAGS}
CC_FLAGS_OPT := ${CC_FLAGS} -O3 

SRC_C_SELF := \
	mode.c \
	keywords.c \
	file.c \
	file-reader.c \
	compiler-errors.c \
	lexer.c \
	token-list.c \
	token-transformer.c \
	parser.c \
	pstate.c \
	declaration-parser.c \
	node-list.c \
	debug-printer.c \
	c-file-printer.c \
	symbol-table.c \
	source-to-source.c \
	preprocessor.c \
	header-file-extractor.c \
	symbol-table-builder.c \
	srcgen.c \
	pratt-parser.c \
	statement-parser.c \
	type-parser.c \
	user-type-parser.c \
	variable-definition-parser.c \
	literal-parser.c \
	balanced-construct-parser.c \
	printer.c \
	global-includes.c \
	main.c

SRC_C := $(SRC_C_SELF)

# lib/tests/value-hashtable-test.sh
# lib/tests/string-tokenizer-test.sh \

LIB_TESTS := \
	lib/tests/buffer-test.sh \
	lib/tests/cdl-printer-test.sh \
	lib/tests/string-tokenizer-test.sh \
	lib/tests/string-tree-test.sh \
	lib/tests/string-util-test.sh \
	lib/tests/sub-process-test.sh \
	lib/tests/terminal-test.sh \
	lib/tests/uint64-test.sh \
	lib/tests/value-alist-test.sh \
	lib/tests/value-array-test.sh \
	lib/tests/value-tree-test.sh

TESTS := $(wildcard tests/*.c tests/expressions/*.e tests/expressions/*.expr tests/statements/*.stmt)
TESTS += $(LIB_TESTS)

SUITE_TESTS := $(shell grep -v '^#' test-suite.txt)

# ----------------------------------------------------------------------
# Target Definitions
# ----------------------------------------------------------------------

build/bin/omni-c-stable: omni-c-stable.c
	gcc ${CC_FLAGS_OPT} -o build/bin/omni-c-stable -std=gnu99 omni-c-stable.c

build/gen-files/reflection-header.c: runtime/reflection.h tools/embed-file.lua
	tools/embed-file.lua reflection_header runtime/reflection.h > build/gen-files/reflection-header.c

build/bin/omni-c: build/bin/omni-c-stable build/gen-files/reflection-header.c ${C_ARMYKNIFE_SRC_C} ${SRC_C_SELF}
	/usr/bin/time --portability ${STABLE_BINARY_PATH} generate-library --use-statement-parser=true --omit-c-armyknife-include=true --c-output-file=build/omni-c.c ${C_ARMYKNIFE_SRC_C} ${SRC_C_SELF} build/gen-files/reflection-header.c
	gcc ${CC_FLAGS_OPT} -o build/bin/omni-c -std=gnu99 build/omni-c.c

build/bin/self: build/bin/omni-c
	/usr/bin/time --portability ${OMNI_C} generate-library --use-statement-parser=true --omit-c-armyknife-include=true --c-output-file=build/self.c ${C_ARMYKNIFE_SRC_C} ${SRC_C_SELF} build/gen-files/reflection-header.c
	gcc ${CC_FLAGS_OPT} -o build/bin/self build/self.c

build/bin/selfself: build/bin/omni-c build/bin/self
	/usr/bin/time --portability build/bin/self generate-library --use-statement-parser=true --omit-c-armyknife-include=true --c-output-file=build/selfself.c ${C_ARMYKNIFE_SRC_C} ${SRC_C_SELF} build/gen-files/reflection-header.c
	gcc ${CC_FLAGS_OPT} -o build/bin/selfself -std=gnu99 build/selfself.c

test: build/bin/omni-c
	tools/run-tests.lua ${TESTS}

selftest: build/bin/self
	OMNI_C_EXECUTABLE=build/bin/self tools/run-tests.lua ${TESTS}

test-suite: build/bin/omni-c
	tools/run-tests.lua ${SUITE_TESTS}

# clang-format will take multiple files but if a file is missing it
# doesn't tell you which file is missing.
format:
	@for file in ${SRC_C} tools/random-code-generator.c; do \
		echo "Formatting $$file"; \
		clang-format -i $$file; \
	done

tags:
	etags *.c

clean:
	# This is all that should be necessary for a clean build
	rm -rf build/
	# I would rather a "presubmit" rule such as no files named
	# "a.out" to capture certain junk like this I've been known to
	# create.  TODO(jawilson): make a warning
	rm -f a.out tests/a.out tools/a.out
	# Again, this would be better as a presubmit rule.
	# 
	# You shouldn't be creating .h files here - this will possibly
	# end in tears and we should be more polite. I'm probably
	# going to rename runtime/reflection.h just to avoid a strange
	# special case.
	#
	find . -name '*.h' -not -path './runtime/*' -delete || true
	# Emacs backup files, my speciality! If you had an issue with
	# your text editor crashing and your leaving these behind,
	# then make clean is not your friend here. This 
	find . -name '*~' -delete || true
	# Unknown
	find ../examples/*.enum-generated.h -delete || true
	# Figure out if still needed
	rm -f tools/syntax-c
	rm -f test-out.c test-out.o
	rm -f self self.c selfself selfself.c global-header-file.h.gch
	rm -f tests/*.out tests/expressions/*.out tests/statements/*.out

diff: clean
	git difftool HEAD

cfd: clean format diff
	echo "nop"

how-big: clean
	cat ${SRC_C} | wc --lines

cf: format clean

# This isn't a serious benchmark but it kind of gets at the
# compile/run-tests nature of omni-c development so it's interesting
# to me.
benchmark:
	/bin/bash -c 'time (make clean && make && make test)'

