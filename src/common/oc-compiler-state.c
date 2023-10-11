#ifndef _OC_COMPILER_STATE_H_
#define _OC_COMPILER_STATE_H_

#include "../../../c-armyknife-lib/c-armyknife-lib.h"

struct oc_compiler_state_S {
  string_hashtable_t* namespaces;
};

typedef struct oc_compiler_state_S oc_compiler_state_t;

extern oc_compiler_state_t* make_oc_compiler_state(void);

#endif /* _OC_COMPILER_STATE_H_ */

oc_compiler_state_t* make_oc_compiler_state(void) {
  oc_compiler_state_t* result = malloc_struct(oc_compiler_state_t);
  result->namespaces = make_string_hashtable(100);
  return result;
}
