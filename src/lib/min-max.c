#line 2 "min-max.c"
/**
 * @file min-max.c
 *
 * Macros version of min and max functions.
 */

#ifndef _MIN_MAX_H_
#define _MIN_MAX_H_

/**
 * @macro min
 *
 * Produce the minimum of a and b. Prior to C23, we evalutate one of
 * the arguments twice but we eventually hope to evaluate each
 * argument only once and use the "auto" argument so that the macro
 * works for any type that can be compared with <.
 */
#define min(a, b) ((a) < (b) ? (a) : (b))

/**
 * @macro max
 *
 * Produce the maximum of a and b. Prior to C23, we evalutate one of
 * the arguments twice but we eventually hope to evaluate each
 * argument only once and use the "auto" argument so that the macro
 * works for any type that can be compared with >.
 */
#define max(a, b) ((a) > (b) ? (a) : (b))

#endif /* _MIN_MAX_H_ */
