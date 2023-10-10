#line 2 "oc-namespace.c"

#ifndef _OC_NAMESPACE_H_
#define _OC_NAMESPACE_H_

#include "../../../c-armyknife-lib/c-armyknife-lib.h"

struct oc_namesspace_S {
  char* name; // Example std, std::io or even the empty string.
  string_hashtable_t* functions;
  string_hashtable_t* variables;
  string_hashtable_t* structures;
  string_hashtable_t* enums;
  string_hashtable_t* typedefs;
}

#endif /* _OC_NAMESPACE_H_ */
