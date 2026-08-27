/**
 * @file readline.c
 *
 * A simple input from the terminal with a fallback to simply reading
 * from stdin.
 */

/**
 * Read a line from the terminal with very basic editing (cursor
 * movement, delete) falling back to just reading a line from stdin if
 * we aren't inside a terminal.
 */

buffer_t* readline(char* prompt, boolean_t init_and_restore) {
  fprintf(stdout, "%s", prompt);
  fflush(stdout);

  if (!is_full_tty()) {
    char buffer[1024];
    fgets(buffer, sizeof(buffer), stdin);
    buffer_t* result = make_buffer(strlen(buffer));
    buffer_append_string(result, buffer);
    // Remove the newline that fgets returns
    result->length--;
    return result;
  }

  if (init_and_restore) {
    if (term_init() != 0) {
      log_fatal("Failed to init the terminl for input.");
      fatal_error(ERROR_ILLEGAL_STATE);
    }
  }

  buffer_t* result = make_buffer(80);
  buffer_t* tmp = make_buffer(5);
  uint32_t column = 0;

  while (true) {
    term_input_event_t ev = {0};
    if (term_poll_event(&ev)) {
      if (ev.type == TERM_EVENT_KEY && ev.key.action == TERM_KEY_ACTION_PRESS) {
	if (ev.key.keycode == 0) {
	  buffer_append_code_point(result, ev.key.codepoint);
	  column += 1;
	  buffer_clear(tmp);
	  term_move_cursor_to_start_of_line(tmp);
	  buffer_append_string(tmp, prompt);
	  buffer_append_buffer(tmp, result);
	  buffer_write_all(stdout, tmp);
	  fflush(stdout);
	} else if (ev.key.action == TERM_KEY_ACTION_PRESS) {
	  // TODO(jawilson): handle modifiers
	  // TODO(jawilson): handle cursor left/right and backspace/delete
	  switch (ev.key.keycode) {
	  case TERM_KEY_ENTER:
	    fprintf(stdout, "\r\n");
	    goto finish;
	    break;
	  case TERM_KEY_BACKSPACE:
	    if (result->length > 0) {
	      result->length--;
	    }
	    break;
	  default:
	    break;
	  }
	}
      }
    } else {
      // Sleep 1ms
      usleep(1000);
    }
  }

 finish:
  if (init_and_restore) {
    term_restore();
  }

  return result;
}
