# @file test.c

A set of macros and inline functions to help write tests for the
armyknife library (or your own tests).
 
## @macro test_assert

Evaluate the condition and fail the test if the result is false.
 
## @macro test_assert_string_equal

Assert that two c strings are the same.
 
## @macro test_assert_uint64_equal

Assert that two values are the same by casting both of them to
uint64_t and seeing if they are "==".

Except that we lose the "location" information, this would be
better as an inline function rather than a macro.
 
## @macro test_fail

Immediately fail a test after printing a message (supplied the same
way as printf, log_fatal, etc.)

This is a macro instead of a function because it automatically
passes the current file and line number to test_fail_and_exit which
enables uniform and convenient error messagages according to GNU
guidelines (so they will look like gcc compiler errors if your IDE
understands those).

It is *advised* (at least for now) to not pass complicated
expressions to `test_fail` if those are likely to fail. Instead run
the test in a debugger and set a break-point on
`test_fail_and_exit`.
 
## @function test_fail_and_exit

Set a break-point here to debug a test but normally you will use
the macro `test_fail` since it is much more convenient.
 
