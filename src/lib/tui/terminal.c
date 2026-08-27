/**
 * @file terminal.c
 *
 * A low-level terminal output library that doesn't depend on ncurses,
 * etc. Realistically most terminals are now decendants of xterm
 * (itelf like vt100 or ANSI deep down) and tend to converge on how
 * new features work so we are going to just initialy code for Konsole
 * and go from there.
 *
 * Based on this document, I feel we can assume we have 24bit color
 * support and handle other terminals differently in the future
 * (perhaps by just not emitting colors):
 *
 * https://github.com/termstandard/colors
 *
 * So far it looks like this output works well on many Linux terminals
 * as well as Console and Windows Terminal (and WezTerm and
 * Alacritty...).
 */

/**
 * @struct box_drawing_t
 *
 * A structure to hold unicode code-points for each part of a box to
 * draw in the terminal.
 */
typedef box_drawing_t = struct {
  uint32_t upper_left_corner;
  uint32_t upper_right_corner;
  uint32_t lower_left_corner;
  uint32_t lower_right_corner;
  uint32_t top_edge;
  uint32_t left_edge;
  uint32_t right_edge;
  uint32_t bottom_edge;
};

#define TERM_ESCAPE_START_STR "\033["
#define TERM_ESCAPE_END_STR "m"

#define TERM_ESCAPE_STRING_START_AND_END(str)                                  \
  (TERM_ESCAPE_START_STR str TERM_ESCAPE_END_STR)
#define TERM_ESCAPE_STRING(str) (TERM_ESCAPE_START_STR str)

/**
 * @function term_set_foreground_color
 *
 * Append a terminal escape sequence to a buffer that instructs the
 * terminal to change to foreground color to a 24bit RGB value.
 *
 * (Currently we don't automatically "downgrade" to 256 color or 16
 * color terminals).
 */
void term_set_foreground_color(buffer_t* buffer, uint32_t color) {
  uint8_t blue = color & 0xff;
  uint8_t green = (color >> 8) & 0xff;
  uint8_t red = (color >> 16) & 0xff;

  // Escape sequence for setting foreground color (ESC [ 38; 2; r; g; b m)
  buffer_printf(buffer, TERM_ESCAPE_STRING_START_AND_END("38;2;%d;%d;%d"), red,
                green, blue);
}

/**
 * @function term_set_background_color
 *
 * Append a terminal escape sequence to a buffer that instructs the
 * terminal to change to background color to a 24bit RGB value.
 *
 * (Currently we don't automatically "downgrade" to 256 color or 16
 * color terminals).
 */
void term_set_background_color(buffer_t* buffer, uint32_t color) {
  uint8_t blue = color & 0xff;
  uint8_t green = (color >> 8) & 0xff;
  uint8_t red = (color >> 16) & 0xff;

  // Escape sequence for setting background color (ESC [ 48; 2; r; g; b m)
  buffer_printf(buffer, TERM_ESCAPE_STRING_START_AND_END("48;2;%d;%d;%d"), red,
                green, blue);
}

/**
 * @function term_move_cursor_absolute
 *
 * Append a terminal escape sequence to a buffer that instructs the
 * terminal to move it's "cursor" to a zero based x, y absolute
 * coordinate.
 *
 * (Traditionally terminals operate on a 1 based coordinate system but
 * zero based indexes would perhaps be choosen now since most
 * programming languages are zero based).
 */
void term_move_cursor_absolute(buffer_t* buffer, int x, int y) {
  // Escape sequence for cursor movement (ESC [ y; x H)
  buffer_printf(buffer, TERM_ESCAPE_STRING("%d;%dH"), y + 1, x + 1);
}

/**
 * @function term_move_cursor_relative
 *
 * Append a terminal escape sequence to a buffer that instructs the
 * terminal to move it's "cursor" relative to it's current poition. 0
 * doesn't change the column or row, while negative will move either
 * left or to the "top" of the terminal, while positive numbers move
 * right or downwards towards the bottom of the terminal.
 */
void term_move_cursor_relative(buffer_t* buffer, int x, int y) {
  // First handle the x position
  if (x > 0) {
    buffer_printf(buffer, TERM_ESCAPE_STRING("%dC"), x);
  } else if (x < 0) {
    buffer_printf(buffer, TERM_ESCAPE_STRING("%dD"), -x);
  }
  if (y > 0) {
    buffer_printf(buffer, TERM_ESCAPE_STRING("%dB"), y);
  } else if (y < 0) {
    buffer_printf(buffer, TERM_ESCAPE_STRING("%dA"), -y);
  }
}

void term_move_cursor_to_start_of_line(buffer_t* buffer) {
  buffer_append_string(buffer, "\x1b[1G");
}

/**
 * @function term_bold
 *
 * Append a terminal escape sequence to a buffer that turns on "bold"
 * text.
 */
void term_bold(buffer_t* buffer) {
  buffer_printf(buffer, TERM_ESCAPE_STRING("1m"));
}

/**
 * @function term_dim
 *
 * Append a terminal escape sequence to a buffer that turns on "dim"
 * text.
 */
void term_dim(buffer_t* buffer) {
  buffer_printf(buffer, TERM_ESCAPE_STRING("2m"));
}

/**
 * @function term_dim
 *
 * Append a terminal escape sequence to a buffer that turns on "italic"
 * text.
 */
void term_italic(buffer_t* buffer) {
  buffer_printf(buffer, TERM_ESCAPE_STRING("3m"));
}

/**
 * @function term_underline
 *
 * Append a terminal escape sequence to a buffer that turns on
 * "underline" text.
 */
void term_underline(buffer_t* buffer) {
  buffer_printf(buffer, TERM_ESCAPE_STRING("4m"));
}

void term_strikethrough(buffer_t* buffer) {
  buffer_printf(buffer, TERM_ESCAPE_STRING("9m"));
}

void term_overline(buffer_t* buffer) {
  buffer_printf(buffer, TERM_ESCAPE_STRING("53m"));
}

void term_superscript(buffer_t* buffer) {
  buffer_printf(buffer, TERM_ESCAPE_STRING("73m"));
}

void term_subscript(buffer_t* buffer) {
  buffer_printf(buffer, TERM_ESCAPE_STRING("74m"));
}

void term_slow_blink(buffer_t* buffer) {
  buffer_printf(buffer, TERM_ESCAPE_STRING("5m"));
}

void term_fast_blink(buffer_t* buffer) {
  buffer_printf(buffer, TERM_ESCAPE_STRING("6m"));
}

/**
 * @function term_reset_formatting
 *
 * Append a terminal escape sequence to a buffer that resets the
 * formatting (and appears to cancel the foreground and background
 * color as well).
 */
void term_reset_formatting(buffer_t* buffer) {
  buffer_printf(buffer, TERM_ESCAPE_STRING("0m"));
}

/**
 * @function term_clear_screen
 *
 * Append a terminal escape sequence to a buffer that clears the
 * entire terminal.
 */
void term_clear_screen(buffer_t* buffer) {
  buffer_printf(buffer, TERM_ESCAPE_STRING("2J"));
}

/**
 * @function term_draw_box
 *
 * Append the terminal escape sequences to a buffer that draws a box.
 */
void term_draw_box(buffer_t* buffer, uint16_t x0, uint16_t y0, uint16_t x1,
                   uint16_t y1, box_drawing_t* box) {
  // top
  term_move_cursor_absolute(buffer, x0, y0);
  buffer_append_code_point(buffer, box->upper_left_corner);
  for (uint64_t x = x0 + 1; x < x1; x++) {
    buffer_append_code_point(buffer, box->top_edge);
  }
  buffer_append_code_point(buffer, box->upper_right_corner);

  // bottom
  term_move_cursor_absolute(buffer, x0, y1);
  buffer_append_code_point(buffer, box->lower_left_corner);
  for (uint64_t x = x0 + 1; x < x1; x++) {
    buffer_append_code_point(buffer, box->bottom_edge);
  }
  buffer_append_code_point(buffer, box->lower_right_corner);

  // the sides (and the middle)
  for (int y = y0 + 1; y < y1; y++) {
    term_move_cursor_absolute(buffer, x0, y);
    buffer_append_code_point(buffer, box->left_edge);

    for (int x = x0 + 1; x < x1; x++) {
      buffer_append_code_point(buffer, ' ');
    }
    // buffer = term_move_cursor_absolute(buffer, x1, y);

    buffer_append_code_point(buffer, box->right_edge);
  }
}

void term_disable_autowrap(buffer_t* buffer) {
  buffer_append_string(buffer, "\033[?7l");
}

void term_enable_autowrap(buffer_t* buffer) {
  buffer_append_string(buffer, "\033[?7h");
}

// Set window title.
// ESC ]0;this is the window title BEL

// ESC ]8;;link ST (hyperlink)

/**
 * @function term_alt_buffer
 *
 * Switch to the alternative buffer. This is a good place to hide a
 * debugger UI...
 */
void term_alt_buffer(buffer_t* buffer) { buffer_printf(buffer, "\033[?1049h"); }

/**
 * @function term_main_buffer
 *
 * Switch back to the main buffer. This won't necessarily return the
 * cursor to it's last position (or does it?).
 */
void term_main_buffer(buffer_t* buffer) {
  buffer_printf(buffer, "\033[?1049l");
}

void term_home(buffer_t* buffer) { buffer_printf(buffer, "\033[H"); }
