# @file terminal.c

A simplistic terminal library that doesn't depend on ncurses,
etc. Realistically most terminals are now decendants of xterm
(itelf like vt100 or ANSI deep down) and tend to converge on how
new features work so we are going to just initialy code for Konsole
and go from there.

Based on this document, I feel we can assume we have 24bit color
support and handle other terminals differently in the future
(perhaps by just not emitting colors):

https://github.com/termstandard/colors
 
## @struct box_drawing_t

A structure to hold unicode code-points for each part of a box to
draw in the terminal.
 
## @struct term_keypress_t

Represents a keypress from the terminal. Either one of code_point
or key_code will be non-zero if n_bytes_consumed is greater than
zero.
 
## @function term_bold

Append a terminal escape sequence to a buffer that turns on "bold"
text.
 
## @function term_clear_screen

Append a terminal escape sequence to a buffer that clears the
entire terminal.
 
## @function term_dim

Append a terminal escape sequence to a buffer that turns on "dim"
text.
 
## @function term_dim

Append a terminal escape sequence to a buffer that turns on "italic"
text.
 
## @function term_draw_box

Append the terminal escape sequences to a buffer that draws a box.
 
## @function term_echo_off

Append a terminal escape sequence to turn on hardware echoing.
 
## @function term_echo_off

Turn off canonical input mode and echo and return the original
terminal settings so they can be restored.
 
## @function term_move_cursor_absolute

Append a terminal escape sequence to a buffer that instructs the
terminal to move it's "cursor" to a zero based x, y absolute
coordinate.

(Traditionally terminals operate on a 1 based coordinate system but
zero based indexes would perhaps be choosen now since most
programming languages are zero based).
 
## @function term_move_cursor_relative

Append a terminal escape sequence to a buffer that instructs the
terminal to move it's "cursor" relative to it's current poition. 0
doesn't change the column or row, while negative will move either
left or to the "top" of the terminal, while positive numbers move
right or downwards towards the bottom of the terminal.
 
## @function term_reset_formatting

Append a terminal escape sequence to a buffer that resets the
formatting (and appears to cancel the foreground and background
color as well).
 
## @function term_set_background_color

Append a terminal escape sequence to a buffer that instructs the
terminal to change to background color to a 24bit RGB value.

(Currently we don't automatically "downgrade" to 256 color or 16
color terminals).
 
## @function term_set_foreground_color

Append a terminal escape sequence to a buffer that instructs the
terminal to change to foreground color to a 24bit RGB value.

(Currently we don't automatically "downgrade" to 256 color or 16
color terminals).
 
## @function term_underline

Append a terminal escape sequence to a buffer that turns on
"underline" text.
 
