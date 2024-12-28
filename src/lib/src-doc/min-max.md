# @file min-max.c

Macros version of min and max functions.
 
## @macro max

Produce the maximum of a and b. Prior to C23, we evalutate one of
the arguments twice but we eventually hope to evaluate each
argument only once and use the "auto" argument so that the macro
works for any type that can be compared with >.
 
## @macro min

Produce the minimum of a and b. Prior to C23, we evalutate one of
the arguments twice but we eventually hope to evaluate each
argument only once and use the "auto" argument so that the macro
works for any type that can be compared with <.
 
