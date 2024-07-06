/* Automatically extracted prototypes from symbol-table.c */

symbol_table_map_t* make_symbol_table_map(void);

symbol_table_t* make_symbol_table(void);

void symbol_table_add_definition_node(symbol_table_map_t* map, char* key_string, parse_node_t* node);

void symbol_table_add_children(symbol_table_t* symbol_table, declarations_node_t* root);

