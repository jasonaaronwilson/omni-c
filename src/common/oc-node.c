#line 2 "oc-node.c"

/**
 * This is the parse tree after conversion from the similar parse tree
 * in TSNode's.
 */

#ifndef _OC_NODE_H_
#define _OC_NODE_H_

#include "../../../c-armyknife-lib/c-armyknife-lib.h"

enum oc_node_tag_t {
  OC_NODE_NONE,
  ...
  
};

struct oc_node_S {
  oc_node_tag_t tag;

  char* text; // only for leaf nodes like constants and identifiers

  uint64_t n_children;
  struct oc_node_S** children;

  // Additional fields are per tag and per analysis phase

  char* var_name; // an alpha renamed version of the variable name
                  // alpha renaming

  char* expr_type; // example: uint64 or string

  TSNode ts_node; // Unless this is generated code, we can access the
                  // original TSNode
};

// Recursively walks a ts_node and it's children and produces a
// oc_node_t* from it.
oc_node_t* ts_node_to_oc_node(TSNode ts_node);

#endif /* _OC_NODE_H_ */

oc_node_tag_t ts_node_name_to_tag(char* name) {
  return OC_NODE_NONE;
}

void extract_oc_node_text(oc_node_t* oc_node, TSNode ts_node, buffer_t* source_code) {
  if (ts_node_child_count(ts_node) == 0) {
    oc_node->text = buffer_c_substring(source_code, ts_node_start_byte(ts_node), ts_node_end_byte(ts_node));
  }
}

oc_node_t* ts_node_to_oc_node(TSNode ts_node, buffer_t* source_code) {
  oc_node_t* result = malloc_struct(oc_node_t);
  result->tag = ts_node_name_to_tag(ts_node_type(ts_node));
  extract_oc_node_text(result, ts_node, source_code);
  result->n_children = ts_node_named_child_count(ts_node);
  result->children = (oc_node_t**)(malloc_bytes(sizeof(oc_node_t*) * result->n_children));
  for (int i = 0; i < result->n_children; i++) {
    result->children[i] = ts_node_to_oc_node(ts_node_named_child(ts_node, i), source_code);
  }
  return result;
}
