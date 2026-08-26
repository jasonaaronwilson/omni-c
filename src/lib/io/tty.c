boolean_t is_tty_input(void) {
  return isatty(fileno(stdout)) && !string_equal("dumb", getenv("TERM"));
}

boolean_t is_tty_output(void) {
  return isatty(fileno(stdout)) && !string_equal("dumb", getenv("TERM"));
}

boolean_t is_full_tty(void) {
  return is_tty_input() && is_tty_output();
}

boolean_t use_tty_colors_and_faces(void) {
  // We should probably validate the value of INSIDE_EMACS in case
  // someone sets it to something like false (also maybe
  // empty?). Potentially look at TERM?
  //
  // This probably won't work on windows emacs since I believe it uses
  // pipes so isatty wil fail.
  // maybe check not empty... (*val == '\0')
  return isatty(fileno(stdout)) && getenv("INSIDE_EMACS") != nullptr;
}
