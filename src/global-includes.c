/**
 * @file global-includes.c
 *
 * Since omni-c essentially concatenates all files in a library or
 * binary as one big compilation unit, we only need to have one copy
 * of each system include which we are going to start doing here. We
 * also don't need most header file includes (which will be done in
 * the next cleanup step).
 */

#include <ctype.h>
#include <errno.h>
#include <execinfo.h>
#include <fcntl.h>
#include <limits.h>
#include <signal.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/select.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <termios.h>
#include <time.h>
#include <unistd.h>
/* #include <gc.h> */
