uint32_t term_width(void) {
  CONSOLE_SCREEN_BUFFER_INFO csbi = {0};
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    if (hOut != INVALID_HANDLE_VALUE && GetConsoleScreenBufferInfo(hOut, &csbi)) {
        return (uint32_t)(csbi.srWindow.Right - csbi.srWindow.Left + 1);
    }
    return 80; /* Fallback default width */
}

uint32_t term_height(void) {
    CONSOLE_SCREEN_BUFFER_INFO csbi = {0};
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    if (hOut != INVALID_HANDLE_VALUE && GetConsoleScreenBufferInfo(hOut, &csbi)) {
        return (uint32_t)(csbi.srWindow.Bottom - csbi.srWindow.Top + 1);
    }
    return 24; /* Fallback default height */
}

/**
 * Turn off canonical input mode and echo and return the original
 * terminal settings so they can be restored.
 */
DWORD term_echo_off(void) {
    HANDLE hIn = GetStdHandle(STD_INPUT_HANDLE);
    DWORD old_mode = 0;

    if (hIn != INVALID_HANDLE_VALUE && GetConsoleMode(hIn, &old_mode)) {
        /* Clear ENABLE_LINE_INPUT (canonical mode) and ENABLE_ECHO_INPUT (echo) */
        DWORD new_mode = old_mode & ~(ENABLE_LINE_INPUT | ENABLE_ECHO_INPUT);
        SetConsoleMode(hIn, new_mode);
    }

    return old_mode;
}

/**
 * Restore original terminal input settings.
 */
void term_echo_restore(DWORD old_mode) {
    HANDLE hIn = GetStdHandle(STD_INPUT_HANDLE);
    if (hIn != INVALID_HANDLE_VALUE) {
        SetConsoleMode(hIn, old_mode);
    }
}
