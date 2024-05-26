#include <stdio.h>

// This program outputs a trivial text file which may or may not be a
// valid C file since a trailing backslash is not followed by a
// newline (or anything else...) The resulting file has proven
// difficult to produce with the text editors I know (emacs or
// nano). I suspect we can safely ignore this as long as it doesn't
// crash the compiler...

// (gcc generate-ends-in-backslash.c ; ./a.out > generated-ends-in-backslash.c ; gcc generated-ends-in-backslash.c ; ./a.out)

int main(int argc, void** argv) {
  // Notice no newline here...
  printf("%s", "#include <stdio.h>\n#include <stdlib.h>\nint main(int argc, char** argv) {exit(0);}");
  printf("\n#define foo() bar \\");
}

