//
// Explanation of why this file is about this change massively:
//
// I've had a change of heart on the direction of this interpreter
// (this one being so derivative). I've already accepted the "slight"
// pain of not just using the built in "read" to be both the lexical
// analyzer and the thing that side-steps the parsing process (only to
// do that over and over again in the interpreter however cheap
// compared to parsing, arguably not strictly worse than parsing
// unless analyzed and measured).
//
// I fooled around on paper and I'm simply curious about how this
// would work. I think we can use a tagged stack, tagged environment,
// and an untagged heap (since we use a conservative collector).
// 
// The idea to the tagged stack is to use two stack pointers, one to
// 64bit words, and one 1/8 of the size to hold a convenient 8-bit
// tag. (So two allocations instead of one, they can live anywhere, no
// spatial locality *required* but really small stacks are possible I
// guess). Incrementing or decrementing these pointers can be done in
// parallel and since this is an interpreted virtual machine and real
// machines are very wide now, there is essentially no real code size
// bloat (the size of the VM is a constant, dynamicly compile
// code...). Atomicity is maybe a downside but we have to deal with
// that in many ways on modern processors now (a conservative
// collector doesn't care but worth considering as a more general
// mode). Otherwise squint and pretend we have a 72bit computer! If
// you *trust* what you are loading from the heap, which you can,
// because of our strict code generation protocol. The tag stack only
// puts an extra 1/8 burden on the memory system (aka the TLB).
//
// The interesting part to me is actually the "instruction stream",
// namely what if it was also split that into two as well (we'll call
// them the opcode stream and an immediate stream). In this case the
// opcode stream always increments on every virtual cycle but the
// immediate stream only increments when needed. One goes a byte at a
// time, the other goes 64bits at a time (in the simplest version
// without having to do lot of adds for padding, not that big of a
// deal). If we further virtualize each basic block such that there is
// no assumed fallthrough (so each bblock ends in a two way branch
// (simply consuming two immediates), each basic block can have a
// header.
//
// The simplest header is actually just the offset to the immedidate
// stream (or a pointer to it if we want to waste space and make them
// 64bits). We have enough information to set *pc* and
// *immediates-pc*. A single byte might limit the size of a basic
// block to a few hundred instructions (though always easy to branch
// to a new basic block, just saying...). Like I said earlier, it
// could just be a full pointer, just not necessary. ULEB128 is not
// *super* crazy option.
//
// The simplest scheme laid out in memory looks like: header | opcode
// stream | [?padding (nops!)?] | immediate stream.
//
// If the opcode and immediate stream pointers have spatial locality,
// that is probably going to be good for performance and this is the
// norm with short basic blocks (loads from nearby locations are are
// optimized by all modern processors).
//
// How dense is this encoding?
//
// My first guess is that this encoding is not very dense especially
// on a 64bit machine because most basic blocks are very short. (Two
// way branches are less crazy on a 32bit version of this machine.) It
// will probably be better in strange ways like debugging.
//
// We are kind of just getting started: I want to do some funky
// dynamic dispatch based on the tag stack to find our next "dynamic"
// target (a hash lookup) and create a sensible but unusual machine
// that will require further insight.
// 

/**
 * @file
 *
 * Roci is an interpreter for a "nano" Scheme interpreter with an
 * intentionally minimal runtime (very limited I/O). It's intended to
 * be used as a configuration language for a cross platform build tool
 * (rather than a serious Scheme implementation -- or even a lame
 * speed-run towards Greenspuns's tenth rule).
 *
 * The tokenizer and parser adhere to C like syntax instead of a
 * traditional s-expression syntax.
 *
 * Unlike Javascript, you are going to still want those "semicolons"
 * to terminate simple statements. If you can't offer that, go
 *
 * Another major change from "real" Scheme is that if/and/or only
 * operate on actual booleans and will fatal error if given non boolean
 * values.
 *
 * You want to kill my code...
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

/**
 * @function eval_if
 *
 * Evaluate the conditional part of the expression. If it is not true
 * or false, then sigal an error, otherwise evaluate the "true" arm
 * (called in the consequence in SICP) or the "false" arm (called the
 * alternative in SICP). In this format, the then part must always be
 * present though the parser will assume the expression should return
 * "false" which works because the parser also special cases "return"
 * when parsing blocks so if statements never generate a value that
 * can be bound to a variable or returned.
 */
atom_t eval_if(env_t env, pair_t* list) {
  pair_t* args = atom_to_pair(list->cdr);
  atom_t condition = eval(env, args->car);

  if (condition.tag == ATOM_TAG_BOOLEAN) {
    roci_runtime_error(ROCI_RUNTIME_ERROR_BOOLEAN_REQUIRED);
  }

  if (condition.tag == ATOM_TAG_BOOLEAN && condition.value.u64 == 1) {
    // Evaluate the 'then' (true) branch (consequene)
    pair_t* then_part = atom_to_pair(args->cdr);
    return eval(env, then_part->car);
  } else {
    // Evaluate the 'else' branch (alternative)
    pair_t* else_part = atom_to_pair(atom_to_pair(args->cdr)->cdr);
    return eval(env, else_part->car);
  }
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
