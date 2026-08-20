/**
 * @struct term_keypress_t
 *
 * Represents a keypress from the terminal. Either one of code_point
 * or key_code will be non-zero if n_bytes_consumed is greater than
 * zero.
 */
typedef term_keypress_t = struct {
  uint32_t code_point;
  uint8_t key_code;
  uint8_t n_bytes_consumed;
  uint8_t shift : 1;
  uint8_t ctrl : 1;
  uint8_t meta : 1;
  uint8_t super : 1;
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
