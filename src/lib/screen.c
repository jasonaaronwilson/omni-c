/**
 * @file screen.c
 *
 * An abstraction over a terminal screen.
 */

typedef screen_t = struct {
  uint32_t width;
  uint32_t height;
  style_t current_style;
  style_t* styles;
  uint32_t* chars;
};

screen_t* get_initial_screen(void) {
  screen_t* screen = malloc_struct(screen_t);
  screen->width = term_width();
  screen->height = term_height();
  screen->current_style = set_foreground_green(0, 0xff);
  uint32_t num = screen->width * screen->height;
  screen->styles = cast(style_t*, malloc_bytes(sizeof(style_t) * num));
  screen->chars = cast(uint32_t*, malloc_bytes(sizeof(uint32_t) * num));
  return screen;
}

void screen_fill(screen_t* screen, uint32_t ch, style_t style) {
  uint32_t limit = screen->width * screen->height;
  for (int i = 0; i < limit; i++) {
    screen->chars[i] = ch;
    screen->styles[i] = style;
  }
}

void screen_set_char(screen_t* screen, uint32_t ch, style_t style, 
		     uint32_t row, uint32_t column) {
  int offset = (row * screen->width) + column;
  screen->chars[offset] = ch;
  screen->styles[offset] = style;
}

void screen_to_ansi_buffer(screen_t* screen, buffer_t* buffer) {
  term_move_cursor_absolute(buffer, 0, 0);
  // style_t previous_style = screen->styles[0];
  // style_to_buffer(buffer, previous_style);
  for (int r = 0; r < screen->height - 1; r++) {
    for (int c = 0; c < screen->width - 1; c++) {
      uint32_t index = (r * screen->width) + c;
      style_t style = screen->styles[index];
      term_move_cursor_absolute(buffer, c, r);
      style_to_buffer(buffer, style);
      /*
	if (previous_style != style) {
	style_to_buffer(buffer, style);
	previous_style = style;
	}
      */
      uint32_t cp = screen->chars[index];
      buffer_append_code_point(buffer, cp ? cp : '-');
    }
    buffer_printf(buffer, "\n");
  }
}

void style_to_buffer(buffer_t* buffer, style_t style) {
  term_set_foreground_color(buffer, get_foreground(style));
  term_set_background_color(buffer, get_background(style));
  if (get_bold(style)) {
    term_bold(buffer);
  }
  if (get_dim(style)) {
    term_dim(buffer);
  }
  if (get_italic(style)) {
    term_italic(buffer);
  }
  if (get_underline(style)) {
    term_underline(buffer);
  }
  // TODO(jawilson): overline, strikethrough, superscript, subscript,
  // slow_blink, fast_blink
}

screen_t* _test_screen = nullptr;
random_state_t* _random = nullptr;

void draw_random_screen(boolean_t output) {
  if (_test_screen == nullptr) {
    _test_screen = get_initial_screen();
    _random = random_state();
  }

  int linenum = random_next(_random) & 0xf;
  int start_column = random_next(_random) & 0xf;

  style_t style = 0;
  style = set_foreground(style, 0xffffff & random_next(_random));
  style = set_background(style, 0xffffff & random_next(_random));

  // style = set_foreground(style, 0xffffff);
  // style = set_background(style, 0xff0000);

  for (int times = 0; times < 100; times++) {
    for (int i = 0; i < 10; i++) {
      screen_set_char(_test_screen, 'A' + i, style, linenum, i + start_column);
    }
  }

  buffer_t* buffer = make_buffer(1000);
  screen_to_ansi_buffer(_test_screen, buffer);

  if (output) {
    buffer_write_all(stdout, buffer);
  } else {
    // buffer = make_buffer(1000);
    // style_to_buffer(buffer, style);
    // fprintf(stdout, "%s", buffer_to_c_string(buffer));
    fprintf(stdout, "width = %d, height = %d, size = %d\n", _test_screen->width, _test_screen->height,
	    buffer->length & 0xffffffff);
    fprintf(stdout, "%s\n", quote_c_string(buffer_to_c_string(buffer)));
  }
}
