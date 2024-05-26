//// *** DO NOT EDIT THE LAST LINE OF THIS FILE WITHOUT FULL
//// *** UNDERSTANDING OF WHAT IT IS TESTING.

//// I wrote this file to determine what common Linux C compilers will
//// do with a file that ends in "\" (without a newline)

//// jawilson@beelink:~/src/omni-c/examples$ gcc -E ends-in-backslash.c
//// # 0 "ends-in-backslash.c"
//// # 0 "<built-in>"
//// # 0 "<command-line>"
//// # 1 "/usr/include/stdc-predef.h" 1 3 4
//// # 0 "<command-line>" 2
//// # 1 "ends-in-backslash.c"
//// ends-in-backslash.c:16:19: warning: backslash-newline at end of file
////    16 | #define foo() bar			\
////       |
//// jawilson@beelink:~/src/omni-c/examples$ clang -E ends-in-backslash.c
//// # 1 "ends-in-backslash.c"
//// # 1 "<built-in>" 1
//// # 1 "<built-in>" 3
//// # 361 "<built-in>" 3
//// # 1 "<command line>" 1
//// # 1 "<built-in>" 2
//// # 1 "ends-in-backslash.c" 2
////
//// jawilson@beelink:~/src/omni-c/examples$ tcc -E ends-in-backslash.c
//// # 1 "ends-in-backslash.c"
//// # 1 "<command line>" 1
//// # 1 "ends-in-backslash.c" 2
////
//// ends-in-backslash.c:32: warning: foo redefined
////
//// This stuff has a tendendancy to change perhaps if posted to
//// Reddit... Here are the compiler versions I'm using (Debian
//// Bookworm if that matters...)
////
//// jawilson@beelink:~/src/omni-c/examples$ gcc --version
//// gcc (Debian 12.2.0-14) 12.2.0
//// Copyright (C) 2022 Free Software Foundation, Inc.
//// This is free software; see the source for copying conditions.  There is NO
//// warranty; not even for MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
////
//// jawilson@beelink:~/src/omni-c/examples$ clang --version
//// Debian clang version 14.0.6
//// Target: x86_64-pc-linux-gnu
//// Thread model: posix
//// InstalledDir: /usr/bin
//// jawilson@beelink:~/src/omni-c/examples$ tcc --version
//// tcc version 0.9.27 (x86_64 Linux)

//// My conclusion is that this is essentially "undefined" though we
//// see that gcc gives a decent warning, clang doesn't care (which
//// isn't a big deal), and tcc might be a bit confused.

//// Most text editors are so helpful they will automatically add a
//// newline at the end of this file but we don't want that here so
//// *don't edit this file please* unless you know what you are doing!
//// nano 7.2 doesn't appear to add the extra newline but Emacs and I
//// suspect most "text" editors will add a newline.

#define foo() bar \
