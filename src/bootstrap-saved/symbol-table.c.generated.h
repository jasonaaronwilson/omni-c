/* Automatically extracted prototypes from symbol-table.c */

symbol_table_map_t* make_symbol_table_map(void);

symbol_table_t* make_symbol_table(void);

symbol_table_binding_t* symbol_table_map_get(symbol_table_map_t* map, char* key_string);

parse_node_t* symbol_table_map_get_only_definition(symbol_table_map_t* map, char* key_string);

void symbol_table_add_declaration_node(symbol_table_map_t* map, char* key_string, parse_node_t* node);

void symbol_table_add_declartions(symbol_table_t* symbol_table, declarations_node_t* root);

void add_parse_and_add_top_level_definitions(symbol_table_t* symbol_table, value_array_t* file_names);

buffer_t* symbol_table_stats(buffer_t* buffer, symbol_table_t* symbol_table);

buffer_t* buffer_appennd_dbg_symbol_table_map(buffer_t* buffer, symbol_table_map_t* symbol_table_map);

buffer_t* buffer_append_dgb_symbol_table(buffer_t* buffer, symbol_table_t* symbol_table);

