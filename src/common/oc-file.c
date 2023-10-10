#line 2 "oc-file.c"
/**
 * An oc_file contains either an Omni C source file, a standard C
 * source file, uninterpreted data (for embed), or at times perhaps
 * even a library.
 */

#ifndef _OC_FILE_H_
#define _OC_FILE_H_

enum oc_file_tag {
  OMNI_C_SOURCE_FILE,
  STD_C_SOURCE_FILE,
  DATA_FILE,
}

struct oc_file_S {
  oc_file_tag_t tag;
  char* file_name;
  buffer_t data;
  string_hashtable_t* namespaces;
}

#endif /* _OC_FILE_H_ */

