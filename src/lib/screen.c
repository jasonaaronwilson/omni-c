/**
 * @file screen.c
 *
 * An abstraction over a terminal screen.
 */

/**
 * @struct screen_t
 *
 * This is an abstraction for the physical terminal screen. Rendering
 * to this screen is not immediate as changes should be written back
 * to front in case of things like pop-up dialog boxes. Typically you
 * render to a cheap abstract screen_window_t which represents only a
 * portion of the entire screen.
 */
typedef screen_t = struct {
  uint32_t width;
  uint32_t height;
  // style_t current_style;
  style_t* styles;
  uint32_t* chars;
};

typedef screen_window_t = struct {
  screen_t* screen;
  uint32_t x_offset;
  uint32_t y_offset;
  uint32_t width;
  uint32_t height;
  style_t current_style;
};

screen_t* get_initial_screen(void) {
  screen_t* screen = malloc_struct(screen_t);
  screen->width = term_width();
  screen->height = term_height();
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

screen_window_t* make_root_screen_window(screen_t* screen) {
  screen_window_t* window = malloc_struct(screen_window_t);
  window->screen = screen;
  window->width = screen->width;
  window->height = screen->height;
  // Simple pure green on black, no text effects. The true default of
  // black on black isn't user friendly when first coding something
  // up.
  window->current_style = set_foreground_green(0, 0xff);
  return window;
}

screen_window_t* screen_window_clone(screen_window_t* window) {
  if (window == nullptr) {
    return nullptr;
  }
  screen_window_t* result = malloc_struct(screen_window_t);
  memcpy(result, window, sizeof(*window));
  return result;
}

void hidden_screen_set_char(screen_t* screen, uint32_t ch, style_t style,
			    uint32_t row, uint32_t column) {
  // TODO(jawilson): silent clipping?
  int offset = (row * screen->width) + column;
  screen->chars[offset] = ch;
  screen->styles[offset] = style;
}

/**
 * @function screen_window_set_char
 *
 * The lowest level of interaction. Note that clipping normally means
 * you don't have to worry about much.
 */
void screen_window_set_char(screen_window_t* window, uint32_t ch, style_t style,
			    uint32_t row, uint32_t column) {
  if (row < 0 || row >= window->height) {
    return;
  }
  if (column < 0 || column >= window->width) {
    return;
  }

  hidden_screen_set_char(window->screen, ch, style, row + window->y_offset, column + window->x_offset);
}

void screen_window_split_vertically(screen_window_t* window, double fraction,
				    screen_window_t** top_out, screen_window_t** bottom_out) {
  screen_window_t* top = screen_window_clone(window);
  screen_window_t* bottom = screen_window_clone(window);
  uint32_t new_top_height = window->height * fraction;
  top->height = new_top_height;
  bottom->height = window->height - top->height;
  bottom->y_offset += new_top_height;
  *top_out = top;
  *bottom_out = bottom;
}

void screen_window_split_horizontally(screen_window_t* window, double fraction,
				      screen_window_t** left_out, screen_window_t** right_out) {
  screen_window_t* left = screen_window_clone(window);
  screen_window_t* right = screen_window_clone(window);
  uint32_t new_left_width = window->width * fraction;
  left->width = new_left_width;
  right->width = window->width - new_left_width;
  right->x_offset += new_left_width;
  *left_out = left;
  *right_out = right;
}

box_drawing_t* get_default_window_border_box(void) {
  box_drawing_t* box = malloc_struct(box_drawing_t);
  box->upper_left_corner = 0x250c;
  box->upper_right_corner = 0x2510;
  box->top_edge = 0x2500;
  box->bottom_edge = 0x2500;
  box->lower_left_corner = 0x2514;
  box->lower_right_corner = 0x2518;
  box->left_edge = 0x2502;
  box->right_edge = 0x2502;
  return box;
}

screen_window_t* screen_window_draw_border(screen_window_t* window, box_drawing_t* box) {
  // First the top
  for (int i = 0; i < window->width; i++) {
    screen_window_set_char(window, box->top_edge, window->current_style, 0, i);
  }
  // Now the bottom
  for (int i = 0; i < window->width; i++) {
    screen_window_set_char(window, box->top_edge, window->current_style, window->height-1, i);
  }
  // Now the left
  for (int i = 0; i < window->height; i++) {
    screen_window_set_char(window, box->left_edge, window->current_style, i, 0);
  }
  // Now the right
  for (int i = 0; i < window->height; i++) {
    screen_window_set_char(window, box->left_edge, window->current_style, i, window->width-1);
  }
  // Now the corners
  screen_window_set_char(window, box->upper_left_corner, window->current_style, 0, 0);
  screen_window_set_char(window, box->upper_right_corner, window->current_style, 0, window->width-1);
  screen_window_set_char(window, box->lower_left_corner, window->current_style, window->height-1, 0);
  screen_window_set_char(window, box->lower_right_corner, window->current_style, window->height-1, window->width-1);


  // TODO(jawilson): the sides and then the corners

  screen_window_t* result = screen_window_clone(window);
  result->x_offset += 1;
  result->y_offset += 1;
  result->width -= 2;
  result->height -= 2;

  return result;
}

void screenline_to_ansi_buffer(screen_t* screen, buffer_t* buffer, int row) {
  style_t prev_style = -1;

  // term_reset_formatting(buffer);
  term_move_cursor_absolute(buffer, 0, row);
  // style_t previous_style = screen->styles[0];
  // style_to_buffer(buffer, previous_style);
  // screen->height, screen->width
  for (int c = 0; c < screen->width; c++) {
    uint32_t index = (row * screen->width) + c;
    style_t style = screen->styles[index];
    if (style != prev_style || c == 0) {
      style_to_buffer(buffer, style);
      prev_style = style;
    }
    uint32_t cp = screen->chars[index];
    buffer_append_code_point(buffer, cp ? cp : '-');
  }
  // buffer_printf(buffer, "\n");
}

void write_screen(screen_t* screen) {
  buffer_t* buffer = make_buffer(1000);
  // term_disable_autowrap(buffer);
  // -1 is a kludge for sure...
  for (int row = 0; row < screen->height; row++) {
    buffer_clear(buffer);
    screenline_to_ansi_buffer(screen, buffer, row);
    buffer_write_all_chunked(stdout, buffer);
    fflush(stdout);
    usleep(5);
  }
  // buffer_clear(buffer);
  // term_enable_autowrap(buffer);
  // buffer_write_all_chunked(stdout, buffer);
  // fflush(stdout);
  // usleep(5);
}

void style_to_buffer(buffer_t* buffer, style_t style) {
  term_reset_formatting(buffer);
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
  if (get_strikethrough(style)) {
    term_strikethrough(buffer);
  }
  if (get_overline(style)) {
    term_overline(buffer);
  }
  if (get_superscript(style)) {
    term_superscript(buffer);
  }
  if (get_subscript(style)) {
    term_subscript(buffer);
  }
  if (get_slow_blink(style)) {
    term_slow_blink(buffer);
  }
  if (get_fast_blink(style)) {
    term_fast_blink(buffer);
  }

  // slow_blink, fast_blink
}

/* ================================================================================ */
// Gross testing code below

//

screen_t* _test_screen = nullptr;
random_state_t* _random = nullptr;

void draw_random_chars_in_window(screen_window_t* window) {
  for (int i = 0; i < 26; i++) {
    // Sometime write outside of the window
    uint32_t x = random_next_uint64_below(_random, window->width * 2);
    uint32_t y = random_next_uint64_below(_random, window->height * 2);
    screen_window_set_char(window, 'A' + i, window->current_style, y, x);
  }
}

void draw_random_screen(boolean_t output) {

  if (_test_screen == nullptr) {
    _test_screen = get_initial_screen();
    _random = random_state();
  }

  screen_window_t* root = make_root_screen_window(_test_screen);

  screen_window_t* top = nullptr;
  screen_window_t* bottom = nullptr;
  screen_window_split_vertically(root, 0.8, &top, &bottom);

  screen_window_t* top_left = nullptr;
  screen_window_t* top_right = nullptr;
  screen_window_split_horizontally(top, 0.5, &top_left, &top_right);

  box_drawing_t* box = get_default_window_border_box();
  top_left = screen_window_draw_border(top_left, box);
  top_right = screen_window_draw_border(top_right, box);
  bottom = screen_window_draw_border(bottom, box);

  top_left->current_style = set_underline(set_foreground_red(0, 0xff), true);
  draw_random_chars_in_window(top_left);

  top_right->current_style = set_fast_blink(set_foreground_blue(0, 0xff), true);
  draw_random_chars_in_window(top_right);

  bottom->current_style = set_foreground_green(0, 0xff);
  draw_random_chars_in_window(bottom);

  write_screen(_test_screen);
  sleep(2);

  buffer_t* buffer = make_buffer(100);
  style_to_buffer(buffer, set_foreground(0LL, 0xff00ULL));
  term_move_cursor_absolute(buffer, 0, _test_screen->height - 5);
  buffer_write_all_chunked(stdout, buffer);
  // fprintf(stdout, "X <-- The cursor ended up here!\n");

  /*

  screen_fill(_test_screen, 0, set_foreground(0LL, 0xff00ULL));

  for (int times = 0; times < 30; times++) {
    style_t style = 0;
    int linenum = random_next(_random) & 0xf;
    linenum += random_next(_random) & 0xf;
    int start_column = random_next(_random) & 0xf;
    start_column += random_next(_random) & 0xf;
    start_column += random_next(_random) & 0xf;
    start_column += random_next(_random) & 0xf;
    start_column += random_next(_random) & 0xf;
    style = set_foreground(style, 0xffffff & random_next(_random));
    style = set_background(style, 0xffffff & random_next(_random));
    for (int i = 0; i < 10; i++) {
      hidden_screen_set_char(_test_screen, 'A' + i, style, linenum, i + start_column);
    }
  }

  buffer_t* buffer = make_buffer(1000);
  // screen_to_ansi_buffer(_test_screen, buffer);

  if (output) {
    write_screen(_test_screen);
    sleep(2);
    buffer_t* buffer = make_buffer(1000);
    style_to_buffer(buffer, set_foreground(0LL, 0xff00ULL));
    term_move_cursor_absolute(buffer, 0, _test_screen->height - 5);
    buffer_write_all_chunked(stdout, buffer);
    fprintf(stdout, "\nwidth = %d, height = %d, size = %d\n", _test_screen->width, _test_screen->height,
	    buffer->length & 0xffffffff);
  } else {
    // buffer = make_buffer(1000);
    // style_to_buffer(buffer, style);
    // fprintf(stdout, "%s", buffer_to_c_string(buffer));
    fprintf(stdout, "width = %d, height = %d, size = %d\n", _test_screen->width, _test_screen->height,
	    buffer->length & 0xffffffff);
    fprintf(stdout, "%s\n", quote_c_string(buffer_to_c_string(buffer)));
  }

  */
}
