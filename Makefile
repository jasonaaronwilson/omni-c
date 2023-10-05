all: doc-format

# --list-style decimal 

doc-format:
	mdformat --wrap 80 README.md
