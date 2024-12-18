all: omni-c

export ARMYKNIFE_LIB_LOG_LEVEL = WARN

# Import
include ../../c-armyknife-lib/Makefile.sources

C_ARMYKNIFE_SRC_C := $(addprefix ../../c-armyknife-lib/, $(SRC_C))

# Google Gemini tells me this is a bad idea but I'm trying it out anyways.
BUILD_DIR := $(shell mkdir -p build build/bin build/gen-files build/tests build/tests/statements build/tests/expressions ; echo `pwd`/build)

STABLE_BINARY_PATH := ./omni-c-stable
OMNI_C := build/bin/omni-c

WARN_FLAGS := -Wall -Werror -Wno-error=format -Wno-format -Wno-error=return-local-addr
CC_FLAGS := -g -rdynamic -I ${WARN_FLAGS}
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

omni-c-stable: omni-c-stable.c
	gcc ${CC_FLAGS_OPT} -o omni-c-stable -std=gnu99 omni-c-stable.c

${BUILD_DIR}/gen-files/reflection-header.c: runtime/reflection.h
	tools/embed-file.lua reflection_header runtime/reflection.h > ${BUILD_DIR}/gen-files/reflection-header.c

omni-c: omni-c-stable ${BUILD_DIR}/gen-files/reflection-header.c
	/usr/bin/time --portability ${STABLE_BINARY_PATH} generate-library --use-statement-parser=true --omit-c-armyknife-include=true --output-file=build/omni-c.c ${C_ARMYKNIFE_SRC_C} ${SRC_C_SELF} build/gen-files/reflection-header.c
	gcc ${CC_FLAGS_OPT} -o build/bin/omni-c -std=gnu99 build/omni-c.c

self: omni-c
	/usr/bin/time --portability ${OMNI_C} generate-library --use-statement-parser=true --omit-c-armyknife-include=true --c-output-file=build/self.c ${C_ARMYKNIFE_SRC_C} ${SRC_C_SELF} build/gen-files/reflection-header.c
	gcc ${CC_FLAGS_OPT} -o build/bin/self build/self.c

selfself: omni-c self
	/usr/bin/time --portability build/bin/self generate-library --use-statement-parser=true --omit-c-armyknife-include=true --c-output-file=build/selfself.c ${C_ARMYKNIFE_SRC_C} ${SRC_C_SELF} build/gen-files/reflection-header.c
	gcc ${CC_FLAGS_OPT} -o build/bin/selfself -std=gnu99 build/selfself.c

# This feels outdated and was probably more centered on finding bugs
# we could have found more easily if we simply have better error
# messages. Consider this dead.
bisect-self: omni-c
	tools/bisect.sh ${SRC_C_SELF}

## # Pattern rule for generating header files
## %.h: %.c
##	./omni-c generate-header-file --c-output-file=$@ $<

TESTS := $(wildcard tests/*.c tests/expressions/*.e tests/expressions/*.expr tests/statements/*.stmt)
SUITE_TESTS := $(shell grep -v '^#' test-suite.txt)

test: omni-c
	tools/run-tests.lua ${TESTS}

test-suite: omni-c
	tools/run-tests.lua ${SUITE_TESTS}

# clang-format will take multiple files but if a file is missing it
# doesn't tell you which file is missing.
format:
	@for file in ${SRC_C} tools/random-code-generator.c; do \
		echo "Formatting $$file"; \
		clang-format -i $$file; \
	done

# jawilson(fixme): not omni-c-stable
tags:
	etags *.c


clean:
	# This is all that should be necessary for a clean build
	rm -rf omni-c-stable build/
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
	find . -name '*.h' -not -path './runtime/*' | xargs -n 1 rm -f
	# Emacs backup files, my speciality! If you had an issue with
	# your text editor crashing and your leaving these behind,
	# then make clean is not your friend here. This 
	find . -name '*~' | xargs -n 1 rm -f
	# Unknown
	find ../examples/*.enum-generated.h | xargs -n 1 rm -f
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

