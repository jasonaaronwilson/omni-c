all: omni-c

# make && (cd / ; find ~/src/ -name '*.h' ) | xargs -n 5 ./omni-c build | grep WARNING 1>/tmp/foo
# (cd / ; find ~/src/ -name '*.c' ) | xargs -n 5 ./omni-c build | grep WARNING

FOO_SRC_C =  \
	common/oc-namespace.c \
	armyknife-lib-impl.c \
	main.c

CC_FLAGS=-g -rdynamic -I ~/src/tree-sitter/lib/include 

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
	main.c

TREE_SITTER_FILES = ~/src/tree-sitter-c/src/parser.c ~/src/tree-sitter/libtree-sitter.a

generate-header-files: ${SRC_C}
	../../c-single-source-file/generate-header-file ${SRC_C}
	# find . -name '*.h' | xargs -n 1 chmod u-w

omni-c: ${SRC_C} generate-header-files
	gcc ${CC_FLAGS} -o omni-c ${SRC_C} ${TREE_SITTER_FILES}

test: omni-c
	./omni-c print-tokens --parse-and-print=true \
		../examples/structures.oc \
		../examples/functions.oc \
		../examples/typedefs.oc \
		../examples/global-variables.oc

format:
	clang-format -i ${SRC_C}

clean:
	rm -f omni-c
	find . -name '*.h' | xargs -n 1 rm -f
	find . -name '*~' | xargs -n 1 rm -f

diff: clean
	git difftool HEAD

cfd: clean format diff
	echo "nop"

how-big: clean
	cat ${SRC_C} | wc --lines

cf: format clean


