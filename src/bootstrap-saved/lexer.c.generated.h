/* Automatically extracted prototypes from lexer.c */

char* token_to_string(oc_token_t* token);

oc_token_t* make_derived_token(oc_token_t* source_token);

__attribute__((warn_unused_result)) buffer_t* append_token_debug_string(buffer_t* buffer, oc_token_t token);

buffer_t* buffer_append_token_string(buffer_t* buffer, oc_token_t* token);

token_or_error_t tokenize_whitespace(buffer_t* buffer, uint64_t start_position);

boolean_t is_identifier_start(uint32_t code_point);

token_or_error_t tokenize_identifier(buffer_t* buffer, uint64_t start_position);

token_or_error_t tokenize_numeric(buffer_t* buffer, uint64_t start_position);

token_or_error_t tokenize_punctuation(buffer_t* buffer, uint64_t start_position);

boolean_t is_comment_start(buffer_t* buffer, uint64_t position);

token_or_error_t tokenize_comment(buffer_t* buffer, uint64_t start_position);

boolean_t is_string_literal_start(buffer_t* buffer, uint64_t position);

token_or_error_t tokenize_quoted_literal_common(buffer_t* buffer, uint64_t start_position, char* opening_sequence, char* quoted_closing_sequence, char* closing_sequence, token_type_t token_type, tokenizer_error_t unterminated_error_code);

token_or_error_t tokenize_string_literal(buffer_t* buffer, uint64_t start_position);

boolean_t is_character_literal_start(buffer_t* buffer, uint64_t position);

token_or_error_t tokenize_character_literal(buffer_t* buffer, uint64_t start_position);

oc_tokenizer_result_t tokenize(buffer_t* buffer);

