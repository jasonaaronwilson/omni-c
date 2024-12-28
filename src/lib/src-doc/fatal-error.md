# @file fatal-error.c

The intent is that everything but a normal program exit will end up
here. (To catch SIGSIGV errors you may call
configure_fatal_errors() first with catch_sigsegv set.)

Note that you can use fatal_error's to your advantage by setting
the environment variable ARMYKNIFE_FATAL_ERROR_SLEEP_SECONDS to
some value to give yourself enough time to attach a debugger.

In this case C's macros are paying off as the file and line number
are easy to obtain.

TODO(jawilson): environment variable to be quieter...
 
## @constants error_code_t
 
## @macro fatal_error

Terminates the program with a fatal error.
 
