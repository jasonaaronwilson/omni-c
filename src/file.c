/**
 * An file_t contains either an Omni C source file, a standard C
 * source file, uninterpreted data (for embed), or at times perhaps
 * even a library.
 */

#ifndef _FILE_H_
#define _FILE_H_

#include <c-armyknife-lib.h>

typedef enum {
  OMNI_C_SOURCE_FILE,
  STD_C_SOURCE_FILE,
  DATA_FILE,
} file_tag_t;

typedef struct file_S {
  file_tag_t tag;
  char* file_name;
  buffer_t* data;
} file_t;

#include "file.c.generated.h"

#endif /* _FILE_H_ */
