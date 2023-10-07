all: build

build:
	clang                                   \
	  -I ~/src/tree-sitter/lib/include      \
	  c-to-sexpression.c                    \
	  ~/src/tree-sitter-c/src/parser.c      \
	  ~/src/tree-sitter/libtree-sitter.a    \
	  -o c-to-sexpression

diff:
	git difftool HEAD

clean:
	rm -f c-to-sexpression *~

