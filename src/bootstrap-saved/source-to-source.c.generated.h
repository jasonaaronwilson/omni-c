/* Automatically extracted prototypes from source-to-source.c */

token_t* generate_struct_name_from_typedef_name(token_t* name);

void split_structure_typedefs(symbol_table_t* symbol_table);

void reorder_symbol_table_typedefs(symbol_table_t* symbol_table);

void reorder_symbol_table_typedefs__process_binding(symbol_table_map_t* typedefs, symbol_table_binding_t* binding, value_array_t* reordered_bindings);

struct_node_t* get_full_structure_definition_node(symbol_table_binding_t* binding);

symbol_table_binding_t* resolve_typename_to_structure_binding(symbol_table_t* symbol_table, type_node_t* type_node);

void reorder_symbol_table_structures_process_binding(symbol_table_t* symbol_table, symbol_table_binding_t* binding, value_array_t* reordered_bindings);

void reorder_symbol_table_structures(symbol_table_t* symbol_table);

void convert_nullptr_to_null(value_array_t* tokens);

