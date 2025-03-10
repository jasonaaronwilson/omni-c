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
    break;
  case PARSE_NODE_CASE_LABEL:
    node_list_add_node(&target_block->statements, node);
    break;
  case PARSE_NODE_COMPOUND_LITERAL:
    break;
  case PARSE_NODE_CONDITIONAL:
    // linearize_conditional(target_block, to_conditional_node(node));
    break;
  case PARSE_NODE_CONTINUE_STATEMENT:
    node_list_add_node(&target_block->statements, node);
    break;
  case PARSE_NODE_DEFAULT_LABEL:
    node_list_add_node(&target_block->statements, node);
    break;
  case PARSE_NODE_DESIGNATED_INITIALIZER:
    break;
  case PARSE_NODE_DO_STATEMENT:
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
    // linearize_if_statement(target_block, to_if_statement_node(node));
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
