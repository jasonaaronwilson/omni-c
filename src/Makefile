all: omni-c

STABLE_BINARY_PATH := $(if $(wildcard omni-c),./omni-c,../../omni-c-stable/src/omni-c)

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
	main.c

SRC_C := armyknife-lib-impl.c lexer-to-string-helper.c to-string-helpers.c $(SRC_C_SELF)


# Derived list of header file names
SRC_H = $(SRC_C:.c=.h)

generate-header-files: ${SRC_C}
	../../c-single-source-file/generate-header-file ${SRC_C}

omni-c: ${SRC_C} generate-header-files
	gcc ${CC_FLAGS} -o omni-c ${SRC_C}

omni-c-opt: omni-c ${SRC_C} generate-header-files
	gcc ${CC_FLAGS_OPT} -o omni-c-opt ${SRC_C}

test-print-tokens: omni-c
	./omni-c print-tokens --parse-and-print=true \
		../examples/structures.oc \
		../examples/functions.oc \
		../examples/typedefs.oc \
		../examples/global-variables.oc \
		../examples/simple-c-proprocess.oc \
		../examples/enum.oc

self-parse-test: omni-c-opt
	/usr/bin/time --portability ./omni-c-opt print-tokens --parse-and-print=true ${SRC_C_SELF}
	wc --lines ${SRC_C}

extract-enums: omni-c-opt
	/usr/bin/time --portability ./omni-c-opt extract-enums --unique-prototype-header-files=true ${SRC_C}
	./omni-c-opt extract-enums --unique-prototype-header-files=true ../examples/enum.oc
	wc --lines ${SRC_C}

extract-prototypes:
	$(STABLE_BINARY_PATH) extract-prototypes --unique-prototype-header-files=true ${SRC_C}
	wc --lines ${SRC_C}

do-generate-header-file: omni-c-opt
	/usr/bin/time --portability ./omni-c generate-header-file --output-file=global-header-file.h ${SRC_C}
	gcc -c -Wall -Werror -std=c99 global-header-file.h

# TODO(jawilson): -Werror 
self: omni-c
	/usr/bin/time --portability ./omni-c generate-library --dump-symbol-table=false --output-file=self.c ${SRC_C_SELF}
	gcc -o self -std=gnu99 -DOMNI_C_SELF_GENERATED ${WARN_FLAGS} self.c armyknife-lib-impl.c

## # Pattern rule for generating header files
## %.h: %.c
##	./omni-c generate-header-file --output-file=$@ $<

TESTS := $(wildcard tests/*.sh)

test: omni-c
	tools/run-tests.lua ${TESTS}

bootstrap:
	./bootstrap.sh

save-bootstrap-prototypes: extract-prototypes
	cp -upv --target=bootstrap-saved/ *.c.generated.h

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
	rm -f tools/syntax-c
	rm -f test-out.c test-out.o self self.c global-header-file.h.gch a.out tools/a.out

diff: clean
	git difftool HEAD

cfd: clean format diff
	echo "nop"

how-big: clean
	cat ${SRC_C} | wc --lines

cf: format clean
