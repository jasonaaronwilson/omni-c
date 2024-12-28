#line 2 "string-tree.c"

/**
 * @file string-tree.c
 *
 * This is a balanced binary tree to associate a string to a value
 * using an underlying value-tree.c.
 */

#ifndef _STRING_TREE_H_
#define _STRING_TREE_H_

struct string_tree_S {};

typedef struct string_tree_S string_tree_t;

/**
 * @function string_tree_find
 *
 * Find the value associate with the key in the tree.
 */
static inline value_result_t string_tree_find(string_tree_t* t, char* key) {
  return value_tree_find(cast(value_tree_t*, t), cmp_string_values,
                         str_to_value(key));
}

/**
 * @function string_tree_insert
 *
 * Insert an association of key and a value (or update the current
 * value stored in the tree).
 */
__attribute__((warn_unused_result)) static inline string_tree_t*
    string_tree_insert(string_tree_t* t, char* key, value_t value) {
  return cast(string_tree_t*,
              value_tree_insert(cast(value_tree_t*, t), cmp_string_values,
                                str_to_value(key), value));
}

/**
 * @function string_tree_delete
 *
 * Delete the association of key (if it exists in the tree). It is not
 * an error to delete a key that isn't present in the table.
 */
__attribute__((warn_unused_result)) static inline string_tree_t*
    string_tree_delete(string_tree_t* t, char* key) {
  return cast(string_tree_t*,
              value_tree_delete(cast(value_tree_t*, t), cmp_string_values,
                                str_to_value(key)));
}

/**
 * @macro string_tree_foreach
 *
 * Perform an inorder traversal of a string-tree.
 *
 * key_var is created in a new block scope with type char*.
 *
 * value_var is created in a new block scope with type value_t and you
 * will probably want to use something like ".ptr" or ".u64" on the
 * value to obtain the actual value.
 *
 * statements should be a normal C block, aka, something like:
 * ```
 * {
 *   statement1();
 *   statement2();
 * }
 * ```
 *
 * Unforunately it is not possible to use "break" or "continue" with
 * this style of loop (and worse, there will be no compilation error
 * or warning if you accidentally do that...)
 */
#define string_tree_foreach(tree, key_var, value_var, statements)              \
  do {                                                                         \
    value_tree_foreach(cast(value_tree_t*, tree), key_var##_value, value_var,  \
                       {                                                       \
                         char* key_var = (key_var##_value).str;                \
                         statements;                                           \
                       });                                                     \
  } while (0)

#endif /* _STRING_TREE_H_ */
