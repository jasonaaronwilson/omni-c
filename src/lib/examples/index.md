# Examples

This directory contains several example programs that utilize
`c-armyknife-lib`. You may even find a few of these useful!

## unique-file-lines

Like "cat" but keeps track of which lines have already been seen so
there will be no duplicate "unix" lines in the output. While this is
likely to be usable for some pretty large files if you have a lot of
RAM, it will eventually run out of memory.

## line-histogram

This program reads one or more files specified on the command line and
counts the number of occurrences of each line (except empty ones).

## sort-lines

This program reads one or more files specified on the command line and
outputs the sorted result using "strcmp" as the comparator.

## unique-line-generator

This program outputs lines containing (mostly) unique numbers using
the PRNG built-into armyknife lib.


