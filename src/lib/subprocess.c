#line 2 "process.c"
#ifndef _PROCESS_H_
#define _PROCESS_H_

/**
 * @file process.c
 *
 * Utility to launch a sub-processes and interfact with it.
 *
 * (This version will probably only work on Unix like operating
 * systems but we should be able to make an MS-Windows version at some
 * point.)
 */
typedef struct {
  value_array_t* argv;             // argv[0] is the program executable path
  pid_t pid;                       // Process ID
  int exit_status;                 // Child exit status
} subprocess_t;

subprocess_t* make
