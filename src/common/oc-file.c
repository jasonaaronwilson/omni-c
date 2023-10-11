/**
 * An oc_file_t contains either an Omni C source file, a standard C
 * source file, uninterpreted data (for embed), or at times perhaps
 * even a library.
 */

#ifndef _OC_FILE_H_
#define _OC_FILE_H_

#include <tree_sitter/api.h>
#include "../../../c-armyknife-lib/c-armyknife-lib.h"

typedef enum {
  OMNI_C_SOURCE_FILE,
  STD_C_SOURCE_FILE,
  DATA_FILE,
} oc_file_tag_t;

struct oc_file_S {
  oc_file_tag_t tag;
  char* file_name;
  buffer_t* data;
  TSTree *tree;
};

typedef struct oc_file_S oc_file_t;

#endif /* _OC_FILE_H_ */

