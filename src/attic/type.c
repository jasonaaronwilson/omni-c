#line 2 "type.c"
#ifndef _TYPE_H_
#define _TYPE_H_

#include <c-armyknife-lib.h>

/**
 * @file type
 *
 * This represents static types in a compiled program.
 */


/**
 * @enum type_kind_t
 *
 * Sub-categorizes the type_t
 */
typedef enum {
  TYPE_KIND_UNKNOWN,
  TYPE_KIND_PRIMITVE,
  TYPE_KIND_UNDEFINED_TYPE_NAME,
  TYPE_KIND_POINTER,
  TYPE_KIND_ARRAY,
  TYPE_KIND_ENUM,
  TYPE_KIND_STRUCTURE,
  TYPE_KIND_UNION,
} type_kind_t;

/**
 * @defined_constant
 *
 * Represents the max arity of 
 */
#define MAX_TYPE_COMPONENTS 8

/**
 * @structure type_node_t
 *
 * Represents primitive types like "int", user types like an enum, and
 * complex combinations.
 */
typedef struct type_node_S {
  type_kind_t type_kind;
  boolean_t is_resolved;
  char* c_type_string;
  int64_t array_size; // -1 means no size
  struct type_node_S* components[MAX_TYPE_COMPONENTS];
} type_node_t;


