all: bin

bin: syntax-c.c
	gcc -o syntax-c syntax-c.c

test: bin
	echo "int x = 0;" | ./syntax-c
	! { echo "{" | ./syntax-c }

foo:
	! { echo "}" | ./syntax-c }
	! { echo "#error\n" | ./syntax-c }

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



