#ifndef _OC_NAMESPACE_H_
#define _OC_NAMESPACE_H_

#include "../../../c-armyknife-lib/c-armyknife-lib.h"

struct oc_namesspace_S {
  char* name; // Example std, std::io or even the empty string.

  string_hashtable_t* includes;
  string_hashtable_t* defines;
  string_hashtable_t* typedefs;
  string_hashtable_t* enums;
  string_hashtable_t* structure_forward_declarations;
  string_hashtable_t* structures;
  string_hashtable_t* prototypes;
  string_hashtable_t* inlines;
  string_hashtable_t* global_variables;
  string_hashtable_t* functions;
}

#endif /* _OC_NAMESPACE_H_ */
