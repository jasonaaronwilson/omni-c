/* Automatically extracted prototypes from main.c */

void do_print_tokens(value_array_t* tokens, char* message);

void print_tokens(void);

void extract_command(char* command);

void configure_parse_expression(void);

void configure_flags(void);

void configure_print_tokens_command(void);

void configure_extract_enums_command(void);

void configure_extract_prototypes_command(void);

void configure_generate_c_output_file(void);

boolean_t is_inlined_function(function_node_t* node);

void dump_symbol_table(char* phase_name, symbol_table_t* symbol_table);

void generate_c_output_file(boolean_t is_library);

void parse_expression_string_and_print_parse_tree(char* expression);

int main(int argc, char** argv);

