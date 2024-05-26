//// This is like ends-in-backslash.c but also has a trailing
//// newline. Unlike that file, you can safely edit this with most
//// text editors without changing the intent of the file (as long as
//// you don't purposefully omit the final newline).

//// gcc -E seems to give the same warning as before, clang -E
//// continues to not apparently care and tcc -E stops printing a
//// warning.
////
//// My conclusion is that we can treat "\\\n" and "\\" <EOF>
//// similarly and no one will care...

#define foo() bar \
