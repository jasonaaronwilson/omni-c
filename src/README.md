# Omni C Compiler Source Code

This is the Omni C transpiler source code.

## Compiling Omni C

If you just want to use Omni C, currently just do this:

1. sudo apt install <prerequisites> (including gcc, tcc, clang and
   lua5.4, lua-filesystem). Obviously this is specific to your linux
   flavor.
1. cd ~
1. mkdir src
1. cd src
1. download and unpack https://hboehm.info/gc/gc_source/gc.tar.gz
1. cd gc-8.2.8 ; ./configure && make && make install
1. `git clone https://github.com/jasonaaronwilson/omni-c.git`
1. `(cd omni-c/src/ ; make && make test)`

There's no "install" for omni-c yet because it's mostly only
appropriate for compiling itself.

## Developing Omni C

Now that omni-c is self-hosting via omni-c-stable.c, you can pretty
much just start modifying files and run make.

Occasionally we will want to update omni-c-stable.c. The key is to
never add a feature and use it within omni-c until you've checked in
sufficient tests for a new feature.

## Automatically Testing Omni C

`make test` should be all you need though you may want to add more
tests (or debug a broken test). Currently I'm not tracking test
coverage though after our alpha release I will look into it.

Omni C uses a lua script (tools/run-tests.lua) to run the small number
of tests that we currently have. These are definitely not *unit tests*
as they invoke the full Omni C binary and can't be run unless the full
omni-c compiler can be built.

Tests come in roughly roughly varieties:

1. "text" - we invoke the omni-c compiler with one or more human
   readable text files (and the command line arguments for that test
   "type"), and then we compare the output to a human readable golden
   text file (if it exists). These give us some of the benefits of
   "unit tests" without having a zillion extra compilation targets to
   worry about building. While these *could* be semi-readable C output
   files (aka the default output format for Omni C), we actually
   output internal data-structures in the "cson" format" (comma-less
   JSON).

   Currently tests names ending in ".e" are treated as "expression
   tests" (which help us tell about operator precdence, etc.). I'll
   soon add "stmt" tests (and probably rename ".e" files to ".expr").

2. "native" - (these end in .c) and contain legal source programs
   which we should be able to run through omni-c to create an output
   library in C output format, invoke a "real" C compiler, and then
   invoke the executable with zero arguments and see if they return 0
   (success) or non-zero. If the file doesn't contain the word "main",
   we do most of the work except we just build a .o file and consider
   that success because partial programs without a main routine
   obviously can not be executed.

3. "script" - the lua test runner actually started as a simple bash
   based test runner I wrote for c-armyknife-lib that simply invoked a
   list of anything Unix can run and made the summary readable at a
   glance at the end (how many passed, which ones failed, all at the
   bottom). Since unix allows executable scripts, you can literally do
   almost anything and make it a test. If there will be 3 or more
   tests of the same type, it's probably best to just incorporate the
   logic into run-tests.lua

Even if I thought omni-c was going to be over a million lines of code
(my target is 25K for the pure C transpiler plus 10K for the initial
standard library on top of "libc"), I'm not sure I would do it that
differently - especially since we can always change the approach
later.

If you come from a Java world, then it seems insane to start up a new
JVM for each test but C isn't Java (which may be a reason you might
prefer using Omni C when it makes sense):

My barebones Intel N100 PC showed some differences but this only
really allows me to tell if starting Java is 5-10X slower than echo or
omni-c.


```
/usr/bin/time -v echo
	Command being timed: "echo"
	User time (seconds): 0.00
	System time (seconds): 0.00
	Percent of CPU this job got: 0%
	Elapsed (wall clock) time (h:mm:ss or m:ss): 0:00.00

/usr/bin/time -v ./omni-c
	Command being timed: "./omni-c"
	User time (seconds): 0.00
	System time (seconds): 0.00
	Percent of CPU this job got: 0%
	Elapsed (wall clock) time (h:mm:ss or m:ss): 0:00.00

/usr/bin/time -v java
	Command being timed: "java"
	User time (seconds): 0.05
	System time (seconds): 0.02
	Percent of CPU this job got: 116%
	Elapsed (wall clock) time (h:mm:ss or m:ss): 0:00.06
```

Using a script to execute these 100 times, the numbers are:

```
	Command being timed: "./tools/repeat.sh echo"
	User time (seconds): 0.00
	System time (seconds): 0.00
	Percent of CPU this job got: 80%
	Elapsed (wall clock) time (h:mm:ss or m:ss): 0:00.00

	Command being timed: "./tools/repeat.sh ./omni-c"
	User time (seconds): 0.03
	System time (seconds): 0.11
	Percent of CPU this job got: 102%
	Elapsed (wall clock) time (h:mm:ss or m:ss): 0:00.13

	Command being timed: "./tools/repeat.sh ./omni-c parse-expression --expression=tgt += src1 + src2;"
	User time (seconds): 0.05
	System time (seconds): 0.12
	Percent of CPU this job got: 103%
	Elapsed (wall clock) time (h:mm:ss or m:ss): 0:00.18

	Command being timed: "./tools/repeat.sh java"
	User time (seconds): 4.23
	System time (seconds): 1.59
	Percent of CPU this job got: 125%
	Elapsed (wall clock) time (h:mm:ss or m:ss): 0:04.65
```

For more realism, instead of just invoking omni-c without arguments,
we actually go ahead of parse the expression and generate the test
output making it more realistic and we are 0.18s vs 4.65s which is
almost 26 times faster. Invoking omni-c with no arguments (which
produces the "help/usage" text, is actually often slower than parsing
the expression and printing the output so most of the time is being
spent starting up omni-c but it isn't as terrible as it would be if
omni-c was written in Java.
