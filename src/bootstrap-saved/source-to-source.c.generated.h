/* Automatically extracted prototypes from source-to-source.c */

oc_token_t* generate_struct_name_from_typedef_name(oc_token_t* name);

void split_structure_typedefs(symbol_table_t* symbol_table);

void reorder_symbol_table_typedefs(symbol_table_t* symbol_table);

void reorder_symbol_table_typedefs__process_binding(symbol_table_map_t* typedefs, symbol_table_binding_t* binding, value_array_t* reordered_bindings);

void reorder_symbol_table_structures_process_binding(symbol_table_t* symbol_table, symbol_table_binding_t* binding, value_array_t* reordered_bindings);

void reorder_symbol_table_structures(symbol_table_t* symbol_table);

