all: c-armyknife-lib

# MODE should be "traditional" or "omni-c".
#
# Frankly this only matter if you are making *changes* to
# c-armyknife-lib and since no one has every sent me a pull request,
# maybe c-armyknife-lib isn't popular.

# MODE := omni-c
MODE := traditional

### TODO(jawilson): also build and install a library in case someone
### wants to use c-armyknife-lib in a bigger project without ever
### defining C_ARMYKNIFE_LIB_IMPL.

install: c-armyknife-lib
	sudo install -m 755 c-armyknife-lib.h /usr/local/include/

include Makefile.sources

ORDERED_H := $(SRC_C:.c=.h)

### ----------------------------------------------------------------------
### Omni C Mode
### ----------------------------------------------------------------------

ifeq ($(MODE),omni-c)

OMNI_CC := ../omni-c/src/build/bin/omni-c

# This flag is currently optional since c-armyknife-lib doesn't use
# any special features of omni-c yet.
OMNI_C_FLAGS := --use-statement-parser=true

c-armyknife-lib:
	${OMNI_CC} generate-header-file ${OMNI_C_FLAGS} --output-file=c-armyknife-lib-omni-c.h ${SRC_C}
	${OMNI_CC} generate-library ${OMNI_C_FLAGS} --output-file=c-armyknife-lib-omni-c.c ${SRC_C}
	cat header-comment.txt >c-armyknife-lib.h
	echo '#ifdef C_ARMYKNIFE_LIB_IMPL' >>c-armyknife-lib.h
	cat c-armyknife-lib-omni-c.c >>c-armyknife-lib.h
	echo '#else /* C_ARMYKNIFE_LIB_IMPL */' >>c-armyknife-lib.h
	cat c-armyknife-lib-omni-c.h >>c-armyknife-lib.h
	echo '#endif /* C_ARMYKNIFE_LIB_IMPL */' >>c-armyknife-lib.h
	cat c-armyknife-lib.h | grep -v "#line" >c-armyknife-lib-no-lines.h

else ifeq ($(MODE),traditional)

### ----------------------------------------------------------------------
### Traditional Mode
### ----------------------------------------------------------------------

generate-header-files: ${SRC_C}
	/usr/local/bin/generate-header-file ${SRC_C}
	chmod -w ${ORDERED_H}

c-armyknife-lib: ${SRC_C} generate-header-files
	cat header-comment.txt ${ORDERED_H} >c-armyknife-lib.h
	echo '#ifdef C_ARMYKNIFE_LIB_IMPL' >>c-armyknife-lib.h
	cat ${SRC_C} >>c-armyknife-lib.h
	echo '#endif /* C_ARMYKNIFE_LIB_IMPL */' >>c-armyknife-lib.h
	cat c-armyknife-lib.h | grep -v "#line" >c-armyknife-lib-no-lines.h

else
    $(error MODE must be set to 'omni-c' or 'traditional')
endif

### ======================================================================

format:
	clang-format -i ${SRC_C} ${SRC_H} tests/*.c examples/*.c

clean:
	rm -rf *~ docs/*~ tests/*~ examples/*~ TAGS doxygen-docs *.o ${ORDERED_H} tests/build/* examples/build/*

diff: clean
	git difftool HEAD

how-big: clean
	cat ${SRC_C} ${SRC_H} | wc --lines

tags:
	etags ${SRC_C} ${SRC_H}

TESTS := \
	./compile-test.sh \
	./uint64-test.sh \
	./string-util-test.sh \
	./buffer-test.sh \
	./value-alist-test.sh \
	./string-alist-test.sh \
	./value-hashtable-test.sh \
	./string-hashtable-test.sh \
	./flag-test.sh \
	./tokenizer-test.sh \
	./io-test.sh \
	./value-array-test.sh \
	./random-test.sh \
	./value-tree-test.sh \
	./string-tree-test.sh \
	./logger-test.sh \
	./terminal-test.sh \
	./cdl-printer-test.sh \
	./sub-process-test.sh \
	./splitjoin-test.sh

test:
	(cd tests && mkdir -p build && ../run-tests.sh ${TESTS})

doc:
	c-markdown-extractor --output-dir=./src-doc/ *.c

cfd:	clean format diff

cf:	clean format

# These are generated files that I happen to check-in to make it
# easier for people to use this as a "single source file" but seem to
# diverge a bit which should be investigated (maybe as simple as I
# started using lazygit and I didn't include them like git add --all
# would do?)
very-clean: clean
	git checkout c-armyknife-lib-no-lines.h c-armyknife-lib.h	

