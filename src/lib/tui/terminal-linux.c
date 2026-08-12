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
  // TODO(jawilson): this is from a gemini code review...
  // newt.c_cc[VMIN] = 1;  // Read blocks until at least 1 byte is available
  // newt.c_cc[VTIME] = 0; // No timeout
  tcsetattr(STDIN_FILENO, TCSANOW, &newt);

  return oldt;
}

/**
 * @function term_echo_off
 *
 * Append a terminal escape sequence to turn on hardware echoing.
 */
void term_echo_restore(struct termios oldt) {
  // Restore the original terminal settings
  tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
}
