/*

 * @file quote-util.c
 *
 * A couple of routines to help with quoting and unquoting C strings.

char* quote_c_string(char* input) {
  if (input == nullptr) return nullptr;

  // Allocate buffer (account for at least the surrounding quotes)
  buffer_t* buf = make_buffer(strlen(input) + 10);

  // Opening quote
  buffer_append_byte(buf, '"');

  for (size_t i = 0; input[i] != '\0'; i++) {
    uint8_t c = cast(uint8_t, input[i]);

      switch (c) {
      case '"':  buffer_append_byte(buf, '\\'); buffer_append_byte(buf, '"');  break;
      case '\\': buffer_append_byte(buf, '\\'); buffer_append_byte(buf, '\\'); break;
      case '\n': buffer_append_byte(buf, '\\'); buffer_append_byte(buf, 'n');  break;
      case '\t': buffer_append_byte(buf, '\\'); buffer_append_byte(buf, 't');  break;
      case '\r': buffer_append_byte(buf, '\\'); buffer_append_byte(buf, 'r');  break;
      case '\b': buffer_append_byte(buf, '\\'); buffer_append_byte(buf, 'b');  break;
      case '\f': buffer_append_byte(buf, '\\'); buffer_append_byte(buf, 'f');  break;
      default:
	if (c >= 32 && c < 126) {
	  // Append normal character
	  buffer_append_byte(buf, c);
	  else {
	    buffer_printf(buf, "\\x%02x");
	  }
	}
	break;
      }
  }

  // Closing quote and null-terminator
  buffer_append_byte(buf, '"');

  return buffer_to_c_string(buf);
}
*/

