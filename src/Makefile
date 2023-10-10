all: omni-c

SRC_C = common/oc-compiler-state.c \
	common/oc-namespace.c \
	common/oc-node.c \
	armyknife-lib-impl.c \
	main.c

generate-header-files: ${SRC_C}
	../../c-single-source-file/generate-header-file ${SRC_C}
	find . -name '*.h' | xargs -n 1 chmod u-w

omni-c: ${SRC_C} generate-header-files
	gcc -g -o omni-c ${SRC_C}

clean:
	rm -f omni-c
	find . -name '*.h' | xargs -n 1 rm -f
	find . -name '*~' | xargs -n 1 rm -f


