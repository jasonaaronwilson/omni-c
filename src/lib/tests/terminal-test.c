//
// Manual Test for our simple terminal support.
//

#include <stdlib.h>

#define C_ARMYKNIFE_LIB_IMPL
#include "../c-armyknife-lib.h"

int main(int argc, char** argv) {
  open_arena_for_test();

  buffer_t* buffer = make_buffer(100);

  buffer = term_clear_screen(buffer);

  for (int j = 0; j < 5; j++) {
    buffer = term_move_cursor_absolute(buffer, 5 + j, 10 + j);
    buffer = buffer_printf(buffer, "Hello ");
    buffer = term_set_foreground_color(buffer, 0xff0000);
    buffer = term_set_background_color(buffer, 0xffffff);
    buffer = buffer_printf(buffer, "World!\n");
  }

  buffer = term_move_cursor_relative(buffer, -2, -2);
  buffer = buffer_printf(buffer, "*");
  buffer = term_move_cursor_relative(buffer, 3, 3);
  buffer = buffer_printf(buffer, "*");

  buffer = term_bold(buffer);
  buffer = buffer_printf(buffer, "[Bold]");
  buffer = term_reset_formatting(buffer);

  buffer = term_underline(buffer);
  buffer = buffer_printf(buffer, "[Underline]");
  buffer = term_reset_formatting(buffer);

  buffer = term_italic(buffer);
  buffer = buffer_printf(buffer, "[Italic]");
  buffer = term_reset_formatting(buffer);

  box_drawing_t box = {
      .upper_left_corner = 0x250c,
      .upper_right_corner = 0x2510,
      .top_edge = 0x2500,
      .bottom_edge = 0x2500,
      .lower_left_corner = 0x2514,
      .lower_right_corner = 0x2518,
      .left_edge = 0x2502,
      .right_edge = 0x2502,
  };

  for (int i = 0; i < 20; i++) {
    buffer = term_draw_box(buffer, 5 + i, 5 + i, 40 + i, 20 + i, &box);
  }

  printf("%s", buffer_to_c_string(buffer));

  close_arena_for_test();

  // TODO(jawilson): ability to query the initial foreground and
  // background color so we can reset the terminal back to it's
  // initial state?
  exit(0);
}
