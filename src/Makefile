all: omni-c

FOO_SRC_C = common/oc-compiler-state.c \
	common/oc-namespace.c \
	common/oc-node.c \
	armyknife-lib-impl.c \
	main.c

CC_FLAGS=-g -rdynamic

SRC_C = armyknife-lib-impl.c \
	main.c

generate-header-files: ${SRC_C}
	../../c-single-source-file/generate-header-file ${SRC_C}
	find . -name '*.h' | xargs -n 1 chmod u-w

omni-c: ${SRC_C} generate-header-files
	gcc ${CC_FLAGS} -o omni-c ${SRC_C}

format:
	clang-format -i ${SRC_C}

clean:
	rm -f omni-c
	find . -name '*.h' | xargs -n 1 rm -f
	find . -name '*~' | xargs -n 1 rm -f

diff: clean
	git difftool HEAD

how-big: clean
	cat ${SRC_C} | wc --lines

cf: format clean


