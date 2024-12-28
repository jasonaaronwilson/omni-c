#line 2 "value-alist.c"
/**
 * @file value-alist.c
 *
 * An association list (a type of map) from value_t to value_t.
 */

#ifndef _VALUE_ALIST_H_
#define _VALUE_ALIST_H_

struct value_alist_S {
  struct value_alist_S* next;
  value_t key;
  value_t value;
};

typedef struct value_alist_S value_alist_t;

extern value_result_t value_alist_find(value_alist_t* list,
                                       value_comparison_fn cmp_fn, value_t key);

__attribute__((warn_unused_result)) extern value_alist_t*
    value_alist_insert(value_alist_t* list, value_comparison_fn cmp_fn,
                       value_t key, value_t value);

__attribute__((warn_unused_result)) extern value_alist_t*
    value_alist_delete(value_alist_t* list, value_comparison_fn cmp_fn,
                       value_t key);

__attribute__((warn_unused_result)) extern uint64_t
    value_alist_length(value_alist_t* list);

/**
 * @macro value_alist_foreach
 *
 * Allows iteration over the keys and values in a string association
 * list.
 */
#define value_alist_foreach(alist, key_var, value_var, statements)             \
  do {                                                                         \
    value_alist_t* head = alist;                                               \
    while (head) {                                                             \
      value_t key_var = head->key;                                             \
      value_t value_var = head->value;                                         \
      statements;                                                              \
      head = head->next;                                                       \
    }                                                                          \
  } while (0)

#endif /* _VALUE_ALIST_H_ */

/**
 * @function value_alist_insert
 *
 * Insert a new key and value into an assocation list.
 */
value_alist_t* value_alist_insert(value_alist_t* list,
                                  value_comparison_fn cmp_fn, value_t key,
                                  value_t value) {
  value_alist_t* result = malloc_struct(value_alist_t);
  result->next = value_alist_delete(list, cmp_fn, key);
  result->key = key;
  result->value = value;
  return result;
}

/**
 * @function value_alist_delete
 *
 * Delete the key and associated value from the given association
 * list. Neither the key nor the value associated are themselves
 * freed.
 */
value_alist_t* value_alist_delete(value_alist_t* list,
                                  value_comparison_fn cmp_fn, value_t key) {
  // This appears to be logically correct but could easily blow out
  // the stack with a long list.
  if (list == NULL) {
    return list;
  }
  if ((*cmp_fn)(key, list->key) == 0) {
    value_alist_t* result = list->next;
    free_bytes(list);
    return result;
  }
  list->next = value_alist_delete(list->next, cmp_fn, key);
  return list;
}

/**
 * @function value_alist_find
 *
 * Find the value associate with the given key. Use is_ok() or
 * is_not_ok() to see if the value is valid (i.e., if the key was
 * actually found).
 */
value_result_t value_alist_find(value_alist_t* list, value_comparison_fn cmp_fn,
                                value_t key) {
  while (list) {
    if (cmp_fn(key, list->key) == 0) {
      return compound_literal(value_result_t, {.val = list->value});
    }
    list = list->next;
  }
  return compound_literal(value_result_t, {.nf_error = NF_ERROR_NOT_FOUND});
}

/**
 * @function value_alist_length
 *
 * Determine the length of an alist.
 *
 * The alist argument MAY be null.
 */
__attribute__((warn_unused_result)) extern uint64_t
    value_alist_length(value_alist_t* list) {
  uint64_t result = 0;
  while (list) {
    result++;
    list = list->next;
  }
  return result;
}
