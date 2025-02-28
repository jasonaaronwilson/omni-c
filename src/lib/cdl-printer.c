/**
 * @file cdl-printer.c
 *
 * There isn't a formal specification for CDL however it's very easy
 * to *pretty print* and definitely on the simpler side for
 * parsing. Most importantly, it tends to look "clean" or "open"
 * versus other formats.
 *
 * At first glance, CDL looks just like JSON if you imagine removing
 * all commas and replacing ":" with the more readable " = " sequence.
 *
 * CDL only really has tables (aka maps), arrays, and strings. Parsers
 * should utilize a schema to determine when to treat a string as
 * something else like a boolean (the standard is true/false),
 * integer, float, etc.
 *
 * The key to making CDL look sane is the string quoting rule. As long
 * as all of the characters (aka unicode code-points) in string are
 * "safe", there is no need to put double quotes around the
 * string. The "unsafe" characters are of a very limited set: code
 * points less than 32, code-point 44, and these code-points: " ",
 * "\\"", "'", "{", "}", "[", "]", "(", ")", "`", '#', ":", ",", "=".
 *
 * This library only provides a printer. For reading, we'll delay that
 * until Omni C can use it's reflection API to automatically write
 * readers for us.
 *
 * CDL stands for "clear data language", "C data language", or
 * "comma-less data language", take your pick!
 */

typedef struct {
  buffer_t* buffer;
  char* key_token;
  int indention_level;
} cdl_printer_t;

cdl_printer_t* make_cdl_printer(buffer_t* buffer) {
  cdl_printer_t* result = malloc_struct(cdl_printer_t);
  result->buffer = buffer;
  return result;
}

void cdl_indent(cdl_printer_t* printer) {
  buffer_append_repeated_byte(printer->buffer, ' ',
                              4 * printer->indention_level);
}

boolean_t is_safe_string(char* string) {
  buffer_t* buffer = buffer_from_string(string);
  for (int pos = 0; pos < buffer_length(buffer);) {
    utf8_decode_result_t decode_result = buffer_utf8_decode(buffer, pos);
    if (decode_result.error) {
      fatal_error(ERROR_ILLEGAL_UTF_8_CODE_POINT);
    }
    uint32_t code_point = decode_result.code_point;
    if (code_point <= 32) {
      return false;
    }
    switch (code_point) {
    case '"':
    case '#':
    case '(':
    case ')':
    case ',':
    case ':':
    case '=':
    case '[':
    case '\'':
    case ']':
    case '`':
    case '{':
    case '}':
      return false;
    }
    pos += decode_result.num_bytes;
  }
  return true;
}

void cdl_output_token(cdl_printer_t* printer, char* string) {
  cdl_indent(printer);
  if (printer->key_token != NULL) {
    buffer_printf(printer->buffer, "%s = %s\n", printer->key_token, string);
    printer->key_token = NULL;
  } else {
    buffer_printf(printer->buffer, "%s\n", string);
  }
}

void cdl_boolean(cdl_printer_t* printer, boolean_t boolean) {
  cdl_output_token(printer, boolean ? "true" : "false");
}

void cdl_string(cdl_printer_t* printer, char* string) {
  if (!is_safe_string(string)) {
    cdl_output_token(printer, string_printf("\"%s\"", string));
  } else {
    cdl_output_token(printer, string);
  }
}

void cdl_int64(cdl_printer_t* printer, int64_t number) {
  cdl_output_token(printer, string_printf("%ld", number));
}

void cdl_uint64(cdl_printer_t* printer, uint64_t number) {
  cdl_output_token(printer, uint64_to_string(number));
}

void cdl_double(cdl_printer_t* printer, double number) {
  cdl_output_token(printer, string_printf("%lf", number));
}

void cdl_start_array(cdl_printer_t* printer) {
  cdl_output_token(printer, "[");
  printer->indention_level += 1;
}

void cdl_end_array(cdl_printer_t* printer) {
  printer->indention_level -= 1;
  cdl_output_token(printer, "]");
}

void cdl_start_table(cdl_printer_t* printer) {
  cdl_output_token(printer, "{");
  printer->indention_level += 1;
}

void cdl_key(cdl_printer_t* printer, char* key) { printer->key_token = key; }

void cdl_end_table(cdl_printer_t* printer) {
  printer->indention_level -= 1;
  cdl_output_token(printer, "}");
}
