all: omni-c

BUILD_DIR := $(shell mkdir -p build build/bin build/gen-files build/tests build/tests/statements build/tests/expressions  ; echo `pwd`/build)

# The commented out STABLE_BINARY_PATH looks wrong. The wild-card
# should look for ../../omni-c-stable/src/omni-c-stable and use that
# if available though warn if that directory exists without a omni-c
# executable...
#
# STABLE_BINARY_PATH := $(if $(wildcard ${BUILD_DIR}/bin/omni-c),${BUILD_DIR}/bin//omni-c,../../omni-c-stable/src/omni-c)
STABLE_BINARY_PATH := ../../omni-c-stable/src/omni-c
OMNI_C := build/bin/omni-c

asdf:
	echo ${BUILD_DIR} -- ${STABLE_BINARY_PATH}

WARN_FLAGS := -Wall -Werror -Wno-error=format -Wno-format
CC_FLAGS := -g -rdynamic -I ${WARN_FLAGS}
CC_FLAGS_OPT := ${CC_FLAGS} -O3 

SRC_C_SELF = \
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

generate-reflection-embedding: runtime/reflection.h
	tools/embed-file.lua reflection_header runtime/reflection.h > ${BUILD_DIR}/gen-files/reflection-header.c

generate-header-files: ${SRC_C}
	cp -p ${SRC_C} ${BUILD_DIR}
	(cd ${BUILD_DIR} && ../../../c-single-source-file/generate-header-file ${SRC_C})

omni-c: ${SRC_C} generate-header-files
	gcc ${CC_FLAGS} -Ibuild -I. -o build/bin/omni-c ${SRC_C}

omni-c-opt: omni-c ${SRC_C} generate-header-files
	gcc ${CC_FLAGS_OPT} -Ibuild -I. -o build/bin/omni-c-opt ${SRC_C}

test-print-tokens: omni-c
	${OMNI_C} print-tokens --parse-and-print=true \
		../examples/structures.oc \
		../examples/functions.oc \
		../examples/typedefs.oc \
		../examples/global-variables.oc \
		../examples/simple-c-proprocess.oc \
		../examples/enum.oc

extract-enums: omni-c-opt
	/usr/bin/time --portability ./omni-c-opt extract-enums --unique-prototype-header-files=true ${SRC_C}
	./omni-c-opt extract-enums --unique-prototype-header-files=true ../examples/enum.oc
	wc --lines ${SRC_C}

extract-prototypes:
	$(STABLE_BINARY_PATH) extract-prototypes --unique-prototype-header-files=true ${SRC_C}
	mv *.c.generated.h build/
	wc --lines ${SRC_C}

do-generate-header-file: omni-c
	/usr/bin/time --portability ${OMNI_C} generate-header-file --output-file=global-header-file.h ${SRC_C}
	gcc -c -Wall -Werror -std=c99 global-header-file.h

# TODO(jawilson): -Werror 
self: omni-c
	/usr/bin/time --portability ${OMNI_C} generate-library --use-statement-parser=true --dump-symbol-table=false --output-file=build/self.c ${SRC_C_SELF}
	gcc -o build/bin/self -std=gnu99 -DOMNI_C_SELF_GENERATED ${WARN_FLAGS} self.c armyknife-lib-impl.c reflection-header.c


selfself: omni-c self
	/usr/bin/time --portability build/bin/self generate-library --dump-symbol-table=false --output-file=build/selfself.c ${SRC_C_SELF}
	gcc -o build/bin/selfself -std=gnu99 -DOMNI_C_SELF_GENERATED ${WARN_FLAGS} selfself.c armyknife-lib-impl.c reflection-header.c


bisect-self: omni-c
	tools/bisect.sh ${SRC_C_SELF}

## # Pattern rule for generating header files
## %.h: %.c
##	./omni-c generate-header-file --output-file=$@ $<

TESTS := $(wildcard tests/*.c tests/expressions/*.e tests/expressions/*.expr tests/statements/*.stmt)

test: omni-c
	tools/run-tests.lua ${TESTS}

bootstrap:
	./bootstrap.sh

save-bootstrap-prototypes: extract-prototypes
	cp -upv --target=bootstrap-saved/ build/*.c.generated.h

format:
	clang-format -i ${SRC_C}
	clang-format -i tools/random-code-generator.c

tags:
	etags *.c bootstrap-saved/*.h

clean:
	rm -f omni-c omni-c-opt
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
