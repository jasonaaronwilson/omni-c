#ifndef _HEADER_FILE_GUARD_
#define _HEADER_FILE_GUARD_

#ifndef _REFLECTION_H_
#define _REFLECTION_H_

#include <string.h>

/* ====================================================================== */
// These are the reflection API data structures for a program compiled
// with Omni C. We use linked lists instead of value_array_t to keep
// compiled programs independent of any particular library
// data-structures.
/* ====================================================================== */

// ----------------------------------------------------------------------
// Enumerations
// ----------------------------------------------------------------------

typedef struct enum_element_metadata_S {
  struct enum_element_metadata_S* next;
  char* name;
  long value;
} enum_element_metadata_t;

/**
 * @structure enum_metadata_t
 *
 * The runtime metadata for a reflected enumeration.
 */
typedef struct {
  char* name;
  enum_element_metadata_t* elements;
} enum_metadata_t;

// ----------------------------------------------------------------------
// Structures
// ----------------------------------------------------------------------

typedef struct field_metadata_S {
  struct field_metadata_S* next;
  char* name;
  char* type_name_string;
  int start_offset;
} field_metadata_t;

/**
 * @structure structure_metadata_t
 *
 * The runtime metadata for a reflected structure.
 */
typedef struct {
  char* name;
  int size;
  int alignment;
  field_metadata_t* fields;
} structure_metadata_t;

// ----------------------------------------------------------------------
// Unions 
// ----------------------------------------------------------------------

/*
 * Unions could be treated exactly like structures however there is no
 * uniform way to tag a union so making use of the metadata is a bit
 * problematic. For that reason (and because omni-c doesn't need them
 * yet), we are skipping them for now.
 */

// ----------------------------------------------------------------------
// Functions
// ----------------------------------------------------------------------

typedef struct function_arg_metadata_S {
  struct function_arg_metadata_t* next;
  char* name;
  char* type_string;
} function_arg_metadata_t;

/**
 * @structure function_metadata_t
 *
 * The runtime metadata for a reflected structure.
 */
typedef struct {
  char* name;
  function_arg_metadata_t* arguments;
} function_metadata_t;

#endif /* _REFLECTION_H_ */
// ========== system includes ==========

#include <stdbool.h>
#include <stdint.h>
#include <execinfo.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdarg.h>
#include <ctype.h>
#include <errno.h>
#include <fcntl.h>
#include <limits.h>
#include <sys/select.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <termios.h>
#include <time.h>
#include <sys/types.h>
#include <sys/wait.h>

// ========== defines ==========

#define _OMNI_C_H_

#define cast(type, expr) ((type) (expr))

#define block_expr(block) block

#define _MIN_MAX_H_

#define min(a, b) ((a) < (b) ? (a) : (b))

#define max(a, b) ((a) > (b) ? (a) : (b))

#define _BOOLEAN_H_

#define _COMPOUND_LITERAL_H_

#define compound_literal(type, ...) ((type) __VA_ARGS__)

#define _FN_H_

#define fn_t(return_type, ...) typeof(return_type(*)(__VA_ARGS__))

#define _LEB128_H_

#define ERROR_INSUFFICIENT_INPUT -1

#define ERROR_TOO_BIG -2

#define _FATAL_ERROR_H_

#define fatal_error(code) fatal_error_impl(__FILE__, __LINE__, code)

#define _VALUE_H_

#define boolean_to_value(x) compound_literal(value_t, {.u64 = x})

#define u64_to_value(x) compound_literal(value_t, {.u64 = x})

#define i64_to_value(x) compound_literal(value_t, {.i64 = x})

#define str_to_value(x) compound_literal(value_t, {.str = x})

#define ptr_to_value(x) compound_literal(value_t, {.ptr = x})

#define dbl_to_value(x) compound_literal(value_t, {.dbl = x})

#define _ALLOCATE_H_

#define malloc_bytes(amount) (checked_malloc(__FILE__, __LINE__, amount))

#define free_bytes(ptr) (checked_free(__FILE__, __LINE__, ptr))

#define malloc_struct(struct_name)                                             \
  ((struct_name*) (checked_malloc(__FILE__, __LINE__, sizeof(struct_name))))

#define malloc_copy_of(source, number_of_bytes)                                \
  (checked_malloc_copy_of(__FILE__, __LINE__, source, number_of_bytes))

#define ARMYKNIFE_MEMORY_ALLOCATION_START_PADDING 8

#define ARMYKNIFE_MEMORY_ALLOCATION_END_PADDING 8

#define ARMYKNIFE_MEMORY_ALLOCATION_HASHTABLE_SIZE 16

#define ARMYKNIFE_MEMORY_ALLOCATION_MAXIMUM_AMOUNT (1L << 48)

#define START_PADDING_BYTE (170 & 0xff)

#define END_PADDING_BYTE ((~START_PADDING_BYTE) & 0xff)

#define _UINT64_H_

#define _STRING_UTIL_H_

#define STRING_PRINTF_INITIAL_BUFFER_SIZE 1024

#define _LOGGER_H_

#define LOGGER_OFF 0

#define LOGGER_TRACE 1

#define LOGGER_DEBUG 2

#define LOGGER_INFO 3

#define LOGGER_WARN 4

#define LOGGER_FATAL 5

#define LOGGER_TEST 6

#define LOGGER_DEFAULT_LEVEL LOGGER_WARN

#define log_none(format, ...)                                                  \
  do {                                                                         \
  } while (0);

#define log_off(format, ...)                                                   \
  do {                                                                         \
    if (0) {                                                                   \
      logger_impl(__FILE__, __LINE__, __FUNCTION__, LOGGER_TRACE, format,      \
                  ##__VA_ARGS__);                                              \
    }                                                                          \
  } while (0)

#define log_trace(format, ...)                                                 \
  do {                                                                         \
    if (global_logger_state.level <= LOGGER_TRACE) {                           \
      logger_impl(__FILE__, __LINE__, __FUNCTION__, LOGGER_TRACE, format,      \
                  ##__VA_ARGS__);                                              \
    }                                                                          \
  } while (0)

#define log_debug(format, ...)                                                 \
  do {                                                                         \
    if (global_logger_state.level <= LOGGER_DEBUG) {                           \
      logger_impl(__FILE__, __LINE__, __FUNCTION__, LOGGER_DEBUG, format,      \
                  ##__VA_ARGS__);                                              \
    }                                                                          \
  } while (0)

#define log_info(format, ...)                                                  \
  do {                                                                         \
    if (global_logger_state.level <= LOGGER_INFO) {                            \
      logger_impl(__FILE__, __LINE__, __FUNCTION__, LOGGER_INFO, format,       \
                  ##__VA_ARGS__);                                              \
    }                                                                          \
  } while (0)

#define log_warn(format, ...)                                                  \
  do {                                                                         \
    if (global_logger_state.level <= LOGGER_WARN) {                            \
      logger_impl(__FILE__, __LINE__, __FUNCTION__, LOGGER_WARN, format,       \
                  ##__VA_ARGS__);                                              \
    }                                                                          \
  } while (0)

#define log_fatal(format, ...)                                                 \
  do {                                                                         \
    if (global_logger_state.level <= LOGGER_FATAL) {                           \
      logger_impl(__FILE__, __LINE__, __FUNCTION__, LOGGER_FATAL, format,      \
                  ##__VA_ARGS__);                                              \
    }                                                                          \
  } while (0)

#define log_test(format, ...)                                                  \
  do {                                                                         \
    logger_impl(__FILE__, __LINE__, __FUNCTION__, LOGGER_TEST, format,         \
                ##__VA_ARGS__);                                                \
  } while (0)

#define _UTF8_DECODER_H_

#define _BUFFER_H_

#define BUFFER_PRINTF_INITIAL_BUFFER_SIZE 1024

#define _VALUE_ARRAY_H_

#define value_array_get_ptr(array, index_expression, cast_type)                \
  (cast(cast_type, value_array_get(array, index_expression).ptr))

#define _VALUE_ALIST_H_

#define value_alist_foreach(alist, key_var, value_var, statements)             \
  do {                                                                         \
    value_alist_t* head = alist;                                               \
    while (head) {                                                             \
      value_t key_var = head->key;                                             \
      value_t value_var = head->value;                                         \
      statements;                                                              \
      head = head->next;                                                       \
    }                                                                          \
  } while (0)

#define _STRING_ALIST_H_

#define string_alist_foreach(alist, key_var, value_var, statements)            \
  do {                                                                         \
    value_alist_foreach(cast(value_alist_t*, alist), key_var##_value,          \
                        value_var, {                                           \
                          char* key_var = (key_var##_value).str;               \
                          statements;                                          \
                        });                                                    \
  } while (0)

#define _VALUE_HASHTABLE_H_

#define ARMYKNIFE_HT_LOAD_FACTOR 0.75

#define AK_HT_UPSCALE_MULTIPLIER 1.75

#define value_ht_foreach(ht, key_var, value_var, statements)                   \
  do {                                                                         \
    for (int ht_index = 0; ht_index < ht->n_buckets; ht_index++) {             \
      value_alist_t* alist = ht->buckets[ht_index];                            \
      if (alist != NULL) {                                                     \
        value_alist_foreach(alist, key_var, value_var, statements);            \
      }                                                                        \
    }                                                                          \
  } while (0)

#define _STRING_HASHTABLE_H_

#define string_ht_foreach(ht, key_var, value_var, statements)                  \
  do {                                                                         \
    value_ht_foreach(to_value_hashtable(ht), key_var##_value, value_var, {     \
      char* key_var = (key_var##_value).str;                                   \
      statements;                                                              \
    });                                                                        \
  } while (0)

#define _VALUE_TREE_H_

#define value_tree_foreach(tree, key_var, value_var, statements)               \
  do {                                                                         \
    int stack_n_elements = 0;                                                  \
    value_tree_t* stack[64];                                                   \
    value_tree_t* current = tree;                                              \
    while (current != NULL || stack_n_elements > 0) {                          \
      while (current != NULL) {                                                \
        stack[stack_n_elements++] = current;                                   \
        current = current->left;                                               \
      }                                                                        \
      current = stack[--stack_n_elements];                                     \
      value_t key_var = current->key;                                          \
      value_t value_var = current->value;                                      \
      statements;                                                              \
      current = current->right;                                                \
    }                                                                          \
  } while (0)

#define _STRING_TREE_H_

#define string_tree_foreach(tree, key_var, value_var, statements)              \
  do {                                                                         \
    value_tree_foreach(cast(value_tree_t*, tree), key_var##_value, value_var,  \
                       {                                                       \
                         char* key_var = (key_var##_value).str;                \
                         statements;                                           \
                       });                                                     \
  } while (0)

#define _FLAG_H_

#define _IO_H_

#define FILE_COPY_STREAM_BUFFER_SIZE 1024

#define _TERMINAL_H_

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

#define TERM_ESCAPE_START_STR "\033["

#define TERM_ESCAPE_END_STR "m"

#define TERM_ESCAPE_STRING_START_AND_END(str)                                  \
  (TERM_ESCAPE_START_STR str TERM_ESCAPE_END_STR)

#define TERM_ESCAPE_STRING(str) (TERM_ESCAPE_START_STR str)

#define _TOKENIZER_H_

#define _RANDOM_H_

#define _CDL_PRINTER_H_

#define _SUB_PROCESS_H_

#define _TEST_H_

#define test_fail(format, ...)                                                 \
  do {                                                                         \
    test_fail_and_exit(__FILE__, __LINE__, format, ##__VA_ARGS__);             \
  } while (0)

#define test_assert(condition)                                                 \
  do {                                                                         \
    if (!(condition))                                                          \
      test_fail("A test assertion failed. Condition expression was: %s",       \
                #condition);                                                   \
  } while (0)

#define test_assert_integer_equal(a, b)                                        \
  do {                                                                         \
    unsigned long long casted_a = (unsigned long long) (a);                    \
    unsigned long long casted_b = (unsigned long long) (b);                    \
    if (a != b) {                                                              \
      test_fail(                                                               \
          "An integer comparision failed\n  Expected:\n    ⟦%llu⟧\n  "     \
          "But was:\n    ⟦%llu⟧\n",                                            \
          casted_a, casted_b);                                                 \
    }                                                                          \
  } while (0)

#define test_assert_string_equal(a, b)                                          \
  do {                                                                          \
    if (!b) {                                                                   \
      test_fail(                                                                \
          "A test string equal assertion failed\n  Expected:\n    ⟦%s⟧\n  " \
          "But was:\n    nullptr\n",                                            \
          a);                                                                   \
    }                                                                           \
    if (!string_equal(a, b)) {                                                  \
      test_fail(                                                                \
          "A test string equal assertion failed\n  Expected:\n    ⟦%s⟧\n  " \
          "But was:\n    ⟦%s⟧\n",                                               \
          a, b);                                                                \
    }                                                                           \
  } while (0)

// ========== enums ==========

// ========== typedefs ==========

typedef bool boolean_t;

typedef struct unsigned_decode_result__generated_S unsigned_decode_result;

typedef struct signed_decode_result__generated_S signed_decode_result;

typedef struct fatal_error_config_S fatal_error_config_t;

typedef enum {
  ERROR_UKNOWN,
  ERROR_SIGSEGV,
  ERROR_ACCESS_OUT_OF_BOUNDS,
  ERROR_BAD_COMMAND_LINE,
  ERROR_DYNAMICALLY_SIZED_TYPE_ILLEGAL_IN_CONTAINER,
  ERROR_ILLEGAL_ENUM_VALUE,
  ERROR_ILLEGAL_INITIAL_CAPACITY,
  ERROR_ILLEGAL_NULL_ARGUMENT,
  ERROR_ILLEGAL_ZERO_HASHCODE_VALUE,
  ERROR_ILLEGAL_RANGE,
  ERROR_MEMORY_ALLOCATION,
  ERROR_MEMORY_FREE_NULL,
  ERROR_NOT_REACHED,
  ERROR_REFERENCE_NOT_EXPECTED_TYPE,
  ERROR_UNIMPLEMENTED,
  ERROR_OPEN_LOG_FILE,
  ERROR_TEST,
  ERROR_INTERNAL_ASSERTION_FAILURE,
  ERROR_BAD_ALLOCATION_SIZE,
  ERROR_ILLEGAL_ARGUMENT,
  ERROR_MEMORY_START_PADDING_ERROR,
  ERROR_MEMORY_END_PADDING_ERROR,
  ERROR_FATAL,
  ERROR_ILLEGAL_STATE,
  ERROR_ILLEGAL_INPUT,
  ERROR_ILLEGAL_UTF_8_CODE_POINT,
  ERROR_ILLEGAL_TERMINAL_COORDINATES,
} error_code_t;

typedef union  {
  uint64_t u64;
  uint64_t i64;
  char* str;
  void* ptr;
  void* dbl;
} value_t;

typedef enum {
  NF_OK,
  NF_ERROR_NOT_FOUND,
  NF_ERROR_NOT_PARSED_AS_NUMBER,
  NF_ERROR_NOT_PARSED_AS_EXPECTED_ENUM,
} non_fatal_error_code_t;

typedef struct value_result_t__generated_S value_result_t;

typedef fn_t(int, value_t, value_t) value_comparison_fn;

typedef fn_t(uint64_t, value_t) value_hash_fn;

typedef struct memory_hashtable_bucket_S memory_hashtable_bucket_t;

typedef struct logger_state_S logger_state_t;

typedef struct utf8_decode_result_S utf8_decode_result_t;

typedef struct buffer_S buffer_t;

typedef struct line_and_column_S line_and_column_t;

typedef struct value_array_S value_array_t;

typedef struct value_alist_S value_alist_t;

typedef struct string_alist_S string_alist_t;

typedef struct value_hashtable_S value_hashtable_t;

typedef struct string_hashtable_S string_hashtable_t;

typedef struct value_tree_S value_tree_t;

typedef struct string_tree_S string_tree_t;

typedef enum {
  flag_type_none,
  flag_type_boolean,
  flag_type_string,
  flag_type_uint64,
  flag_type_int64,
  flag_type_double,
  flag_type_enum,
  flag_type_custom,
} flag_type_t;

typedef struct program_descriptor_S program_descriptor_t;

typedef struct command_descriptor_S command_descriptor_t;

typedef struct flag_descriptor_S flag_descriptor_t;

typedef struct flag_key_value_S flag_key_value_t;

typedef struct box_drawing_S box_drawing_t;

typedef struct term_keypress_S term_keypress_t;

typedef struct random_state_S random_state_t;

typedef struct cdl_printer_t__generated_S cdl_printer_t;

typedef enum {
  EXIT_STATUS_UNKNOWN,
  EXIT_STATUS_NORMAL_EXIT,
  EXIT_STATUS_SIGNAL,
  EXIT_STATUS_ABNORMAL,
} sub_process_exit_status_t;

typedef struct sub_process_t__generated_S sub_process_t;

// ========== stuctures/unions ==========

struct fatal_error_config_S {
  boolean_t catch_sigsegv;
};

struct memory_hashtable_bucket_S {
  uint64_t malloc_address;
  uint64_t malloc_size;
  char* allocation_filename;
  uint64_t allocation_line_number;
};

struct logger_state_S {
  boolean_t initialized;
  int level;
  char* logger_output_filename;
  FILE* output;
};

struct utf8_decode_result_S {
  uint32_t code_point;
  uint8_t num_bytes;
  boolean_t error;
};

struct buffer_S {
  uint32_t length;
  uint32_t capacity;
  uint8_t* elements;
};

struct value_array_S {
  uint32_t length;
  uint32_t capacity;
  value_t* elements;
};

struct value_alist_S {
  struct value_alist_S* next;
  value_t key;
  value_t value;
};

struct string_alist_S {
};

struct value_hashtable_S {
  uint64_t n_buckets;
  uint64_t n_entries;
  value_alist_t** buckets;
};

struct string_hashtable_S {
};

struct value_tree_S {
  value_t key;
  value_t value;
  uint32_t level;
  struct value_tree_S* left;
  struct value_tree_S* right;
};

struct string_tree_S {
};

struct program_descriptor_S {
  char* name;
  char* description;
  string_tree_t* flags;
  string_tree_t* commands;
  value_array_t** write_back_file_args_ptr;
};

struct command_descriptor_S {
  program_descriptor_t* program;
  char* name;
  char* description;
  char** write_back_ptr;
  value_array_t** write_back_file_args_ptr;
  string_tree_t* flags;
};

struct flag_descriptor_S {
  char* name;
  char* description;
  flag_type_t flag_type;
  char* help_string;
  void* write_back_ptr;
  int enum_size;
  string_tree_t* enum_values;
};

struct flag_key_value_S {
  char* key;
  char* value;
};

struct box_drawing_S {
  uint32_t upper_left_corner;
  uint32_t upper_right_corner;
  uint32_t lower_left_corner;
  uint32_t lower_right_corner;
  uint32_t top_edge;
  uint32_t left_edge;
  uint32_t right_edge;
  uint32_t bottom_edge;
};

struct term_keypress_S {
  uint32_t code_point;
  uint8_t key_code;
  uint8_t n_bytes_consumed;
  uint8_t shift;
  uint8_t ctrl;
  uint8_t meta;
  uint8_t super;
  uint8_t hyper;
};

struct random_state_S {
  uint64_t a;
  uint64_t b;
};

struct unsigned_decode_result__generated_S {
  uint64_t number;
  int size;
};

struct signed_decode_result__generated_S {
  uint64_t number;
  int size;
};

struct value_result_t__generated_S {
  union  {
    uint64_t u64;
    int64_t i64;
    double dbl;
    char* str;
    void* ptr;
    value_t val;
} ;
  non_fatal_error_code_t nf_error;
};

struct line_and_column_S {
  uint64_t line;
  uint64_t column;
};

struct cdl_printer_t__generated_S {
  buffer_t* buffer;
  char* key_token;
  int indention_level;
};

struct sub_process_t__generated_S {
  value_array_t* argv;
  pid_t pid;
  int stdin;
  int stdout;
  int stderr;
  sub_process_exit_status_t exit_status;
  int exit_code;
  int exit_signal;
};

// ========== global variables ==========

extern fatal_error_config_t fatal_error_config;

extern boolean_t is_initialized;

extern boolean_t should_log_value;

extern uint64_t number_of_bytes_allocated;

extern uint64_t number_of_malloc_calls;

extern uint64_t number_of_free_calls;

extern memory_hashtable_bucket_t memory_ht[ARMYKNIFE_MEMORY_ALLOCATION_HASHTABLE_SIZE];

extern logger_state_t global_logger_state;

extern program_descriptor_t* current_program;

extern command_descriptor_t* current_command;

extern flag_descriptor_t* current_flag;

extern random_state_t shared_random_state;

// ========== function prototypes ==========

extern unsigned encode_sleb_128(int64_t Value, uint8_t* p);
extern unsigned encode_uleb_128(uint64_t Value, uint8_t* p);
extern unsigned_decode_result decode_uleb_128(const uint8_t* p, const uint8_t* end);
extern signed_decode_result decode_sleb_128(const uint8_t* p, const uint8_t* end);
extern _Noreturn void fatal_error_impl(char* file, int line, int error_code);
extern const char* fatal_error_code_to_string(int error_code);
extern void configure_fatal_errors(fatal_error_config_t config);
void segmentation_fault_handler(int signal_number);
void print_fatal_error_banner();
void print_backtrace();
void print_error_code_name(int error_code);
char* get_command_line();
char* get_program_path();
int cmp_string_values(value_t value1, value_t value2);
uint64_t hash_string_value(value_t value1);
extern uint8_t* checked_malloc(char* file, int line, uint64_t amount);
extern uint8_t* checked_malloc_copy_of(char* file, int line, uint8_t* source, uint64_t amount);
extern void checked_free(char* file, int line, void* pointer);
extern void check_memory_hashtable_padding();
void check_start_padding(uint8_t* address);
void check_end_padding(uint8_t* address, char* filename, uint64_t line);
uint64_t mumurhash64_mix(uint64_t h);
void track_padding(char* file, int line, uint8_t* address, uint64_t amount);
void untrack_padding(uint8_t* malloc_address);
extern int uint64_highest_bit_set(uint64_t n);
extern int string_is_null_or_empty(const char* str1);
extern int string_equal(const char* str1, const char* str2);
extern int string_starts_with(const char* str1, const char* str2);
extern int string_ends_with(const char* str1, const char* str2);
extern boolean_t string_contains_char(const char* str, char ch);
extern int string_index_of_char(const char* a, char ch);
extern char* uint64_to_string(uint64_t number);
extern uint64_t string_hash(const char* str);
extern char* string_substring(const char* str, int start, int end);
extern value_result_t string_parse_uint64(const char* string);
extern value_result_t string_parse_uint64_dec(const char* string);
extern value_result_t string_parse_uint64_hex(const char* string);
extern value_result_t string_parse_uint64_bin(const char* string);
extern char* string_duplicate(const char* src);
extern char* string_append(const char* a, const char* b);
extern char* string_left_pad(const char* a, int count, char ch);
extern char* string_right_pad(const char* a, int count, char ch);
__attribute__((format(printf, 1, 2))) extern char* string_printf(char* format, ...);
char* string_truncate(char* str, int limit, char* at_limit_suffix);
uint64_t fasthash64(const void* buf, size_t len, uint64_t seed);
extern void logger_init(void);
__attribute__((format(printf, 5, 6))) extern void logger_impl(char* file, int line_number, const char* function, int level, char* format, ...);
value_result_t parse_log_level_enum(char* str);
char* logger_level_to_string(int level);
extern utf8_decode_result_t utf8_decode(const uint8_t* utf8_bytes);
extern buffer_t* make_buffer(uint64_t initial_capacity);
extern uint64_t buffer_length(buffer_t* buffer);
extern uint8_t buffer_get(buffer_t* buffer, uint64_t position);
extern char* buffer_c_substring(buffer_t* buffer, uint64_t start, uint64_t end);
extern char* buffer_to_c_string(buffer_t* buffer);
extern void buffer_clear(buffer_t* buffer);
extern buffer_t* buffer_increase_capacity(buffer_t* buffer, uint64_t capacity);
extern buffer_t* buffer_append_byte(buffer_t* buffer, uint8_t byte);
extern buffer_t* buffer_append_bytes(buffer_t* buffer, uint8_t* bytes, uint64_t n_bytes);
extern buffer_t* buffer_append_buffer(buffer_t* buffer, buffer_t* src_buffer);
extern buffer_t* buffer_append_sub_buffer(buffer_t* buffer, uint64_t start_position, uint64_t end_position, buffer_t* src_buffer);
extern buffer_t* buffer_append_string(buffer_t* buffer, const char* str);
__attribute__((format(printf, 2, 3))) extern buffer_t* buffer_printf(buffer_t* buffer, char* format, ...);
extern buffer_t* buffer_append_repeated_byte(buffer_t* buffer, uint8_t byte, int count);
utf8_decode_result_t buffer_utf8_decode(buffer_t* buffer, uint64_t position);
extern buffer_t* buffer_append_code_point(buffer_t* buffer, uint32_t code_point);
boolean_t buffer_match_string_at(buffer_t* buffer, uint64_t start_position, char* str);
buffer_t* buffer_from_string(char* string);
buffer_t* buffer_adjust_region(buffer_t* buffer, uint64_t original_start, uint64_t original_end, uint64_t new_width);
buffer_t* buffer_replace_all(buffer_t* buffer, char* original_text, char* replacement_text);
buffer_t* buffer_replace_matching_byte(buffer_t* buffer, uint8_t original, uint8_t replacement);
boolean_t buffer_region_contains(buffer_t* buffer, uint64_t start, uint64_t end, char* text);
uint64_t buffer_beginning_of_line(buffer_t* buffer, uint64_t start);
uint64_t buffer_end_of_line(buffer_t* buffer, uint64_t start);
buffer_t* buffer_to_uppercase(buffer_t* buffer);
buffer_t* buffer_to_lowercase(buffer_t* buffer);
line_and_column_t buffer_position_to_line_and_column(buffer_t* buffer, uint64_t position);
extern value_array_t* make_value_array(uint64_t initial_capacity);
extern value_t value_array_get(value_array_t* array, uint32_t index);
extern void value_array_replace(value_array_t* array, uint32_t index, value_t element);
extern void value_array_add(value_array_t* array, value_t element);
extern void value_array_push(value_array_t* array, value_t element);
extern value_t value_array_pop(value_array_t* array);
extern void value_array_insert_at(value_array_t* array, uint32_t position, value_t element);
extern value_t value_array_delete_at(value_array_t* array, uint32_t position);
void value_array_ensure_capacity(value_array_t* array, uint32_t required_capacity);
extern value_result_t value_alist_find(value_alist_t* list, value_comparison_fn cmp_fn, value_t key);
__attribute__((warn_unused_result)) extern value_alist_t* value_alist_insert(value_alist_t* list, value_comparison_fn cmp_fn, value_t key, value_t value);
__attribute__((warn_unused_result)) extern value_alist_t* value_alist_delete(value_alist_t* list, value_comparison_fn cmp_fn, value_t key);
__attribute__((warn_unused_result)) extern uint64_t value_alist_length(value_alist_t* list);
extern value_hashtable_t* make_value_hashtable(uint64_t n_buckets);
extern value_hashtable_t* value_ht_insert(value_hashtable_t* ht, value_hash_fn hash_fn, value_comparison_fn cmp_fn, value_t key, value_t value);
extern value_hashtable_t* value_ht_delete(value_hashtable_t* ht, value_hash_fn hash_fn, value_comparison_fn cmp_fn, value_t key);
extern value_result_t value_ht_find(value_hashtable_t* ht, value_hash_fn hash_fn, value_comparison_fn cmp_fn, value_t key);
extern void value_hashtable_upsize_internal(value_hashtable_t* ht, value_hash_fn hash_fn, value_comparison_fn cmp_fn);
extern value_result_t value_tree_find(value_tree_t* t, value_comparison_fn cmp_fn, value_t key);
__attribute__((warn_unused_result)) extern value_tree_t* value_tree_insert(value_tree_t* t, value_comparison_fn cmp_fn, value_t key, value_t value);
__attribute__((warn_unused_result)) extern value_tree_t* value_tree_delete(value_tree_t* t, value_comparison_fn cmp_fn, value_t key);
value_tree_t* value_tree_skew(value_tree_t* t);
value_tree_t* value_tree_split(value_tree_t* t);
value_tree_t* make_value_tree_leaf(value_t key, value_t value);
value_tree_t* value_tree_decrease_level(value_tree_t* t);
value_tree_t* value_tree_predecessor(value_tree_t* t);
value_tree_t* value_tree_successor(value_tree_t* t);
extern void flag_program_name(char* name);
extern void flag_description(char* description);
extern void flag_file_args(value_array_t** write_back_ptr);
extern void flag_command(char* name, char** write_back_ptr);
extern void flag_boolean(char* name, boolean_t* write_back_ptr);
extern void flag_string(char* name, char** write_back_ptr);
extern void flag_uint64(char* name, uint64_t* write_back_ptr);
extern void flag_int64(char* name, int64_t* write_back_ptr);
extern void flag_double(char* name, double* write_back_ptr);
extern void flag_enum(char* name, int* write_back_ptr);
extern void flag_enum_64(char* name, uint64_t* write_back_ptr);
extern void flag_enum_value(char* name, uint64_t value);
extern void flag_alias(char* alias);
extern char* flag_parse_command_line(int argc, char** argv);
extern void flag_print_help(FILE* out, char* error);
command_descriptor_t* flag_find_command_descriptor(char* name);
flag_descriptor_t* flag_find_flag_descriptor(command_descriptor_t* command, char* name);
flag_key_value_t flag_split_argument(char* arg);
char* parse_and_write_value(flag_descriptor_t* flag, flag_key_value_t key_value);
char* parse_and_write_boolean(flag_descriptor_t* flag, flag_key_value_t key_value);
char* parse_and_write_uint64(flag_descriptor_t* flag, flag_key_value_t key_value);
char* parse_and_write_enum(flag_descriptor_t* flag, flag_key_value_t key_value);
void add_flag(char* name, void* write_back_ptr, flag_type_t flag_type);
void flag_print_flags(FILE* out, char* header, string_tree_t* flags);
__attribute__((warn_unused_result)) extern buffer_t* buffer_append_file_contents(buffer_t* bytes, char* file_name);
__attribute__((warn_unused_result)) extern buffer_t* buffer_append_all(buffer_t* buffer, FILE* input);
extern void buffer_write_file(buffer_t* bytes, char* file_name);
__attribute__((warn_unused_result)) extern buffer_t* buffer_read_until(buffer_t* buffer, FILE* input, char end_of_line);
extern buffer_t* buffer_read_ready_bytes(buffer_t* buffer, FILE* input, uint64_t max_bytes);
extern buffer_t* buffer_read_ready_bytes_file_number(buffer_t* buffer, int file_number, uint64_t max_bytes);
int file_peek_byte(FILE* input);
boolean_t file_eof(FILE* input);
void file_copy_stream(FILE* input, FILE* output, boolean_t until_eof, uint64_t size);
void file_skip_bytes(FILE* input, uint64_t n_bytes);
__attribute__((warn_unused_result)) extern buffer_t* term_clear_screen(buffer_t* buffer);
__attribute__((warn_unused_result)) extern buffer_t* term_set_foreground_color(buffer_t* buffer, uint32_t color);
__attribute__((warn_unused_result)) extern buffer_t* term_set_background_color(buffer_t* buffer, uint32_t color);
__attribute__((warn_unused_result)) extern buffer_t* term_move_cursor_absolute(buffer_t* buffer, int x, int y);
__attribute__((warn_unused_result)) extern buffer_t* term_move_cursor_relative(buffer_t* buffer, int x, int y);
__attribute__((warn_unused_result)) extern buffer_t* term_bold(buffer_t* buffer);
__attribute__((warn_unused_result)) extern buffer_t* term_dim(buffer_t* buffer);
__attribute__((warn_unused_result)) extern buffer_t* term_italic(buffer_t* buffer);
__attribute__((warn_unused_result)) extern buffer_t* term_underline(buffer_t* buffer);
__attribute__((warn_unused_result)) extern buffer_t* term_reset_formatting(buffer_t* buffer);
__attribute__((warn_unused_result)) extern buffer_t* term_draw_box(buffer_t* buffer, uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, box_drawing_t* box);
extern struct termios term_echo_off();
extern void term_echo_restore(struct termios oldt);
extern value_array_t* string_tokenize(const char* str, const char* delimiters);
extern value_array_t* buffer_tokenize(buffer_t* buffer, const char* delimiters);
extern value_array_t* tokenize_memory_range(uint8_t* start, uint64_t length, const char* delimiters);
void add_duplicate(value_array_t* token_array, const char* data);
extern random_state_t random_state_for_test(void);
extern uint64_t random_next_uint64(random_state_t* state);
extern uint64_t random_next_uint64_below(random_state_t* state, uint64_t maximum);
random_state_t* random_state(void);
uint64_t random_next(random_state_t* state);
cdl_printer_t* make_cdl_printer(buffer_t* buffer);
void cdl_boolean(cdl_printer_t* printer, boolean_t bolean);
void cdl_string(cdl_printer_t* printer, char* string);
void cdl_int64(cdl_printer_t* printer, int64_t number);
void cdl_uint64(cdl_printer_t* printer, uint64_t number);
void cdl_double(cdl_printer_t* printer, double number);
void cdl_start_array(cdl_printer_t* printer);
void cdl_end_array(cdl_printer_t* printer);
void cdl_start_table(cdl_printer_t* printer);
void cdl_key(cdl_printer_t* printer, char* key);
void cdl_end_table(cdl_printer_t* printer);
void cdl_indent(cdl_printer_t* printer);
boolean_t is_symbol(char* string);
void cdl_output_token(cdl_printer_t* printer, char* string);
sub_process_t* make_sub_process(value_array_t* argv);
boolean_t sub_process_launch(sub_process_t* sub_process);
uint64_t sub_process_write(sub_process_t* sub_process, buffer_t* data, uint64_t start_position);
void sub_process_close_stdin(sub_process_t* sub_process);
void sub_process_read(sub_process_t* sub_process, buffer_t* stdout, buffer_t* stderr);
void sub_process_wait(sub_process_t* sub_process);
void sub_process_record_exit_status(sub_process_t* sub_process, pid_t pid, int status);
boolean_t is_sub_process_running(sub_process_t* sub_process);
__attribute__((format(printf, 3, 4))) void test_fail_and_exit(char* file_name, int line_number, char* format, ...);
char* error_code_to_string(error_code_t value);
error_code_t string_to_error_code(char* value);
enum_metadata_t* error_code_metadata();
char* non_fatal_error_code_to_string(non_fatal_error_code_t value);
non_fatal_error_code_t string_to_non_fatal_error_code(char* value);
enum_metadata_t* non_fatal_error_code_metadata();
char* flag_type_to_string(flag_type_t value);
flag_type_t string_to_flag_type(char* value);
enum_metadata_t* flag_type_metadata();
char* sub_process_exit_status_to_string(sub_process_exit_status_t value);
sub_process_exit_status_t string_to_sub_process_exit_status(char* value);
enum_metadata_t* sub_process_exit_status_metadata();

// ========== inlined functions ==========

static inline boolean_t is_ok(value_result_t value){
  return ((value.nf_error)==NF_OK);
}

static inline boolean_t is_not_ok(value_result_t value){
  return ((value.nf_error)!=NF_OK);
}

static inline boolean_t should_log_memory_allocation(){
  if (is_initialized)
  {
    return should_log_value;
  }
  char* var = getenv("ARMYKNIFE_LOG_MEMORY_ALLOCATION");
  (is_initialized=true);
  if (((var!=NULL)&&(strcmp(var, "true")==0)))
  {
    (should_log_value=true);
  }
  return should_log_value;
}

static inline boolean_t is_hex_digit(char ch){
  return (((ch>='0')&&(ch<='9'))||((ch>='a')&&(ch<='f')));
}

static inline uint64_t hex_digit_to_value(char ch){
  if (((ch>='0')&&(ch<='9')))
  {
    return (ch-'0');
  }
  else
  {
    return ((ch-'a')+10);
  }
}

static inline uint64_t mix(uint64_t h){
  (h^=(h>>23));
  (h*=0x2127599bf4325c37ULL);
  (h^=(h>>47));
  return h;
}

static inline boolean_t should_log_info(){
  return ((global_logger_state.level)<=LOGGER_INFO);
}

static inline value_result_t alist_find(string_alist_t* list, char* key){
  return value_alist_find((/*CAST*/(value_alist_t*) list), cmp_string_values, str_to_value(key));
}

__attribute__((warn_unused_result)) static inline string_alist_t* alist_insert(string_alist_t* list, char* key, value_t value){
  return (/*CAST*/(string_alist_t*) value_alist_insert((/*CAST*/(value_alist_t*) list), cmp_string_values, str_to_value(key), value));
}

__attribute__((warn_unused_result)) static inline string_alist_t* alist_delete(string_alist_t* list, char* key){
  return (/*CAST*/(string_alist_t*) value_alist_delete((/*CAST*/(value_alist_t*) list), cmp_string_values, str_to_value(key)));
}

__attribute__((warn_unused_result)) static inline uint64_t alist_length(string_alist_t* list){
  return value_alist_length((/*CAST*/(value_alist_t*) list));
}

static inline uint64_t value_ht_num_entries(value_hashtable_t* ht){
  return (ht->n_entries);
}

static inline value_hashtable_t* to_value_hashtable(string_hashtable_t* ht){
  return (/*CAST*/(value_hashtable_t*) ht);
}

static inline string_hashtable_t* make_string_hashtable(uint64_t n_buckets){
  return (/*CAST*/(string_hashtable_t*) make_value_hashtable(n_buckets));
}

__attribute__((warn_unused_result)) static inline string_hashtable_t* string_ht_insert(string_hashtable_t* ht, char* key, value_t value){
  return (/*CAST*/(string_hashtable_t*) value_ht_insert(to_value_hashtable(ht), hash_string_value, cmp_string_values, str_to_value(key), value));
}

__attribute__((warn_unused_result)) static inline string_hashtable_t* string_ht_delete(string_hashtable_t* ht, char* key){
  return (/*CAST*/(string_hashtable_t*) value_ht_delete(to_value_hashtable(ht), hash_string_value, cmp_string_values, str_to_value(key)));
}

static inline value_result_t string_ht_find(string_hashtable_t* ht, char* key){
  return value_ht_find(to_value_hashtable(ht), hash_string_value, cmp_string_values, str_to_value(key));
}

static inline uint64_t string_ht_num_entries(string_hashtable_t* ht){
  return value_ht_num_entries(to_value_hashtable(ht));
}

static inline uint64_t value_tree_min_level(uint32_t a, uint32_t b){
  return ((a<b) ? a : b);
}

static inline boolean_t value_tree_is_leaf(value_tree_t* t){
  return (((t->left)==NULL)&&((t->right)==NULL));
}

static inline value_result_t string_tree_find(string_tree_t* t, char* key){
  return value_tree_find((/*CAST*/(value_tree_t*) t), cmp_string_values, str_to_value(key));
}

__attribute__((warn_unused_result)) static inline string_tree_t* string_tree_insert(string_tree_t* t, char* key, value_t value){
  return (/*CAST*/(string_tree_t*) value_tree_insert((/*CAST*/(value_tree_t*) t), cmp_string_values, str_to_value(key), value));
}

__attribute__((warn_unused_result)) static inline string_tree_t* string_tree_delete(string_tree_t* t, char* key){
  return (/*CAST*/(string_tree_t*) value_tree_delete((/*CAST*/(value_tree_t*) t), cmp_string_values, str_to_value(key)));
}

static inline uint64_t rotl(uint64_t x, int k){
  return ((x<<k)|(x>>(64-k)));
}

static inline void open_arena_for_test(void){
}

static inline void close_arena_for_test(void){
}



#endif /* _HEADER_FILE_GUARD_ */
