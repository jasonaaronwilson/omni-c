/* Automatically extracted prototypes from printer.c */

printer_t* make_printer(buffer_t* buffer, int indent_width);

printer_t* append_string(printer_t* printer, char* string);

printer_t* append_token(printer_t* printer, token_t* token);

printer_t* printer_newline(printer_t* printer);

printer_t* printer_space(printer_t* printer);

printer_t* printer_indent(printer_t* printer);

printer_t* printer_increase_indent(printer_t* printer);

printer_t* printer_decrease_indent(printer_t* printer);

