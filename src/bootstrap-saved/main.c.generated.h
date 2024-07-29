/* Automatically extracted prototypes from main.c */

void do_print_tokens(value_array_t* tokens, char* message);

void print_tokens(void);

void extract_command(char* command);

void configure_flags(void);

void configure_print_tokens_command(void);

void configure_extract_enums_command(void);

void configure_extract_prototypes_command(void);

void configure_test_symbol_table_command(void);

void configure_generate_c_output_file(void);

boolean_t is_inlined_function(function_node_t* node);

void generate_c_output_file(boolean_t is_library);

int main(int argc, char** argv);

