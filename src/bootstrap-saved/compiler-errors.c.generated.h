/* Automatically extracted prototypes from compiler-errors.c */

#ifdef OMNI_C_INCLUDE_GENERATED_HEADER_FILES
buffer_t* buffer_append_human_readable_error(buffer_t* buffer, compiler_error_t* error);

src_code_snippets_t get_source_code_snippet(buffer_t* buffer, uint64_t location, int before_lines, int after_lines);

char* do_common_replacements(char* template, compiler_error_t* error);

buffer_t* buffer_append_human_readable_tokenizer_error(buffer_t* buffer, compiler_error_t* error);

buffer_t* buffer_append_human_readable_parser_error(buffer_t* buffer, compiler_error_t* error);

#endif /* OMNI_C_INCLUDE_GENERATED_HEADER_FILES */
