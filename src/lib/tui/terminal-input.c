/**
 * To support TUI input across platforms (Windows) and to potentially
 * allow a TUI to be embedded inside of a traditional GUI as
 * mono-spaced gridspace, we abstract out terminal input into a simple
 * non-blocking polling model that returns easy to use events.
 */

/* --- Event Category Types --- */

typedef enum {
    TERM_EVENT_NONE = 0,
    TERM_EVENT_KEY,
    TERM_EVENT_MOUSE,
    TERM_EVENT_RESIZE
} term_event_type_t;

/* --- Modifiers (Shared) --- */

typedef enum {
    TERM_MOD_NONE      = 0,
    TERM_MOD_SHIFT     = 1 << 0,
    TERM_MOD_ALT       = 1 << 1,
    TERM_MOD_CTRL      = 1 << 2,
    TERM_MOD_SUPER     = 1 << 3,
    TERM_MOD_CAPS_LOCK = 1 << 4,
    TERM_MOD_NUM_LOCK  = 1 << 5
} term_modifier_t;

/* --- Keyboard Types --- */

typedef enum {
    TERM_KEY_ACTION_UNKNOWN = 0,
    TERM_KEY_ACTION_PRESS   = 1,
    TERM_KEY_ACTION_REPEAT  = 2,
    TERM_KEY_ACTION_RELEASE = 3
} term_key_action_t;

typedef enum {
    TERM_KEY_LOC_STANDARD = 0,
    TERM_KEY_LOC_NUMPAD   = 1,
    TERM_KEY_LOC_LEFT     = 2,
    TERM_KEY_LOC_RIGHT    = 3
} term_key_location_t;

typedef enum {
    TERM_KEY_NONE = 0,

    // Navigation & Editing
    TERM_KEY_ESCAPE,
    TERM_KEY_ENTER,
    TERM_KEY_TAB,
    TERM_KEY_BACKSPACE,
    TERM_KEY_INSERT,
    TERM_KEY_DELETE,
    TERM_KEY_HOME,
    TERM_KEY_END,
    TERM_KEY_PAGE_UP,
    TERM_KEY_PAGE_DOWN,
    TERM_KEY_UP,
    TERM_KEY_DOWN,
    TERM_KEY_LEFT,
    TERM_KEY_RIGHT,

    // Function Keys
    TERM_KEY_F1,  TERM_KEY_F2,  TERM_KEY_F3,  TERM_KEY_F4,
    TERM_KEY_F5,  TERM_KEY_F6,  TERM_KEY_F7,  TERM_KEY_F8,
    TERM_KEY_F9,  TERM_KEY_F10, TERM_KEY_F11, TERM_KEY_F12,

    // System / Locks
    TERM_KEY_PRINT_SCREEN,
    TERM_KEY_SCROLL_LOCK,
    TERM_KEY_PAUSE,
    TERM_KEY_CAPS_LOCK,
    TERM_KEY_NUM_LOCK

    // Modifier Keys (when pressed/released standalone)
    //
    // LEFT vs RIGHT is put into the location if a user needs that
    // level of detail.
    TERM_KEY_SHIFT,
    TERM_KEY_CTRL,
    TERM_KEY_ALT,
    TERM_KEY_SUPER,
    TERM_KEY_HYPER,
    TERM_KEY_META,

    // TODO(jawilson): multimedia controls
} term_keycode_t;

typedef struct {
    uint32_t            codepoint; // Raw ASCII or Unicode (0 for functional keys like F1, Arrows)
    term_keycode_t      keycode;   // TERM_KEY_* (TERM_KEY_NONE for printable characters)
    term_modifier_t     modifiers; // Bitmask of active modifier keys
    term_key_location_t location;  // Standard, Numpad, Left, Right
    term_key_action_t   action;    // Press, Repeat, Release
} term_keypress_t;

/* --- Mouse Types --- */

typedef enum {
    TERM_MOUSE_ACTION_PRESS,
    TERM_MOUSE_ACTION_RELEASE,
    TERM_MOUSE_ACTION_MOVE,
    TERM_MOUSE_ACTION_DRAG,
    TERM_MOUSE_ACTION_WHEEL_UP,
    TERM_MOUSE_ACTION_WHEEL_DOWN,
    TERM_MOUSE_ACTION_WHEEL_LEFT,
    TERM_MOUSE_ACTION_WHEEL_RIGHT
} term_mouse_action_t;

typedef enum {
    TERM_MOUSE_BTN_NONE    = 0,
    TERM_MOUSE_BTN_LEFT    = 1,
    TERM_MOUSE_BTN_MIDDLE  = 2,
    TERM_MOUSE_BTN_RIGHT   = 3,
    TERM_MOUSE_BTN_EXTRA_1 = 4,
    TERM_MOUSE_BTN_EXTRA_2 = 5
} term_mouse_button_t;

typedef struct {
    int32_t             x;         // 0-indexed column
    int32_t             y;         // 0-indexed row
    term_mouse_button_t button;    // Active or triggering button
    term_mouse_action_t action;    // Press, Release, Move, Scroll, etc.
    term_modifier_t     modifiers; // Shift/Alt/Ctrl held during mouse action
} term_mouse_event_t;

/* --- Window Resize Type --- */

typedef struct {
    int32_t cols;
    int32_t rows;
} term_resize_event_t;

/* --- Unified Input Event --- */

typedef struct {
    term_event_type_t type;
    union {
        term_keypress_t     key;
        term_mouse_event_t   mouse;
        term_resize_event_t  resize;
    };
} term_input_event_t;
