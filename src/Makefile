all: omni-c

# make && (cd / ; find ~/src/ -name '*.h' ) | xargs -n 5 ./omni-c build | grep WARNING 1>/tmp/foo
# (cd / ; find ~/src/ -name '*.c' ) | xargs -n 5 ./omni-c build | grep WARNING

FOO_SRC_C =  \
	common/oc-namespace.c \
	armyknife-lib-impl.c \
	main.c

CC_FLAGS := -g -rdynamic -I ~/src/tree-sitter/lib/include -DOMNI_C_INCLUDE_GENERATED_HEADER_FILES 

CC_FLAGS_OPT := -g -rdynamic -I ~/src/tree-sitter/lib/include -DOMNI_C_INCLUDE_GENERATED_HEADER_FILES -O3

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
	main.c

# TODO(jawilson): generate this list automatically from the above
# which is a little less risky for the first checkin which we can live
# with for a bit.
SRC_C_GENERATED_H = armyknife-lib-impl.c.header.h \
	mode.c.header.h \
	keywords.c.header.h \
	oc-file.c.header.h \
	file-reader.c.header.h \
	compiler-errors.c.header.h \
	lexer.c.header.h \
	token-transformer.c.header.h \
	parser.c.header.h \
	to-string-helpers.c.header.h \
	node-list.c.header.h \
	debug-printer.c.header.h \
	header-file-printer.c.header.h \
	main.c.header.h

generate-header-files: ${SRC_C}
	../../c-single-source-file/generate-header-file ${SRC_C}
	rm -f ${SRC_C_GENERATED_H}
	touch ${SRC_C_GENERATED_H}
	# find . -name '*.h' | xargs -n 1 chmod u-w

omni-c: ${SRC_C} generate-header-files
	gcc ${CC_FLAGS} -o omni-c ${SRC_C}

omni-c-opt: ${SRC_C} generate-header-files
	gcc ${CC_FLAGS_OPT} -o omni-c-opt ${SRC_C}

test: omni-c
	./omni-c print-tokens --parse-and-print=true \
		../examples/structures.oc \
		../examples/functions.oc \
		../examples/typedefs.oc \
		../examples/global-variables.oc \
		../examples/simple-c-proprocess.oc

self-parse-test: omni-c-opt
	/usr/bin/time --portability ./omni-c-opt print-tokens --parse-and-print=true ${SRC_C}
	wc --lines ${SRC_C}

extract-prototypes: omni-c-opt
	/usr/bin/time --portability ./omni-c-opt extract-prototypes --unique-prototype-header-files=true ${SRC_C}
	wc --lines ${SRC_C}

format:
	clang-format -i ${SRC_C}

clean:
	rm -f omni-c omni-c-opt
	find . -name '*.h' | xargs -n 1 rm -f
	find . -name '*~' | xargs -n 1 rm -f
	rm -f tools/syntax-c

diff: clean
	git difftool HEAD

cfd: clean format diff
	echo "nop"

how-big: clean
	cat ${SRC_C} | wc --lines

cf: format clean
