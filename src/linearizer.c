/**
 * @file linearizer.c
 *
 * The (parse-tree) linearizer takes a function body and converts it
 * into very simple statements such that the order of evaluation is
 * unambiguous (left to right evaluation...) and there are no "nested"
 * expressions (and certain forms like if or loops always have
 * "blocks" as children and variables (or constants) as conditions).
 *
 * Obviously this introduces many temporaries however the types of
 * these expressions are not known until the type-lowering phase is
 * done so we simply use "auto" as the type for now (in theory we
 * should be able to turn this into C23 and use c-file-printer.c to
 * test it).
 *
 * (If this was a native code compiler, we could try to "lower" to
 * bblocks and "IR" instead (or maybe directly to native code)).
 */

/**
 * @function linearize_statement
 *
 * Linearize a statement node into the target block.
 */
void linearize_statement(block_node_t* target_block,
                         tmp_provider_t* tmp_provider, parse_node_t* node) {
  switch (node->tag) {
  case PARSE_NODE_BLOCK:
    linearize_block(target_block, tmp_provider, to_block_node(node));
    break;
  case PARSE_NODE_BREAK_STATEMENT:
    node_list_add_node(&target_block->statements, node);
    break;
  case PARSE_NODE_CALL:
    linearize_statement_call_node(target_block, tmp_provider,
                                  to_call_node(node));
    break;
  case PARSE_NODE_CASE_LABEL:
    node_list_add_node(&target_block->statements, node);
    break;
  case PARSE_NODE_COMPOUND_LITERAL:
    // FIXME
    node_list_add_node(&target_block->statements, node);
    break;
  case PARSE_NODE_CONDITIONAL:
    // This shouldn't happen right?
    // linearize_conditional(target_block, to_conditional_node(node));
    break;
  case PARSE_NODE_CONTINUE_STATEMENT:
    node_list_add_node(&target_block->statements, node);
    break;
  case PARSE_NODE_DEFAULT_LABEL:
    node_list_add_node(&target_block->statements, node);
    break;
  case PARSE_NODE_DESIGNATED_INITIALIZER:
    // FIXME (or maybe this can't happen here, only as an expression?)
    node_list_add_node(&target_block->statements, node);
    break;
  case PARSE_NODE_DO_STATEMENT:
    linearize_do_statement(target_block, tmp_provider,
                           to_do_statement_node(node));
    break;
  case PARSE_NODE_EMPTY_STATEMENT:
    // Empty statements either disappear or get converted into empty
    // blocks
    break;
  case PARSE_NODE_EXPRESSION_STATEMENT:
    // linearize_expression_statement(target_block,
    // to_expression_statement_node(node));
    break;
  case PARSE_NODE_FOR_STATEMENT:
    // linearize_for_statement(target_block, to_for_statement_node(node));
    break;
  case PARSE_NODE_IF_STATEMENT:
    linearize_if_statement(target_block, tmp_provider,
                           to_if_statement_node(node));
    break;
  case PARSE_NODE_LABEL_STATEMENT:
    node_list_add_node(&target_block->statements, node);
    break;
  case PARSE_NODE_RETURN_STATEMENT:
    break;
  case PARSE_NODE_SWITCH_STATEMENT:
    break;
  case PARSE_NODE_VARIABLE_DEFINITION:
    break;
  case PARSE_NODE_WHILE_STATEMENT:
    break;

  case PARSE_NODE_BALANCED_CONSTRUCT:
  default:
    log_fatal("Unexpected input node to linearizer %s",
              parse_node_type_to_string(node->tag));
    fatal_error(ERROR_ILLEGAL_STATE);
  }
}

/**
 * @function linearize_expression
 *
 * Linearize an expression into the target block creating the given
 * temporary to hold the final result.
 */
void linearize_expression(block_node_t* target_block,
                          tmp_provider_t* tmp_provider, parse_node_t* node,
                          token_t* target) {
  switch (node->tag) {
  case PARSE_NODE_CALL:
    break;
  case PARSE_NODE_COMPOUND_LITERAL:
    break;
  case PARSE_NODE_CONDITIONAL:
    // linearize_conditional(target_block, to_conditional_node(node));
    break;
  case PARSE_NODE_IDENTIFIER:
    // linearize_identifier(target_block, to_identifier_node(node));
    break;
  case PARSE_NODE_LITERAL:
    break;
  case PARSE_NODE_OPERATOR:
    break;
  case PARSE_NODE_TYPE:
    break;
  case PARSE_NODE_VARIABLE_DEFINITION:
    break;
  case PARSE_NODE_BALANCED_CONSTRUCT:
    break;
  default:
    log_fatal("Unexpected input node to linearizer %s",
              parse_node_type_to_string(node->tag));
    fatal_error(ERROR_ILLEGAL_STATE);
  }
}

void linearize_block(block_node_t* target_block, tmp_provider_t* tmp_provider,
                     block_node_t* node) {
  uint64_t length = node_list_length(node->statements);
  for (uint64_t i = 0; i < length; i++) {
    linearize_statement(target_block, tmp_provider,
                        node_list_get(node->statements, i));
  }
}

// This is used when a statement is a call where the value is ignored
void linearize_statement_call_node(block_node_t* target_block,
                                   tmp_provider_t* tmp_provider,
                                   call_node_t* node) {
  call_node_t* rewritten = make_call_node();
  if (node->function->tag == PARSE_NODE_IDENTIFIER) {
    rewritten->function = node->function;
  } else {
    token_t* fn_ptr_target = tmp_provider->get(tmp_provider);
    linearize_expression(target_block, tmp_provider, node->function,
                         fn_ptr_target);
    rewritten->function = tmp_to_var_reference(fn_ptr_target);
  }

  for (int i = 0; i < node_list_length(node->args); i++) {
    parse_node_t* arg = node_list_get(node->args, i);
    token_t* arg_target = tmp_provider->get(tmp_provider);
    linearize_expression(target_block, tmp_provider, arg, arg_target);
    node_list_add_node(&rewritten->args, tmp_to_var_reference(arg_target));
  }
  node_list_add_node(&target_block->statements, to_node(rewritten));
}

void linearize_if_statement(block_node_t* target_block,
                            tmp_provider_t* tmp_provider,
                            if_statement_node_t* node) {

  token_t* condition_target = tmp_provider->get(tmp_provider);
  linearize_expression(target_block, tmp_provider, node->if_condition,
                       condition_target);
  // fix first token?
  block_node_t* if_true = make_block_node(nullptr);
  if (node->if_true != nullptr) {
    linearize_statement(if_true, tmp_provider, node->if_true);
  }

  block_node_t* if_else = make_block_node(nullptr);
  if (node->if_else != nullptr) {
    linearize_statement(if_else, tmp_provider, node->if_else);
  }

  node_list_add_node(
      &target_block->statements,
      to_node(make_if_statement(node->first_token,
                                tmp_to_var_reference(condition_target),
                                to_node(if_true), to_node(if_else))));
}

void linearize_do_statement(block_node_t* target_block,
                            tmp_provider_t* tmp_provider,
                            do_statement_node_t* node) {
  block_node_t* body = make_block_node(nullptr);
  linearize_statement(body, tmp_provider, node->body);
  token_t* condition_target = tmp_provider->get(tmp_provider);
  linearize_expression(body, tmp_provider, node->condition, condition_target);
  node_list_add_node(
      &target_block->statements,
      to_node(make_do_statement(node->first_token, to_node(body),
                                tmp_to_var_reference(condition_target))));
}

/* ================================================================ */

/**
 * @struct tmp_provider_t
 *
 * A "closure" that returns a new temporary variable name each time
 * get is called. We do this instead of using a single "global"
 * variable because we hope to one day be able to process top-level
 * forms in parallel but still have deterministic output.
 *
 * (At this point this is obviously not strictly necessary, but using
 * more unusual constructs in a self compiling compiler isn't
 * necessarily a bad thing...)
 */
typedef tmp_provider_t = struct {
  fn_t(token_t*) get;
  uint32_t count;
};

/**
 * @function make_tmp_provider
 *
 * Returns a new temporary variable name provider.
 */
tmp_provider_t* make_tmp_provider() {
  tmp_provider_t* result = malloc_struct(tmp_provider_t);
  result->get = &tmp_provider_get;
  return result;
}

token_t* tmp_provider_get(tmp_provider_t* data) {
  token_t* result = malloc_struct(token_t);
  result->buffer = make_buffer(8);
  buffer_printf(result->buffer, "tmp__%d", data->count++);
  result->end = result->buffer->length;
  return result;
}

parse_node_t* tmp_to_var_reference(token_t* tmp) {
  identifier_node_t* result = make_identifier_node();
  result->token = tmp;
  return cast(parse_node_t*, result);
}
