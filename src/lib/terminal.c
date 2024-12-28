#line 2 "terminal.c"
#ifndef _TERMINAL_H_
#define _TERMINAL_H_

#include <termios.h>

/**
 * @file terminal.c
 *
 * A simplistic terminal library that doesn't depend on ncurses,
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
 */

/**
 * @struct box_drawing_t
 *
 * A structure to hold unicode code-points for each part of a box to
 * draw in the terminal.
 */
struct box_drawing_S {
  uint32_t upper_left_corner;
  uint32_t upper_right_corner;
  uint32_t lower_left_corner;
  uint32_t lower_right_corner;
  uint32_t top_edge;
  uint32_t left_edge;
  uint32_t right_edge;
  uint32_t bottom_edge;
};

typedef struct box_drawing_S box_drawing_t;

/**
 * @struct term_keypress_t
 *
 * Represents a keypress from the terminal. Either one of code_point
 * or key_code will be non-zero if n_bytes_consumed is greater than
 * zero.
 */
struct term_keypress_S {
  uint32_t code_point;
  uint8_t key_code;
  uint8_t n_bytes_consumed;
  uint8_t shift : 1;
  uint8_t ctrl : 1;
  uint8_t meta : 1;
  uint8_t super : 1;
  uint8_t hyper : 1;
};

// Names and descriptions are from ncurses but the values are
// different.
#define KEY_DOWN 1      /* down-arrow key */
#define KEY_UP 2        /* up-arrow key */
#define KEY_LEFT 3      /* left-arrow key */
#define KEY_RIGHT 4     /* right-arrow key */
#define KEY_HOME 5      /* home key */
#define KEY_BACKSPACE 6 /* backspace key */
#define KEY_F0 7        /* Function keys */
#define KEY_F1 8
#define KEY_F2 9
#define KEY_F3 10
#define KEY_F4 11
#define KEY_F5 12
#define KEY_F6 13
#define KEY_F7 14
#define KEY_F8 15
#define KEY_F9 16
#define KEY_F10 17
#define KEY_F11 18
#define KEY_F12 19
#define KEY_F13 20
#define KEY_F14 21
#define KEY_F15 22
#define KEY_DL 23        /* delete-line key */
#define KEY_IL 24        /* insert-line key */
#define KEY_DC 25        /* delete-character key */
#define KEY_IC 26        /* insert-character key */
#define KEY_EIC 27       /* sent by rmir or smir in insert mode */
#define KEY_CLEAR 28     /* clear-screen or erase key */
#define KEY_EOS 29       /* clear-to-end-of-screen key */
#define KEY_EOL 30       /* clear-to-end-of-line key */
#define KEY_SF 31        /* scroll-forward key */
#define KEY_SR 32        /* scroll-backward key */
#define KEY_NPAGE 33     /* next-page key */
#define KEY_PPAGE 34     /* previous-page key */
#define KEY_STAB 35      /* set-tab key */
#define KEY_CTAB 36      /* clear-tab key */
#define KEY_CATAB 37     /* clear-all-tabs key */
#define KEY_ENTER 38     /* enter/send key */
#define KEY_PRINT 39     /* print key */
#define KEY_LL 40        /* lower-left key (home down) */
#define KEY_A1 41        /* upper left of keypad */
#define KEY_A3 42        /* upper right of keypad */
#define KEY_B2 43        /* center of keypad */
#define KEY_C1 44        /* lower left of keypad */
#define KEY_C3 45        /* lower right of keypad */
#define KEY_BTAB 46      /* back-tab key */
#define KEY_BEG 47       /* begin key */
#define KEY_CANCEL 48    /* cancel key */
#define KEY_CLOSE 49     /* close key */
#define KEY_COMMAND 50   /* command key */
#define KEY_COPY 51      /* copy key */
#define KEY_CREATE 52    /* create key */
#define KEY_END 53       /* end key */
#define KEY_EXIT 54      /* exit key */
#define KEY_FIND 55      /* find key */
#define KEY_HELP 56      /* help key */
#define KEY_MARK 57      /* mark key */
#define KEY_MESSAGE 58   /* message key */
#define KEY_MOVE 59      /* move key */
#define KEY_NEXT 60      /* next key */
#define KEY_OPEN 61      /* open key */
#define KEY_OPTIONS 62   /* options key */
#define KEY_PREVIOUS 63  /* previous key */
#define KEY_REDO 64      /* redo key */
#define KEY_REFERENCE 65 /* reference key */
#define KEY_REFRESH 66   /* refresh key */
#define KEY_REPLACE 67   /* replace key */
#define KEY_RESTART 68   /* restart key */
#define KEY_RESUME 69    /* resume key */
#define KEY_SAVE 70      /* save key */
#define KEY_SBEG 71      /* shifted begin key */
#define KEY_SCANCEL 72   /* shifted cancel key */
#define KEY_SCOMMAND 73  /* shifted command key */
#define KEY_SCOPY 74     /* shifted copy key */
#define KEY_SCREATE 75   /* shifted create key */
#define KEY_SDC 76       /* shifted delete-character key */
#define KEY_SDL 77       /* shifted delete-line key */
#define KEY_SELECT 78    /* select key */
#define KEY_SEND 79      /* shifted end key */
#define KEY_SEOL 80      /* shifted clear-to-end-of-line key */
#define KEY_SEXIT 81     /* shifted exit key */
#define KEY_SFIND 82     /* shifted find key */
#define KEY_SHELP 83     /* shifted help key */
#define KEY_SHOME 84     /* shifted home key */
#define KEY_SIC 85       /* shifted insert-character key */
#define KEY_SLEFT 86     /* shifted left-arrow key */
#define KEY_SMESSAGE 87  /* shifted message key */
#define KEY_SMOVE 88     /* shifted move key */
#define KEY_SNEXT 89     /* shifted next key */
#define KEY_SOPTIONS 90  /* shifted options key */
#define KEY_SPREVIOUS 91 /* shifted previous key */
#define KEY_SPRINT 92    /* shifted print key */
#define KEY_SREDO 93     /* shifted redo key */
#define KEY_SREPLACE 94  /* shifted replace key */
#define KEY_SRIGHT 95    /* shifted right-arrow key */
#define KEY_SRSUME 96    /* shifted resume key */
#define KEY_SSAVE 97     /* shifted save key */
#define KEY_SSUSPEND 98  /* shifted suspend key */
#define KEY_SUNDO 99     /* shifted undo key */
#define KEY_SUSPEND 100  /* suspend key */
#define KEY_UNDO 101     /* undo key */
#define KEY_MOUSE 102    /* Mouse event has occurred */
#define KEY_RESIZE 103   /* Terminal resize event */

typedef struct term_keypress_S term_keypress_t;

__attribute__((warn_unused_result)) extern buffer_t*
    term_clear_screen(buffer_t* buffer);

__attribute__((warn_unused_result)) extern buffer_t*
    term_set_foreground_color(buffer_t* buffer, uint32_t color);

__attribute__((warn_unused_result)) extern buffer_t*
    term_set_background_color(buffer_t* buffer, uint32_t color);

__attribute__((warn_unused_result)) extern buffer_t*
    term_move_cursor_absolute(buffer_t* buffer, int x, int y);

__attribute__((warn_unused_result)) extern buffer_t*
    term_move_cursor_relative(buffer_t* buffer, int x, int y);

__attribute__((warn_unused_result)) extern buffer_t*
    term_bold(buffer_t* buffer);

__attribute__((warn_unused_result)) extern buffer_t* term_dim(buffer_t* buffer);

__attribute__((warn_unused_result)) extern buffer_t*
    term_italic(buffer_t* buffer);

__attribute__((warn_unused_result)) extern buffer_t*
    term_underline(buffer_t* buffer);

__attribute__((warn_unused_result)) extern buffer_t*
    term_reset_formatting(buffer_t* buffer);

__attribute__((warn_unused_result)) extern buffer_t*
    term_draw_box(buffer_t* buffer, uint16_t x0, uint16_t y0, uint16_t x1,
                  uint16_t y1, box_drawing_t* box);

extern struct termios term_echo_off();
extern void term_echo_restore(struct termios oldt);

// TODO(jawilson): terminal queries like request cursor position

#endif /* _TERMINAL_H_ */

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
__attribute__((warn_unused_result)) extern buffer_t*
    term_set_foreground_color(buffer_t* buffer, uint32_t color) {
  uint8_t blue = color & 0xff;
  uint8_t green = (color >> 8) & 0xff;
  uint8_t red = (color >> 16) & 0xff;

  // Escape sequence for setting foreground color (ESC [ 38; 2; r; g; b m)
  return buffer_printf(buffer,
                       TERM_ESCAPE_STRING_START_AND_END("38;2;%d;%d;%d"), red,
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
__attribute__((warn_unused_result)) extern buffer_t*
    term_set_background_color(buffer_t* buffer, uint32_t color) {
  uint8_t blue = color & 0xff;
  uint8_t green = (color >> 8) & 0xff;
  uint8_t red = (color >> 16) & 0xff;

  // Escape sequence for setting background color (ESC [ 48; 2; r; g; b m)
  return buffer_printf(buffer,
                       TERM_ESCAPE_STRING_START_AND_END("48;2;%d;%d;%d"), red,
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
__attribute__((warn_unused_result)) extern buffer_t*
    term_move_cursor_absolute(buffer_t* buffer, int x, int y) {
  // Escape sequence for cursor movement (ESC [ y; x H)
  return buffer_printf(buffer, TERM_ESCAPE_STRING("%d;%dH"), y + 1, x + 1);
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
__attribute__((warn_unused_result)) extern buffer_t*
    term_move_cursor_relative(buffer_t* buffer, int x, int y) {
  // First handle the x position
  if (x > 0) {
    buffer = buffer_printf(buffer, TERM_ESCAPE_STRING("%dC"), x);
  } else if (x < 0) {
    buffer = buffer_printf(buffer, TERM_ESCAPE_STRING("%dD"), -x);
  }
  if (y > 0) {
    buffer = buffer_printf(buffer, TERM_ESCAPE_STRING("%dB"), y);
  } else {
    buffer = buffer_printf(buffer, TERM_ESCAPE_STRING("%dA"), -y);
  }
  return buffer;
}

/**
 * @function term_bold
 *
 * Append a terminal escape sequence to a buffer that turns on "bold"
 * text.
 */
__attribute__((warn_unused_result)) extern buffer_t*
    term_bold(buffer_t* buffer) {
  return buffer_printf(buffer, TERM_ESCAPE_STRING("1m"));
}

/**
 * @function term_dim
 *
 * Append a terminal escape sequence to a buffer that turns on "dim"
 * text.
 */
__attribute__((warn_unused_result)) extern buffer_t*
    term_dim(buffer_t* buffer) {
  return buffer_printf(buffer, TERM_ESCAPE_STRING("2m"));
}

/**
 * @function term_dim
 *
 * Append a terminal escape sequence to a buffer that turns on "italic"
 * text.
 */
__attribute__((warn_unused_result)) extern buffer_t*
    term_italic(buffer_t* buffer) {
  return buffer_printf(buffer, TERM_ESCAPE_STRING("3m"));
}

/**
 * @function term_underline
 *
 * Append a terminal escape sequence to a buffer that turns on
 * "underline" text.
 */
__attribute__((warn_unused_result)) extern buffer_t*
    term_underline(buffer_t* buffer) {
  return buffer_printf(buffer, TERM_ESCAPE_STRING("4m"));
}

/**
 * @function term_reset_formatting
 *
 * Append a terminal escape sequence to a buffer that resets the
 * formatting (and appears to cancel the foreground and background
 * color as well).
 */
__attribute__((warn_unused_result)) extern buffer_t*
    term_reset_formatting(buffer_t* buffer) {
  return buffer_printf(buffer, TERM_ESCAPE_STRING("0m"));
}

/**
 * @function term_clear_screen
 *
 * Append a terminal escape sequence to a buffer that clears the
 * entire terminal.
 */
__attribute__((warn_unused_result)) extern buffer_t*
    term_clear_screen(buffer_t* buffer) {
  return buffer_printf(buffer, TERM_ESCAPE_STRING("2J"));
}

/**
 * @function term_draw_box
 *
 * Append the terminal escape sequences to a buffer that draws a box.
 */
__attribute__((warn_unused_result)) extern buffer_t*
    term_draw_box(buffer_t* buffer, uint16_t x0, uint16_t y0, uint16_t x1,
                  uint16_t y1, box_drawing_t* box) {
  // top
  buffer = term_move_cursor_absolute(buffer, x0, y0);
  buffer = buffer_append_code_point(buffer, box->upper_left_corner);
  for (uint64_t x = x0 + 1; x < x1; x++) {
    buffer = buffer_append_code_point(buffer, box->top_edge);
  }
  buffer = buffer_append_code_point(buffer, box->upper_right_corner);

  // bottom
  buffer = term_move_cursor_absolute(buffer, x0, y1);
  buffer = buffer_append_code_point(buffer, box->lower_left_corner);
  for (uint64_t x = x0 + 1; x < x1; x++) {
    buffer = buffer_append_code_point(buffer, box->bottom_edge);
  }
  buffer = buffer_append_code_point(buffer, box->lower_right_corner);

  // the sides (and the middle)
  for (int y = y0 + 1; y < y1; y++) {
    buffer = term_move_cursor_absolute(buffer, x0, y);
    buffer = buffer_append_code_point(buffer, box->left_edge);

    for (int x = x0 + 1; x < x1; x++) {
      buffer = buffer_append_code_point(buffer, ' ');
    }
    // buffer = term_move_cursor_absolute(buffer, x1, y);

    buffer = buffer_append_code_point(buffer, box->right_edge);
  }

  return buffer;
}

/**
 * @function term_echo_off
 *
 * Turn off canonical input mode and echo and return the original
 * terminal settings so they can be restored.
 */
extern struct termios term_echo_off() {
  struct termios oldt;
  struct termios newt;
  tcgetattr(STDIN_FILENO, &oldt);
  newt = oldt;

  // Get the original terminal settings
  tcgetattr(STDIN_FILENO, &oldt);
  newt = oldt;

  // Disable canonical input mode and echo
  newt.c_lflag &= ~(ICANON | ECHO);
  tcsetattr(STDIN_FILENO, TCSANOW, &newt);

  return oldt;
}

/**
 * @function term_echo_off
 *
 * Append a terminal escape sequence to turn on hardware echoing.
 */
extern void term_echo_restore(struct termios oldt) {
  // Restore the original terminal settings
  tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
}

// Set window title.
// ESC ]0;this is the window title BEL

// ESC ]8;;link ST (hyperlink)
