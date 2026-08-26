/* ========================================================================= */
/* --- Windows Console State & Lifecycle ---                                 */
/* ========================================================================= */

static HANDLE h_stdin  = INVALID_HANDLE_VALUE;
static HANDLE h_stdout = INVALID_HANDLE_VALUE;
static DWORD orig_in_mode  = 0;
static DWORD orig_out_mode = 0;
static int raw_mode_active = 0;

static uint16_t pending_high_surrogate = 0;
static DWORD prev_mouse_button_state   = 0;

int term_init(void) {
    h_stdin  = GetStdHandle(STD_INPUT_HANDLE);
    h_stdout = GetStdHandle(STD_OUTPUT_HANDLE);

    if (h_stdin == INVALID_HANDLE_VALUE || h_stdout == INVALID_HANDLE_VALUE) {
        return -1;
    }

    if (!GetConsoleMode(h_stdin, &orig_in_mode))   return -1;
    if (!GetConsoleMode(h_stdout, &orig_out_mode)) return -1;

    // Enable mouse, window resize events, and extended flags; disable echo and line editing
    DWORD in_mode = ENABLE_EXTENDED_FLAGS | ENABLE_WINDOW_INPUT | ENABLE_MOUSE_INPUT;
    in_mode &= ~(ENABLE_ECHO_INPUT | ENABLE_LINE_INPUT | ENABLE_PROCESSED_INPUT);

    // Output mode: VT processing best-effort
    DWORD out_mode = orig_out_mode | ENABLE_PROCESSED_OUTPUT | ENABLE_VIRTUAL_TERMINAL_PROCESSING;

    if (!SetConsoleMode(h_stdin, in_mode)) return -1;
    SetConsoleMode(h_stdout, out_mode);

    raw_mode_active = 1;
    pending_high_surrogate = 0;
    prev_mouse_button_state = 0;

    return 0;
}

void term_restore(void) {
    if (!raw_mode_active) return;

    SetConsoleMode(h_stdin, orig_in_mode);
    SetConsoleMode(h_stdout, orig_out_mode);
    raw_mode_active = 0;
}

/* ========================================================================= */
/* --- Event Parsers ---                                                     */
/* ========================================================================= */

static term_modifier_t parse_windows_modifiers(DWORD state, WCHAR ch) {
    term_modifier_t m = TERM_MOD_NONE;

    // Detect AltGr: Windows sends Left-Ctrl + Right-Alt simultaneously
    int is_altgr = ((state & LEFT_CTRL_PRESSED) && (state & RIGHT_ALT_PRESSED));

    if (is_altgr && ch != 0) {
        // Suppress Ctrl + Alt flags on composed printable chars produced via AltGr
    } else {
        if (state & (LEFT_ALT_PRESSED | RIGHT_ALT_PRESSED))   m |= TERM_MOD_ALT;
        if (state & (LEFT_CTRL_PRESSED | RIGHT_CTRL_PRESSED)) m |= TERM_MOD_CTRL;
    }

    if (state & SHIFT_PRESSED)    m |= TERM_MOD_SHIFT;
    if (state & CAPSLOCK_ON)      m |= TERM_MOD_CAPS_LOCK;
    if (state & NUMLOCK_ON)       m |= TERM_MOD_NUM_LOCK;

    return m;
}

static term_keycode_t map_vk_to_keycode(WORD vk, term_key_location_t *loc, DWORD state) {
    *loc = (state & ENHANCED_KEY) ? TERM_KEY_LOC_STANDARD : TERM_KEY_LOC_STANDARD;

    switch (vk) {
        // Navigation & Editing
        case VK_ESCAPE:   return TERM_KEY_ESCAPE;
        case VK_RETURN:   return TERM_KEY_ENTER;
        case VK_TAB:      return TERM_KEY_TAB;
        case VK_BACK:     return TERM_KEY_BACKSPACE;
        case VK_INSERT:   return TERM_KEY_INSERT;
        case VK_DELETE:   return TERM_KEY_DELETE;
        case VK_HOME:     return TERM_KEY_HOME;
        case VK_END:      return TERM_KEY_END;
        case VK_PRIOR:    return TERM_KEY_PAGE_UP;
        case VK_NEXT:     return TERM_KEY_PAGE_DOWN;
        case VK_UP:       return TERM_KEY_UP;
        case VK_DOWN:     return TERM_KEY_DOWN;
        case VK_LEFT:     return TERM_KEY_LEFT;
        case VK_RIGHT:    return TERM_KEY_RIGHT;

        // Function Keys
        case VK_F1:       return TERM_KEY_F1;
        case VK_F2:       return TERM_KEY_F2;
        case VK_F3:       return TERM_KEY_F3;
        case VK_F4:       return TERM_KEY_F4;
        case VK_F5:       return TERM_KEY_F5;
        case VK_F6:       return TERM_KEY_F6;
        case VK_F7:       return TERM_KEY_F7;
        case VK_F8:       return TERM_KEY_F8;
        case VK_F9:       return TERM_KEY_F9;
        case VK_F10:      return TERM_KEY_F10;
        case VK_F11:      return TERM_KEY_F11;
        case VK_F12:      return TERM_KEY_F12;

        // System / Locks
        case VK_SNAPSHOT: return TERM_KEY_PRINT_SCREEN;
        case VK_SCROLL:   return TERM_KEY_SCROLL_LOCK;
        case VK_PAUSE:    return TERM_KEY_PAUSE;
        case VK_CAPITAL:  return TERM_KEY_CAPS_LOCK;
        case VK_NUMLOCK:  return TERM_KEY_NUM_LOCK;

        // Standalone Modifiers
        case VK_SHIFT:
        case VK_LSHIFT:   *loc = (vk == VK_RSHIFT) ? TERM_KEY_LOC_RIGHT : TERM_KEY_LOC_LEFT; return TERM_KEY_SHIFT;
        case VK_RSHIFT:   *loc = TERM_KEY_LOC_RIGHT; return TERM_KEY_SHIFT;
        case VK_CONTROL:
        case VK_LCONTROL: *loc = (state & ENHANCED_KEY) ? TERM_KEY_LOC_RIGHT : TERM_KEY_LOC_LEFT; return TERM_KEY_CTRL;
        case VK_RCONTROL: *loc = TERM_KEY_LOC_RIGHT; return TERM_KEY_CTRL;
        case VK_MENU:
        case VK_LMENU:    *loc = (state & ENHANCED_KEY) ? TERM_KEY_LOC_RIGHT : TERM_KEY_LOC_LEFT; return TERM_KEY_ALT;
        case VK_RMENU:    *loc = TERM_KEY_LOC_RIGHT; return TERM_KEY_ALT;
        case VK_LWIN:     *loc = TERM_KEY_LOC_LEFT;  return TERM_KEY_SUPER;
        case VK_RWIN:     *loc = TERM_KEY_LOC_RIGHT; return TERM_KEY_SUPER;

        default:          return TERM_KEY_NONE;
    }
}

static int handle_key_record(const KEY_EVENT_RECORD *k, term_input_event_t *ev) {
    WCHAR ch = k->uChar.UnicodeChar;

    // Buffer UTF-16 High Surrogate (0xD800 - 0xDBFF)
    if (k->bKeyDown && (ch >= 0xD800 && ch <= 0xDBFF)) {
        pending_high_surrogate = cast(uint16_t, ch);
        return 0; // Incomplete surrogate pair; wait for the next event record
    }

    ev->type = TERM_EVENT_KEY;
    memset(&ev->key, 0, sizeof(ev->key));

    if (k->bKeyDown) {
        ev->key.action = (k->wRepeatCount > 1) ? TERM_KEY_ACTION_REPEAT : TERM_KEY_ACTION_PRESS;
    } else {
        ev->key.action = TERM_KEY_ACTION_RELEASE;
        pending_high_surrogate = 0;
    }

    ev->key.modifiers = parse_windows_modifiers(k->dwControlKeyState, ch);

    term_key_location_t loc = TERM_KEY_LOC_STANDARD;
    term_keycode_t keycode = map_vk_to_keycode(k->wVirtualKeyCode, &loc, k->dwControlKeyState);
    ev->key.location = loc;

    if (keycode != TERM_KEY_NONE) {
        ev->key.keycode = keycode;
        ev->key.codepoint = 0;
        pending_high_surrogate = 0;
    } else {
        ev->key.keycode = TERM_KEY_NONE;

        // Decode UTF-16 Low Surrogate (0xDC00 - 0xDFFF) if preceded by a high surrogate
        if (pending_high_surrogate != 0 && (ch >= 0xDC00 && ch <= 0xDFFF)) {
            ev->key.codepoint = 0x10000 +
                                (((uint32_t)(pending_high_surrogate - 0xD800) << 10) |
                                 ((uint32_t)(ch - 0xDC00)));
            pending_high_surrogate = 0;
        } else {
            pending_high_surrogate = 0;
            ev->key.codepoint = cast(uint32_t, ch);
        }
    }

    return 1;
}

static int handle_mouse_record(const MOUSE_EVENT_RECORD *m, term_input_event_t *ev) {
    ev->type = TERM_EVENT_MOUSE;
    memset(&ev->mouse, 0, sizeof(ev->mouse));

    ev->mouse.x = m->dwMousePosition.X;
    ev->mouse.y = m->dwMousePosition.Y;
    ev->mouse.modifiers = parse_windows_modifiers(m->dwControlKeyState, 0);

    if (m->dwEventFlags & MOUSE_WHEELED) {
        short wheel_delta = cast(short, HIWORD(m->dwButtonState));
        ev->mouse.action = (wheel_delta > 0) ? TERM_MOUSE_ACTION_WHEEL_UP : TERM_MOUSE_ACTION_WHEEL_DOWN;
        ev->mouse.button = TERM_MOUSE_BTN_NONE;
        return 1;
    }

    if (m->dwEventFlags & MOUSE_HWHEELED) {
        short wheel_delta = cast(short, HIWORD(m->dwButtonState));
        ev->mouse.action = (wheel_delta > 0) ? TERM_MOUSE_ACTION_WHEEL_RIGHT : TERM_MOUSE_ACTION_WHEEL_LEFT;
        ev->mouse.button = TERM_MOUSE_BTN_NONE;
        return 1;
    }

    if (m->dwEventFlags == MOUSE_MOVED) {
        ev->mouse.action = (m->dwButtonState != 0) ? TERM_MOUSE_ACTION_DRAG : TERM_MOUSE_ACTION_MOVE;
    } else {
        ev->mouse.action = (m->dwButtonState > prev_mouse_button_state) ? TERM_MOUSE_ACTION_PRESS : TERM_MOUSE_ACTION_RELEASE;
    }

    DWORD changed = m->dwButtonState ^ prev_mouse_button_state;
    DWORD active  = (ev->mouse.action == TERM_MOUSE_ACTION_RELEASE) ? changed : m->dwButtonState;

    if (active & FROM_LEFT_1ST_BUTTON_PRESSED)      ev->mouse.button = TERM_MOUSE_BTN_LEFT;
    else if (active & RIGHTMOST_BUTTON_PRESSED)     ev->mouse.button = TERM_MOUSE_BTN_RIGHT;
    else if (active & FROM_LEFT_2ND_BUTTON_PRESSED) ev->mouse.button = TERM_MOUSE_BTN_MIDDLE;
    else if (active & FROM_LEFT_3RD_BUTTON_PRESSED) ev->mouse.button = TERM_MOUSE_BTN_EXTRA_1;
    else if (active & FROM_LEFT_4TH_BUTTON_PRESSED) ev->mouse.button = TERM_MOUSE_BTN_EXTRA_2;
    else                                            ev->mouse.button = TERM_MOUSE_BTN_NONE;

    prev_mouse_button_state = m->dwButtonState;
    return 1;
}

static int handle_resize_record(const WINDOW_BUFFER_SIZE_RECORD *r, term_input_event_t *ev) {
    ev->type = TERM_EVENT_RESIZE;
    ev->resize.cols = r->dwSize.X;
    ev->resize.rows = r->dwSize.Y;
    return 1;
}

/* ========================================================================= */
/* --- Public Polling API ---                                                */
/* ========================================================================= */

int term_poll_event(term_input_event_t *ev) {
    if (!raw_mode_active || h_stdin == INVALID_HANDLE_VALUE) return 0;

    DWORD num_events = 0;
    if (!GetNumberOfConsoleInputEvents(h_stdin, &num_events) || num_events == 0) {
        return 0;
    }

    INPUT_RECORD ir;
    DWORD read_count = 0;

    while (num_events > 0) {
        if (!ReadConsoleInputW(h_stdin, &ir, 1, &read_count) || read_count == 0) {
            return 0;
        }

        switch (ir.EventType) {
            case KEY_EVENT:
                if (handle_key_record(&ir.Event.KeyEvent, ev)) {
                    return 1;
                }
                break;

            case MOUSE_EVENT:
                pending_high_surrogate = 0;
                return handle_mouse_record(&ir.Event.MouseEvent, ev);

            case WINDOW_BUFFER_SIZE_EVENT:
                pending_high_surrogate = 0;
                return handle_resize_record(&ir.Event.WindowBufferSizeEvent, ev);

            case FOCUS_EVENT:
            case MENU_EVENT:
            default:
                break;
        }

        GetNumberOfConsoleInputEvents(h_stdin, &num_events);
    }

    return 0;
}
