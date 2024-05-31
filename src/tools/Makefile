all: bin

define expect_success
$(if $(filter-out 0,$(shell $1 > /dev/null 2>&1; echo $$?)), \
    $(error Command "$1" failed unexpectedly (expected success)) \
)
endef

define expect_failure
$(if $(filter 0, $(shell $1; echo $$?)), \
    $(error Command "$1" succeeded unexpectedly (expected failure)) \
)
endef

bin: syntax-c.c
	gcc -o syntax-c syntax-c.c

test: bin
	$(call expect_failure, echo "{" | ./syntax-c)
	$(call expect_failure, echo "}" | ./syntax-c)
	$(call expect_success, echo "/* Hello world */" | ./syntax-c)
	$(call expect_success, echo "#include <stdio.h>" | ./syntax-c)

format:
	clang-format -i ${SRC_C}

clean:
	rm ./syntax-c
	find . -name '*.h' | xargs -n 1 rm -f
	find . -name '*~' | xargs -n 1 rm -f

diff: clean
	git difftool HEAD

cf: format clean

cfd: clean format diff
	echo "nop"



