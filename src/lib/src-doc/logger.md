# @file logger.c

A "logger" is a type of code instrumentation and provides the
ability to put explicit "print statements" into your code without
necessarily having a large impact on the performance of that code
as long as the logger is turned off for that "level" of detail.

If you've temporarily inserted print statements into your program,
you've probably already learned that logging is very useful and
also somewhat expensive in terms of run-time performance. [^1]

Since logging is expensive, it is useful to be able to turn it on
and off (even without recompiling) which is done based on the
logging level. When logging is turned off, the cost is meant to be
equivalent to a load, compare, and branch though logging can effect
the C compiler's optimizations, so it is **not** recommended to be
left in critical loops. Obviously if the code is compiled into the
binary, even if the code to skip the logging doesn't considerably
increase run-time performance, it may still have an impact for
example on the output binary size.

The default log level is "WARN" though it is possible to override
this with #define ARMYKNIFE_LIB_DEFAULT_LOG_LEVEL <level> when
including the library implementation (or from your build command
which allows C preprocessor definitions to be injected into your
source code, one reason you may want a debug vs production builds).

Additionally, when the first log statement is encountered, we
examine the environment variable named ARMYKNIFE_LIB_LOG_LEVEL if
you want to adjust the level after compilation. Future versions
will certainly provide more control such as turn on logging only
for specific files as well as giving the C compiler enough
information to remove some logging code completely from the
binary).

There are a set number of levels and they are defined like so:

OFF = 0
TRACE = 1
DEBUG = 2
INFO = 3
WARN = 4
FATAL = 5
TEST = 6

The most overlooked part of logging may be that putting PII or
other information into logs may violate GDPR and other privacy laws
depending on how the logs are processed and retained. Our
recommendation is to never intentionally log PII. It's especially
important to keep this in mind if you are developing an internet
application that the user isn't running on their own machine which
isn't an initial focus of this library.

[^1]: For this implementation, getting a timestamp is probably one
kernel call and doing the actual output, since logging is less
useful when buffered, requires at least another kernel
call. Finally, formatting strings for human readability is
relatively expensive itself. For example, printing a large number
may require dozens or hundreds of cycles while adding two numbers
may take less than a single cycle on a modern pipelined processor).
 
## @macro log_debug

Log at the DEBUG level using printf style formatting.
 
## @macro log_fatal

Log at the FATAL level using printf style formatting.

Typically this is only done before invoking fatal_error though I
don't have a convenient way to enforce this.
 
## @macro log_info

Log at the INFO level using printf style formatting.
 
## @macro log_none

This will never ever log and should have essentially zero impact on
compilation (including detecting errors). In other words it should
behave like an empty statment ";".

On the other hand, errors in these statements will not be caught
and therefore it can't be depended on to keep working as you
refactor code and decide later that you want to turn it on.
 
## @macro log_off

This will never log however the compiler *should* still check to
make sure the code is legal and compiles. Any sufficiently smart
compiler with some level of optimization turned on should not
change it's code generation strategy at all if you leave one of these
statements in your source code and you should easily be able to
upgrade them to a real level later.
 
## @macro log_test

Log at the TEST level using printf style formatting. This should
only be used inside of test code to communicate very basic
information back to the user when running a test and is therefore
independent of the actual log level.
 
## @macro log_trace

Log at the TRACE level using printf style formatting.
 
## @macro log_warn

Log at the WARN level using printf style formatting.
 
## @function logger_impl

This is the non macro version entry point into the logger. Normally
it wouldn't be called directly since it is less convenient than the
macro versions.
 
## @function logger_init

This function modifies the logging level based on the environment
variable ARMYKNIFE_LIB_LOG_LEVEL (which currently must be a
number).

While not required to actually use logging, the logging level will
be set to LOGGER_WARN unless you change it in a debugger, and those
logging statements will be sent to stderr which is probably not
convenient.
 
## Determine if logging at the INFO level is enabled.
 
