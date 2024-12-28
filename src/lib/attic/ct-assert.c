#line 2 "ct-assert.c"
/**
 * @file ct-assert.c
 *
 * Provide a basic compile time assert facility.
 */

#ifndef _CT_ASSERT_H_
#define _CT_ASSERT_H_

// Do a compile time assertion. Using shorter name so that if someone
// google's this they are more likely to find out where I got
// this. Note that this only works where a statement is allowed (not
// top-level in a file).

// When this fails, you'll see something like:
//
// main.c:18:3: error: array size is negative
//
// We can change to use vendor specific version in the future.

#define ct_assert(e) ((void) sizeof(char[1 - 2 * !(e)]))

#endif /* _CT_ASSERT_H_ */
