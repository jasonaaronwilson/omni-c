/*

In case the code outline doesn't speak for itself, this will be
different from ordinary recursive decent parsers that return a parse
node since we generate code while parsing to avoid ever creating parse
nodes. Were trying to capture the simplicty of something like tcc
without while maintaining SICP like simplificity (SICP of course
already works on "parse nodes" being the nature of Scheme, so I
couldn't just borrow that solution).

assignment_ok only ever goes from true to false. If multiple
assignments are seen, we will signal an error immediately like other
parse routines do in these situations. This allows us to reuse code
when parsing either a lhs or rhs to also handle just parsing a rhs
since we only want a single assignment per statement. (That's actually
a partially arbirary choice made to be similar to Scheme but I think
it will be beneficial for roci as some errors will get caught
earlier.)

We will incur the unsual recursive decent parser penality of doing a
bunch of recursive calls that essentially do no work (except doing a
bunch of symbol peeks and matches). We can definitely generate a
synthetic bechmark to see if this constant time overhead is unbearable
or not. We can try using inline to give the compiler more to work
with.

I think when this is finished, it will seem a bit elegant (though
definitely not fancy). Except for a bit of look-ahead in the recursive
leafs, the code structure itself heavily mirrors a standard recursive
decent expression parser.

*/

/*
void XXX_roci_compile_statement(roci_compiler_state_t* state) {
  token_t* token = roci_peek_token(state);
  log_info("CURRENT TOKEN IS %s", token_to_string(token));
  if (token_matches(token, "return")) {
    roci_compile_return(state);
  } else if (token_matches(token, "if")) {
    roci_compile_if(state);
  } else if (token_matches(token, "let")) {
    roci_compile_let(state);
  } else if (token_matches(token, "while")) {
    roci_compile_while(state);
  } else if (token_matches(token, "record")) {
    roci_compile_record(state);
  } else {
    roci_compile_call_or_assignment(state);
  }
}
*/

typedef assignment_cont_t = enum {
  ASSIGNMENT_CONTINUE_NONE,
  ASSIGNMENT_CONTINUE_DID_CALL,
  ASSIGNMENT_CONTINUE_VARIABLE_SET,
  ASSIGNMENT_CONTINUE_FIELD_SET,
  ASSIGNMENT_CONTINUE_INDEX_SET,
};

static inline boolean_t return_for_assignment(assignment_cont_t cont) {
  return (cont == ASSIGNMENT_CONTINUE_VARIABLE_SET)
    || (cont == ASSIGNMENT_CONTINUE_FIELD_SET)
    || (cont == ASSIGNMENT_CONTINUE_INDEX_SET);
}

void roci_compile_call_or_assignment_statement(roci_compiler_state_t* state) {
  boolean_t assignment_ok = true;
  assignment_cont_t cont = roci_compile_expression2(state, assignment_ok);
  if (cont == ASSIGNMENT_CONTINUE_NONE) {
    // error. we put something on the stack without anything to
    // consuming it. You won't be able to do "foo() && bar();" like in C...
  } else if (cont == ASSIGNMENT_CONTINUE_DID_CALL) {
    // check for semi colon and return after dropping the result
    finish_call_statement(state);
  } else if (cont == ASSIGNMENT_CONTINUE_VARIABLE_SET) {
    // figure out variable name before recursing on RHS
    finish_variable_assignment_statement(state);
  } else if (cont == ASSIGNMENT_CONTINUE_FIELD_SET) {
    // parse RHS and do an assignment (in reverse order?)
    finish_field_assignment_statement(state);
  } else if (cont == ASSIGNMENT_CONTINUE_INDEX_SET) {
    // parse RHS and emit call to operator_index_set function which
    // can either hard-code "methods" on the first argument foo[index]
    // = bar ===> operator_index_set(obj, index, value)
    finish_index_assignment_statement(state);
  }
}

///
///
/// As you read down the page, each level becomes higher precedence.
///
///

assignment_cont_t roci_compile_expression2(roci_compiler_state_t* state, boolean_t assignment_ok) {
  return roci_compile_logical_or(state, assignment_ok);
}

assignment_cont_t roci_compile_logical_or(roci_compiler_state_t* state, boolean_t assignment_ok) {
  assignment_cont_t cont = roci_compile_logical_and(state, assignment_ok);
  return cont;
  /*
  if (return_for_assignment(cont)) {
    return cont;
  }
  token_t* token = roci_peek_token(state);
  if (token_matches(token, "||")) {
    cont = roci_compile_logical_and(state, false);
    if (return_for_assignment(right_cont)) {
      return cont;
    }
    return cont;
  } else {
    return cont;
  }
  */
}

assignment_cont_t roci_compile_logical_and(roci_compiler_state_t* state, boolean_t assignment_ok) {
  assignment_cont_t cont = roci_compile_bitwise_or(state, assignment_ok);
  return cont;
  /*
  if (return_for_assignment(cont)) {
    return cont;
  }
  while (true) {
    token_t* token = roci_peek_token(state);
    if (!token_matches(token, "&")) {
      return cont;
    }
    roci_expect_token(state, "&");
    cont = roci_compile_bitwise_or(state, false);
    roci_emit_binary_operator(state, "operator_infix_&");
  }
  */
}

assignment_cont_t roci_compile_bitwise_or(roci_compiler_state_t* state, boolean_t assignment_ok) {
  return roci_compile_bitwise_xor(state, assignment_ok);
}

assignment_cont_t roci_compile_bitwise_xor(roci_compiler_state_t* state, boolean_t assignment_ok) {
  return roci_compile_bitwise_and(state, assignment_ok);
}

assignment_cont_t roci_compile_bitwise_and(roci_compiler_state_t* state, boolean_t assignment_ok) {
  return roci_compile_equality(state, assignment_ok);
}


boolean_t is_equality_operator(char* token_str) {
  return string_equal(token_str, "==") || string_equal(token_str, "!=");
}

assignment_cont_t roci_compile_equality(roci_compiler_state_t* state, boolean_t assignment_ok) {
  assignment_cont_t cont = roci_compile_relational(state, assignment_ok);
  if (return_for_assignment(cont)) {
    return cont;
  }
  while (true) {
    token_t* token = roci_peek_token(state);
    char* token_string = token_to_string(token);
    if (!is_equality_operator(token_string)) {
      return cont;
    }
    roci_expect_token(state, token_string);
    cont = roci_compile_relational(state, false);
    roci_emit_binary_operator(state, string_append("operator_infix", token_string), token);
  }
  return cont;
}

assignment_cont_t roci_compile_relational(roci_compiler_state_t* state, boolean_t assignment_ok) {
  return roci_compile_additive(state, assignment_ok);
}

boolean_t is_additive_operator(char* token_str) {
  return string_equal(token_str, "+") || string_equal(token_str, "-");
}

assignment_cont_t roci_compile_additive(roci_compiler_state_t* state, boolean_t assignment_ok) {
  assignment_cont_t cont = roci_compile_multiplicative(state, assignment_ok);
  if (return_for_assignment(cont)) {
    return cont;
  }
  while (true) {
    token_t* token = roci_peek_token(state);
    char* token_string = token_to_string(token);
    if (!is_additive_operator(token_string)) {
      return cont;
    }
    roci_expect_token(state, token_string);
    cont = roci_compile_multiplicative(state, false);
    roci_emit_binary_operator(state, string_append("operator_infix", token_string), token);
  }
  return cont;
}

boolean_t is_multiplicative_operator(char* token_str) {
  return string_equal(token_str, "*") || string_equal(token_str, "/") || string_equal(token_str, "%");
}

assignment_cont_t roci_compile_multiplicative(roci_compiler_state_t* state, boolean_t assignment_ok) {
  assignment_cont_t cont = roci_compile_unary(state, assignment_ok);
  if (return_for_assignment(cont)) {
    return cont;
  }
  while (true) {
    token_t* token = roci_peek_token(state);
    char* token_string = token_to_string(token);
    if (!is_multiplicative_operator(token_string)) {
      return cont;
    }
    roci_expect_token(state, token_string);
    cont = roci_compile_unary(state, false);
    roci_emit_binary_operator(state, string_append("operator_infix", token_string), token);
  }
  return cont;
}

assignment_cont_t roci_compile_unary(roci_compiler_state_t* state, boolean_t assignment_ok) {
  // prefix + or -
  return roci_compile_primitive(state, assignment_ok);
}

assignment_cont_t roci_compile_primitive(roci_compiler_state_t* state, boolean_t assignment_ok) {
  token_t* token = roci_peek_token(state);
  char* token_string = token_to_string(token);

  // TODO(jawilson): peek ahead for = or not (or call).

  if (token->type == TOKEN_TYPE_IDENTIFIER) {
    if (string_equal(token_string, "true")) {
      roci_next_token(state);
      roci_emit_opcode(state, ROCI_OPCODE_PUSH_TRUE);
    } else if (string_equal(token_string, "false")) {
      roci_next_token(state);
      roci_emit_opcode(state, ROCI_OPCODE_PUSH_FALSE);
    } else {
      token_t* next_token = roci_peek_over_tokens(state, 1);
      char* next_token_string = token_to_string(next_token);
      if (string_equal(next_token_string, "=")) {
	if (!assignment_ok) {
	  log_warn("multiple assignments or a bad assignment seen");
	  roci_compiler_error(state, ROCI_COMPILE_TIME_ERROR_UNEXPECTED_ASSIGNMENT);
	  /* NOT-REACHED */
	  return ASSIGNMENT_CONTINUE_NONE;
	}
	return ASSIGNMENT_CONTINUE_VARIABLE_SET;
      }
      if (string_equal(next_token_string, "(")) {
	roci_compile_function_call(state);
      } else {
	roci_next_token(state);
	roci_emit_get_var(state->current_bb, token_string);
	goto handle_postfix;
      }
    }
  }

  if (token->type == TOKEN_TYPE_STRING_LITERAL) {
    roci_next_token(state);
    char* str = string_unquote_c_string(token_to_string(token));
    buffer_append_byte(state->current_bb->opcodes, ROCI_OPCODE_PUSH_STRING);
    value_array_add(state->current_bb->data, str_to_value(str));
    goto handle_postfix;
  }

  if (token->type == TOKEN_TYPE_INTEGER_LITERAL) {
    roci_next_token(state);
    value_result_t parsed = string_parse_uint64(token_to_string(token));
    if (parsed.nf_error != NF_OK) {
      log_warn("Failed to parse integer token %s", token_to_string(token));
      roci_compiler_error(state, ROCI_COMPILE_TIME_ERROR);
    }
    buffer_append_byte(state->current_bb->opcodes, ROCI_OPCODE_PUSH_INTEGER);
    value_array_add(state->current_bb->data, parsed.val);
    goto handle_postfix;
  }

  if (token->type == TOKEN_TYPE_FLOAT_LITERAL) {
    roci_next_token(state);
    double dbl = string_parse_double(token_to_string(token));
    buffer_append_byte(state->current_bb->opcodes, ROCI_OPCODE_PUSH_DOUBLE);
    value_array_add(state->current_bb->data, dbl_to_value(dbl));
    goto handle_postfix;
  }

  if (string_equal(token_string, "(")) {
    roci_next_token(state); 
    assignment_cont_t cont = roci_compile_expression2(state, false);
    roci_expect_token(state, ")");
    goto handle_postfix;
  }

 handle_postfix:
  return roci_compile_postfix(state, assignment_ok);
}

assignment_cont_t roci_compile_postfix(roci_compiler_state_t* state, boolean_t assignment_ok) {
  token_t* token = roci_peek_token(state);
  char* token_string = token_to_string(token);
  
  if (string_equal(token_string, "[")) {
    roci_next_token(state);
    assignment_cont_t cont = roci_compile_expression2(state, false);
    token_t* close = roci_peek_token(state);
    roci_expect_token(state, "]");
    if (token_matches(roci_peek_token(state), "=")) {
      // FIXME HERE
      return ASSIGNMENT_CONTINUE_INDEX_SET;
    }
    roci_emit_binary_operator(state, "operator[]", close);
    return ASSIGNMENT_CONTINUE_NONE;
  }

  // Then handle postfix operators


  //
  // Handle calls
  // Handle index operations
  // Handle field reference operation
  //
  // Otherwise an error.
  return ASSIGNMENT_CONTINUE_NONE;
}

void roci_emit_binary_operator(roci_compiler_state_t* state, char* name, token_t* debug_token) {
  roci_emit_get_var(state->current_bb, name);

  roci_bb_builder_t* return_bb = roci_new_bblock(state, "return_bb");
  buffer_append_byte(state->current_bb->opcodes, ROCI_OPCODE_CALL);
  value_array_add(state->current_bb->data, i64_to_value(2));
  value_array_add(state->current_bb->data,
                  str_to_value(return_bb->bblock_label));
  state->current_bb = return_bb;
  roci_emit_debug_info(state, debug_token);
}


// Why do we need this? Seems misplaced...
void finish_call_statement(roci_compiler_state_t* state) {
  roci_expect_token(state, ";");
  roci_emit_opcode(state, ROCI_OPCODE_DROP);
}

void finish_variable_assignment_statement(roci_compiler_state_t* state) {
  token_t* varname_token = roci_next_token(state);
  // TODO(jawilson): verify identifier
  roci_emit_debug_info(state, varname_token);
  roci_expect_token(state, "=");
  roci_compile_expression2(state, false);
  roci_expect_token(state, ";");
  roci_emit_token_string_datum(state, token_to_string(varname_token));
  roci_emit_opcode(state, ROCI_OPCODE_SET_VAR);
}

void finish_field_assignment_statement(roci_compiler_state_t* state) {
  // parse RHS and do an assignment (in reverse order?)
}

void finish_index_assignment_statement(roci_compiler_state_t* state) {
  // parse RHS and emit call to operator_index_set function which
  // can either hard-code "methods" on the first argument foo[index]
  // = bar ===> operator_index_set(obj, index, value)
}

/*

These are the standard names Gemini suggested for recursive decent
parsing. We only need a subset of these:

   parse_expression()      // Top-level entry (comma operator or assignment)
   parse_assignment()      // = += -= *= /= %= &= ^= |= <<= >>=
   parse_conditional()     // ? : (ternary)
+  parse_logical_or()      // ||
+  parse_logical_and()     // &&
+  parse_bitwise_or()      // |
+  parse_bitwise_xor()     // ^
+  parse_bitwise_and()     // &
+  parse_equality()        // == !=
+  parse_relational()      // < <= > >=
   parse_shift()           // << >>
+  parse_additive()        // + - (traditionally "expression" in simple math)
+  parse_multiplicative()  // * / % (traditionally "term")
   parse_cast()            // (type) expr
+  parse_unary()           // ! ~ + - * & sizeof (traditionally "factor")
+  parse_primary()         // identifier, literal number/string, ( expr )

+  parse_postfix()         // foo(), arr[], obj.field, ptr->field, x++, x--

*/
