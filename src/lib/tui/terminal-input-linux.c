
static struct termios orig_termios;
static int raw_mode_active = 0;

int term_init(void) {
    if (tcgetattr(STDIN_FILENO, &orig_termios) == -1) return -1;

    struct termios raw = orig_termios;
    raw.c_iflag &= ~(BRKINT | ICRNL | INPCK | ISTRIP | IXON);
    raw.c_oflag &= ~(OPOST);
    raw.c_cflag |= (CS8);
    raw.c_lflag &= ~(ECHO | ICANON | IEXTEN | ISIG);
    raw.c_cc[VMIN] = 0;  // Non-blocking read
    raw.c_cc[VTIME] = 0;

    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw) == -1) return -1;
    raw_mode_active = 1;

    // Enable Kitty progressive enhancement flags + SGR mouse (1006) +
    // Any-event mouse (1003) Kitty flag 15: Disambiguate escape
    // codes, Report event types, Report alternate keys, Report all
    // keys as escape codes
    const char *init_seq = "\x1b[=15u\x1b[?1003h\x1b[?1006h";
    if (write(STDOUT_FILENO, init_seq, strlen(init_seq)) < 0) {
        // Continue anyway
    }

    return 0;
}

void term_restore(void) {
    if (!raw_mode_active) return;

    // Pop Kitty flags, disable mouse
    const char *cleanup_seq = "\x1b[<u\x1b[?1003l\x1b[?1006l";
    if (write(STDOUT_FILENO, cleanup_seq, strlen(cleanup_seq)) < 0) {
        // Ignored on cleanup
    }

    tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_termios);
    raw_mode_active = 0;
}

static term_modifier_t parse_modifiers(int mods) {
    term_modifier_t m = TERM_MOD_NONE;
    if (mods & 1)  m |= TERM_MOD_SHIFT;
    if (mods & 2)  m |= TERM_MOD_ALT;
    if (mods & 4)  m |= TERM_MOD_CTRL;
    if (mods & 8)  m |= TERM_MOD_SUPER;
    if (mods & 16) m |= TERM_MOD_CAPS_LOCK;
    if (mods & 32) m |= TERM_MOD_NUM_LOCK;
    return m;
}

static int parse_sgr_mouse(const char *buf, size_t len, term_input_event_t *ev) {
  int b = 0;
  int x = 0;
  int y = 0;
  char final_char = 0;
  int n = 0;

    if (sscanf(buf, "\x1b[<%d;%d;%d%c%n", &b, &x, &y, &final_char, &n) < 4) {
        return 0;
    }

    ev->type = TERM_EVENT_MOUSE;
    ev->mouse.x = x - 1;
    ev->mouse.y = y - 1;
    ev->mouse.modifiers = TERM_MOD_NONE;

    if (b & 4)  ev->mouse.modifiers |= TERM_MOD_SHIFT;
    if (b & 8)  ev->mouse.modifiers |= TERM_MOD_ALT;
    if (b & 16) ev->mouse.modifiers |= TERM_MOD_CTRL;

    int btn_code  = b & 3;
    int is_motion = (b & 32) != 0;
    int is_wheel  = (b & 64) != 0;

    if (is_wheel) {
        if (btn_code == 0)      ev->mouse.action = TERM_MOUSE_ACTION_WHEEL_UP;
        else if (btn_code == 1) ev->mouse.action = TERM_MOUSE_ACTION_WHEEL_DOWN;
        else if (btn_code == 2) ev->mouse.action = TERM_MOUSE_ACTION_WHEEL_LEFT;
        else if (btn_code == 3) ev->mouse.action = TERM_MOUSE_ACTION_WHEEL_RIGHT;
        ev->mouse.button = TERM_MOUSE_BTN_NONE;
    } else if (is_motion) {
        ev->mouse.action = (b & 3) ? TERM_MOUSE_ACTION_DRAG : TERM_MOUSE_ACTION_MOVE;
        ev->mouse.button = cast(term_mouse_button_t, (btn_code + 1));
    } else {
        ev->mouse.action = (final_char == 'M') ? TERM_MOUSE_ACTION_PRESS : TERM_MOUSE_ACTION_RELEASE;
        ev->mouse.button = cast(term_mouse_button_t, btn_code + 1);
    }

    return n; // Return bytes consumed
}


static void map_kitty_pua_key(uint32_t codepoint, term_keypress_t *k) {
    k->codepoint = 0;
    k->location = TERM_KEY_LOC_STANDARD;

    switch (codepoint) {
        // Navigation & editing
        case 57352: k->keycode = TERM_KEY_UP; break;
        case 57353: k->keycode = TERM_KEY_DOWN; break;
        case 57354: k->keycode = TERM_KEY_RIGHT; break;
        case 57355: k->keycode = TERM_KEY_LEFT; break;
        case 57356: k->keycode = TERM_KEY_PAGE_UP; break;
        case 57357: k->keycode = TERM_KEY_PAGE_DOWN; break;
        case 57358: k->keycode = TERM_KEY_HOME; break;
        case 57359: k->keycode = TERM_KEY_END; break;
        case 57360: k->keycode = TERM_KEY_INSERT; break;
        case 57361: k->keycode = TERM_KEY_DELETE; break;

        // Function keys
        case 57376: k->keycode = TERM_KEY_F1; break;
        case 57377: k->keycode = TERM_KEY_F2; break;
        case 57378: k->keycode = TERM_KEY_F3; break;
        case 57379: k->keycode = TERM_KEY_F4; break;
        case 57380: k->keycode = TERM_KEY_F5; break;
        case 57381: k->keycode = TERM_KEY_F6; break;
        case 57382: k->keycode = TERM_KEY_F7; break;
        case 57383: k->keycode = TERM_KEY_F8; break;
        case 57384: k->keycode = TERM_KEY_F9; break;
        case 57385: k->keycode = TERM_KEY_F10; break;
        case 57386: k->keycode = TERM_KEY_F11; break;
        case 57387: k->keycode = TERM_KEY_F12; break;

        // Standalone modifier reporting (Kitty flags 1 / 8)
        case 57441: k->keycode = TERM_KEY_SHIFT;  k->location = TERM_KEY_LOC_LEFT; break;
        case 57442: k->keycode = TERM_KEY_CTRL;   k->location = TERM_KEY_LOC_LEFT; break;
        case 57443: k->keycode = TERM_KEY_ALT;    k->location = TERM_KEY_LOC_LEFT; break;
        case 57444: k->keycode = TERM_KEY_SUPER;  k->location = TERM_KEY_LOC_LEFT; break;
        case 57447: k->keycode = TERM_KEY_SHIFT; k->location = TERM_KEY_LOC_RIGHT; break;
        case 57448: k->keycode = TERM_KEY_CTRL;  k->location = TERM_KEY_LOC_RIGHT; break;
        case 57449: k->keycode = TERM_KEY_ALT;   k->location = TERM_KEY_LOC_RIGHT; break;
        case 57450: k->keycode = TERM_KEY_SUPER; k->location = TERM_KEY_LOC_RIGHT; break;

        // System / Locks
        case 57364: k->keycode = TERM_KEY_PRINT_SCREEN; break;
        case 57365: k->keycode = TERM_KEY_PAUSE; break;
        case 57367: k->keycode = TERM_KEY_CAPS_LOCK; break;
        case 57368: k->keycode = TERM_KEY_SCROLL_LOCK; break;
        case 57369: k->keycode = TERM_KEY_NUM_LOCK; break;

        default:
	  printf("Unmapped PUA code-point: %u 0x%X", codepoint, codepoint);
            // Unmapped PUA
            k->keycode = TERM_KEY_NONE;
            break;
    }
}

static void map_tilde_code(int code, term_keypress_t *k) {
    switch (code) {
        case 1:  k->keycode = TERM_KEY_HOME; break;
        case 2:  k->keycode = TERM_KEY_INSERT; break;
        case 3:  k->keycode = TERM_KEY_DELETE; break;
        case 4:  k->keycode = TERM_KEY_END; break;
        case 5:  k->keycode = TERM_KEY_PAGE_UP; break;
        case 6:  k->keycode = TERM_KEY_PAGE_DOWN; break;
        case 7:  k->keycode = TERM_KEY_HOME; break;
        case 8:  k->keycode = TERM_KEY_END; break;
        case 11: k->keycode = TERM_KEY_F1; break;
        case 12: k->keycode = TERM_KEY_F2; break;
        case 13: k->keycode = TERM_KEY_F3; break;
        case 14: k->keycode = TERM_KEY_F4; break;
        case 15: k->keycode = TERM_KEY_F5; break;
        case 17: k->keycode = TERM_KEY_F6; break;
        case 18: k->keycode = TERM_KEY_F7; break;
        case 19: k->keycode = TERM_KEY_F8; break;
        case 20: k->keycode = TERM_KEY_F9; break;
        case 21: k->keycode = TERM_KEY_F10; break;
        case 23: k->keycode = TERM_KEY_F11; break;
        case 24: k->keycode = TERM_KEY_F12; break;
        default: k->keycode = TERM_KEY_NONE; break;
    }
}

// There is probably a minimum size for this buffer (what-ever the
// longest terminal input sequence is
#define TERM_IN_BUF_SIZE 1024

static char in_buf[TERM_IN_BUF_SIZE];
static size_t in_len = 0;

// TODO(jawilson): we already have this somewhere else...

/* Decode UTF-8 codepoint; returns number of bytes consumed */
static int decode_utf8(const char *s, size_t len, uint32_t *out_cp) {
    if (len == 0) return 0;
    unsigned char c0 = cast(unsigned char, s[0]);

    if (c0 < 0x80) {
        *out_cp = c0;
        return 1;
    } else if ((c0 & 0xE0) == 0xC0) {
        if (len < 2) return 0; // Incomplete
        *out_cp = ((c0 & 0x1F) << 6) 
	  | cast(unsigned char, s[1] & 0x3F);
        return 2;
    } else if ((c0 & 0xF0) == 0xE0) {
        if (len < 3) return 0;
        *out_cp = ((c0 & 0x0F) << 12) 
	  | (cast(unsigned char, s[1] & 0x3F) << 6) 
	  | (cast(unsigned char, s[2] & 0x3F));
        return 3;
    } else if ((c0 & 0xF8) == 0xF0) {
      if (len < 4) return 0;
      *out_cp = ((c0 & 0x07) << 18)
	| (cast(unsigned char, s[1] & 0x3F) << 12) 
	| (cast(unsigned char, s[2] & 0x3F) << 6)
	| (cast(unsigned char, s[3] & 0x3F));
        return 4;
    }

    *out_cp = c0;
    return 1; // Invalid byte, consume 1
}

static int parse_csi_key(const char *buf, size_t len, term_input_event_t *ev) {
    if (len < 3 || buf[0] != '\x1b' || buf[1] != '[') return 0;

    // Find terminator: 'u', '~', or letters 'A'-'Z'/'a'-'z'
    size_t term_idx = 2;
    while (term_idx < len) {
        char c = buf[term_idx];
        if ((c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z') || c == '~' || c == '@') {
            break;
        }
        term_idx++;
    }

    if (term_idx >= len) return 0; // Sequence incomplete, wait for more data

    char final_char = buf[term_idx];
    const char *p = buf + 2;
    const char *end = buf + term_idx;

    uint32_t key_code = 0;
    uint32_t shifted_code = 0;
    int mods = 1;
    int action = 1;

    // 1. Primary keycode (e.g. 47 for '/')
    while (p < end && *p >= '0' && *p <= '9') {
        key_code = key_code * 10 + (*p - '0');
        p++;
    }

    // Capture :shifted_code (e.g. 63 for '?')
    if (p < end && *p == ':') {
        p++;
        while (p < end && *p >= '0' && *p <= '9') {
            shifted_code = shifted_code * 10 + (*p - '0');
            p++;
        }
        // Skip any subsequent subfields (e.g. :base_layout)
        while (p < end && *p == ':') {
            p++;
            while (p < end && *p >= '0' && *p <= '9') p++;
        }
    }

    // 2. Modifiers & action (;mods or ;mods:action)
    if (p < end && *p == ';') {
        p++;
        int m = 0;
        while (p < end && *p >= '0' && *p <= '9') {
            m = m * 10 + (*p - '0');
            p++;
        }
        if (m > 0) mods = m;

        if (p < end && *p == ':') {
            p++;
            int a = 0;
            while (p < end && *p >= '0' && *p <= '9') {
                a = a * 10 + (*p - '0');
                p++;
            }
            if (a > 0) action = a;
        }
    }

    ev->type = TERM_EVENT_KEY;
    memset(&ev->key, 0, sizeof(ev->key));

    if (action == 1)      ev->key.action = TERM_KEY_ACTION_PRESS;
    else if (action == 2) ev->key.action = TERM_KEY_ACTION_REPEAT;
    else if (action == 3) ev->key.action = TERM_KEY_ACTION_RELEASE;
    else                  ev->key.action = TERM_KEY_ACTION_PRESS;

    ev->key.modifiers = parse_modifiers(mods - 1);
    ev->key.location = TERM_KEY_LOC_STANDARD;

    if (final_char == 'u') {
        if (key_code >= 57344) {
            map_kitty_pua_key(key_code, &ev->key);
        } else if (key_code == 13) {
            ev->key.keycode = TERM_KEY_ENTER;
        } else if (key_code == 27) {
            ev->key.keycode = TERM_KEY_ESCAPE;
        } else if (key_code == 9) {
            ev->key.keycode = TERM_KEY_TAB;
        } else if (key_code == 127 || key_code == 8) {
            ev->key.keycode = TERM_KEY_BACKSPACE;
        } else {
            // Prefer shifted codepoint when Shift is active and subfield is provided
            if (shifted_code != 0 && (ev->key.modifiers & TERM_MOD_SHIFT)) {
                ev->key.codepoint = shifted_code;
            } else {
                ev->key.codepoint = key_code;
            }
            ev->key.keycode = TERM_KEY_NONE;
        }
    } else if (final_char == '~') {
        map_tilde_code(key_code, &ev->key);
    } else {
        // Letter terminators (e.g. \x1b[A, \x1b[1;5A)
        switch (final_char) {
            case 'A': ev->key.keycode = TERM_KEY_UP; break;
            case 'B': ev->key.keycode = TERM_KEY_DOWN; break;
            case 'C': ev->key.keycode = TERM_KEY_RIGHT; break;
            case 'D': ev->key.keycode = TERM_KEY_LEFT; break;
            case 'H': ev->key.keycode = TERM_KEY_HOME; break;
            case 'F': ev->key.keycode = TERM_KEY_END; break;
            case 'Z': ev->key.keycode = TERM_KEY_TAB; ev->key.modifiers |= TERM_MOD_SHIFT; break;

	      // F1 - F4 CSI letter variants (\x1b[P, \x1b[1;2Q, etc.)
            case 'P': ev->key.keycode = TERM_KEY_F1; break;
            case 'Q': ev->key.keycode = TERM_KEY_F2; break;
            case 'R': ev->key.keycode = TERM_KEY_F3; break;
            case 'S': ev->key.keycode = TERM_KEY_F4; break;

            default:  ev->key.keycode = TERM_KEY_NONE; break;
        }
    }

    return cast(int, (term_idx + 1)); // Exact bytes consumed
}

int term_poll_event(term_input_event_t *ev) {
    // 1. Ingest any pending bytes from stdin into accumulator
    if (in_len < sizeof(in_buf) - 1) {
        ssize_t n = read(STDIN_FILENO, in_buf + in_len, sizeof(in_buf) - 1 - in_len);
        if (n > 0) {
            in_len += n;
            in_buf[in_len] = '\0';
        }
    }

    if (in_len == 0) return 0;

    int consumed = 0;

    // 2. Escape Sequence Handling (SGR Mouse, CSI, SS3, Escape)
    if (in_buf[0] == '\x1b') {
        // SGR Mouse: \x1b[<...
        if (in_len >= 3 && in_buf[1] == '[' && in_buf[2] == '<') {
            consumed = parse_sgr_mouse(in_buf, in_len, ev);
            if (consumed > 0) goto event_ready;
            if (in_len < 32) return 0; // Wait for terminating M/m
        }

        // CSI Key (Kitty 'u', Tilde '~', or letters 'A'-'Z')
        if (in_len >= 2 && in_buf[1] == '[') {
            consumed = parse_csi_key(in_buf, in_len, ev);
            if (consumed > 0) goto event_ready;
            if (in_len < 32) return 0; // Wait for full sequence
        }

        // SS3 sequences: \x1bO... (F1-F4)
        if (in_len >= 3 && in_buf[1] == 'O') {
            ev->type = TERM_EVENT_KEY;
            memset(&ev->key, 0, sizeof(ev->key));
            ev->key.action = TERM_KEY_ACTION_PRESS;
            ev->key.location = TERM_KEY_LOC_STANDARD;
            switch (in_buf[2]) {
                case 'P': ev->key.keycode = TERM_KEY_F1; break;
                case 'Q': ev->key.keycode = TERM_KEY_F2; break;
                case 'R': ev->key.keycode = TERM_KEY_F3; break;
                case 'S': ev->key.keycode = TERM_KEY_F4; break;
                default:  ev->key.keycode = TERM_KEY_NONE; break;
            }
            consumed = 3;
            goto event_ready;
        }

        // Standalone Escape
        if (in_len == 1) {
            ev->type = TERM_EVENT_KEY;
            memset(&ev->key, 0, sizeof(ev->key));
            ev->key.keycode = TERM_KEY_ESCAPE;
            ev->key.action = TERM_KEY_ACTION_PRESS;
            ev->key.location = TERM_KEY_LOC_STANDARD;
            consumed = 1;
            goto event_ready;
        }
    }

    // 3. UTF-8 / Standard ASCII Fallback
    {
        uint32_t cp = 0;
        consumed = decode_utf8(in_buf, in_len, &cp);
        if (consumed == 0) return 0; // Partial UTF-8 multibyte sequence, wait for next tick

        ev->type = TERM_EVENT_KEY;
        memset(&ev->key, 0, sizeof(ev->key));
        ev->key.action = TERM_KEY_ACTION_PRESS;
        ev->key.location = TERM_KEY_LOC_STANDARD;

        // Legacy Control Characters: Ctrl+A (1) to Ctrl+Z (26), excluding Tab (9) & Enter (10/13)
        if (cp >= 1 && cp <= 26 && cp != 9 && cp != 10 && cp != 13) {
            ev->key.codepoint = 'a' + (cp - 1);
            ev->key.keycode = TERM_KEY_NONE;
            ev->key.modifiers |= TERM_MOD_CTRL;
        } else if (cp == 13 || cp == 10) {
            ev->key.keycode = TERM_KEY_ENTER;
        } else if (cp == 9) {
            ev->key.keycode = TERM_KEY_TAB;
        } else if (cp == 127 || cp == 8) {
            ev->key.keycode = TERM_KEY_BACKSPACE;
        } else {
            ev->key.codepoint = cp;
            ev->key.keycode = TERM_KEY_NONE;
        }
    }

event_ready:
    // 4. Slide consumed bytes out of the accumulator buffer
    if (consumed > 0) {
        memmove(in_buf, in_buf + consumed, in_len - consumed);
        in_len -= consumed;
        in_buf[in_len] = '\0';
        return 1;
    }

    return 0;
}
