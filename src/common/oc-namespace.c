#ifndef _OC_NAMESPACE_H_
#define _OC_NAMESPACE_H_

#include "../../../c-armyknife-lib/c-armyknife-lib.h"

struct oc_namesspace_S {
  char* name; // Example std, std::io or even the empty string.

  // Use arrays (of oc_node_t) so that we perserve the input order,
  // relieve us of he burden of creating unique keys, and additionally
  // is more deterministic.
  value_array_t* includes;
  value_array_t* defines;
  value_array_t* typedefs;
  value_array_t* enums;
  value_array_t* structure_forward_declarations;
  value_array_t* structures;
  value_array_t* prototypes;
  value_array_t* inlines;
  value_array_t* global_variables;
  value_array_t* functions;
}

#endif /* _OC_NAMESPACE_H_ */
