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
	main.c

SRC_C := armyknife-lib-impl.c lexer-to-string-helper.c to-string-helpers.c reflection-header.c $(SRC_C_SELF)

# Derived list of header file names
SRC_H = $(SRC_C:.c=.h)

omni-c-stable: omni-c-stable.c
	gcc ${CC_FLAGS_OPT} -o omni-c-stable -std=gnu99 -DOMNI_C_SELF_GENERATED omni-c-stable.c

generate-reflection-embedding: runtime/reflection.h
	tools/embed-file.lua reflection_header runtime/reflection.h > ${BUILD_DIR}/gen-files/reflection-header.c

omni-c: omni-c-stable
	/usr/bin/time --portability ${STABLE_BINARY_PATH} generate-library --use-statement-parser=true --omit-c-armyknife-include=true --output-file=build/omni-c.c ${C_ARMYKNIFE_SRC_C} ${SRC_C_SELF} build/gen-files/reflection-header.c
	gcc ${CC_FLAGS_OPT} -o build/bin/omni-c -std=gnu99 -DOMNI_C_SELF_GENERATED build/omni-c.c

self: omni-c
	/usr/bin/time --portability ${OMNI_C} generate-library --use-statement-parser=true --omit-c-armyknife-include=true --output-file=build/self.c ${C_ARMYKNIFE_SRC_C} ${SRC_C_SELF} build/gen-files/reflection-header.c
	gcc ${CC_FLAGS_OPT} -o build/bin/self -DOMNI_C_SELF_GENERATED build/self.c

selfself: omni-c self
	/usr/bin/time --portability build/bin/self generate-library --use-statement-parser=true --omit-c-armyknife-include=true --output-file=build/selfself.c ${C_ARMYKNIFE_SRC_C} ${SRC_C_SELF} build/gen-files/reflection-header.c
	gcc ${CC_FLAGS_OPT} -o build/bin/selfself -std=gnu99 -DOMNI_C_SELF_GENERATED build/selfself.c

# This feels outdated and was probably more centered on finding bugs
# we could have found more easily if we simply have better error
# messages. Consider this dead.
bisect-self: omni-c
	tools/bisect.sh ${SRC_C_SELF}

## # Pattern rule for generating header files
## %.h: %.c
##	./omni-c generate-header-file --output-file=$@ $<

TESTS := $(wildcard tests/*.c tests/expressions/*.e tests/expressions/*.expr tests/statements/*.stmt)
SUITE_TESTS := $(shell grep -v '^#' test-suite.txt)

test: omni-c
	tools/run-tests.lua ${TESTS}

test-suite: omni-c
	tools/run-tests.lua ${SUITE_TESTS}

# bootstrap:
#	./bootstrap.sh

save-bootstrap-prototypes: extract-prototypes
	cp -upv --target=bootstrap-saved/ build/*.c.generated.h

format:
	clang-format -i ${SRC_C}
	clang-format -i tools/random-code-generator.c

tags:
	etags *.c bootstrap-saved/*.h

clean:
	rm -f omni-c-stable omni-c omni-c-opt
	find . -name '*.h' -not -path './bootstrap-saved/*' -not -path './runtime/*' | xargs -n 1 rm -f
	find . -name '*~' | xargs -n 1 rm -f
	find ../examples/*.enum-generated.h | xargs -n 1 rm -f
	rm -rf  build/
	rm -f tools/syntax-c
	rm -f test-out.c test-out.o self self.c selfself selfself.c global-header-file.h.gch a.out tools/a.out
	rm -f tests/a.out tmp.c
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

