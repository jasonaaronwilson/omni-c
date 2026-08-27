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
    char raw[1024];
    if (!fgets(raw, sizeof(raw), stdin)) {
      return NULL;
    }
    size_t len = strlen(raw);
    if (len > 0 && (raw[len - 1] == '\n' || raw[len - 1] == '\r')) {
      raw[--len] = '\0';
      if (len > 0 && raw[len - 1] == '\r') {
        raw[--len] = '\0';
      }
    }
    buffer_t* res = make_buffer(len + 1);
    buffer_append_string(res, raw);
    return res;
  }

  if (init_and_restore && term_init() != 0) {
    log_fatal("Failed to init the terminal for input.");
    fatal_error(ERROR_ILLEGAL_STATE);
  }

  codepoint_array_t* line = cp_array_make();
  buffer_t* tmp = make_buffer(256);
  size_t column = 0;

  while (true) {
    term_input_event_t ev = {0};
    if (term_poll_event(&ev)) {
      if (ev.type == TERM_EVENT_KEY && ev.key.action == TERM_KEY_ACTION_PRESS) {
        if (ev.key.keycode == 0) {
          // Printable codepoint insertion
          cp_array_insert(line, column, ev.key.codepoint);
          column++;
          readline_repaint(prompt, line, tmp, column);
        } else {
          switch (ev.key.keycode) {
            case TERM_KEY_ENTER:
              fprintf(stdout, "\r\n");
              goto finish;

            case TERM_KEY_BACKSPACE:
              if (column > 0) {
                column--;
                cp_array_delete(line, column);
                readline_repaint(prompt, line, tmp, column);
              }
              break;

            case TERM_KEY_DELETE:
              if (column < line->length) {
                cp_array_delete(line, column);
                readline_repaint(prompt, line, tmp, column);
              }
              break;

            case TERM_KEY_LEFT:
              if (column > 0) {
                column--;
                readline_repaint(prompt, line, tmp, column);
              }
              break;

            case TERM_KEY_RIGHT:
              if (column < line->length) {
                column++;
                readline_repaint(prompt, line, tmp, column);
              }
              break;

            case TERM_KEY_HOME:
              if (column > 0) {
                column = 0;
                readline_repaint(prompt, line, tmp, column);
              }
              break;

            case TERM_KEY_END:
              if (column != line->length) {
                column = line->length;
                readline_repaint(prompt, line, tmp, column);
              }
              break;

            default:
              break;
          }
        }
      }
    } else {
      usleep(1000);
    }
  }

 finish:
  if (init_and_restore) {
    term_restore();
  }

  buffer_t* result = cp_array_to_buffer(line);

  return result;
}

void readline_repaint(const char* prompt, const codepoint_array_t* line, buffer_t* tmp, size_t column) {
  buffer_clear(tmp);
  term_move_cursor_to_start_of_line(tmp);
  term_clear_entire_line(tmp);
  buffer_append_string(tmp, cast(char*, prompt));

  // Render line as UTF-8 into terminal output buffer
  for (size_t i = 0; i < line->length; i++) {
    buffer_append_code_point(tmp, line->chars[i]);
  }

  // Adjust cursor position (assuming 1 display col per codepoint / ASCII prompt)
  term_move_cursor_to_column(tmp, column + 1 + strlen(prompt));
  buffer_write_all(stdout, tmp);
  fflush(stdout);
}
