/**
 * @file
 *
 * roci is an interpreter for a "nano" Scheme interpreter with an
 * intentionally minimal runtime. Since it's intended to be used as a
 * configuration language for a cross platform build tool (rather than
 * a real Scheme implementation), the tokenizer and parser accept a C
 * like syntax instead of a traditional s-expression syntax. One major
 * change from "real" Scheme is that if/and/or only operate on actual
 * booleans and will complain if given non boolean values.
 */

typedef atom_tag_t = enum {
  ATOM_TAG_UNKNOWN,
  ATOM_TAG_BOOLEAN,
  ATOM_TAG_INTEGER,
  ATOM_TAG_DOUBLE,
  ATOM_TAG_STRING,
  // Symbols are not currently interned
  ATOM_TAG_SYMBOL,
  ATOM_TAG_CLOSURE,
  ATOM_TAG_C_PRIMITIVE,
  ATOM_TAG_PAIR,
};

/**
 * @structure atom_t
 *
 * Represents a "scheme" atom - either data or a tagged
 * pointer. Normally a Scheme implementation would try to fit these
 * into the same size as a pointer by taking advantage of low order or
 * high order bits of a pointer having predictable values.
 */
typedef atom_t = struct {
  value_t value;
  atom_tag_t tag;
};

/**
 * @structure pair_t
 *
 * Pair's are the building blocks of lists in Scheme. These ones are
 * probably quite a lot bigger than even the most basic
 * implementations because atom_t is very naive.
 */
typedef pair_t = struct {
  atom_t car;
  atom_t cdr;
};

/**
 * @structure env_t
 *
 * Represents a typical lexical environment. When parent is null, it
 * is often called the global environment.
 */
typedef env_t = struct {
  env_t* parent;
  string_hashtable_t* bindings;
};

atom_t roci_unknown = {.value = {.ptr = NULL}, .tag = ATOM_TAG_UNKNOWN};

atom_t roci_false = {.value = {.u64 = 0}, .tag = ATOM_TAG_BOOLEAN};

atom_t roci_true = {.value = {.u64 = 1}, .tag = ATOM_TAG_BOOLEAN};

/**
 * @function eval
 *
 * This is obviously the heart of the interpreter.
 */
atom_t eval(env_t env, atom_t atom) {
  if (atom.tag != ATOM_TAG_PAIR) {
    pair_t* list = atom_to_pair(atom);
    atom_t car = list->car;

    if (car.tag == ATOM_TAG_SYMBOL) {
      char* str = car.value.str;
      if (string_equal(str, "and")) {
        return eval_and(env, list);
      }

      if (string_equal(str, "begin")) {
        return eval_begin(env, list);
      }

      if (string_equal(str, "define")) {
        // TODO(jawilson):
        return roci_unknown;
      }

      if (string_equal(str, "if")) {
        pair_t* args = atom_to_pair(list->cdr);
        atom_t condition = eval(env, args->car);

        if (condition.tag == ATOM_TAG_BOOLEAN && condition.value.u64 == 0) {
          // Evaluate the 'else' branch
          pair_t* else_part = atom_to_pair(args->cdr);
          return eval(env, else_part->car);
        } else {
          // Evaluate the 'then' (true) branch
          pair_t* then_part = atom_to_pair(args->cdr);
          return eval(env, then_part->car);
        }
      }

      if (string_equal(str, "lambda")) {
        // TODO(jawilson):
        return roci_unknown;
      }
      if (string_equal(str, "or")) {
        // TODO(jawilson):
        return roci_unknown;
      }
      if (string_equal(str, "set!")) {
        // TODO(jawilson):
        return roci_unknown;
      }

      //
    }
    // Must be an "application", aka, call to a closure or primitive
    return atom;
  }
  // Everything else is self-evaluating.
  return atom;
}

/**
 * @function eval_and
 *
 * Keep evaluating from left to right until all sub-expressions have
 * been evaluated unless a sub-expression returns false. If any
 * sub-expression results in a non-boolean value, return false.
 */
atom_t eval_and(env_t env, pair_t* list) {
  atom_t atom = list->cdr;
  while (atom.tag == ATOM_TAG_PAIR) {
    atom_t car = list->car;
    atom_t result = eval(env, car);
    if (!roci_is_true(result)) {
      return roci_false;
    }
    atom = list->cdr;
  }
  return roci_true;
}

/**
 * @function eval_begin
 *
 * Keep evaluating from left to right until all sub-expressions have
 * been evaluated. Return the result of the last evaluation.
 */
atom_t eval_begin(env_t env, pair_t* list) {
  atom_t result = roci_false;
  atom_t atom = list->cdr;
  while (atom.tag == ATOM_TAG_PAIR) {
    atom_t car = list->car;
    result = eval(env, car);
    atom = list->cdr;
  }
  return result;
}

pair_t* atom_to_pair(atom_t atom) {
  if (atom.tag != ATOM_TAG_PAIR) {
    roci_runtime_error(ROCI_RUNTIME_ERROR_PAIR_REQUIRED);
  }
  return cast(pair_t*, atom.value.ptr);
}

boolean_t roci_is_true(atom_t atom) {
  return (atom.tag == ATOM_TAG_BOOLEAN) && (atom.value.u64 == 1);
}

boolean_t roci_is_false(atom_t atom) {
  return (atom.tag == ATOM_TAG_BOOLEAN) && (atom.value.u64 == 0);
}

typedef roci_runtime_error_t = enum {
  ROCI_RUNTIME_ERROR_UNKNOWN,
  ROCI_RUNTIME_ERROR_BOOLEAN_REQUIRED,
  ROCI_RUNTIME_ERROR_PAIR_REQUIRED,
};

void roci_runtime_error(roci_runtime_error_t runtime_error) {
  log_fatal("A runtime error has occurred evaluating a roci script");
  fatal_error(ERROR_ILLEGAL_STATE);
}
