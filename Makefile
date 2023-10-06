all: doc-format

# --list-style decimal 

diff: clean
	git difftool HEAD

doc-format:
	mdformat --wrap 80 README.md

clean:
	find . -name *~ | xargs rm -f
