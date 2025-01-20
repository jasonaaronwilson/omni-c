all: doc-format

# --list-style decimal 

diff: clean
	git difftool HEAD

doc-format:
	mdformat --wrap 80 README.md

keywords:
	column -c 70 ~/src/omni-c/documentation/keywords.txt

clean:
	find . -name *~ | xargs rm -f
