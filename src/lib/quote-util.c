/**
 * @file quote-util.c
 *
 * A couple of routines to help with quoting and unquoting C strings.
 */

char* quote_c_string(char* input) {
  if (input == nullptr) return nullptr;

  buffer_t* buf = make_buffer(strlen(input) + 10);
  buffer_append_byte(buf, '"');

  for (size_t i = 0; input[i] != 0; i++) {
    uint8_t c = cast(uint8_t, input[i]);

    switch (c) {
    case '"':  buffer_append_byte(buf, '\\'); buffer_append_byte(buf, '"');  break;
    case '\\': buffer_append_byte(buf, '\\'); buffer_append_byte(buf, '\\'); break;
    case '\n': buffer_append_byte(buf, '\\'); buffer_append_byte(buf, 'n');  break;
    case '\t': buffer_append_byte(buf, '\\'); buffer_append_byte(buf, 't');  break;
    case '\r': buffer_append_byte(buf, '\\'); buffer_append_byte(buf, 'r');  break;
      // case '\b': buffer_append_byte(buf, '\\'); buffer_append_byte(buf, 'b');  break;
      // case '\f': buffer_append_byte(buf, '\\'); buffer_append_byte(buf, 'f');  break;
    default:
      if (c >= 32 && c < 126) {
	// Append normal character
	buffer_append_byte(buf, c);
      } else {
	buffer_printf(buf, "\\x%02x", c);
      }
      break;
    }
  }

  buffer_append_byte(buf, '"');
  return buffer_to_c_string(buf);
}

char* string_unquote_c_string(char* input) {
  if (input == nullptr) return nullptr;

  int limit = strlen(input) - 1;
  buffer_t* buf = make_buffer(limit + 10);
  for (size_t i = 1; i < limit;) {
    uint8_t c = cast(uint8_t, input[i++]);
    // log_fatal("c = %d\n", c);
    if (c == '\\') {
      uint8_t c2 = cast(uint8_t, input[i++]);
      // log_fatal("c2 = %d\n", c2);
      switch(c2) {
      case '"':  buffer_append_byte(buf, '\"'); break;
      case '\\': ; buffer_append_byte(buf, '\\'); break;
      case 'n': buffer_append_byte(buf, '\n'); break;
      case 't': buffer_append_byte(buf, '\t'); break;
      case 'r': buffer_append_byte(buf, '\r'); break;
      // case 'b': buffer_append_byte(buf, '\\'); buffer_append_byte(buf, 'b');  break;
      // case 'f': buffer_append_byte(buf, '\\'); buffer_append_byte(buf, 'f');  break;
      case 'x': {
	buffer_t* hex = make_buffer(3);
	buffer_append_byte(hex, input[i++]);
	buffer_append_byte(hex, input[i++]);
	value_result_t result = string_parse_uint64_hex(buffer_to_c_string(hex));
	if (!is_ok(result)) {
	  fatal_error(ERROR_ILLEGAL_INPUT);
	}
	buffer_append_byte(buf, result.u64);
	break;
      }
    default:
      fatal_error(ERROR_ILLEGAL_INPUT);
      break;
      }
    } else {
      buffer_append_byte(buf, c);
    }
  }

  return buffer_to_c_string(buf);
}
