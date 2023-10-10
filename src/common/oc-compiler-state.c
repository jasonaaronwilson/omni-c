#line 2 "oc-compiler-state.c"

#ifndef _OC_COMPILER_STATE_H_
#define _OC_COMPILER_STATE_H_

#include "../../../c-armyknife-lib/c-armyknife-lib.h"

struct oc_compiler_state_S {
  char* name; // Example std, std::io or even the empty string.
  string_hashtable_t* namespaces;
}

#endif /* _OC_COMPILER_STATE_H_ */
