# @file dynamic-value.c

dynamic_value_t is similar to value_t except that dynamic_value_t
is (and will always be) dynamically tagged at runtime whereas we
play games with value_t (and the C type-checker) so that it can
stand-in for cases where we would just as soon use C++ style
templates and some form of type-erasure (namely of course in the
small collections library we have). dynamic_value_t is not unlike
"any" in Go wherease value_t is kind of an abomination that
shouldn't have to exist if C was sane (C++ templates are sane -
much of the rest of C++ is over-engineered and some of it is even
kind of deprecated and not considered part of "Modern C++" while
still potentially being used by many older programs and libraries.)

A big downside to dynamic_value_t besides a little bit of extra
checking and run-time tag manipulation is that they are about
*twice* the size of value_t (128 bits vs 64 bits).

Dynamic value's are a good fit for something like s-expressions as
well as the values that may be manipulated inside of a dynamically
typed domain specific language.

In order to allow dynamic_value_t to work better with user defined
types, you can can define these two options before including
c-armyknife-lib.h.

@compiliation_option AK_USER_DYNAMIC_VALUE_TAGS()
@compiliation_option AK_USER_DYNAMIC_VALUE_UNION_MEMBERS()
 
