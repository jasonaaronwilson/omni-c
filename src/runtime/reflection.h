#ifndef _REFLECTION_H_
#define _REFLECTION_H_

#define fn_t(return_type, ...) typeof(return_type(*)(__VA_ARGS__))

#include <string.h>

/* ====================================================================== */
// These are the reflection API data structures for a program compiled
// with Omni C. We use linked lists instead of value_array_t to keep
// compiled programs independent of any particular library
// data-structures.
/* ====================================================================== */

// ----------------------------------------------------------------------
// Enumerations
// ----------------------------------------------------------------------

typedef struct enum_element_metadata_S {
  struct enum_element_metadata_S* next;
  char* name;
  long value;
} enum_element_metadata_t;

/**
 * @structure enum_metadata_t
 *
 * The runtime metadata for a reflected enumeration.
 */
typedef struct {
  char* name;
  enum_element_metadata_t* elements;
} enum_metadata_t;

// ----------------------------------------------------------------------
// Structures
// ----------------------------------------------------------------------

typedef struct field_metadata_S {
  struct field_metadata_S* next;
  char* name;
  char* type_name_string;
  int start_offset;
} field_metadata_t;

/**
 * @structure structure_metadata_t
 *
 * The runtime metadata for a reflected structure.
 */
typedef struct {
  char* name;
  int size;
  int alignment;
  field_metadata_t* fields;
} structure_metadata_t;

// ----------------------------------------------------------------------
// Unions 
// ----------------------------------------------------------------------

/*
 * Unions could be treated exactly like structures however there is no
 * uniform way to tag a union so making use of the metadata is a bit
 * problematic. For that reason (and because omni-c doesn't need them
 * yet), we are skipping them for now.
 */

// ----------------------------------------------------------------------
// Functions
// ----------------------------------------------------------------------

typedef struct function_arg_metadata_S {
  struct function_arg_metadata_t* next;
  char* name;
  char* type_string;
} function_arg_metadata_t;

/**
 * @structure function_metadata_t
 *
 * The runtime metadata for a reflected structure.
 */
typedef struct {
  char* name;
  function_arg_metadata_t* arguments;
} function_metadata_t;

#endif /* _REFLECTION_H_ */
