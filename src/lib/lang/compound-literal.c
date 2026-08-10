/**
 * @file compound-literal.c
 *
 * This file exists solely for interfacing with early versions of the
 * omni-c compiler where it is temporarily more convenient to have a
 * keyword to denote compound literals (the ones that look like
 * casts).
 *
 * We don't need it when the literal looks like this:
 *
 * ```
 * point_t a = { 10, 20 };
 * int foo[5] = { 0 };
 * ```
 *
 * But we do need it when the the literal looks like this:
 *
 * ```
 * (point_t) { .x = 10, .y = 20 }
`* ```
 *
 * This is somewhat similar to usage of cast() macro though in the
 * case of cast, sometimes it actually becomes easier to read.
 *
 * ```
 * (int) (a - b) -----> cast(int, a - b)
 * ```
 */

// ======================================================================
// This section is extraced to compound-literal.h
// ======================================================================

#define compound_literal(type, ...) ((type) __VA_ARGS__)
