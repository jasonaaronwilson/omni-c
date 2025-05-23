# ======================================================================
#
# This Makefile is invoked indirectly from OMNI_C_ROOT
#
# ======================================================================

.PHONY: all clean format tags diff cfd how-big cf benchmark omni-c-stable omni-c self selfself lib lib-examples

# ----------------------------------------------------------------------
# Aliases
# ----------------------------------------------------------------------

all: ${BUILD_DIR}/bin/omni-c

omni-c-stable: ${BUILD_DIR}/bin/omni-c-stable

omni-c: ${BUILD_DIR}/bin/omni-c

self: ${BUILD_DIR}/bin/self

selfself: ${BUILD_DIR}/bin/selfself

lib: ${BUILD_DIR}/bin/lib.oar

# ----------------------------------------------------------------------
# Variable Definitions
# ----------------------------------------------------------------------

# --- Variables Passed from Top-Level Makefile ---

# OMNI_C_ROOT: Root directory of the omni-c project.
# BUILD_DIR:   Directory for build artifacts (absolute path).
ifndef OMNI_C_ROOT
  $(error OMNI_C_ROOT is not defined)
endif

ifndef BUILD_DIR
  $(error BUILD_DIR is not defined)
endif

# --- Internal Variables ---

export ARMYKNIFE_LIB_LOG_LEVEL = WARN

# Import
include lib/Makefile.sources

C_ARMYKNIFE_SRC_C := $(addprefix lib/, $(SRC_C))

$(shell mkdir -p ${BUILD_DIR}/bin ${BUILD_DIR}/gen-files ${BUILD_DIR}/tests ${BUILD_DIR}/tests/statements ${BUILD_DIR}/tests/expressions ${BUILD_DIR}/tmp)

STABLE_BINARY_PATH := ${BUILD_DIR}/bin/omni-c-stable
OMNI_C := ${BUILD_DIR}/bin/omni-c

# no-unused-function could be interesting to look into
WARN_FLAGS := -Wall -Werror -Wno-error=format -Wno-format -Wno-error=return-local-addr -Wno-error=parentheses-equality -Wno-unused-function
CC_FLAGS := -g -rdynamic ${WARN_FLAGS}
CC_FLAGS_OPT := ${CC_FLAGS} -O3 

# --- Conditionally define flags for MacOS
# If IS_MAC_OS is 'true', GC_INCLUDE_FLAG becomes '-I/path/include', else empty.
GC_INCLUDE_FLAG := $(if $(filter true,${IS_MAC_OS}),-I${BREW_GC_PREFIX}/include)
# If IS_MAC_OS is 'true', GC_LIBPATH_FLAG becomes '-L/path/lib', else empty.
GC_LIBPATH_FLAG := $(if $(filter true,${IS_MAC_OS}),-L${BREW_GC_PREFIX}/lib)

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
	linearizer.c \
	main.c \
	archive-command.c \
	build-command.c \
	generate-c-output-file.c \
	generate-header-file-command.c \
	generate-library-command.c \
	c-compiler-backend.c \
	git-hash-object.c \
	print-tokens.c \
	parse-test.c \
	test-command.c \
	flags.c

SRC_C := $(SRC_C_SELF)

# ----------------------------------------------------------------------
# Target Definitions
# ----------------------------------------------------------------------

${BUILD_DIR}/bin/omni-c-stable: omni-c-stable.c
	gcc ${CC_FLAGS_OPT} ${GC_INCLUDE_FLAG} -o ${BUILD_DIR}/bin/omni-c-stable -std=gnu99 omni-c-stable.c ${GC_LIBPATH_FLAG} -lgc

${BUILD_DIR}/gen-files/reflection-header.c: embed/reflection.h ${OMNI_C_ROOT}/tools/embed-file.lua
	${OMNI_C_ROOT}/tools/embed-file.lua reflection_header embed/reflection.h > ${BUILD_DIR}/gen-files/reflection-header.c

${BUILD_DIR}/bin/lib.oar: ${C_ARMYKNIFE_SRC_C}
	${STABLE_BINARY_PATH} archive ${C_ARMYKNIFE_SRC_C} --archive-output-file=${BUILD_DIR}/bin/lib.oar

${BUILD_DIR}/bin/omni-c: ${BUILD_DIR}/bin/omni-c-stable ${BUILD_DIR}/gen-files/reflection-header.c ${C_ARMYKNIFE_SRC_C} ${SRC_C_SELF} lib
	/usr/bin/time -p ${STABLE_BINARY_PATH} generate-library --use-statement-parser=true --omit-c-armyknife-include=true --c-output-file=${BUILD_DIR}/omni-c.c ${BUILD_DIR}/bin/lib.oar ${SRC_C_SELF} ${BUILD_DIR}/gen-files/reflection-header.c
	gcc ${CC_FLAGS_OPT} ${GC_INCLUDE_FLAG} -o ${BUILD_DIR}/bin/omni-c -std=gnu99 ${BUILD_DIR}/omni-c.c ${GC_LIBPATH_FLAG} -lgc

# Test --dump-symbol-table=true -- not recommend within emacs since
# it's output handling is so slow though any fast enough terminal like
# gnome terminal or if you redirect to a file.
${BUILD_DIR}/bin/self: ${BUILD_DIR}/bin/omni-c
	/usr/bin/time -p ${OMNI_C} generate-library --use-statement-parser=true --omit-c-armyknife-include=true --c-output-file=${BUILD_DIR}/self.c ${BUILD_DIR}/bin/lib.oar ${SRC_C_SELF} ${BUILD_DIR}/gen-files/reflection-header.c
	gcc ${CC_FLAGS_OPT} ${GC_INCLUDE_FLAG} -o ${BUILD_DIR}/bin/self ${BUILD_DIR}/self.c ${GC_LIBPATH_FLAG} -lgc

${BUILD_DIR}/bin/selfself: ${BUILD_DIR}/bin/omni-c ${BUILD_DIR}/bin/self
	/usr/bin/time -p ${BUILD_DIR}/bin/self generate-library --use-statement-parser=true --omit-c-armyknife-include=true --c-output-file=${BUILD_DIR}/selfself.c ${BUILD_DIR}/bin/lib.oar ${SRC_C_SELF} ${BUILD_DIR}/gen-files/reflection-header.c
	gcc ${CC_FLAGS_OPT} ${GC_INCLUDE_FLAG} -o ${BUILD_DIR}/bin/selfself -std=gnu99 ${BUILD_DIR}/selfself.c ${GC_LIBPATH_FLAG} -lgc

# clang-format will take multiple files but if a file is missing it
# doesn't tell you which file is missing.
format:
	@for file in ${SRC_C} ${OMNI_C_ROOT}/tools/random-code-generator.c; do \
		echo "Formatting $$file"; \
		clang-format -i $$file; \
	done

tags:
	find . -type f -name "*.c" ! -name "omni-c-stable.c" -print0 | xargs -0 etags -o ${BUILD_DIR}/TAGS

clean:
	# This is all that should be necessary for a clean build
	find "${BUILD_DIR}" -maxdepth 1 -not -name 'config.mk' -not -path "${BUILD_DIR}" -exec rm -rf {} +

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

lib-examples:
	@$(MAKE) -C lib/lib/examples examples
