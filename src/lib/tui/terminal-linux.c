typedef term_echo_restore_t = struct {
  struct termios state;
};

uint32_t term_width(void) {
  struct winsize w;
  ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
  return w.ws_col;
}

uint32_t term_height(void) {
  struct winsize w;
  ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
  return w.ws_row;
}

/**
 * @function term_echo_off
 *
 * Turn off canonical input mode and echo and return the original
 * terminal settings so they can be restored.
 */
term_echo_restore_t term_echo_off() {
  struct termios oldt;
  struct termios newt;
  tcgetattr(STDIN_FILENO, &oldt);
  newt = oldt;

  // Get the original terminal settings
  tcgetattr(STDIN_FILENO, &oldt);
  newt = oldt;

  // Disable canonical input mode and echo
  newt.c_lflag &= ~(ICANON | ECHO);
  // TODO(jawilson): this is from a gemini code review...
  // newt.c_cc[VMIN] = 1;  // Read blocks until at least 1 byte is available
  // newt.c_cc[VTIME] = 0; // No timeout
  tcsetattr(STDIN_FILENO, TCSANOW, &newt);

  return compound_literal(term_echo_restore_t, {.state = oldt});
}

/**
 * @function term_echo_restore
 *
 * Append a terminal escape sequence to turn on hardware echoing.
 */
void term_echo_restore(term_echo_restore_t restore) {
  // Restore the original terminal settings
  tcsetattr(STDIN_FILENO, TCSANOW, &(restore.state));
}

/**
 * Unix consoles either support it or they don't. I suppose we could
 * check if the terminal actually supports utf-8. Gemini suggested
 * some locale stuff and a call to nl_langinfo OR to probe the
 * terminal by printing a multi-byte code-point and seeing by how much
 * the cursor position changes...
 */
void enable_utf8_console(void) {
}
