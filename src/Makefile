all: omni-c

CC_FLAGS := -g -rdynamic -I ~/src/tree-sitter/lib/include
CC_FLAGS_OPT := -g -rdynamic -I ~/src/tree-sitter/lib/include -O3

SRC_C = armyknife-lib-impl.c \
	mode.c \
	keywords.c \
	oc-file.c \
	file-reader.c \
	compiler-errors.c \
	lexer.c \
	token-transformer.c \
	parser.c \
	to-string-helpers.c \
	node-list.c \
	debug-printer.c \
	header-file-printer.c \
	symbol-table.c \
	source-to-source.c \
	preprocessor.c \
	header-file-extractor.c \
	main.c

# Derived list of header file names
SRC_H = $(SRC_C:.c=.h)

generate-header-files: ${SRC_C}
	../../c-single-source-file/generate-header-file ${SRC_C}

omni-c: ${SRC_C} generate-header-files
	gcc ${CC_FLAGS} -o omni-c ${SRC_C}

omni-c-opt: omni-c ${SRC_C} generate-header-files
	gcc ${CC_FLAGS_OPT} -o omni-c-opt ${SRC_C}

test: omni-c
	./omni-c print-tokens --parse-and-print=true \
		../examples/structures.oc \
		../examples/functions.oc \
		../examples/typedefs.oc \
		../examples/global-variables.oc \
		../examples/simple-c-proprocess.oc \
		../examples/enum.oc

self-parse-test: omni-c-opt
	/usr/bin/time --portability ./omni-c-opt print-tokens --parse-and-print=true ${SRC_C}
	wc --lines ${SRC_C}

extract-enums: omni-c-opt
	/usr/bin/time --portability ./omni-c-opt extract-enums --unique-prototype-header-files=true ${SRC_C}
	./omni-c-opt extract-enums --unique-prototype-header-files=true ../examples/enum.oc
	wc --lines ${SRC_C}

extract-prototypes: omni-c-opt
	/usr/bin/time --portability ./omni-c-opt extract-prototypes --unique-prototype-header-files=true ${SRC_C}
	wc --lines ${SRC_C}

test-symbol-table: omni-c-opt
	/usr/bin/time --portability ./omni-c test-symbol-table ${SRC_C}
	./omni-c print-tokens --parse-and-print=true ../examples/struct-typedefs.oc
	ARMYKNIFE_LIB_LOG_LEVEL=info ./omni-c test-symbol-table ../examples/struct-typedefs.oc
	/usr/bin/time --portability ./omni-c test-symbol-table ../examples/out-of-order-typedef.oc
	/usr/bin/time --portability ./omni-c test-symbol-table ../examples/in-order-typedef.oc
	/usr/bin/time --portability ./omni-c test-symbol-table ../examples/in-order-structures.oc
	/usr/bin/time --portability ./omni-c test-symbol-table ../examples/out-of-order-structures.oc
	wc --lines ${SRC_C}

do-generate-header-file: omni-c-opt
	/usr/bin/time --portability ./omni-c generate-header-file --output-file=gloabl-header-file.h ${SRC_C}

## # Pattern rule for generating header files
## %.h: %.c
##	./omni-c generate-header-file --output-file=$@ $<

bootstrap:
	./bootstrap.sh

save-bootstrap-prototypes: extract-prototypes
	cp -upv --target=bootstrap-saved/ *.c.generated.h

format:
	clang-format -i ${SRC_C}

clean:
	rm -f omni-c omni-c-opt
	find . -name '*.h' -not -path './bootstrap-saved/*' | xargs -n 1 rm -f
	find . -name '*~' | xargs -n 1 rm -f
	find ../examples/*.enum-generated.h | xargs -n 1 rm -f
	rm -f tools/syntax-c

diff: clean
	git difftool HEAD

cfd: clean format diff
	echo "nop"

how-big: clean
	cat ${SRC_C} | wc --lines

cf: format clean
