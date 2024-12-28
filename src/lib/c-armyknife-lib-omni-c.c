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

fatal_error_config_t fatal_error_config = {0};

boolean_t is_initialized = false;

boolean_t should_log_value = false;

uint64_t number_of_bytes_allocated = 0;

uint64_t number_of_malloc_calls = 0;

uint64_t number_of_free_calls = 0;

memory_hashtable_bucket_t memory_ht[ARMYKNIFE_MEMORY_ALLOCATION_HASHTABLE_SIZE];

logger_state_t global_logger_state;

program_descriptor_t* current_program;

command_descriptor_t* current_command;

flag_descriptor_t* current_flag;

random_state_t shared_random_state = {0};

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


// ========== functions ==========

/* i=0 j=1 */
unsigned encode_sleb_128(int64_t Value, uint8_t* p){
  uint8_t* orig_p = p;
  int More;
  do  {
    uint8_t Byte = (Value&0x7f);
    (Value>>=7);
    (More=(!(((Value==0)&&((Byte&0x40)==0))||((Value==(-1))&&((Byte&0x40)!=0)))));
    if (More)
    (Byte|=0x80);
    ((*(p++))=Byte);
  }
while (More);
  return (/*CAST*/(unsigned) (p-orig_p));
}

/* i=1 j=1 */
unsigned encode_uleb_128(uint64_t Value, uint8_t* p){
  uint8_t* orig_p = p;
  do  {
    uint8_t Byte = (Value&0x7f);
    (Value>>=7);
    if ((Value!=0))
    (Byte|=0x80);
    ((*(p++))=Byte);
  }
while ((Value!=0));
  return (/*CAST*/(unsigned) (p-orig_p));
}

/* i=2 j=1 */
unsigned_decode_result decode_uleb_128(const uint8_t* p, const uint8_t* end){
  const uint8_t* orig_p = p;
  uint64_t Value = 0;
  unsigned Shift = 0;
  do  {
    if ((p==end))
    {
      unsigned_decode_result result = ((unsigned_decode_result) {0, ERROR_INSUFFICIENT_INPUT});
      return result;
    }
    uint64_t Slice = ((*p)&0x7f);
    if ((((Shift>=64)&&(Slice!=0))||(((Slice<<Shift)>>Shift)!=Slice)))
    {
      unsigned_decode_result result = ((unsigned_decode_result) {0, ERROR_TOO_BIG});
      return result;
    }
    (Value+=(Slice<<Shift));
    (Shift+=7);
  }
while (((*(p++))>=128));
  unsigned_decode_result result = ((unsigned_decode_result) {Value, cast(unsigned, p - orig_p)});
  return result;
}

/* i=3 j=1 */
signed_decode_result decode_sleb_128(const uint8_t* p, const uint8_t* end){
  const uint8_t* orig_p = p;
  int64_t Value = 0;
  unsigned Shift = 0;
  uint8_t Byte;
  do  {
    if ((p==end))
    {
      signed_decode_result result = ((signed_decode_result) {0, ERROR_INSUFFICIENT_INPUT});
      return result;
    }
    (Byte=(*p));
    uint64_t Slice = (Byte&0x7f);
    if ((((Shift>=64)&&(Slice!=((Value<0) ? 0x7f : 0x00)))||(((Shift==63)&&(Slice!=0))&&(Slice!=0x7f))))
    {
      signed_decode_result result = ((signed_decode_result) {0, ERROR_TOO_BIG});
      return result;
    }
    (Value|=(Slice<<Shift));
    (Shift+=7);
    (++p);
  }
while ((Byte>=128));
  if (((Shift<64)&&(Byte&0x40)))
  (Value|=((-1ULL)<<Shift));
  signed_decode_result result = ((signed_decode_result) {Value, (p - orig_p)});
  return result;
}

/* i=4 j=1 */
_Noreturn void fatal_error_impl(char* file, int line, int error_code){
  print_fatal_error_banner();
  print_backtrace();
  fprintf(stderr, "%s:%d: FATAL ERROR %d", file, line, error_code);
  print_error_code_name(error_code);
  fprintf(stderr, "\nCommand line: %s\n\n", get_command_line());
  char* sleep_str = getenv("ARMYKNIFE_FATAL_ERROR_SLEEP_SECONDS");
  if ((sleep_str!=NULL))
  {
    value_result_t sleep_time = string_parse_uint64(sleep_str);
    if (is_ok(sleep_time))
    {
      fprintf(stderr, "Sleeping for %lu seconds so you can attach a debugger.\n", (sleep_time.u64));
      fprintf(stderr, "  gdb -tui %s %d\n", get_program_path(), getpid());
      sleep((sleep_time.u64));
    }
  }
  else
  {
    fprintf(stderr, "(ARMYKNIFE_FATAL_ERROR_SLEEP_SECONDS is not set)\n");
  }
  fprintf(stderr, "Necessaria Morte Mori...\n");
  exit((-(error_code+100)));
}

/* i=5 j=1 */
const char* fatal_error_code_to_string(int error_code){
  switch (error_code)
  {
    case ERROR_UKNOWN:
    return "ERROR_UKNOWN";
    case ERROR_MEMORY_ALLOCATION:
    return "ERROR_MEMORY_ALLOCATION";
    case ERROR_MEMORY_FREE_NULL:
    return "ERROR_MEMORY_FREE_NULL";
    case ERROR_REFERENCE_NOT_EXPECTED_TYPE:
    return "ERROR_REFERENCE_NOT_EXPECTED_TYPE";
    case ERROR_ILLEGAL_INITIAL_CAPACITY:
    return "ERROR_ILLEGAL_INITIAL_CAPACITY";
    case ERROR_DYNAMICALLY_SIZED_TYPE_ILLEGAL_IN_CONTAINER:
    return "ERROR_DYNAMICALLY_SIZED_TYPE_ILLEGAL_IN_CONTAINER";
    case ERROR_ACCESS_OUT_OF_BOUNDS:
    return "ERROR_ACCESS_OUT_OF_BOUNDS";
    case ERROR_NOT_REACHED:
    return "ERROR_NOT_REACHED";
    case ERROR_ILLEGAL_ZERO_HASHCODE_VALUE:
    return "ERROR_ILLEGAL_ZERO_HASHCODE_VALUE";
    case ERROR_UNIMPLEMENTED:
    return "ERROR_UNIMPLEMENTED";
    case ERROR_ILLEGAL_NULL_ARGUMENT:
    return "ERROR_ILLEGAL_NULL_ARGUMENT";
    case ERROR_ILLEGAL_ARGUMENT:
    return "ERROR_ILLEGAL_ARGUMENT";
    case ERROR_MEMORY_START_PADDING_ERROR:
    return "ERROR_MEMORY_START_PADDING_ERROR";
    case ERROR_MEMORY_END_PADDING_ERROR:
    return "ERROR_MEMORY_END_PADDING_ERROR";
    default:
    return "error";
  }
}

/* i=6 j=1 */
void configure_fatal_errors(fatal_error_config_t config){
  (fatal_error_config=config);
  if ((config.catch_sigsegv))
  {
    signal(SIGSEGV, segmentation_fault_handler);
  }
}

/* i=7 j=0 */
void segmentation_fault_handler(int signal_number){
  fatal_error(ERROR_SIGSEGV);
}

/* i=8 j=1 */
void print_fatal_error_banner(){
  fprintf(stderr, "\n========== FATAL_ERROR ==========\n");
}

/* i=9 j=1 */
void print_backtrace(){
  do  {
    void* array[10];
    int size = backtrace(array, 10);
    char** strings = backtrace_symbols(array, size);
    for (
      int i = 0;
      (i<size);
      (i++))
    {
      printf("#%d %s\n", i, (strings[i]));
    }
  }
while (0);
}

/* i=10 j=1 */
void print_error_code_name(int error_code){
  fprintf(stderr, " ");
  fprintf(stderr, "*** ");
  fprintf(stderr, "%s", fatal_error_code_to_string(error_code));
  fprintf(stderr, " ***\n");
}

/* i=11 j=0 */
char* get_command_line(){
  buffer_t* buffer = buffer_append_file_contents(make_buffer(1), "/proc/self/cmdline");
  buffer_replace_matching_byte(buffer, 0, ' ');
  return buffer_to_c_string(buffer);
}

/* i=12 j=0 */
char* get_program_path(){
  char buf[4096];
  int n = readlink("/proc/self/exe", buf, (sizeof(buf)));
  if ((n>0))
  {
    return string_duplicate(buf);
  }
  else
  {
    return "<program-path-unknown>";
  }
}

/* i=15 j=1 */
int cmp_string_values(value_t value1, value_t value2){
  return strcmp((value1.str), (value2.str));
}

/* i=16 j=1 */
uint64_t hash_string_value(value_t value1){
  return string_hash((value1.str));
}

/* i=17 j=1 */
uint8_t* checked_malloc(char* file, int line, uint64_t amount){
  if (((amount==0)||(amount>ARMYKNIFE_MEMORY_ALLOCATION_MAXIMUM_AMOUNT)))
  {
    fatal_error(ERROR_BAD_ALLOCATION_SIZE);
  }
  if (should_log_memory_allocation())
  {
    fprintf(stderr, "ALLOCATE %s:%d -- n_bytes=%lu already_allocated=%lu n_calls=%lu\n", file, line, amount, number_of_bytes_allocated, number_of_malloc_calls);
  }
  check_memory_hashtable_padding();
  uint64_t amount_with_padding = ((ARMYKNIFE_MEMORY_ALLOCATION_START_PADDING+amount)+ARMYKNIFE_MEMORY_ALLOCATION_END_PADDING);
  uint8_t* result = malloc(amount_with_padding);
  if ((result==NULL))
  {
    fatal_error_impl(file, line, ERROR_MEMORY_ALLOCATION);
  }
  if (should_log_memory_allocation())
  {
    fprintf(stderr, "ALLOCATE %s:%d -- %lu -- ptr=%lu\n", file, line, amount, (/*CAST*/(unsigned long) result));
  }
  memset(result, 0, amount_with_padding);
  track_padding(file, line, result, amount);
  (number_of_bytes_allocated+=amount);
  (number_of_malloc_calls++);
  return (result+ARMYKNIFE_MEMORY_ALLOCATION_START_PADDING);
}

/* i=18 j=1 */
uint8_t* checked_malloc_copy_of(char* file, int line, uint8_t* source, uint64_t amount){
  uint8_t* result = checked_malloc(file, line, amount);
  memcpy(result, source, amount);
  return result;
}

/* i=19 j=1 */
void checked_free(char* file, int line, void* pointer){
  if (should_log_memory_allocation())
  {
    fprintf(stderr, "DEALLOCATE %s:%d -- %lu\n", file, line, (/*CAST*/(uint64_t) pointer));
  }
  if ((pointer==NULL))
  {
    fatal_error_impl(file, line, ERROR_MEMORY_FREE_NULL);
  }
  check_memory_hashtable_padding();
  uint8_t* malloc_pointer = ((/*CAST*/(uint8_t*) pointer)-ARMYKNIFE_MEMORY_ALLOCATION_START_PADDING);
  untrack_padding(malloc_pointer);
  (number_of_free_calls++);
  free(malloc_pointer);
}

/* i=20 j=1 */
void check_memory_hashtable_padding(){
  for (
    int i = 0;
    (i<ARMYKNIFE_MEMORY_ALLOCATION_HASHTABLE_SIZE);
    (i++))
  {
    if ((((memory_ht[i]).malloc_address)!=0))
    {
      uint64_t malloc_start_address = ((memory_ht[i]).malloc_address);
      uint64_t malloc_size = ((memory_ht[i]).malloc_size);
      check_start_padding((/*CAST*/(uint8_t*) malloc_start_address));
      check_end_padding((/*CAST*/(uint8_t*) ((malloc_start_address+ARMYKNIFE_MEMORY_ALLOCATION_START_PADDING)+malloc_size)), ((memory_ht[i]).allocation_filename), ((memory_ht[i]).allocation_line_number));
    }
  }
}

/* i=22 j=0 */
void check_start_padding(uint8_t* address){
  for (
    int i = 0;
    (i<ARMYKNIFE_MEMORY_ALLOCATION_START_PADDING);
    (i++))
  {
    if (((address[i])!=START_PADDING_BYTE))
    {
      fatal_error(ERROR_MEMORY_START_PADDING_ERROR);
    }
  }
}

/* i=23 j=0 */
void check_end_padding(uint8_t* address, char* filename, uint64_t line){
  for (
    int i = 0;
    (i<ARMYKNIFE_MEMORY_ALLOCATION_END_PADDING);
    (i++))
  {
    if (((address[i])!=END_PADDING_BYTE))
    {
      fprintf(stderr, "FATAL: someone clobbered past an allocation %lu. (allocated " "here: %s:%lu)\n", (/*CAST*/(uint64_t) address), filename, line);
      fatal_error(ERROR_MEMORY_END_PADDING_ERROR);
    }
  }
}

/* i=24 j=0 */
uint64_t mumurhash64_mix(uint64_t h){
  (h*=(h>>33));
  (h*=0xff51afd7ed558ccdL);
  (h*=(h>>33));
  (h*=0xc4ceb9fe1a85ec53L);
  (h*=(h>>33));
  return h;
}

/* i=25 j=0 */
void track_padding(char* file, int line, uint8_t* address, uint64_t amount){
  for (
    int i = 0;
    (i<ARMYKNIFE_MEMORY_ALLOCATION_START_PADDING);
    (i++))
  {
    ((address[i])=START_PADDING_BYTE);
  }
  uint8_t* end_padding_address = ((address+amount)+ARMYKNIFE_MEMORY_ALLOCATION_START_PADDING);
  for (
    int i = 0;
    (i<ARMYKNIFE_MEMORY_ALLOCATION_END_PADDING);
    (i++))
  {
    ((end_padding_address[i])=END_PADDING_BYTE);
  }
  if ((ARMYKNIFE_MEMORY_ALLOCATION_HASHTABLE_SIZE>0))
  {
    int bucket = (mumurhash64_mix((/*CAST*/(uint64_t) address))%ARMYKNIFE_MEMORY_ALLOCATION_HASHTABLE_SIZE);
    (((memory_ht[bucket]).malloc_address)=(/*CAST*/(uint64_t) address));
    (((memory_ht[bucket]).malloc_size)=amount);
    (((memory_ht[bucket]).allocation_filename)=file);
    (((memory_ht[bucket]).allocation_line_number)=line);
  }
}

/* i=26 j=0 */
void untrack_padding(uint8_t* malloc_address){
  check_start_padding(malloc_address);
  if ((ARMYKNIFE_MEMORY_ALLOCATION_HASHTABLE_SIZE>0))
  {
    int bucket = (mumurhash64_mix((/*CAST*/(uint64_t) malloc_address))%ARMYKNIFE_MEMORY_ALLOCATION_HASHTABLE_SIZE);
    (((memory_ht[bucket]).malloc_address)=0);
    (((memory_ht[bucket]).malloc_size)=0);
    (((memory_ht[bucket]).allocation_filename)=0);
    (((memory_ht[bucket]).allocation_line_number)=0);
  }
}

/* i=27 j=1 */
int uint64_highest_bit_set(uint64_t n){
  if ((n>=(1ULL<<32)))
  {
    return (uint64_highest_bit_set((n>>32))+32);
  }
  else
  if ((n>=(1ULL<<16)))
  {
    return (uint64_highest_bit_set((n>>16))+16);
  }
  else
  if ((n>=(1ULL<<8)))
  {
    return (uint64_highest_bit_set((n>>8))+8);
  }
  else
  if ((n>=(1ULL<<4)))
  {
    return (uint64_highest_bit_set((n>>4))+4);
  }
  else
  if ((n>=(1ULL<<2)))
  {
    return (uint64_highest_bit_set((n>>2))+2);
  }
  else
  if ((n>=(1ULL<<1)))
  {
    return (uint64_highest_bit_set((n>>1))+1);
  }
  else
  {
    return 0;
  }
}

/* i=28 j=1 */
int string_is_null_or_empty(const char* str){
  return ((str==NULL)||(strlen(str)==0));
}

/* i=29 j=1 */
int string_equal(const char* str1, const char* str2){
  if (string_is_null_or_empty(str1))
  {
    return string_is_null_or_empty(str2);
  }
  return (strcmp(str1, str2)==0);
}

/* i=30 j=1 */
int string_starts_with(const char* str1, const char* str2){
  return (strncmp(str1, str2, strlen(str2))==0);
}

/* i=31 j=1 */
int string_ends_with(const char* str1, const char* str2){
  size_t len1 = strlen(str1);
  size_t len2 = strlen(str2);
  if ((len2>len1))
  {
    return 0;
  }
  return (strcmp((str1+(len1-len2)), str2)==0);
}

/* i=32 j=1 */
boolean_t string_contains_char(const char* str, char ch){
  return (string_index_of_char(str, ch)>=0);
}

/* i=33 j=1 */
int string_index_of_char(const char* str, char ch){
  if (string_is_null_or_empty(str))
  {
    return (-1);
  }
  int str_length = strlen(str);
  for (
    int i = 0;
    (i<str_length);
    (i++))
  {
    if (((str[i])==ch))
    {
      return i;
    }
  }
  return (-1);
}

/* i=34 j=1 */
char* uint64_to_string(uint64_t number){
  char buffer[32];
  sprintf(buffer, "%lu", number);
  return string_duplicate(buffer);
}

/* i=35 j=1 */
uint64_t string_hash(const char* str){
  return fasthash64(str, strlen(str), 0);
}

/* i=36 j=1 */
char* string_substring(const char* str, int start, int end){
  uint64_t len = strlen(str);
  if ((((start>=len)||(start>=end))||(end<start)))
  {
    fatal_error(ERROR_ILLEGAL_ARGUMENT);
  }
  int result_size = ((end-start)+1);
  char* result = (/*CAST*/(char*) malloc_bytes(result_size));
  for (
    int i = start;
    (i<end);
    (i++))
  {
    ((result[(i-start)])=(str[i]));
  }
  ((result[(result_size-1)])='\0');
  return result;
}

/* i=37 j=1 */
value_result_t string_parse_uint64(const char* string){
  if (string_starts_with(string, "0x"))
  {
    return string_parse_uint64_hex((&(string[2])));
  }
  else
  if (string_starts_with(string, "0b"))
  {
    return string_parse_uint64_bin((&(string[2])));
  }
  else
  {
    return string_parse_uint64_dec(string);
  }
}

/* i=38 j=1 */
value_result_t string_parse_uint64_dec(const char* string){
  uint64_t len = strlen(string);
  uint64_t integer = 0;
  if ((len==0))
  {
    return ((value_result_t) {.u64 = 0, .nf_error = NF_ERROR_NOT_PARSED_AS_NUMBER});
  }
  for (
    int i = 0;
    (i<len);
    (i++))
  {
    char ch = (string[i]);
    if (((ch<'0')||(ch>'9')))
    {
      return ((value_result_t) {.u64 = 0, .nf_error = NF_ERROR_NOT_PARSED_AS_NUMBER});
    }
    uint64_t digit = ((string[i])-'0');
    (integer=((integer*10)+digit));
  }
  return ((value_result_t) {.u64 = integer, .nf_error = NF_OK});
}

/* i=39 j=1 */
value_result_t string_parse_uint64_hex(const char* string){
  uint64_t len = strlen(string);
  uint64_t integer = 0;
  if ((len==0))
  {
    return ((value_result_t) {.u64 = 0, .nf_error = NF_ERROR_NOT_PARSED_AS_NUMBER});
  }
  for (
    int i = 0;
    (i<len);
    (i++))
  {
    char ch = (string[i]);
    if ((!is_hex_digit(ch)))
    {
      return ((value_result_t) {.u64 = 0, .nf_error = NF_ERROR_NOT_PARSED_AS_NUMBER});
    }
    uint64_t digit = hex_digit_to_value(ch);
    (integer=((integer<<4)|digit));
  }
  return ((value_result_t) {.u64 = integer, .nf_error = NF_OK});
}

/* i=40 j=1 */
value_result_t string_parse_uint64_bin(const char* string){
  uint64_t len = strlen(string);
  uint64_t integer = 0;
  if ((len==0))
  {
    return ((value_result_t) {.u64 = 0, .nf_error = NF_ERROR_NOT_PARSED_AS_NUMBER});
  }
  for (
    int i = 0;
    (i<len);
    (i++))
  {
    char ch = (string[i]);
    if (((ch<'0')||(ch>'1')))
    {
      return ((value_result_t) {.u64 = 0, .nf_error = NF_ERROR_NOT_PARSED_AS_NUMBER});
    }
    uint64_t digit = ((string[i])-'0');
    (integer=((integer<<1)|digit));
  }
  return ((value_result_t) {.u64 = integer, .nf_error = NF_OK});
}

/* i=41 j=1 */
char* string_duplicate(const char* src){
  if ((src==NULL))
  {
    return NULL;
  }
  int len = (strlen(src)+1);
  char* result = (/*CAST*/(char*) malloc_bytes(len));
  memcpy(result, src, len);
  return result;
}

/* i=42 j=1 */
char* string_append(const char* a, const char* b){
  if (((a==NULL)||(b==NULL)))
  {
    fatal_error(ERROR_ILLEGAL_NULL_ARGUMENT);
  }
  int total_length = ((strlen(a)+strlen(b))+1);
  char* result = (/*CAST*/(char*) malloc_bytes(total_length));
  strcat(result, a);
  strcat(result, b);
  return result;
}

/* i=43 j=1 */
char* string_left_pad(const char* str, int n, char ch){
  if ((n<0))
  {
    fatal_error(ERROR_ILLEGAL_RANGE);
  }
  int input_length = strlen(str);
  int padding_needed = (n-input_length);
  int len = 1;
  buffer_t* buffer = make_buffer(len);
  for (
    int i = 0;
    (i<padding_needed);
    (i++))
  {
    (buffer=buffer_append_byte(buffer, ch));
  }
  (buffer=buffer_append_string(buffer, str));
  char* result = buffer_to_c_string(buffer);
  free_bytes(buffer);
  return result;
}

/* i=44 j=1 */
char* string_right_pad(const char* str, int n, char ch){
  if ((n<0))
  {
    fatal_error(ERROR_ILLEGAL_RANGE);
  }
  int input_length = strlen(str);
  int padding_needed = (n-input_length);
  int len = 1;
  buffer_t* buffer = make_buffer(len);
  (buffer=buffer_append_string(buffer, str));
  for (
    int i = 0;
    (i<padding_needed);
    (i++))
  {
    (buffer=buffer_append_byte(buffer, ch));
  }
  char* result = buffer_to_c_string(buffer);
  free_bytes(buffer);
  return result;
}

/* i=45 j=1 */
__attribute__((format(printf, 1, 2))) char* string_printf(char* format, ...){
  char buffer[STRING_PRINTF_INITIAL_BUFFER_SIZE];
  int n_bytes = 0;
  do  {
    va_list args;
    va_start(args, format);
    (n_bytes=vsnprintf(buffer, STRING_PRINTF_INITIAL_BUFFER_SIZE, format, args));
    va_end(args);
  }
while (0);
  if ((n_bytes<STRING_PRINTF_INITIAL_BUFFER_SIZE))
  {
    char* result = (/*CAST*/(char*) malloc_bytes((n_bytes+1)));
    strcat(result, buffer);
    return result;
  }
  else
  {
    char* result = (/*CAST*/(char*) malloc_bytes((n_bytes+1)));
    va_list args;
    va_start(args, format);
    int n_bytes_second = vsnprintf(result, (n_bytes+1), format, args);
    va_end(args);
    if ((n_bytes_second!=n_bytes))
    {
      fatal_error(ERROR_INTERNAL_ASSERTION_FAILURE);
    }
    return result;
  }
}

/* i=46 j=1 */
char* string_truncate(char* str, int limit, char* at_limit_suffix){
  buffer_t* buffer = make_buffer(limit);
  for (
    int i = 0;
    ;
    (i++))
  {
    char ch = (str[i]);
    if ((ch=='\0'))
    {
      char* result = buffer_to_c_string(buffer);
      free_bytes(buffer);
      return result;
    }
    (buffer=buffer_append_byte(buffer, ch));
  }
  if (at_limit_suffix)
  {
    (buffer=buffer_append_string(buffer, at_limit_suffix));
  }
  char* result = buffer_to_c_string(buffer);
  free_bytes(buffer);
  return result;
}

/* i=47 j=1 */
uint64_t fasthash64(const void* buf, size_t len, uint64_t seed){
  const uint64_t m = 0x880355f21e6d1965ULL;
  const uint64_t* pos = (/*CAST*/(const uint64_t*) buf);
  const uint64_t* end = (pos+(len/8));
  const unsigned char* pos2;
  uint64_t h = (seed^(len*m));
  uint64_t v;
  while ((pos!=end))
  {
    (v=(*(pos++)));
    (h^=mix(v));
    (h*=m);
  }
  (pos2=(/*CAST*/(const unsigned char*) pos));
  (v=0);
  switch ((len&7))
  {
    case 7:
    (v^=((/*CAST*/(uint64_t) (pos2[6]))<<48));
    case 6:
    (v^=((/*CAST*/(uint64_t) (pos2[5]))<<40));
    case 5:
    (v^=((/*CAST*/(uint64_t) (pos2[4]))<<32));
    case 4:
    (v^=((/*CAST*/(uint64_t) (pos2[3]))<<24));
    case 3:
    (v^=((/*CAST*/(uint64_t) (pos2[2]))<<16));
    case 2:
    (v^=((/*CAST*/(uint64_t) (pos2[1]))<<8));
    case 1:
    (v^=(/*CAST*/(uint64_t) (pos2[0])));
    (h^=mix(v));
    (h*=m);
  }
  return mix(h);
}

/* i=51 j=1 */
void logger_init(void){
  char* level_string = getenv("ARMYKNIFE_LIB_LOG_LEVEL");
  if ((level_string!=NULL))
  {
    value_result_t parsed = string_parse_uint64(level_string);
    if (is_ok(parsed))
    {
      ((global_logger_state.level)=(parsed.u64));
    }
    else
    {
      value_result_t parsed = parse_log_level_enum(level_string);
      if (is_ok(parsed))
      {
        ((global_logger_state.level)=(parsed.u64));
      }
      else
      {
        log_warn("%s could not be converted to a log level.", level_string);
      }
    }
  }
  fprintf(stderr, "Log level is set to %s (%d)\n", logger_level_to_string((global_logger_state.level)), (global_logger_state.level));
  char* output_file_name = getenv("ARMYKNIFE_LIB_LOG_FILE");
  if ((output_file_name!=NULL))
  {
    ((global_logger_state.output)=fopen(output_file_name, "w"));
    if ((!(global_logger_state.output)))
    {
      fatal_error(ERROR_OPEN_LOG_FILE);
    }
    ((global_logger_state.logger_output_filename)=output_file_name);
  }
  else
  {
    ((global_logger_state.output)=stderr);
    ((global_logger_state.initialized)=true);
  }
}

/* i=52 j=1 */
__attribute__((format(printf, 5, 6))) void logger_impl(char* file, int line_number, const char* function, int level, char* format, ...){
  FILE* output = (global_logger_state.output);
  if ((output==NULL))
  {
    (output=stderr);
  }
  if ((level>=(global_logger_state.level)))
  {
    fprintf(output, "%s ", logger_level_to_string(level));
    va_list args;
    fprintf(output, "%s:%d %s | ", file, line_number, function);
    va_start(args, format);
    vfprintf(output, format, args);
    va_end(args);
    fprintf(output, "\n");
  }
}

/* i=54 j=0 */
value_result_t parse_log_level_enum(char* str){
  if (((strcmp("FATAL", str)==0)||(strcmp("fatal", str)==0)))
  {
    return ((value_result_t) {.u64 = LOGGER_FATAL});
  }
  else
  if (((strcmp("WARN", str)==0)||(strcmp("warn", str)==0)))
  {
    return ((value_result_t) {.u64 = LOGGER_WARN});
  }
  else
  if (((strcmp("INFO", str)==0)||(strcmp("info", str)==0)))
  {
    return ((value_result_t) {.u64 = LOGGER_INFO});
  }
  else
  if (((strcmp("DEBUG", str)==0)||(strcmp("debug", str)==0)))
  {
    return ((value_result_t) {.u64 = LOGGER_DEBUG});
  }
  else
  if (((strcmp("TRACE", str)==0)||(strcmp("trace", str)==0)))
  {
    return ((value_result_t) {.u64 = LOGGER_TRACE});
  }
  else
  if (((strcmp("OFF", str)==0)||(strcmp("off", str)==0)))
  {
    return ((value_result_t) {.u64 = LOGGER_OFF});
  }
  else
  {
    return ((value_result_t) {.nf_error = NF_ERROR_NOT_PARSED_AS_EXPECTED_ENUM});
  }
}

/* i=55 j=1 */
char* logger_level_to_string(int level){
  switch (level)
  {
    case LOGGER_OFF:
    return "LOGGER_OFF";
    case LOGGER_TRACE:
    return "TRACE";
    case LOGGER_DEBUG:
    return "DEBUG";
    case LOGGER_INFO:
    return "INFO";
    case LOGGER_WARN:
    return "WARN";
    case LOGGER_FATAL:
    return "FATAL";
    default:
    return "LEVEL_UNKNOWN";
  }
}

/* i=56 j=1 */
utf8_decode_result_t utf8_decode(const uint8_t* array){
  uint8_t firstByte = (array[0]);
  if (((firstByte&0x80)==0))
  {
    return ((utf8_decode_result_t) {.code_point = firstByte, .num_bytes = 1});
  }
  else
  if (((firstByte&0xE0)==0xC0))
  {
    return ((utf8_decode_result_t) {.code_point = ((firstByte & 0x1F) << 6) | (array[1] & 0x3F),
         .num_bytes = 2});
  }
  else
  if (((firstByte&0xF0)==0xE0))
  {
    return ((utf8_decode_result_t) {.code_point = ((firstByte & 0x0F) << 12)
                                           | ((array[1] & 0x3F) << 6)
                                           | (array[2] & 0x3F),
                             .num_bytes = 3});
  }
  else
  if (((firstByte&0xF8)==0xF0))
  {
    return ((utf8_decode_result_t) {.code_point = ((firstByte & 0x07) << 18) | ((array[1] & 0x3F) << 12)
                       | ((array[2] & 0x3F) << 6) | (array[3] & 0x3F),
         .num_bytes = 4});
  }
  else
  {
    return ((utf8_decode_result_t) {.error = true});
  }
}

/* i=57 j=1 */
buffer_t* make_buffer(uint64_t initial_capacity){
  buffer_t* result = malloc_struct(buffer_t);
  if ((initial_capacity<16))
  {
    (initial_capacity=16);
  }
  if ((initial_capacity>0))
  {
    ((result->capacity)=initial_capacity);
    ((result->elements)=malloc_bytes(initial_capacity));
  }
  return result;
}

/* i=58 j=1 */
uint64_t buffer_length(buffer_t* array){
  return (array->length);
}

/* i=59 j=1 */
uint8_t buffer_get(buffer_t* buffer, uint64_t position){
  if ((position<(buffer->length)))
  {
    return ((buffer->elements)[position]);
  }
  else
  {
    fatal_error(ERROR_ACCESS_OUT_OF_BOUNDS);
    return 0;
  }
}

/* i=60 j=1 */
char* buffer_c_substring(buffer_t* buffer, uint64_t start, uint64_t end){
  if ((buffer==NULL))
  {
    fatal_error(ERROR_ILLEGAL_NULL_ARGUMENT);
  }
  if ((start>end))
  {
    fatal_error(ERROR_ILLEGAL_RANGE);
  }
  uint64_t copy_length = (end-start);
  char* result = (/*CAST*/(char*) malloc_bytes((copy_length+1)));
  if ((copy_length>0))
  {
    memcpy(result, (&((buffer->elements)[start])), copy_length);
  }
  ((result[copy_length])='\0');
  return result;
}

/* i=61 j=1 */
char* buffer_to_c_string(buffer_t* buffer){
  return buffer_c_substring(buffer, 0, (buffer->length));
}

/* i=62 j=1 */
void buffer_clear(buffer_t* buffer){
  for (
    int i = 0;
    (i<(buffer->capacity));
    (i++))
  {
    (((buffer->elements)[i])=0);
  }
  ((buffer->length)=0);
}

/* i=63 j=1 */
extern buffer_t* buffer_increase_capacity(buffer_t* buffer, uint64_t capacity){
  if (((buffer->capacity)<capacity))
  {
    uint8_t* new_elements = malloc_bytes(capacity);
    memcpy(new_elements, (buffer->elements), (buffer->length));
    free_bytes((buffer->elements));
    ((buffer->elements)=new_elements);
    ((buffer->capacity)=capacity);
  }
  return buffer;
}

/* i=64 j=1 */
buffer_t* buffer_append_byte(buffer_t* buffer, uint8_t element){
  if (((buffer->length)<(buffer->capacity)))
  {
    (((buffer->elements)[(buffer->length)])=element);
    ((buffer->length)++);
    return buffer;
  }
  (buffer=buffer_increase_capacity(buffer, ((buffer->capacity)*2)));
  return buffer_append_byte(buffer, element);
}

/* i=65 j=1 */
buffer_t* buffer_append_bytes(buffer_t* buffer, uint8_t* bytes, uint64_t n_bytes){
  for (
    int i = 0;
    (i<n_bytes);
    (i++))
  {
    (buffer=buffer_append_byte(buffer, (bytes[i])));
  }
  return buffer;
}

/* i=66 j=1 */
extern buffer_t* buffer_append_buffer(buffer_t* buffer, buffer_t* src_buffer){
  return buffer_append_sub_buffer(buffer, 0, (src_buffer->length), src_buffer);
}

/* i=67 j=1 */
extern buffer_t* buffer_append_sub_buffer(buffer_t* buffer, uint64_t start_position, uint64_t end_position, buffer_t* src_buffer){
  if ((buffer==src_buffer))
  {
    fatal_error(ERROR_ILLEGAL_STATE);
  }
  for (
    uint64_t position = start_position;
    (position<end_position);
    (position++))
  {
    (buffer=buffer_append_byte(buffer, buffer_get(src_buffer, position)));
  }
  return buffer;
}

/* i=68 j=1 */
buffer_t* buffer_append_string(buffer_t* buffer, const char* str){
  return buffer_append_bytes(buffer, (/*CAST*/(uint8_t*) str), strlen(str));
}

/* i=69 j=1 */
__attribute__((format(printf, 2, 3))) buffer_t* buffer_printf(buffer_t* buffer, char* format, ...){
  char cbuffer[BUFFER_PRINTF_INITIAL_BUFFER_SIZE];
  int n_bytes = 0;
  do  {
    va_list args;
    va_start(args, format);
    (n_bytes=vsnprintf(cbuffer, BUFFER_PRINTF_INITIAL_BUFFER_SIZE, format, args));
    va_end(args);
  }
while (0);
  if ((n_bytes<BUFFER_PRINTF_INITIAL_BUFFER_SIZE))
  {
    return buffer_append_string(buffer, cbuffer);
  }
  else
  {
    char* result = (/*CAST*/(char*) malloc_bytes((n_bytes+1)));
    va_list args;
    va_start(args, format);
    int n_bytes_second = vsnprintf(result, (n_bytes+1), format, args);
    va_end(args);
    if ((n_bytes_second!=n_bytes))
    {
      fatal_error(ERROR_INTERNAL_ASSERTION_FAILURE);
    }
    (buffer=buffer_append_string(buffer, result));
    free_bytes(result);
    return buffer;
  }
}

/* i=70 j=1 */
extern buffer_t* buffer_append_repeated_byte(buffer_t* buffer, uint8_t byte, int count){
  for (
    int i = 0;
    (i<count);
    (i++))
  {
    (buffer=buffer_append_byte(buffer, byte));
  }
  return buffer;
}

/* i=71 j=1 */
utf8_decode_result_t buffer_utf8_decode(buffer_t* buffer, uint64_t position){
  if ((position>=(buffer->length)))
  {
    return ((utf8_decode_result_t) {.error = true});
  }
  utf8_decode_result_t result = utf8_decode((&((buffer->elements)[position])));
  if ((result.error))
  {
    return result;
  }
  if (((position+(result.num_bytes))>(buffer->length)))
  {
    return ((utf8_decode_result_t) {.error = true});
  }
  return result;
}

/* i=72 j=1 */
extern buffer_t* buffer_append_code_point(buffer_t* buffer, uint32_t code_point){
  if ((code_point<0x80))
  {
    (buffer=buffer_append_byte(buffer, code_point));
    return buffer;
  }
  else
  if ((code_point<0x800))
  {
    (buffer=buffer_append_byte(buffer, (0xc0|(code_point>>6))));
    (buffer=buffer_append_byte(buffer, (0x80|(code_point&0x3f))));
    return buffer;
  }
  else
  if ((code_point<0x10000))
  {
    (buffer=buffer_append_byte(buffer, (0xe0|(code_point>>12))));
    (buffer=buffer_append_byte(buffer, (0x80|((code_point>>6)&0x3f))));
    (buffer=buffer_append_byte(buffer, (0x80|(code_point&0x3f))));
    return buffer;
  }
  else
  if ((code_point<=0x10FFFF))
  {
    (buffer=buffer_append_byte(buffer, (0xf0|(code_point>>18))));
    (buffer=buffer_append_byte(buffer, (0x80|((code_point>>12)&0x3f))));
    (buffer=buffer_append_byte(buffer, (0x80|((code_point>>6)&0x3f))));
    (buffer=buffer_append_byte(buffer, (0x80|(code_point&0x3f))));
    return buffer;
  }
  else
  {
    fatal_error(ERROR_ILLEGAL_UTF_8_CODE_POINT);
    return 0;
  }
}

/* i=73 j=1 */
boolean_t buffer_match_string_at(buffer_t* buffer, uint64_t start_position, char* str){
  for (
    uint64_t pos = start_position;
    true;
    (pos++))
  {
    uint8_t byte_str = ((/*CAST*/(uint8_t*) str)[(pos-start_position)]);
    if ((byte_str==0))
    {
      return true;
    }
    if ((pos>=buffer_length(buffer)))
    {
      return false;
    }
    uint8_t byte_buf = buffer_get(buffer, pos);
    if ((byte_str!=byte_buf))
    {
      return false;
    }
  }
  return false;
}

/* i=74 j=1 */
buffer_t* buffer_from_string(char* string){
  buffer_t* result = make_buffer(strlen(string));
  (result=buffer_append_string(result, string));
  return result;
}

/* i=75 j=1 */
buffer_t* buffer_adjust_region(buffer_t* buffer, uint64_t start, uint64_t end, uint64_t new_width){
  if ((start>end))
  {
    fatal_error(ERROR_ILLEGAL_RANGE);
  }
  uint64_t original_width = (end-start);
  if ((original_width>new_width))
  {
    uint64_t difference = (original_width-new_width);
    uint64_t tail_length = ((buffer->length)-end);
    memmove((&((buffer->elements)[(end-difference)])), (&((buffer->elements)[end])), tail_length);
    ((buffer->length)-=difference);
  }
  else
  if ((original_width<new_width))
  {
    uint64_t difference = (new_width-original_width);
    uint64_t tail_length = ((buffer->length)-end);
    (buffer=buffer_increase_capacity(buffer, ((buffer->length)+difference)));
    memmove((&((buffer->elements)[(end+difference)])), (&((buffer->elements)[end])), tail_length);
    ((buffer->length)+=difference);
  }
  return buffer;
}

/* i=76 j=1 */
buffer_t* buffer_replace_all(buffer_t* buffer, char* original_text, char* replacement_text){
  int len_original = strlen(original_text);
  int len_replacement = strlen(replacement_text);
  if ((len_original<(buffer->length)))
  {
    uint64_t pos = 0;
    while ((pos<=((buffer->length)-len_original)))
    {
      if (buffer_match_string_at(buffer, pos, original_text))
      {
        (buffer=buffer_adjust_region(buffer, pos, (pos+len_original), len_replacement));
        memmove((&((buffer->elements)[pos])), replacement_text, len_replacement);
        (pos+=len_replacement);
      }
      else
      {
        (pos++);
      }
    }
  }
  return buffer;
}

/* i=77 j=1 */
buffer_t* buffer_replace_matching_byte(buffer_t* buffer, uint8_t original, uint8_t replacement){
  for (
    int i = 0;
    (i<(buffer->length));
    (i++))
  {
    if ((((buffer->elements)[i])==original))
    {
      (((buffer->elements)[i])=replacement);
    }
  }
  return buffer;
}

/* i=78 j=1 */
boolean_t buffer_region_contains(buffer_t* buffer, uint64_t start, uint64_t end, char* text){
  for (
    int i = start;
    (i<end);
    (i++))
  {
    if (buffer_match_string_at(buffer, i, text))
    {
      return true;
    }
  }
  return false;
}

/* i=79 j=1 */
uint64_t buffer_beginning_of_line(buffer_t* buffer, uint64_t start){
  uint64_t position = start;
  while ((position>0))
  {
    (position--);
    if ((buffer_get(buffer, position)=='\n'))
    {
      return (position+1);
    }
  }
  return position;
}

/* i=80 j=1 */
uint64_t buffer_end_of_line(buffer_t* buffer, uint64_t start){
  uint64_t position = start;
  while (((position<(buffer->length))&&(buffer_get(buffer, position)!='\n')))
  {
    (position++);
  }
  return position;
}

/* i=81 j=1 */
buffer_t* buffer_to_uppercase(buffer_t* buffer){
  for (
    uint64_t i = 0;
    (i<(buffer->length));
    (i++))
  {
    (((buffer->elements)[i])=toupper(((buffer->elements)[i])));
  }
  return buffer;
}

/* i=82 j=1 */
buffer_t* buffer_to_lowercase(buffer_t* buffer){
  for (
    uint64_t i = 0;
    (i<(buffer->length));
    (i++))
  {
    (((buffer->elements)[i])=tolower(((buffer->elements)[i])));
  }
  return buffer;
}

/* i=83 j=1 */
line_and_column_t buffer_position_to_line_and_column(buffer_t* buffer, uint64_t position){
  uint64_t line = 1;
  uint64_t column = 1;
  for (
    uint64_t pos = 0;
    (pos<position);
    (pos++))
  {
    uint8_t ch = buffer_get(buffer, pos);
    if ((ch=='\n'))
    {
      (line++);
      (column=1);
    }
    else
    {
      (column++);
    }
  }
  return ((line_and_column_t) {
                                                 .line = line,
                                                 .column = column,
                                             });
}

/* i=84 j=1 */
value_array_t* make_value_array(uint64_t initial_capacity){
  if ((initial_capacity==0))
  {
    (initial_capacity=1);
  }
  value_array_t* result = malloc_struct(value_array_t);
  ((result->capacity)=initial_capacity);
  ((result->elements)=(/*CAST*/(value_t*) malloc_bytes(((sizeof(value_t))*initial_capacity))));
  return result;
}

/* i=85 j=1 */
value_t value_array_get(value_array_t* array, uint32_t index){
  if ((index<(array->length)))
  {
    return ((array->elements)[index]);
  }
  fatal_error(ERROR_ACCESS_OUT_OF_BOUNDS);
  return ((value_t) {0});
}

/* i=86 j=1 */
void value_array_replace(value_array_t* array, uint32_t index, value_t element){
  if ((index<(array->length)))
  {
    (((array->elements)[index])=element);
    return;
  }
  fatal_error(ERROR_ACCESS_OUT_OF_BOUNDS);
}

/* i=87 j=1 */
void value_array_add(value_array_t* array, value_t element){
  value_array_ensure_capacity(array, ((array->length)+1));
  (((array->elements)[((array->length)++)])=element);
}

/* i=88 j=1 */
void value_array_push(value_array_t* array, value_t element){
  value_array_add(array, element);
}

/* i=89 j=1 */
value_t value_array_pop(value_array_t* array){
  if (((array->length)==0))
  {
    fatal_error(ERROR_ACCESS_OUT_OF_BOUNDS);
  }
  uint32_t last_index = ((array->length)-1);
  value_t result = value_array_get(array, last_index);
  (((array->elements)[last_index])=u64_to_value(0));
  ((array->length)--);
  return result;
}

/* i=90 j=1 */
void value_array_insert_at(value_array_t* array, uint32_t position, value_t element){
  if ((position==(array->length)))
  {
    value_array_add(array, element);
    return;
  }
  if ((position>(array->length)))
  {
    fatal_error(ERROR_ACCESS_OUT_OF_BOUNDS);
    return;
  }
  value_array_ensure_capacity(array, ((array->length)+1));
  for (
    int64_t i = ((array->length)-1);
    (i>=position);
    (i--))
  {
    (((array->elements)[(i+1)])=((array->elements)[i]));
  }
  ((array->length)++);
  (((array->elements)[position])=element);
}

/* i=91 j=1 */
value_t value_array_delete_at(value_array_t* array, uint32_t position){
  value_t result = value_array_get(array, position);
  for (
    int i = position;
    (i<((array->length)-1));
    (i++))
  {
    (((array->elements)[i])=((array->elements)[(i+1)]));
  }
  ((array->length)--);
  return result;
}

/* i=92 j=0 */
void value_array_ensure_capacity(value_array_t* array, uint32_t required_capacity){
  if (((array->capacity)<required_capacity))
  {
    uint32_t new_capacity = ((array->capacity)*2);
    if ((new_capacity<required_capacity))
    {
      (new_capacity=required_capacity);
    }
    value_t* new_elements = (/*CAST*/(value_t*) malloc_bytes(((sizeof(value_t))*new_capacity)));
    for (
      int i = 0;
      (i<(array->length));
      (i++))
    {
      ((new_elements[i])=((array->elements)[i]));
    }
    ((array->capacity)=new_capacity);
    free_bytes((array->elements));
    ((array->elements)=new_elements);
    return;
  }
}

/* i=93 j=1 */
value_result_t value_alist_find(value_alist_t* list, value_comparison_fn cmp_fn, value_t key){
  while (list)
  {
    if ((cmp_fn(key, (list->key))==0))
    {
      return ((value_result_t) {.val = list->value});
    }
    (list=(list->next));
  }
  return ((value_result_t) {.nf_error = NF_ERROR_NOT_FOUND});
}

/* i=94 j=1 */
value_alist_t* value_alist_insert(value_alist_t* list, value_comparison_fn cmp_fn, value_t key, value_t value){
  value_alist_t* result = malloc_struct(value_alist_t);
  ((result->next)=value_alist_delete(list, cmp_fn, key));
  ((result->key)=key);
  ((result->value)=value);
  return result;
}

/* i=95 j=1 */
value_alist_t* value_alist_delete(value_alist_t* list, value_comparison_fn cmp_fn, value_t key){
  if ((list==NULL))
  {
    return list;
  }
  if (((*cmp_fn)(key, (list->key))==0))
  {
    value_alist_t* result = (list->next);
    free_bytes(list);
    return result;
  }
  ((list->next)=value_alist_delete((list->next), cmp_fn, key));
  return list;
}

/* i=96 j=1 */
__attribute__((warn_unused_result)) extern uint64_t value_alist_length(value_alist_t* list){
  uint64_t result = 0;
  while (list)
  {
    (result++);
    (list=(list->next));
  }
  return result;
}

/* i=101 j=1 */
value_hashtable_t* make_value_hashtable(uint64_t n_buckets){
  if ((n_buckets<2))
  {
    (n_buckets=2);
  }
  value_hashtable_t* result = malloc_struct(value_hashtable_t);
  ((result->n_buckets)=n_buckets);
  ((result->buckets)=(/*CAST*/(value_alist_t**) malloc_bytes(((sizeof(typeof(value_alist_t*)))*n_buckets))));
  return result;
}

/* i=102 j=1 */
value_hashtable_t* value_ht_insert(value_hashtable_t* ht, value_hash_fn hash_fn, value_comparison_fn cmp_fn, value_t key, value_t value){
  uint64_t hashcode = hash_fn(key);
  int bucket = (hashcode%(ht->n_buckets));
  value_alist_t* list = ((ht->buckets)[bucket]);
  uint64_t len = value_alist_length(list);
  (list=value_alist_insert(list, cmp_fn, key, value));
  (((ht->buckets)[bucket])=list);
  uint64_t len_after = value_alist_length(list);
  if ((len_after>len))
  {
    ((ht->n_entries)++);
    if (((ht->n_entries)>=((ht->n_buckets)*ARMYKNIFE_HT_LOAD_FACTOR)))
    {
      value_hashtable_upsize_internal(ht, hash_fn, cmp_fn);
    }
  }
  return ht;
}

/* i=103 j=1 */
value_hashtable_t* value_ht_delete(value_hashtable_t* ht, value_hash_fn hash_fn, value_comparison_fn cmp_fn, value_t key){
  uint64_t hashcode = hash_fn(key);
  int bucket = (hashcode%(ht->n_buckets));
  value_alist_t* list = ((ht->buckets)[bucket]);
  uint64_t len = value_alist_length(list);
  (list=value_alist_delete(list, cmp_fn, key));
  (((ht->buckets)[bucket])=list);
  uint64_t len_after = value_alist_length(list);
  if ((len_after<len))
  {
    ((ht->n_entries)--);
  }
  return ht;
}

/* i=104 j=1 */
value_result_t value_ht_find(value_hashtable_t* ht, value_hash_fn hash_fn, value_comparison_fn cmp_fn, value_t key){
  uint64_t hashcode = hash_fn(key);
  int bucket = (hashcode%(ht->n_buckets));
  value_alist_t* list = ((ht->buckets)[bucket]);
  return value_alist_find(list, cmp_fn, key);
}

/* i=105 j=1 */
void value_hashtable_upsize_internal(value_hashtable_t* ht, value_hash_fn hash_fn, value_comparison_fn cmp_fn){
  uint64_t new_num_buckets = ((ht->n_buckets)*AK_HT_UPSCALE_MULTIPLIER);
  value_hashtable_t* new_ht = make_value_hashtable(new_num_buckets);
  value_ht_foreach(ht, key, value,   {
    value_hashtable_t* should_be_result = value_ht_insert(new_ht, hash_fn, cmp_fn, key, value);
    if ((new_ht!=should_be_result))
    {
      fatal_error(ERROR_ILLEGAL_STATE);
    }
  }
);
  value_alist_t** old_buckets = (ht->buckets);
  ((ht->buckets)=(new_ht->buckets));
  ((ht->n_buckets)=(new_ht->n_buckets));
  ((ht->n_entries)=(new_ht->n_entries));
  free_bytes(old_buckets);
  free_bytes(new_ht);
}

/* i=113 j=1 */
value_result_t value_tree_find(value_tree_t* t, value_comparison_fn cmp_fn, value_t key){
  if ((t==NULL))
  {
    return ((value_result_t) {.nf_error = NF_ERROR_NOT_FOUND});
  }
  int cmp_result = cmp_fn(key, (t->key));
  if ((cmp_result<0))
  {
    return value_tree_find((t->left), cmp_fn, key);
  }
  else
  if ((cmp_result>0))
  {
    return value_tree_find((t->right), cmp_fn, key);
  }
  else
  {
    return ((value_result_t) {
                                                .val = t->value,
                                            });
  }
}

/* i=114 j=1 */
value_tree_t* value_tree_insert(value_tree_t* t, value_comparison_fn cmp_fn, value_t key, value_t value){
  if ((t==NULL))
  {
    return make_value_tree_leaf(key, value);
  }
  int cmp_result = cmp_fn(key, (t->key));
  if ((cmp_result<0))
  {
    ((t->left)=value_tree_insert((t->left), cmp_fn, key, value));
  }
  else
  if ((cmp_result>0))
  {
    ((t->right)=value_tree_insert((t->right), cmp_fn, key, value));
  }
  else
  {
    ((t->value)=value);
    return t;
  }
  (t=value_tree_skew(t));
  (t=value_tree_split(t));
  return t;
}

/* i=115 j=1 */
value_tree_t* value_tree_delete(value_tree_t* t, value_comparison_fn cmp_fn, value_t key){
  if ((t==NULL))
  {
    return t;
  }
  int cmp_result = cmp_fn(key, (t->key));
  if ((cmp_result<0))
  {
    ((t->left)=value_tree_delete((t->left), cmp_fn, key));
  }
  else
  if ((cmp_result>0))
  {
    ((t->right)=value_tree_delete((t->right), cmp_fn, key));
  }
  else
  {
    if (value_tree_is_leaf(t))
    {
      return NULL;
    }
    else
    if (((t->left)==NULL))
    {
      value_tree_t* L = value_tree_successor(t);
      ((t->key)=(L->key));
      ((t->value)=(L->value));
      ((t->right)=value_tree_delete((t->right), cmp_fn, (L->key)));
    }
    else
    {
      value_tree_t* L = value_tree_predecessor(t);
      ((t->key)=(L->key));
      ((t->value)=(L->value));
      ((t->left)=value_tree_delete((t->left), cmp_fn, (L->key)));
    }
  }
  (t=value_tree_decrease_level(t));
  (t=value_tree_skew(t));
  ((t->right)=value_tree_skew((t->right)));
  if (((t->right)!=NULL))
  {
    (((t->right)->right)=value_tree_skew(((t->right)->right)));
  }
  (t=value_tree_split(t));
  ((t->right)=value_tree_split((t->right)));
  return t;
}

/* i=116 j=0 */
value_tree_t* value_tree_skew(value_tree_t* t){
  if ((t==NULL))
  {
    return NULL;
  }
  if (((t->left)==NULL))
  {
    return t;
  }
  if ((((t->left)->level)==(t->level)))
  {
    value_tree_t* L = (t->left);
    ((t->left)=(L->right));
    ((L->right)=t);
    return L;
  }
  return t;
}

/* i=117 j=0 */
value_tree_t* value_tree_split(value_tree_t* t){
  if ((t==NULL))
  {
    return NULL;
  }
  if ((((t->right)==NULL)||(((t->right)->right)==NULL)))
  {
    return t;
  }
  if (((t->level)==(((t->right)->right)->level)))
  {
    value_tree_t* R = (t->right);
    ((t->right)=(R->left));
    ((R->left)=t);
    ((R->level)++);
    return R;
  }
  return t;
}

/* i=118 j=0 */
value_tree_t* make_value_tree_leaf(value_t key, value_t value){
  value_tree_t* result = malloc_struct(value_tree_t);
  ((result->level)=1);
  ((result->key)=key);
  ((result->value)=value);
  return result;
}

/* i=120 j=0 */
value_tree_t* value_tree_decrease_level(value_tree_t* t){
  if (((t->left)&&(t->right)))
  {
    uint32_t should_be = (value_tree_min_level(((t->left)->level), ((t->right)->level))+1);
    if ((should_be<(t->level)))
    {
      ((t->level)=should_be);
      if ((should_be<((t->right)->level)))
      {
        (((t->right)->level)=should_be);
      }
    }
  }
  return t;
}

/* i=121 j=0 */
value_tree_t* value_tree_predecessor(value_tree_t* t){
  (t=(t->left));
  while (((t->right)!=NULL))
  {
    (t=(t->right));
  }
  return t;
}

/* i=122 j=0 */
value_tree_t* value_tree_successor(value_tree_t* t){
  (t=(t->right));
  while (((t->left)!=NULL))
  {
    (t=(t->left));
  }
  return t;
}

/* i=127 j=1 */
void flag_program_name(char* name){
  (current_program=malloc_struct(program_descriptor_t));
  ((current_program->name)=name);
  (current_command=NULL);
  (current_flag=NULL);
}

/* i=128 j=1 */
void flag_description(char* description){
  if ((current_flag!=NULL))
  {
    ((current_flag->description)=description);
  }
  else
  if ((current_command!=NULL))
  {
    ((current_command->description)=description);
  }
  else
  if ((current_program!=NULL))
  {
    ((current_program->description)=description);
  }
  else
  {
    log_fatal("A current flag, program or command must be executed first");
    fatal_error(ERROR_ILLEGAL_STATE);
  }
}

/* i=129 j=1 */
void flag_file_args(value_array_t** write_back_file_args_ptr){
  if ((current_command!=NULL))
  {
    ((current_command->write_back_file_args_ptr)=write_back_file_args_ptr);
  }
  else
  if ((current_program!=NULL))
  {
    ((current_program->write_back_file_args_ptr)=write_back_file_args_ptr);
  }
  else
  {
    log_fatal("A current program or command must be executed first");
    fatal_error(ERROR_ILLEGAL_STATE);
  }
}

/* i=130 j=1 */
void flag_command(char* name, char** write_back_ptr){
  (current_command=malloc_struct(command_descriptor_t));
  ((current_command->name)=name);
  ((current_command->write_back_ptr)=write_back_ptr);
  (current_flag=NULL);
  ((current_program->commands)=string_tree_insert((current_program->commands), name, ptr_to_value(current_command)));
}

/* i=131 j=1 */
void flag_boolean(char* name, boolean_t* write_back_ptr){
  add_flag(name, write_back_ptr, flag_type_boolean);
}

/* i=132 j=1 */
void flag_string(char* name, char** write_back_ptr){
  add_flag(name, write_back_ptr, flag_type_string);
}

/* i=133 j=1 */
void flag_uint64(char* name, uint64_t* write_back_ptr){
  add_flag(name, write_back_ptr, flag_type_uint64);
}

/* i=134 j=1 */
void flag_int64(char* name, int64_t* write_back_ptr){
  add_flag(name, write_back_ptr, flag_type_int64);
}

/* i=135 j=1 */
void flag_double(char* name, double* write_back_ptr){
  add_flag(name, write_back_ptr, flag_type_double);
}

/* i=136 j=1 */
void flag_enum(char* name, int* write_back_ptr){
  add_flag(name, write_back_ptr, flag_type_enum);
  ((current_flag->enum_size)=((sizeof(int))*8));
}

/* i=137 j=1 */
void flag_enum_64(char* name, uint64_t* write_back_ptr){
  add_flag(name, write_back_ptr, flag_type_enum);
  ((current_flag->enum_size)=64);
}

/* i=138 j=1 */
void flag_enum_value(char* name, uint64_t value){
  if (((!current_flag)||((current_flag->flag_type)!=flag_type_enum)))
  {
    log_fatal("The current flag is not an enum type");
    fatal_error(ERROR_ILLEGAL_STATE);
  }
  ((current_flag->enum_values)=string_tree_insert((current_flag->enum_values), name, u64_to_value(value)));
}

/* i=139 j=1 */
void flag_alias(char* alias){
  if ((current_flag!=NULL))
  {
    if ((current_command!=NULL))
    {
      ((current_command->flags)=string_tree_insert((current_command->flags), alias, ptr_to_value(current_flag)));
    }
    else
    if ((current_program!=NULL))
    {
      ((current_program->flags)=string_tree_insert((current_program->flags), alias, ptr_to_value(current_flag)));
    }
    else
    {
      log_fatal("A current program or command must exist first");
      fatal_error(ERROR_ILLEGAL_STATE);
    }
  }
  else
  {
    log_fatal("A current flag must present to use flag_alias");
    fatal_error(ERROR_ILLEGAL_STATE);
  }
}

/* i=140 j=1 */
char* flag_parse_command_line(int argc, char** argv){
  if ((current_program==NULL))
  {
    log_fatal("flag_parse_command_line can't be called unless flag_program_name() is " "first called.");
    fatal_error(ERROR_ILLEGAL_STATE);
  }
  int start = 1;
  command_descriptor_t* command = NULL;
  if ((current_program->commands))
  {
    if ((argc<=1))
    {
      return "This program requires a command but not enough arguments were " "given";
    }
    char* name = (argv[1]);
    (command=flag_find_command_descriptor(name));
    if ((command==NULL))
    {
      return string_printf("The first command line argument is not a known command: %s", name);
    }
    else
    {
      ((*(command->write_back_ptr))=(command->name));
    }
    (start=2);
  }
  value_array_t* files = make_value_array(argc);
  boolean_t parse_flags = true;
  for (
    int i = start;
    (i<argc);
    (i++))
  {
    char* arg = (argv[i]);
    if (parse_flags)
    {
      if (string_equal(arg, "--"))
      {
        (parse_flags=false);
        continue;
      }
      if (string_starts_with(arg, "-"))
      {
        flag_key_value_t key_value = flag_split_argument(arg);
        if (((key_value.key)==NULL))
        {
          return string_printf("This argument is not a well formed flag: %s", arg);
        }
        flag_descriptor_t* flag = flag_find_flag_descriptor(command, (key_value.key));
        if ((flag==NULL))
        {
          return string_printf("The argument looks like a flag but was not found: '%s'\n\n" "(You may want to use ' -- ' to seperate flags from non flag " "arguments (aka file arguments).)", arg);
        }
        if (((key_value.value)==NULL))
        {
          (i++);
          ((key_value.value)=(argv[i]));
        }
        char* error = parse_and_write_value(flag, key_value);
        if (error)
        {
          return error;
        }
        continue;
      }
    }
    value_array_add(files, str_to_value(arg));
  }
  if (((command!=NULL)&&((command->write_back_file_args_ptr)!=NULL)))
  {
    ((*(command->write_back_file_args_ptr))=files);
  }
  if (((current_program->write_back_file_args_ptr)!=NULL))
  {
    ((*(current_program->write_back_file_args_ptr))=files);
  }
  return NULL;
}

/* i=141 j=1 */
void flag_print_help(FILE* out, char* message){
  fprintf(out, "\nMessage: %s\n", message);
  if ((current_program==NULL))
  {
    fprintf(out, "Command line parsing was not configured so help can not be " "provided.");
    return;
  }
  if (((current_program->commands)!=NULL))
  {
    fprintf(out, "\nUsage: %s <command> <flags> <files>\n", (current_program->name));
    fprintf(out, "\nDescription: %s\n\n", (current_program->description));
    flag_print_flags(out, "Global flags:", (current_program->flags));
    fprintf(out, "\nCommands:\n");
    string_tree_foreach((current_program->commands), key, value,     {
      fprintf(out, "\n    %s\t%s\n", key, ((/*CAST*/(command_descriptor_t*) (value.ptr))->description));
      flag_print_flags(out, "      Flags:", ((/*CAST*/(command_descriptor_t*) (value.ptr))->flags));
    }
);
  }
  else
  {
    fprintf(out, "\nUsage: %s <flags> <files>\n", (current_program->name));
    fprintf(out, "\nDescription: %s\n\n", (current_program->description));
    flag_print_flags(out, "Flags:", (current_program->flags));
  }
}

/* i=142 j=1 */
command_descriptor_t* flag_find_command_descriptor(char* name){
  if (((current_program->commands)==NULL))
  {
    log_fatal("flag_get_command() shouldn't not be called when we don't have any " "defined commands.");
    fatal_error(ERROR_ILLEGAL_STATE);
  }
  value_result_t command_value = string_tree_find((current_program->commands), name);
  if (is_ok(command_value))
  {
    return (/*CAST*/(command_descriptor_t*) (command_value.ptr));
  }
  else
  {
    return NULL;
  }
}

/* i=143 j=1 */
flag_descriptor_t* flag_find_flag_descriptor(command_descriptor_t* command, char* name){
  if ((command!=NULL))
  {
    value_result_t command_flag_value = string_tree_find((command->flags), name);
    if (is_ok(command_flag_value))
    {
      return (/*CAST*/(flag_descriptor_t*) (command_flag_value.ptr));
    }
  }
  value_result_t program_flag_value = string_tree_find((current_program->flags), name);
  if (is_ok(program_flag_value))
  {
    return (/*CAST*/(flag_descriptor_t*) (program_flag_value.ptr));
  }
  return NULL;
}

/* i=144 j=1 */
flag_key_value_t flag_split_argument(char* arg){
  int equal_sign_index = string_index_of_char(arg, '=');
  if ((equal_sign_index>=0))
  {
    char* key = string_substring(arg, 0, equal_sign_index);
    char* value = string_substring(arg, (equal_sign_index+1), strlen(arg));
    return ((flag_key_value_t) {.key = key, .value = value});
  }
  return ((flag_key_value_t) {.key = arg, .value = NULL});
}

/* i=145 j=1 */
char* parse_and_write_value(flag_descriptor_t* flag, flag_key_value_t key_value){
  switch ((flag->flag_type))
  {
    case flag_type_boolean:
    return parse_and_write_boolean(flag, key_value);
    case flag_type_string:
    ((*(/*CAST*/(char**) (flag->write_back_ptr)))=(key_value.value));
    return NULL;
    case flag_type_uint64:
    return parse_and_write_uint64(flag, key_value);
    case flag_type_enum:
    return parse_and_write_enum(flag, key_value);
    default:
    fatal_error(ERROR_ILLEGAL_STATE);
    break;
  }
  return "<ILLEGAL-STATE-NOT-REACHED>";
}

/* i=146 j=1 */
char* parse_and_write_boolean(flag_descriptor_t* flag, flag_key_value_t key_value){
  char* val = (key_value.value);
  if (((string_equal("true", val)||string_equal("t", val))||string_equal("1", val)))
  {
    ((*(/*CAST*/(boolean_t*) (flag->write_back_ptr)))=true);
  }
  else
  if (((string_equal("false", val)||string_equal("f", val))||string_equal("0", val)))
  {
    ((*(/*CAST*/(boolean_t*) (flag->write_back_ptr)))=false);
  }
  else
  {
    return string_printf("boolean flag %s does not accept value %s", (key_value.key), (key_value.value));
  }
  return NULL;
}

/* i=147 j=1 */
char* parse_and_write_uint64(flag_descriptor_t* flag, flag_key_value_t key_value){
  value_result_t val_result = string_parse_uint64((key_value.value));
  if (is_ok(val_result))
  {
    ((*(/*CAST*/(uint64_t*) (flag->write_back_ptr)))=(val_result.u64));
  }
  else
  {
    return string_printf("uint64_t flag %s does not accept value %s", (key_value.key), (key_value.value));
  }
  return NULL;
}

/* i=148 j=1 */
char* parse_and_write_enum(flag_descriptor_t* flag, flag_key_value_t key_value){
  value_result_t val_result = string_tree_find((flag->enum_values), (key_value.value));
  if (is_ok(val_result))
  {
    switch ((flag->enum_size))
    {
      case 64:
      ((*(/*CAST*/(uint64_t*) (flag->write_back_ptr)))=(val_result.u64));
      return NULL;
      case 32:
      ((*(/*CAST*/(uint32_t*) (flag->write_back_ptr)))=(val_result.u64));
      return NULL;
      default:
      fatal_error(ERROR_ILLEGAL_STATE);
      break;
    }
  }
  return string_printf("Flag %s does not accept the argument value %s", (key_value.key), (key_value.value));
}

/* i=149 j=0 */
void add_flag(char* name, void* write_back_ptr, flag_type_t flag_type){
  (current_flag=malloc_struct(flag_descriptor_t));
  ((current_flag->flag_type)=flag_type);
  ((current_flag->name)=name);
  ((current_flag->write_back_ptr)=write_back_ptr);
  if ((current_command!=NULL))
  {
    ((current_command->flags)=string_tree_insert((current_command->flags), name, ptr_to_value(current_flag)));
  }
  else
  if ((current_program!=NULL))
  {
    ((current_program->flags)=string_tree_insert((current_program->flags), name, ptr_to_value(current_flag)));
  }
  else
  {
    log_fatal("A current program or command must be executed first");
    fatal_error(ERROR_ILLEGAL_STATE);
  }
}

/* i=150 j=0 */
void flag_print_flags(FILE* out, char* header, string_tree_t* flags){
  fprintf(out, "%s\n", header);
  string_tree_foreach(flags, key, value,   {
    fprintf(out, "      %s\t%s\n", key, ((/*CAST*/(flag_descriptor_t*) (value.ptr))->description));
  }
);
}

/* i=151 j=1 */
__attribute__((warn_unused_result)) buffer_t* buffer_append_file_contents(buffer_t* bytes, char* file_name){
  uint64_t capacity = (bytes->capacity);
  {
    struct stat st;
    if ((stat(file_name, (&st))<0))
    {
      log_fatal("file does not exist: %s", file_name);
      fatal_error(ERROR_ILLEGAL_STATE);
    }
    (capacity=(st.st_size));
  }
  (bytes=buffer_increase_capacity(bytes, capacity));
  FILE* file = fopen(file_name, "r");
  (bytes=buffer_append_all(bytes, file));
  fclose(file);
  return bytes;
}

/* i=152 j=1 */
__attribute__((warn_unused_result)) extern buffer_t* buffer_append_all(buffer_t* bytes, FILE* input){
  uint8_t buffer[1024];
  while (1)
  {
    uint64_t n_read = fread(buffer, 1, (sizeof(buffer)), input);
    if ((n_read==0))
    {
      break;
    }
    (bytes=buffer_append_bytes(bytes, buffer, n_read));
  }
  return bytes;
}

/* i=153 j=1 */
void buffer_write_file(buffer_t* bytes, char* file_name){
  FILE* file = fopen(file_name, "w");
  if ((file==NULL))
  {
    log_fatal("Failed to open file for writing: %s", file_name);
    log_fatal("strerror(errno) = %s", strerror(errno));
    fatal_error(ERROR_ILLEGAL_STATE);
  }
  size_t bytes_written = fwrite((bytes->elements), 1, (bytes->length), file);
  if ((bytes_written!=(bytes->length)))
  {
    log_fatal("Failed to write %d bytes to %s", (bytes->length), file_name);
    log_fatal("strerror(errno) = %s", strerror(errno));
    fatal_error(ERROR_ILLEGAL_STATE);
  }
  if ((fclose(file)!=0))
  {
    log_fatal("Failed to close file: %s", file_name);
    log_fatal("strerror(errno) = %s", strerror(errno));
    fatal_error(ERROR_ILLEGAL_STATE);
  }
}

/* i=154 j=1 */
buffer_t* buffer_read_until(buffer_t* buffer, FILE* input, char end_of_line){
  while ((!feof(input)))
  {
    int ch = fgetc(input);
    if ((ch<0))
    {
      return buffer;
    }
    if ((ch==end_of_line))
    {
      return buffer;
    }
    (buffer=buffer_append_byte(buffer, ch));
  }
  return buffer;
}

/* i=155 j=1 */
extern buffer_t* buffer_read_ready_bytes(buffer_t* buffer, FILE* input, uint64_t max_bytes){
  int file_number = fileno(input);
  return buffer_read_ready_bytes_file_number(buffer, file_number, max_bytes);
}

/* i=156 j=1 */
extern buffer_t* buffer_read_ready_bytes_file_number(buffer_t* buffer, int file_number, uint64_t max_bytes){
  fcntl(file_number, F_SETFL, (fcntl(file_number, F_GETFL)|O_NONBLOCK));
  uint64_t bytes_remaining = (max_bytes-buffer_length(buffer));
  char read_buffer[1024];
  while ((bytes_remaining>0))
  {
    int bytes_read = read(file_number, read_buffer, (sizeof(read_buffer)));
    if ((bytes_read>0))
    {
      for (
        int i = 0;
        (i<bytes_read);
        (i++))
      {
        (buffer=buffer_append_byte(buffer, (/*CAST*/(uint8_t) (read_buffer[i]))));
        (bytes_remaining--);
      }
    }
    else
    if ((bytes_read==0))
    {
      break;
    }
    else
    {
      if (((errno!=EAGAIN)&&(errno!=EWOULDBLOCK)))
      {
        log_fatal("Error reading from file descriptor %d: %s", file_number, strerror(errno));
        fatal_error(ERROR_ILLEGAL_STATE);
      }
      break;
    }
  }
  return buffer;
}

/* i=157 j=1 */
int file_peek_byte(FILE* input){
  if (feof(input))
  {
    return (-1);
  }
  int result = fgetc(input);
  if ((result>=0))
  {
    ungetc(result, input);
  }
  return result;
}

/* i=158 j=1 */
boolean_t file_eof(FILE* input){
  return (feof(input)||(file_peek_byte(input)<0));
}

/* i=159 j=1 */
void file_copy_stream(FILE* input, FILE* output, boolean_t until_eof, uint64_t size){
  if (until_eof)
  {
    (size=ULLONG_MAX);
  }
  uint8_t buffer[FILE_COPY_STREAM_BUFFER_SIZE];
  while ((size>0))
  {
    int minimum = ((size<FILE_COPY_STREAM_BUFFER_SIZE) ? size : FILE_COPY_STREAM_BUFFER_SIZE);
    uint64_t n_read = fread(buffer, 1, minimum, input);
    if ((n_read==0))
    {
      break;
    }
    fwrite(buffer, 1, n_read, output);
    (size-=n_read);
  }
}

/* i=160 j=1 */
void file_skip_bytes(FILE* input, uint64_t n_bytes){
  while (1)
  {
    if (((n_bytes==0)||feof(input)))
    {
      return;
    }
    int ch = fgetc(input);
    if ((ch<0))
    {
      return;
    }
    (n_bytes--);
  }
}

/* i=161 j=1 */
__attribute__((warn_unused_result)) extern buffer_t* term_clear_screen(buffer_t* buffer){
  return buffer_printf(buffer, TERM_ESCAPE_STRING("2J"));
}

/* i=162 j=1 */
__attribute__((warn_unused_result)) extern buffer_t* term_set_foreground_color(buffer_t* buffer, uint32_t color){
  uint8_t blue = (color&0xff);
  uint8_t green = ((color>>8)&0xff);
  uint8_t red = ((color>>16)&0xff);
  return buffer_printf(buffer, TERM_ESCAPE_STRING_START_AND_END("38;2;%d;%d;%d"), red, green, blue);
}

/* i=163 j=1 */
__attribute__((warn_unused_result)) extern buffer_t* term_set_background_color(buffer_t* buffer, uint32_t color){
  uint8_t blue = (color&0xff);
  uint8_t green = ((color>>8)&0xff);
  uint8_t red = ((color>>16)&0xff);
  return buffer_printf(buffer, TERM_ESCAPE_STRING_START_AND_END("48;2;%d;%d;%d"), red, green, blue);
}

/* i=164 j=1 */
__attribute__((warn_unused_result)) extern buffer_t* term_move_cursor_absolute(buffer_t* buffer, int x, int y){
  return buffer_printf(buffer, TERM_ESCAPE_STRING("%d;%dH"), (y+1), (x+1));
}

/* i=165 j=1 */
__attribute__((warn_unused_result)) extern buffer_t* term_move_cursor_relative(buffer_t* buffer, int x, int y){
  if ((x>0))
  {
    (buffer=buffer_printf(buffer, TERM_ESCAPE_STRING("%dC"), x));
  }
  else
  if ((x<0))
  {
    (buffer=buffer_printf(buffer, TERM_ESCAPE_STRING("%dD"), (-x)));
  }
  if ((y>0))
  {
    (buffer=buffer_printf(buffer, TERM_ESCAPE_STRING("%dB"), y));
  }
  else
  {
    (buffer=buffer_printf(buffer, TERM_ESCAPE_STRING("%dA"), (-y)));
  }
  return buffer;
}

/* i=166 j=1 */
__attribute__((warn_unused_result)) extern buffer_t* term_bold(buffer_t* buffer){
  return buffer_printf(buffer, TERM_ESCAPE_STRING("1m"));
}

/* i=167 j=1 */
__attribute__((warn_unused_result)) extern buffer_t* term_dim(buffer_t* buffer){
  return buffer_printf(buffer, TERM_ESCAPE_STRING("2m"));
}

/* i=168 j=1 */
__attribute__((warn_unused_result)) extern buffer_t* term_italic(buffer_t* buffer){
  return buffer_printf(buffer, TERM_ESCAPE_STRING("3m"));
}

/* i=169 j=1 */
__attribute__((warn_unused_result)) extern buffer_t* term_underline(buffer_t* buffer){
  return buffer_printf(buffer, TERM_ESCAPE_STRING("4m"));
}

/* i=170 j=1 */
__attribute__((warn_unused_result)) extern buffer_t* term_reset_formatting(buffer_t* buffer){
  return buffer_printf(buffer, TERM_ESCAPE_STRING("0m"));
}

/* i=171 j=1 */
__attribute__((warn_unused_result)) extern buffer_t* term_draw_box(buffer_t* buffer, uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, box_drawing_t* box){
  (buffer=term_move_cursor_absolute(buffer, x0, y0));
  (buffer=buffer_append_code_point(buffer, (box->upper_left_corner)));
  for (
    uint64_t x = (x0+1);
    (x<x1);
    (x++))
  {
    (buffer=buffer_append_code_point(buffer, (box->top_edge)));
  }
  (buffer=buffer_append_code_point(buffer, (box->upper_right_corner)));
  (buffer=term_move_cursor_absolute(buffer, x0, y1));
  (buffer=buffer_append_code_point(buffer, (box->lower_left_corner)));
  for (
    uint64_t x = (x0+1);
    (x<x1);
    (x++))
  {
    (buffer=buffer_append_code_point(buffer, (box->bottom_edge)));
  }
  (buffer=buffer_append_code_point(buffer, (box->lower_right_corner)));
  for (
    int y = (y0+1);
    (y<y1);
    (y++))
  {
    (buffer=term_move_cursor_absolute(buffer, x0, y));
    (buffer=buffer_append_code_point(buffer, (box->left_edge)));
    for (
      int x = (x0+1);
      (x<x1);
      (x++))
    {
      (buffer=buffer_append_code_point(buffer, ' '));
    }
    (buffer=buffer_append_code_point(buffer, (box->right_edge)));
  }
  return buffer;
}

/* i=172 j=1 */
extern struct termios term_echo_off(){
  struct termios oldt;
  struct termios newt;
  tcgetattr(STDIN_FILENO, (&oldt));
  (newt=oldt);
  tcgetattr(STDIN_FILENO, (&oldt));
  (newt=oldt);
  ((newt.c_lflag)&=(~(ICANON|ECHO)));
  tcsetattr(STDIN_FILENO, TCSANOW, (&newt));
  return oldt;
}

/* i=173 j=1 */
extern void term_echo_restore(struct termios oldt){
  tcsetattr(STDIN_FILENO, TCSANOW, (&oldt));
}

/* i=174 j=1 */
value_array_t* string_tokenize(const char* str, const char* delimiters){
  return tokenize_memory_range((/*CAST*/(uint8_t*) str), strlen(str), delimiters);
}

/* i=175 j=1 */
value_array_t* buffer_tokenize(buffer_t* buffer, const char* delimiters){
  return tokenize_memory_range((&((buffer->elements)[0])), (buffer->length), delimiters);
}

/* i=176 j=1 */
value_array_t* tokenize_memory_range(uint8_t* str, uint64_t length, const char* delimiters){
  value_array_t* result = make_value_array(1);
  char token_data[1024];
  int cpos = 0;
  for (
    int i = 0;
    (i<length);
    (i++))
  {
    uint8_t ch = (str[i]);
    if (((ch==0)||string_contains_char(delimiters, ch)))
    {
      ((token_data[(cpos++)])='\0');
      if ((strlen(token_data)>0))
      {
        add_duplicate(result, token_data);
      }
      (cpos=0);
    }
    else
    {
      ((token_data[(cpos++)])=ch);
    }
  }
  ((token_data[(cpos++)])='\0');
  if ((strlen(token_data)>0))
  {
    add_duplicate(result, token_data);
  }
  return result;
}

/* i=177 j=1 */
void add_duplicate(value_array_t* token_array, const char* data){
  value_array_add(token_array, str_to_value(string_duplicate(data)));
}

/* i=178 j=1 */
random_state_t random_state_for_test(void){
  return ((random_state_t) {.a = 0x1E1D43C2CA44B1F5, .b = 0x4FDD267452CEDBAC});
}

/* i=180 j=1 */
uint64_t random_next_uint64_below(random_state_t* state, uint64_t maximum){
  if ((maximum==0))
  {
    fatal_error(ERROR_ILLEGAL_ARGUMENT);
  }
  return (random_next(state)%maximum);
  int mask = ((1ULL<<(uint64_highest_bit_set(maximum)+1))-1);
  while (1)
  {
    uint64_t n = random_next(state);
    (n&=mask);
    if ((n<maximum))
    {
      return n;
    }
  }
}

/* i=181 j=0 */
random_state_t* random_state(void){
  if (((shared_random_state.a)==0))
  {
    ((shared_random_state.a)=(0x1E1D43C2CA44B1F5^(/*CAST*/(uint64_t) time(NULL))));
    ((shared_random_state.b)=(0x4FDD267452CEDBAC^(/*CAST*/(uint64_t) time(NULL))));
  }
  return (&shared_random_state);
}

/* i=183 j=0 */
uint64_t random_next(random_state_t* state){
  uint64_t s0 = (state->a);
  uint64_t s1 = (state->b);
  uint64_t result = (rotl((s0*5), 7)*9);
  (s1^=s0);
  ((state->a)=((rotl(s0, 24)^s1)^(s1<<16)));
  ((state->b)=rotl(s1, 37));
  return result;
}

/* i=184 j=1 */
cdl_printer_t* make_cdl_printer(buffer_t* buffer){
  cdl_printer_t* result = malloc_struct(cdl_printer_t);
  ((result->buffer)=buffer);
  return result;
}

/* i=185 j=1 */
void cdl_boolean(cdl_printer_t* printer, boolean_t boolean){
  cdl_output_token(printer, (boolean ? "true" : "false"));
}

/* i=186 j=1 */
void cdl_string(cdl_printer_t* printer, char* string){
  if ((!is_symbol(string)))
  {
    cdl_output_token(printer, string_printf("\"%s\"", string));
  }
  else
  {
    cdl_output_token(printer, string);
  }
}

/* i=187 j=1 */
void cdl_int64(cdl_printer_t* printer, int64_t number){
  cdl_output_token(printer, string_printf("%ld", number));
}

/* i=188 j=1 */
void cdl_uint64(cdl_printer_t* printer, uint64_t number){
  cdl_output_token(printer, uint64_to_string(number));
}

/* i=189 j=1 */
void cdl_double(cdl_printer_t* printer, double number){
  cdl_output_token(printer, string_printf("%lf", number));
}

/* i=190 j=1 */
void cdl_start_array(cdl_printer_t* printer){
  cdl_output_token(printer, "[");
  ((printer->indention_level)+=1);
}

/* i=191 j=1 */
void cdl_end_array(cdl_printer_t* printer){
  ((printer->indention_level)-=1);
  cdl_output_token(printer, "]");
}

/* i=192 j=1 */
void cdl_start_table(cdl_printer_t* printer){
  cdl_output_token(printer, "{");
  ((printer->indention_level)+=1);
}

/* i=193 j=1 */
void cdl_key(cdl_printer_t* printer, char* key){
  ((printer->key_token)=key);
}

/* i=194 j=1 */
void cdl_end_table(cdl_printer_t* printer){
  ((printer->indention_level)-=1);
  cdl_output_token(printer, "}");
}

/* i=195 j=0 */
void cdl_indent(cdl_printer_t* printer){
  buffer_append_repeated_byte((printer->buffer), ' ', (4*(printer->indention_level)));
}

/* i=196 j=0 */
boolean_t is_symbol(char* string){
  for (
    int i = 0;
    ((string[i])!=0);
    (i++))
  {
    if ((i==0))
    {
      if ((!isalpha((string[i]))))
      {
        return false;
      }
    }
    else
    {
      if ((!(isalnum((string[i]))||((string[i])=='_'))))
      {
        return false;
      }
    }
  }
  return true;
}

/* i=197 j=0 */
void cdl_output_token(cdl_printer_t* printer, char* string){
  cdl_indent(printer);
  if (((printer->key_token)!=NULL))
  {
    buffer_printf((printer->buffer), "%s = %s\n", (printer->key_token), string);
    ((printer->key_token)=NULL);
  }
  else
  {
    buffer_printf((printer->buffer), "%s\n", string);
  }
}

/* i=198 j=1 */
sub_process_t* make_sub_process(value_array_t* argv){
  sub_process_t* result = malloc_struct(sub_process_t);
  ((result->argv)=argv);
  ((result->exit_code)=(-1));
  return result;
}

/* i=199 j=1 */
boolean_t sub_process_launch(sub_process_t* sub_process){
  uint64_t length = ((sub_process->argv)->length);
  if ((length<1))
  {
    log_fatal("Expected at least the program path in argv");
    fatal_error(ERROR_ILLEGAL_STATE);
  }
  char** argv = (/*CAST*/(typeof(char**)) malloc_bytes(((length+1)*(sizeof(typeof(char*))))));
  for (
    int i = 0;
    (i<length);
    (i++))
  {
    ((argv[i])=value_array_get_ptr((sub_process->argv), i, typeof(char*)));
  }
  ((argv[length])=NULL);
  int stdin_pipe[2] = {0};
  int stdout_pipe[2] = {0};
  int stderr_pipe[2] = {0};
  if ((((pipe(stdin_pipe)==(-1))||(pipe(stdout_pipe)==(-1)))||(pipe(stderr_pipe)==(-1))))
  {
    log_fatal("Failed to create pipe for stdin, stdout or stderr");
    fatal_error(ERROR_ILLEGAL_STATE);
    return false;
  }
  pid_t pid = fork();
  if ((pid==(-1)))
  {
    log_fatal("fork() failed.");
    fatal_error(ERROR_ILLEGAL_STATE);
    return false;
  }
  if ((pid==0))
  {
    dup2((stdin_pipe[0]), STDIN_FILENO);
    dup2((stdout_pipe[1]), STDOUT_FILENO);
    dup2((stderr_pipe[1]), STDERR_FILENO);
    close((stdin_pipe[0]));
    close((stdin_pipe[1]));
    close((stdout_pipe[0]));
    close((stdout_pipe[1]));
    close((stderr_pipe[0]));
    close((stderr_pipe[1]));
    int exec_exit_status = execvp((argv[0]), argv);
    log_fatal("execvp returned non zero exit status %d", exec_exit_status);
    fatal_error(ERROR_ILLEGAL_STATE);
    return false;
  }
  else
  {
    close((stdin_pipe[0]));
    close((stdout_pipe[1]));
    close((stderr_pipe[1]));
    ((sub_process->pid)=pid);
    ((sub_process->stdin)=(stdin_pipe[1]));
    ((sub_process->stdout)=(stdout_pipe[0]));
    ((sub_process->stderr)=(stderr_pipe[0]));
    free_bytes(argv);
    return true;
  }
}

/* i=200 j=1 */
uint64_t sub_process_write(sub_process_t* sub_process, buffer_t* data, uint64_t start_position){
  int stdin_fd = (sub_process->stdin);
  int flags = fcntl(stdin_fd, F_GETFL, 0);
  fcntl(stdin_fd, F_SETFL, (flags|O_NONBLOCK));
  ssize_t bytes_written = write(stdin_fd, (&((data->elements)[start_position])), ((data->length)-start_position));
  if ((bytes_written==(-1)))
  {
    if (((errno==EAGAIN)||(errno==EWOULDBLOCK)))
    {
      return 0;
    }
    else
    {
      log_fatal("Error writing to subprocess stdin: %s", strerror(errno));
      fatal_error(ERROR_ILLEGAL_STATE);
    }
  }
  return bytes_written;
}

/* i=201 j=1 */
void sub_process_close_stdin(sub_process_t* sub_process){
  if (((sub_process->stdin)!=(-1)))
  {
    if ((close((sub_process->stdin))==(-1)))
    {
      log_fatal("Error closing subprocess stdin: %s", strerror(errno));
      fatal_error(ERROR_ILLEGAL_STATE);
    }
    ((sub_process->stdin)=(-1));
  }
}

/* i=202 j=1 */
void sub_process_read(sub_process_t* sub_process, buffer_t* stdout, buffer_t* stderr){
  if ((stdout!=NULL))
  {
    buffer_read_ready_bytes_file_number(stdout, (sub_process->stdout), 0xffffffff);
  }
  if ((stderr!=NULL))
  {
    buffer_read_ready_bytes_file_number(stderr, (sub_process->stderr), 0xffffffff);
  }
}

/* i=203 j=1 */
void sub_process_wait(sub_process_t* sub_process){
  if (((sub_process->exit_status)!=EXIT_STATUS_UNKNOWN))
  {
    int status = 0;
    pid_t result = waitpid((sub_process->pid), (&status), 0);
    sub_process_record_exit_status(sub_process, result, status);
  }
}

/* i=204 j=0 */
void sub_process_record_exit_status(sub_process_t* sub_process, pid_t pid, int status){
  if ((pid==(-1)))
  {
    ((sub_process->exit_status)=EXIT_STATUS_ABNORMAL);
    return;
  }
  if (WIFEXITED(status))
  {
    ((sub_process->exit_status)=EXIT_STATUS_NORMAL_EXIT);
    ((sub_process->exit_code)=WEXITSTATUS(status));
  }
  else
  if (WIFSIGNALED(status))
  {
    ((sub_process->exit_status)=EXIT_STATUS_SIGNAL);
    ((sub_process->exit_signal)=WTERMSIG(status));
  }
  else
  {
    ((sub_process->exit_status)=EXIT_STATUS_ABNORMAL);
  }
}

/* i=205 j=0 */
boolean_t is_sub_process_running(sub_process_t* sub_process){
  if (((sub_process->exit_status)!=EXIT_STATUS_UNKNOWN))
  {
    return false;
  }
  int status = 0;
  pid_t result = waitpid((sub_process->pid), (&status), WNOHANG);
  if ((result==0))
  {
    return true;
  }
  sub_process_record_exit_status(sub_process, result, status);
  return false;
}

/* i=208 j=0 */
__attribute__((format(printf, 3, 4))) void test_fail_and_exit(char* file_name, int line_number, char* format, ...){
  va_list args;
  fprintf(stdout, "%s:%d: ", file_name, line_number);
  va_start(args, format);
  vfprintf(stdout, format, args);
  fprintf(stdout, "\n");
  va_end(args);
  exit(1);
}

/* i=209 j=0 */
char* error_code_to_string(error_code_t value){
  switch (value) {
    case ERROR_UKNOWN:
return "ERROR_UKNOWN";
  case ERROR_SIGSEGV:
return "ERROR_SIGSEGV";
  case ERROR_ACCESS_OUT_OF_BOUNDS:
return "ERROR_ACCESS_OUT_OF_BOUNDS";
  case ERROR_BAD_COMMAND_LINE:
return "ERROR_BAD_COMMAND_LINE";
  case ERROR_DYNAMICALLY_SIZED_TYPE_ILLEGAL_IN_CONTAINER:
return "ERROR_DYNAMICALLY_SIZED_TYPE_ILLEGAL_IN_CONTAINER";
  case ERROR_ILLEGAL_ENUM_VALUE:
return "ERROR_ILLEGAL_ENUM_VALUE";
  case ERROR_ILLEGAL_INITIAL_CAPACITY:
return "ERROR_ILLEGAL_INITIAL_CAPACITY";
  case ERROR_ILLEGAL_NULL_ARGUMENT:
return "ERROR_ILLEGAL_NULL_ARGUMENT";
  case ERROR_ILLEGAL_ZERO_HASHCODE_VALUE:
return "ERROR_ILLEGAL_ZERO_HASHCODE_VALUE";
  case ERROR_ILLEGAL_RANGE:
return "ERROR_ILLEGAL_RANGE";
  case ERROR_MEMORY_ALLOCATION:
return "ERROR_MEMORY_ALLOCATION";
  case ERROR_MEMORY_FREE_NULL:
return "ERROR_MEMORY_FREE_NULL";
  case ERROR_NOT_REACHED:
return "ERROR_NOT_REACHED";
  case ERROR_REFERENCE_NOT_EXPECTED_TYPE:
return "ERROR_REFERENCE_NOT_EXPECTED_TYPE";
  case ERROR_UNIMPLEMENTED:
return "ERROR_UNIMPLEMENTED";
  case ERROR_OPEN_LOG_FILE:
return "ERROR_OPEN_LOG_FILE";
  case ERROR_TEST:
return "ERROR_TEST";
  case ERROR_INTERNAL_ASSERTION_FAILURE:
return "ERROR_INTERNAL_ASSERTION_FAILURE";
  case ERROR_BAD_ALLOCATION_SIZE:
return "ERROR_BAD_ALLOCATION_SIZE";
  case ERROR_ILLEGAL_ARGUMENT:
return "ERROR_ILLEGAL_ARGUMENT";
  case ERROR_MEMORY_START_PADDING_ERROR:
return "ERROR_MEMORY_START_PADDING_ERROR";
  case ERROR_MEMORY_END_PADDING_ERROR:
return "ERROR_MEMORY_END_PADDING_ERROR";
  case ERROR_FATAL:
return "ERROR_FATAL";
  case ERROR_ILLEGAL_STATE:
return "ERROR_ILLEGAL_STATE";
  case ERROR_ILLEGAL_INPUT:
return "ERROR_ILLEGAL_INPUT";
  case ERROR_ILLEGAL_UTF_8_CODE_POINT:
return "ERROR_ILLEGAL_UTF_8_CODE_POINT";
  case ERROR_ILLEGAL_TERMINAL_COORDINATES:
return "ERROR_ILLEGAL_TERMINAL_COORDINATES";
  default:
    return "<<unknown-error_code>>";
  }
}
/* i=210 j=0 */
error_code_t string_to_error_code(char* value){
  if (strcmp(value, "ERROR_UKNOWN") == 0) {
return ERROR_UKNOWN;
  }
  if (strcmp(value, "ERROR_SIGSEGV") == 0) {
return ERROR_SIGSEGV;
  }
  if (strcmp(value, "ERROR_ACCESS_OUT_OF_BOUNDS") == 0) {
return ERROR_ACCESS_OUT_OF_BOUNDS;
  }
  if (strcmp(value, "ERROR_BAD_COMMAND_LINE") == 0) {
return ERROR_BAD_COMMAND_LINE;
  }
  if (strcmp(value, "ERROR_DYNAMICALLY_SIZED_TYPE_ILLEGAL_IN_CONTAINER") == 0) {
return ERROR_DYNAMICALLY_SIZED_TYPE_ILLEGAL_IN_CONTAINER;
  }
  if (strcmp(value, "ERROR_ILLEGAL_ENUM_VALUE") == 0) {
return ERROR_ILLEGAL_ENUM_VALUE;
  }
  if (strcmp(value, "ERROR_ILLEGAL_INITIAL_CAPACITY") == 0) {
return ERROR_ILLEGAL_INITIAL_CAPACITY;
  }
  if (strcmp(value, "ERROR_ILLEGAL_NULL_ARGUMENT") == 0) {
return ERROR_ILLEGAL_NULL_ARGUMENT;
  }
  if (strcmp(value, "ERROR_ILLEGAL_ZERO_HASHCODE_VALUE") == 0) {
return ERROR_ILLEGAL_ZERO_HASHCODE_VALUE;
  }
  if (strcmp(value, "ERROR_ILLEGAL_RANGE") == 0) {
return ERROR_ILLEGAL_RANGE;
  }
  if (strcmp(value, "ERROR_MEMORY_ALLOCATION") == 0) {
return ERROR_MEMORY_ALLOCATION;
  }
  if (strcmp(value, "ERROR_MEMORY_FREE_NULL") == 0) {
return ERROR_MEMORY_FREE_NULL;
  }
  if (strcmp(value, "ERROR_NOT_REACHED") == 0) {
return ERROR_NOT_REACHED;
  }
  if (strcmp(value, "ERROR_REFERENCE_NOT_EXPECTED_TYPE") == 0) {
return ERROR_REFERENCE_NOT_EXPECTED_TYPE;
  }
  if (strcmp(value, "ERROR_UNIMPLEMENTED") == 0) {
return ERROR_UNIMPLEMENTED;
  }
  if (strcmp(value, "ERROR_OPEN_LOG_FILE") == 0) {
return ERROR_OPEN_LOG_FILE;
  }
  if (strcmp(value, "ERROR_TEST") == 0) {
return ERROR_TEST;
  }
  if (strcmp(value, "ERROR_INTERNAL_ASSERTION_FAILURE") == 0) {
return ERROR_INTERNAL_ASSERTION_FAILURE;
  }
  if (strcmp(value, "ERROR_BAD_ALLOCATION_SIZE") == 0) {
return ERROR_BAD_ALLOCATION_SIZE;
  }
  if (strcmp(value, "ERROR_ILLEGAL_ARGUMENT") == 0) {
return ERROR_ILLEGAL_ARGUMENT;
  }
  if (strcmp(value, "ERROR_MEMORY_START_PADDING_ERROR") == 0) {
return ERROR_MEMORY_START_PADDING_ERROR;
  }
  if (strcmp(value, "ERROR_MEMORY_END_PADDING_ERROR") == 0) {
return ERROR_MEMORY_END_PADDING_ERROR;
  }
  if (strcmp(value, "ERROR_FATAL") == 0) {
return ERROR_FATAL;
  }
  if (strcmp(value, "ERROR_ILLEGAL_STATE") == 0) {
return ERROR_ILLEGAL_STATE;
  }
  if (strcmp(value, "ERROR_ILLEGAL_INPUT") == 0) {
return ERROR_ILLEGAL_INPUT;
  }
  if (strcmp(value, "ERROR_ILLEGAL_UTF_8_CODE_POINT") == 0) {
return ERROR_ILLEGAL_UTF_8_CODE_POINT;
  }
  if (strcmp(value, "ERROR_ILLEGAL_TERMINAL_COORDINATES") == 0) {
return ERROR_ILLEGAL_TERMINAL_COORDINATES;
  }
  return 0;
}
/* i=211 j=0 */
enum_metadata_t* error_code_metadata(){
    static enum_element_metadata_t var_0 = (enum_element_metadata_t) {
        .next = NULL,
        .name = "ERROR_UKNOWN",
        .value = ERROR_UKNOWN
    };
    static enum_element_metadata_t var_1 = (enum_element_metadata_t) {
        .next = &var_0,
        .name = "ERROR_SIGSEGV",
        .value = ERROR_SIGSEGV
    };
    static enum_element_metadata_t var_2 = (enum_element_metadata_t) {
        .next = &var_1,
        .name = "ERROR_ACCESS_OUT_OF_BOUNDS",
        .value = ERROR_ACCESS_OUT_OF_BOUNDS
    };
    static enum_element_metadata_t var_3 = (enum_element_metadata_t) {
        .next = &var_2,
        .name = "ERROR_BAD_COMMAND_LINE",
        .value = ERROR_BAD_COMMAND_LINE
    };
    static enum_element_metadata_t var_4 = (enum_element_metadata_t) {
        .next = &var_3,
        .name = "ERROR_DYNAMICALLY_SIZED_TYPE_ILLEGAL_IN_CONTAINER",
        .value = ERROR_DYNAMICALLY_SIZED_TYPE_ILLEGAL_IN_CONTAINER
    };
    static enum_element_metadata_t var_5 = (enum_element_metadata_t) {
        .next = &var_4,
        .name = "ERROR_ILLEGAL_ENUM_VALUE",
        .value = ERROR_ILLEGAL_ENUM_VALUE
    };
    static enum_element_metadata_t var_6 = (enum_element_metadata_t) {
        .next = &var_5,
        .name = "ERROR_ILLEGAL_INITIAL_CAPACITY",
        .value = ERROR_ILLEGAL_INITIAL_CAPACITY
    };
    static enum_element_metadata_t var_7 = (enum_element_metadata_t) {
        .next = &var_6,
        .name = "ERROR_ILLEGAL_NULL_ARGUMENT",
        .value = ERROR_ILLEGAL_NULL_ARGUMENT
    };
    static enum_element_metadata_t var_8 = (enum_element_metadata_t) {
        .next = &var_7,
        .name = "ERROR_ILLEGAL_ZERO_HASHCODE_VALUE",
        .value = ERROR_ILLEGAL_ZERO_HASHCODE_VALUE
    };
    static enum_element_metadata_t var_9 = (enum_element_metadata_t) {
        .next = &var_8,
        .name = "ERROR_ILLEGAL_RANGE",
        .value = ERROR_ILLEGAL_RANGE
    };
    static enum_element_metadata_t var_10 = (enum_element_metadata_t) {
        .next = &var_9,
        .name = "ERROR_MEMORY_ALLOCATION",
        .value = ERROR_MEMORY_ALLOCATION
    };
    static enum_element_metadata_t var_11 = (enum_element_metadata_t) {
        .next = &var_10,
        .name = "ERROR_MEMORY_FREE_NULL",
        .value = ERROR_MEMORY_FREE_NULL
    };
    static enum_element_metadata_t var_12 = (enum_element_metadata_t) {
        .next = &var_11,
        .name = "ERROR_NOT_REACHED",
        .value = ERROR_NOT_REACHED
    };
    static enum_element_metadata_t var_13 = (enum_element_metadata_t) {
        .next = &var_12,
        .name = "ERROR_REFERENCE_NOT_EXPECTED_TYPE",
        .value = ERROR_REFERENCE_NOT_EXPECTED_TYPE
    };
    static enum_element_metadata_t var_14 = (enum_element_metadata_t) {
        .next = &var_13,
        .name = "ERROR_UNIMPLEMENTED",
        .value = ERROR_UNIMPLEMENTED
    };
    static enum_element_metadata_t var_15 = (enum_element_metadata_t) {
        .next = &var_14,
        .name = "ERROR_OPEN_LOG_FILE",
        .value = ERROR_OPEN_LOG_FILE
    };
    static enum_element_metadata_t var_16 = (enum_element_metadata_t) {
        .next = &var_15,
        .name = "ERROR_TEST",
        .value = ERROR_TEST
    };
    static enum_element_metadata_t var_17 = (enum_element_metadata_t) {
        .next = &var_16,
        .name = "ERROR_INTERNAL_ASSERTION_FAILURE",
        .value = ERROR_INTERNAL_ASSERTION_FAILURE
    };
    static enum_element_metadata_t var_18 = (enum_element_metadata_t) {
        .next = &var_17,
        .name = "ERROR_BAD_ALLOCATION_SIZE",
        .value = ERROR_BAD_ALLOCATION_SIZE
    };
    static enum_element_metadata_t var_19 = (enum_element_metadata_t) {
        .next = &var_18,
        .name = "ERROR_ILLEGAL_ARGUMENT",
        .value = ERROR_ILLEGAL_ARGUMENT
    };
    static enum_element_metadata_t var_20 = (enum_element_metadata_t) {
        .next = &var_19,
        .name = "ERROR_MEMORY_START_PADDING_ERROR",
        .value = ERROR_MEMORY_START_PADDING_ERROR
    };
    static enum_element_metadata_t var_21 = (enum_element_metadata_t) {
        .next = &var_20,
        .name = "ERROR_MEMORY_END_PADDING_ERROR",
        .value = ERROR_MEMORY_END_PADDING_ERROR
    };
    static enum_element_metadata_t var_22 = (enum_element_metadata_t) {
        .next = &var_21,
        .name = "ERROR_FATAL",
        .value = ERROR_FATAL
    };
    static enum_element_metadata_t var_23 = (enum_element_metadata_t) {
        .next = &var_22,
        .name = "ERROR_ILLEGAL_STATE",
        .value = ERROR_ILLEGAL_STATE
    };
    static enum_element_metadata_t var_24 = (enum_element_metadata_t) {
        .next = &var_23,
        .name = "ERROR_ILLEGAL_INPUT",
        .value = ERROR_ILLEGAL_INPUT
    };
    static enum_element_metadata_t var_25 = (enum_element_metadata_t) {
        .next = &var_24,
        .name = "ERROR_ILLEGAL_UTF_8_CODE_POINT",
        .value = ERROR_ILLEGAL_UTF_8_CODE_POINT
    };
    static enum_element_metadata_t var_26 = (enum_element_metadata_t) {
        .next = &var_25,
        .name = "ERROR_ILLEGAL_TERMINAL_COORDINATES",
        .value = ERROR_ILLEGAL_TERMINAL_COORDINATES
    };
    static enum_metadata_t enum_metadata_result = (enum_metadata_t) {
        .name = "error_code_t",
        .elements = &var_26
    };
    return &enum_metadata_result;
}
/* i=212 j=0 */
char* non_fatal_error_code_to_string(non_fatal_error_code_t value){
  switch (value) {
    case NF_OK:
return "NF_OK";
  case NF_ERROR_NOT_FOUND:
return "NF_ERROR_NOT_FOUND";
  case NF_ERROR_NOT_PARSED_AS_NUMBER:
return "NF_ERROR_NOT_PARSED_AS_NUMBER";
  case NF_ERROR_NOT_PARSED_AS_EXPECTED_ENUM:
return "NF_ERROR_NOT_PARSED_AS_EXPECTED_ENUM";
  default:
    return "<<unknown-non_fatal_error_code>>";
  }
}
/* i=213 j=0 */
non_fatal_error_code_t string_to_non_fatal_error_code(char* value){
  if (strcmp(value, "NF_OK") == 0) {
return NF_OK;
  }
  if (strcmp(value, "NF_ERROR_NOT_FOUND") == 0) {
return NF_ERROR_NOT_FOUND;
  }
  if (strcmp(value, "NF_ERROR_NOT_PARSED_AS_NUMBER") == 0) {
return NF_ERROR_NOT_PARSED_AS_NUMBER;
  }
  if (strcmp(value, "NF_ERROR_NOT_PARSED_AS_EXPECTED_ENUM") == 0) {
return NF_ERROR_NOT_PARSED_AS_EXPECTED_ENUM;
  }
  return 0;
}
/* i=214 j=0 */
enum_metadata_t* non_fatal_error_code_metadata(){
    static enum_element_metadata_t var_0 = (enum_element_metadata_t) {
        .next = NULL,
        .name = "NF_OK",
        .value = NF_OK
    };
    static enum_element_metadata_t var_1 = (enum_element_metadata_t) {
        .next = &var_0,
        .name = "NF_ERROR_NOT_FOUND",
        .value = NF_ERROR_NOT_FOUND
    };
    static enum_element_metadata_t var_2 = (enum_element_metadata_t) {
        .next = &var_1,
        .name = "NF_ERROR_NOT_PARSED_AS_NUMBER",
        .value = NF_ERROR_NOT_PARSED_AS_NUMBER
    };
    static enum_element_metadata_t var_3 = (enum_element_metadata_t) {
        .next = &var_2,
        .name = "NF_ERROR_NOT_PARSED_AS_EXPECTED_ENUM",
        .value = NF_ERROR_NOT_PARSED_AS_EXPECTED_ENUM
    };
    static enum_metadata_t enum_metadata_result = (enum_metadata_t) {
        .name = "non_fatal_error_code_t",
        .elements = &var_3
    };
    return &enum_metadata_result;
}
/* i=215 j=0 */
char* flag_type_to_string(flag_type_t value){
  switch (value) {
    case flag_type_none:
return "flag_type_none";
  case flag_type_boolean:
return "flag_type_boolean";
  case flag_type_string:
return "flag_type_string";
  case flag_type_uint64:
return "flag_type_uint64";
  case flag_type_int64:
return "flag_type_int64";
  case flag_type_double:
return "flag_type_double";
  case flag_type_enum:
return "flag_type_enum";
  case flag_type_custom:
return "flag_type_custom";
  default:
    return "<<unknown-flag_type>>";
  }
}
/* i=216 j=0 */
flag_type_t string_to_flag_type(char* value){
  if (strcmp(value, "flag_type_none") == 0) {
return flag_type_none;
  }
  if (strcmp(value, "flag_type_boolean") == 0) {
return flag_type_boolean;
  }
  if (strcmp(value, "flag_type_string") == 0) {
return flag_type_string;
  }
  if (strcmp(value, "flag_type_uint64") == 0) {
return flag_type_uint64;
  }
  if (strcmp(value, "flag_type_int64") == 0) {
return flag_type_int64;
  }
  if (strcmp(value, "flag_type_double") == 0) {
return flag_type_double;
  }
  if (strcmp(value, "flag_type_enum") == 0) {
return flag_type_enum;
  }
  if (strcmp(value, "flag_type_custom") == 0) {
return flag_type_custom;
  }
  return 0;
}
/* i=217 j=0 */
enum_metadata_t* flag_type_metadata(){
    static enum_element_metadata_t var_0 = (enum_element_metadata_t) {
        .next = NULL,
        .name = "flag_type_none",
        .value = flag_type_none
    };
    static enum_element_metadata_t var_1 = (enum_element_metadata_t) {
        .next = &var_0,
        .name = "flag_type_boolean",
        .value = flag_type_boolean
    };
    static enum_element_metadata_t var_2 = (enum_element_metadata_t) {
        .next = &var_1,
        .name = "flag_type_string",
        .value = flag_type_string
    };
    static enum_element_metadata_t var_3 = (enum_element_metadata_t) {
        .next = &var_2,
        .name = "flag_type_uint64",
        .value = flag_type_uint64
    };
    static enum_element_metadata_t var_4 = (enum_element_metadata_t) {
        .next = &var_3,
        .name = "flag_type_int64",
        .value = flag_type_int64
    };
    static enum_element_metadata_t var_5 = (enum_element_metadata_t) {
        .next = &var_4,
        .name = "flag_type_double",
        .value = flag_type_double
    };
    static enum_element_metadata_t var_6 = (enum_element_metadata_t) {
        .next = &var_5,
        .name = "flag_type_enum",
        .value = flag_type_enum
    };
    static enum_element_metadata_t var_7 = (enum_element_metadata_t) {
        .next = &var_6,
        .name = "flag_type_custom",
        .value = flag_type_custom
    };
    static enum_metadata_t enum_metadata_result = (enum_metadata_t) {
        .name = "flag_type_t",
        .elements = &var_7
    };
    return &enum_metadata_result;
}
/* i=218 j=0 */
char* sub_process_exit_status_to_string(sub_process_exit_status_t value){
  switch (value) {
    case EXIT_STATUS_UNKNOWN:
return "EXIT_STATUS_UNKNOWN";
  case EXIT_STATUS_NORMAL_EXIT:
return "EXIT_STATUS_NORMAL_EXIT";
  case EXIT_STATUS_SIGNAL:
return "EXIT_STATUS_SIGNAL";
  case EXIT_STATUS_ABNORMAL:
return "EXIT_STATUS_ABNORMAL";
  default:
    return "<<unknown-sub_process_exit_status>>";
  }
}
/* i=219 j=0 */
sub_process_exit_status_t string_to_sub_process_exit_status(char* value){
  if (strcmp(value, "EXIT_STATUS_UNKNOWN") == 0) {
return EXIT_STATUS_UNKNOWN;
  }
  if (strcmp(value, "EXIT_STATUS_NORMAL_EXIT") == 0) {
return EXIT_STATUS_NORMAL_EXIT;
  }
  if (strcmp(value, "EXIT_STATUS_SIGNAL") == 0) {
return EXIT_STATUS_SIGNAL;
  }
  if (strcmp(value, "EXIT_STATUS_ABNORMAL") == 0) {
return EXIT_STATUS_ABNORMAL;
  }
  return 0;
}
/* i=220 j=0 */
enum_metadata_t* sub_process_exit_status_metadata(){
    static enum_element_metadata_t var_0 = (enum_element_metadata_t) {
        .next = NULL,
        .name = "EXIT_STATUS_UNKNOWN",
        .value = EXIT_STATUS_UNKNOWN
    };
    static enum_element_metadata_t var_1 = (enum_element_metadata_t) {
        .next = &var_0,
        .name = "EXIT_STATUS_NORMAL_EXIT",
        .value = EXIT_STATUS_NORMAL_EXIT
    };
    static enum_element_metadata_t var_2 = (enum_element_metadata_t) {
        .next = &var_1,
        .name = "EXIT_STATUS_SIGNAL",
        .value = EXIT_STATUS_SIGNAL
    };
    static enum_element_metadata_t var_3 = (enum_element_metadata_t) {
        .next = &var_2,
        .name = "EXIT_STATUS_ABNORMAL",
        .value = EXIT_STATUS_ABNORMAL
    };
    static enum_metadata_t enum_metadata_result = (enum_metadata_t) {
        .name = "sub_process_exit_status_t",
        .elements = &var_3
    };
    return &enum_metadata_result;
}

