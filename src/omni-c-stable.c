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

#define _MODE_H_

#define _KEYWORDS_H_

#define _FILE_H_

#define _FILE_READER_H_

#define _COMPILER_ERRORS_H_

#define _LEXER_H_

#define read_token(token_reader_function_name)                                 \
  do {                                                                         \
    token_or_error_t token_or_error = token_reader_function_name(buffer, pos); \
    if (token_or_error.error_code) {                                           \
      return compound_literal(                                                 \
          tokenizer_result_t,                                                  \
          {.tokenizer_error_code = token_or_error.error_code,                  \
           .tokenizer_error_position = token_or_error.error_position});        \
    }                                                                          \
    token = heap_allocate_token(token_or_error.token);                         \
    value_array_add(result_tokens, ptr_to_value(token));                       \
    pos = token_or_error.token.end;                                            \
  } while (0)

#define _TOKEN_LIST_H_

#define _TOKEN_TRANSFORMER_H_

#define _PARSER_H_

#define _PSTATE_H_

#define _DECLARATION_PARSER_H_

#define _NODE_LIST_H_

#define _DEBUG_PRINTER_H_

#define _C_FILE_PRINTER_H_

#define _SYMBOL_TABLE_H_

#define _SOURCE_TO_SOURCE_H_

#define _PREPROCESSOR_H_

#define _SYMBOL_TABLE_BUILDER_H_

#define _SRCGEN_H_

#define _PRATT_PARSER_H_

#define _STATEMNET_PARSER_H_

#define _TYPE_PARSER_H_

#define _USER_TYPE_PARSER_H_

#define _VARIABLE_DEFINITION_PARSER_H_

#define _LITERAL_PARSER_H_

#define _BALANCED_CONSTRUCT_PARSER_H_

#define _PRINTER_H_

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

typedef enum {
  INPUT_MODE_OMNI_C,
  INPUT_MODE_STANDARD_C,
  INPUT_MODE_C_PLUS_PLUS,
} input_mode_t;

typedef enum {
  OUTPUT_MODE_STANDARD_C,
  OUTPUT_MODE_C_PLUS_PLUS,
} output_mode_t;

typedef enum {
  OMNI_C_SOURCE_FILE,
  STD_C_SOURCE_FILE,
  DATA_FILE,
} file_tag_t;

typedef struct file_S file_t;

typedef enum {
  TOKENIZER_ERROR_UNKNOWN,
  TOKENIZER_ERROR_UTF_DECODE_ERROR,
  TOKENIZER_ERROR_UNRECOGNIZED_PUNCTUATION,
  TOKENIZER_ERROR_UNTERMINATED_COMMENT,
  TOKENIZER_ERROR_UNTERMINATED_STRING_LITERAL,
  TOKENIZER_ERROR_UNTERMINATED_CHARACTER_LITERL,
} tokenizer_error_t;

typedef enum {
  PARSE_ERROR_UNKNOWN,
  PARSE_ERROR_COMMA_OR_EQUAL_SIGN_EXPECTED,
  PARSE_ERROR_EXPECTED_FIELD_WIDTH_OR_SEMICOLON,
  PARSE_ERROR_EXPECTED_SEMICOLON,
  PARSE_ERROR_IDENTIFIER_EXPECTED,
  PARSE_ERROR_NOT_LITERAL_NODE,
  PARSE_ERROR_INTEGER_LITERAL_EXPECTED,
  PARSE_ERROR_OPEN_BRACE_EXPECTED,
  PARSE_ERROR_CLOSE_BRACKET_EXPECTED,
  PARSE_ERROR_UNRECOGNIZED_TOP_LEVEL_DECLARATION,
  PARSE_ERROR_SEMICOLON_EXPECTED,
  PARSE_ERROR_CONFLICTING_STORAGE_CLASS_SPECIFIER,
  PARSE_ERROR_CONFLICTING_FUNCTION_SPECIFIER,
  PARSE_ERROR_EXPECTED_OPEN_PAREN_AFTER_UNDERSCORE_ATTRIBUTE,
  PARSE_ERROR_EXPECTED_MATCHING_CLOSE_PAREN_AFTER_UNDERSCORE_ATTRIBUTE,
  PARSE_ERROR_EXPECTED_PREFIX_OPERATOR_OR_TERMINAL,
  PARSE_ERROR_EXPECTED_STATEMENT,
  PARSE_ERROR_EXPECTED_TOKEN,
  PARSE_ERROR_EXPECTED_TOKEN_TYPE,
  PARSE_ERROR_EOF,
  PARSE_ERROR_UNHANDLED_INSTRUCTION,
} parse_error_code_t;

typedef struct compiler_error_S compiler_error_t;

typedef struct src_code_snippets_t__generated_S src_code_snippets_t;

typedef enum {
  TOKEN_TYPE_UNKNOWN,
  TOKEN_TYPE_WHITESPACE,
  TOKEN_TYPE_COMMENT,
  TOKEN_TYPE_IDENTIFIER,
  TOKEN_TYPE_PUNCTUATION,
  TOKEN_TYPE_INTEGER_LITERAL,
  TOKEN_TYPE_FLOAT_LITERAL,
  TOKEN_TYPE_STRING_LITERAL,
  TOKEN_TYPE_CHARACTER_LITERAL,
} token_type_t;

typedef struct token_S token_t;

typedef struct tokenizer_result_S tokenizer_result_t;

typedef struct token_or_error_S token_or_error_t;

typedef enum {
  NUMERIC_LITERAL_ENCODING_UNDECIDED,
  NUMERIC_LITERAL_ENCODING_FLOAT_OR_DECIMAL,
  NUMERIC_LITERAL_ENCODING_BINARY,
  NUMERIC_LITERAL_ENCODING_OCTAL,
  NUMERIC_LITERAL_ENCODING_HEX,
  NUMERIC_LITERAL_ENCODING_DECIMAL,
  NUMERIC_LITERAL_ENCODING_FLOAT,
} numeric_literal_encoding_t;

typedef struct token_list_S token_list_t;

typedef struct token_transformer_options_S token_transformer_options_t;

typedef enum {
  PARSE_NODE_UNKNOWN,
  PARSE_NODE_DECLARATIONS,
  PARSE_NODE_ENUM,
  PARSE_NODE_ENUM_ELEMENT,
  PARSE_NODE_FIELD,
  PARSE_NODE_GLOBAL_FUNCTION,
  PARSE_NODE_VARIABLE_DEFINITION,
  PARSE_NODE_LIST_OF_NODES,
  PARSE_NODE_STRUCT,
  PARSE_NODE_UNION,
  PARSE_NODE_TYPE,
  PARSE_NODE_LITERAL,
  PARSE_NODE_FUNCTION,
  PARSE_NODE_FUNCTION_ARGUMENT,
  PARSE_NODE_TYPEDEF,
  PARSE_NODE_UNPARSED_EXPRESSION,
  PARSE_NODE_ATTRIBUTE,
  PARSE_NODE_CPP_INCLUDE,
  PARSE_NODE_CPP_DEFINE,
  PARSE_NODE_OPERATOR,
  PARSE_NODE_IDENTIFIER,
  PARSE_NODE_IF_STATEMENT,
  PARSE_NODE_FOR_STATEMENT,
  PARSE_NODE_DO_STATEMENT,
  PARSE_NODE_WHILE_STATEMENT,
  PARSE_NODE_EMPTY_STATEMENT,
  PARSE_NODE_BLOCK,
  PARSE_NODE_RETURN_STATEMENT,
  PARSE_NODE_SWITCH_STATEMENT,
  PARSE_NODE_CASE_LABEL,
  PARSE_NODE_DEFAULT_LABEL,
  PARSE_NODE_GOTO_STATEMENT,
  PARSE_NODE_BREAK_STATEMENT,
  PARSE_NODE_CONTINUE_STATEMENT,
  PARSE_NODE_LABEL_STATEMENT,
  PARSE_NODE_VARIABLE_STATEMENT,
  PARSE_NODE_EXPRESSION_STATEMENT,
  PARSE_NODE_BALANCED_CONSTRUCT,
  PARSE_NODE_CALL,
  PARSE_NODE_CONDITIONAL,
} parse_node_type_t;

typedef struct parse_node_S parse_node_t;

typedef boolean_t pstatus_t;

typedef struct pstate_t__generated_S pstate_t;

typedef struct declarations_node_S declarations_node_t;

typedef struct typedef_node_S typedef_node_t;

typedef struct function_node_S function_node_t;

typedef struct function_argument_node_S function_argument_node_t;

typedef struct unparsed_expression_S unparsed_expression_t;

typedef struct attribute_node_S attribute_node_t;

typedef struct cpp_include_node_t__generated_S cpp_include_node_t;

typedef struct cpp_define_node_t__generated_S cpp_define_node_t;

typedef struct node_list_S node_list_t;

typedef struct symbol_table_binding_S symbol_table_binding_t;

typedef struct symbol_table_map_S symbol_table_map_t;

typedef struct symbol_table_S symbol_table_t;

typedef struct c_preprocess_options_t__generated_S c_preprocess_options_t;

typedef struct c_preprocessor_directive_range_t__generated_S c_preprocessor_directive_range_t;

typedef enum {
  PRATT_PARSE_UNKNOWN,
  PRATT_PARSE_PREFIX_OPERATOR,
  PRATT_PARSE_BINARY_OPERATOR,
  PRATT_PARSE_POSTFIX_OPERATOR,
  PRATT_PARSE_CONDITIONAL,
  PRATT_PARSE_IDENTIFIER,
  PRATT_PARSE_LITERAL,
  PRATT_PARSE_SUB_EXPRESSION,
  PRATT_PARSE_INDEX_EXPRESSION,
  PRATT_PARSE_SIZEOF,
  PRATT_PARSE_CAST_MACRO,
  PRATT_PARSE_TYPE_OF,
  PRATT_PARSE_BLOCK_EXPR,
  PRATT_PARSE_CALL,
} pratt_parser_operation_t;

typedef enum {
  LEFT_TO_RIGHT,
  RIGHT_TO_LEFT,
} associativity_t;

typedef enum {
  PRECEDENCE_UNKNOWN = 0,
  PRECEDENCE_COMMA = 10,
  PRECEDENCE_ASSIGNMENT = 20,
  PRECEDENCE_CONDITIONAL = 30,
  PRECEDENCE_LOGICAL_OR = 40,
  PRECEDENCE_LOGICAL_AND = 50,
  PRECEDENCE_OR = 60,
  PRECEDENCE_XOR = 70,
  PRECEDENCE_AND = 80,
  PRECEDENCE_EQUALITY = 90,
  PRECEDENCE_RELATIONAL = 100,
  PRECEDENCE_SHIFT = 110,
  PRECEDENCE_ADDITIVE = 120,
  PRECEDENCE_MULTIPICITIVE = 130,
  PRECEDENCE_UNARY = 140,
  PRECEDENCE_POSTFIX = 150,
  PRECEDENCE_PRIMARY = 160,
} precedence_t;

typedef struct pratt_parser_instruction_t__generated_S pratt_parser_instruction_t;

typedef struct identifier_node_t__generated_S identifier_node_t;

typedef struct operator_node_t__generated_S operator_node_t;

typedef struct call_node_t__generated_S call_node_t;

typedef struct conditional_node_t__generated_S conditional_node_t;

typedef struct if_statement_node_t__generated_S if_statement_node_t;

typedef struct for_statement_node_t__generated_S for_statement_node_t;

typedef struct do_statement_node_t__generated_S do_statement_node_t;

typedef struct while_statement_node_t__generated_S while_statement_node_t;

typedef struct empty_statement_node_t__generated_S empty_statement_node_t;

typedef struct block_node_t__generated_S block_node_t;

typedef struct return_statement_node_t__generated_S return_statement_node_t;

typedef struct switch_statement_node_t__generated_S switch_statement_node_t;

typedef struct case_label_node_t__generated_S case_label_node_t;

typedef struct default_label_node_t__generated_S default_label_node_t;

typedef struct goto_statement_node_t__generated_S goto_statement_node_t;

typedef struct break_statement_node_t__generated_S break_statement_node_t;

typedef struct continue_statement_node_t__generated_S continue_statement_node_t;

typedef struct label_statement_node_t__generated_S label_statement_node_t;

typedef struct variable_statement_node_t__generated_S variable_statement_node_t;

typedef struct expression_statement_node_t__generated_S expression_statement_node_t;

typedef enum {
  TYPE_QUALIFIER_NONE = 0,
  TYPE_QUALIFIER_CONST = 1,
  TYPE_QUALIFIER_VOLATILE = 2,
  TYPE_QUALIFIER_RESTRICT = 4,
} type_qualifier_t;

typedef enum {
  TYPE_NODE_KIND_UNKNOWN,
  TYPE_NODE_KIND_POINTER,
  TYPE_NODE_KIND_ARRAY,
  TYPE_NODE_KIND_SIZED_ARRAY,
  TYPE_NODE_KIND_VARIABLE_SIZED_ARRAY,
  TYPE_NODE_KIND_PRIMITIVE_TYPENAME,
  TYPE_NODE_KIND_TYPENAME,
  TYPE_NODE_KIND_TYPE_EXPRESSION,
  TYPE_NODE_KIND_TYPEOF,
} type_node_kind_t;

typedef struct type_node_S type_node_t;

typedef struct canonical_type_result_s canonical_type_result_t;

typedef struct struct_node_S struct_node_t;

typedef struct union_node_S union_node_t;

typedef struct field_node_S field_node_t;

typedef struct enum_node_S enum_node_t;

typedef struct enum_element_S enum_element_t;

typedef struct variable_definition_node_S variable_definition_node_t;

typedef struct literal_node_S literal_node_t;

typedef struct balanced_construct_node_S balanced_construct_node_t;

typedef struct printer_S printer_t;

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

struct token_S {
  buffer_t* buffer;
  token_type_t type;
  int32_t start;
  int32_t end;
  int32_t line_number;
  int32_t column_number;
  boolean_t is_cpp_token;
};

struct token_or_error_S {
  token_t token;
  tokenizer_error_t error_code;
  uint64_t error_position;
};

struct parse_node_S {
  parse_node_type_t tag;
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

struct file_S {
  file_tag_t tag;
  char* file_name;
  buffer_t* data;
};

struct compiler_error_S {
  uint64_t error_position;
  tokenizer_error_t tokenizer_error_code;
  parse_error_code_t parse_error_code;
  struct token_S* error_token;
  char* file_name;
};

struct src_code_snippets_t__generated_S {
  char* previous_lines;
  char* current_line;
  char* next_lines;
};

struct tokenizer_result_S {
  value_array_t* tokens;
  uint64_t tokenizer_error_position;
  tokenizer_error_t tokenizer_error_code;
};

struct token_list_S {
  value_array_t* list;
};

struct token_transformer_options_S {
  boolean_t keep_whitespace;
  boolean_t keep_javadoc_comments;
  boolean_t keep_comments;
  boolean_t keep_c_preprocessor_lines;
};

struct pstate_t__generated_S {
  value_array_t* tokens;
  uint64_t position;
  boolean_t use_statement_parser;
  parse_node_t* result_node;
  token_t* result_token;
  compiler_error_t error;
};

struct node_list_S {
  value_array_t* list;
};

struct declarations_node_S {
  parse_node_type_t tag;
  node_list_t declarations;
};

struct typedef_node_S {
  parse_node_type_t tag;
  token_t* name;
  type_node_t* type_node;
};

struct function_node_S {
  parse_node_type_t tag;
  node_list_t attributes;
  token_t* storage_class_specifier;
  token_list_t function_specifiers;
  type_node_t* return_type;
  token_t* function_name;
  node_list_t function_args;
  parse_node_t* body;
};

struct function_argument_node_S {
  parse_node_type_t tag;
  type_node_t* arg_type;
  token_t* arg_name;
  boolean_t is_var_args;
};

struct unparsed_expression_S {
  parse_node_type_t tag;
  token_t* first_token;
  token_t* last_token;
};

struct attribute_node_S {
  parse_node_type_t tag;
  token_t* inner_start_token;
  token_t* inner_end_token;
};

struct cpp_include_node_t__generated_S {
  parse_node_type_t tag;
  char* text;
};

struct cpp_define_node_t__generated_S {
  parse_node_type_t tag;
  char* text;
};

struct symbol_table_binding_S {
  char* key_string;
  value_array_t* definition_nodes;
  boolean_t visited;
};

struct symbol_table_map_S {
  string_hashtable_t* ht;
  value_array_t* ordered_bindings;
};

struct symbol_table_S {
  struct symbol_table_S* parent;
  value_array_t* system_includes;
  value_array_t* user_includes;
  value_array_t* defines;
  symbol_table_map_t* enums;
  symbol_table_map_t* typedefs;
  symbol_table_map_t* structures;
  symbol_table_map_t* variables;
  symbol_table_map_t* functions;
};

struct c_preprocess_options_t__generated_S {
  boolean_t keep_system_includes;
  boolean_t keep_user_includes;
};

struct c_preprocessor_directive_range_t__generated_S {
  uint64_t token_start_position;
  uint64_t token_end_position;
  uint64_t buffer_start_position;
  uint64_t buffer_end_position;
  buffer_t* buffer;
};

struct pratt_parser_instruction_t__generated_S {
  token_t* token;
  pratt_parser_operation_t operation;
  precedence_t precedence;
};

struct identifier_node_t__generated_S {
  parse_node_type_t tag;
  token_t* token;
};

struct operator_node_t__generated_S {
  parse_node_type_t tag;
  token_t* operator;
  parse_node_t* left;
  parse_node_t* right;
};

struct call_node_t__generated_S {
  parse_node_type_t tag;
  parse_node_t* function;
  node_list_t args;
};

struct conditional_node_t__generated_S {
  parse_node_type_t tag;
  parse_node_t* condition;
  parse_node_t* expr_if_true;
  parse_node_t* expr_if_false;
};

struct if_statement_node_t__generated_S {
  parse_node_type_t tag;
  parse_node_t* if_condition;
  parse_node_t* if_true;
  parse_node_t* if_else;
};

struct for_statement_node_t__generated_S {
  parse_node_type_t tag;
  parse_node_t* for_init;
  parse_node_t* for_test;
  parse_node_t* for_increment;
  parse_node_t* for_body;
};

struct do_statement_node_t__generated_S {
  parse_node_type_t tag;
  parse_node_t* body;
  parse_node_t* condition;
};

struct while_statement_node_t__generated_S {
  parse_node_type_t tag;
  parse_node_t* condition;
  parse_node_t* body;
};

struct empty_statement_node_t__generated_S {
  parse_node_type_t tag;
  token_t* semi_colon_token;
};

struct block_node_t__generated_S {
  parse_node_type_t tag;
  node_list_t statements;
};

struct return_statement_node_t__generated_S {
  parse_node_type_t tag;
  parse_node_t* expression;
};

struct switch_statement_node_t__generated_S {
  parse_node_type_t tag;
  parse_node_t* expression;
  parse_node_t* block;
};

struct case_label_node_t__generated_S {
  parse_node_type_t tag;
  parse_node_t* expression;
};

struct default_label_node_t__generated_S {
  parse_node_type_t tag;
  token_t* default_token;
};

struct goto_statement_node_t__generated_S {
  parse_node_type_t tag;
  token_t* label;
};

struct break_statement_node_t__generated_S {
  parse_node_type_t tag;
  token_t* break_keyword_token;
};

struct continue_statement_node_t__generated_S {
  parse_node_type_t tag;
  token_t* continue_keyword_token;
};

struct label_statement_node_t__generated_S {
  parse_node_type_t tag;
  token_t* label;
};

struct variable_statement_node_t__generated_S {
  parse_node_type_t tag;
  parse_node_t* expression;
};

struct expression_statement_node_t__generated_S {
  parse_node_type_t tag;
  parse_node_t* expression;
};

struct type_node_S {
  parse_node_type_t tag;
  type_node_kind_t type_node_kind;
  type_qualifier_t qualifiers;
  token_t* type_name;
  parse_node_t* user_type;
  node_list_t type_args;
};

struct canonical_type_result_s {
  token_t* canonical_type;
  int consumed_tokens;
};

struct struct_node_S {
  parse_node_type_t tag;
  token_t* name;
  node_list_t fields;
  boolean_t partial_definition;
};

struct union_node_S {
  parse_node_type_t tag;
  token_t* name;
  node_list_t fields;
  boolean_t partial_definition;
};

struct field_node_S {
  parse_node_type_t tag;
  type_node_t* type;
  token_t* name;
  token_t* bit_field_width;
};

struct enum_node_S {
  parse_node_type_t tag;
  token_t* name;
  node_list_t elements;
  boolean_t partial_definition;
};

struct enum_element_S {
  parse_node_type_t tag;
  token_t* name;
  token_t* value;
};

struct variable_definition_node_S {
  parse_node_type_t tag;
  boolean_t is_global;
  token_t* storage_class_specifier;
  type_node_t* type;
  token_t* name;
  parse_node_t* value;
  value_array_t* suffixes;
};

struct literal_node_S {
  parse_node_type_t tag;
  token_t* token;
  value_array_t* tokens;
  parse_node_t* initializer_node;
  parse_node_t* initializer_type;
};

struct balanced_construct_node_S {
  parse_node_type_t tag;
  token_t* start_token;
  token_t* end_token;
};

struct printer_S {
  buffer_t* buffer;
  uint32_t indent_width;
  uint32_t indent_level;
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

char* c_keywords_array[] = {
    /* C89/C90 Keywords */
    "auto", "break", "case", "char", "const", "continue", "default", "do",
    "double", "else", "enum", "extern", "float", "for", "goto", "if", "int",
    "long", "register", "return", "short", "signed", "sizeof", "static",
    "struct", "switch", "typedef", "union", "unsigned", "void", "volatile",
    "while",

    /* C99 Keywords */
    "_Bool", "_Complex", "_Imaginary", "inline", "restrict",

    /* C11 Keywords */
    "_Alignas", "_Atomic", "_Generic", "_Noreturn", "_Static_assert",
    "_Thread_local",

    /* C23 Keywords */
    "true", "false", "nullptr"};

char* c_builtin_types_array[] = {
    "char",   // Single character
    "short",  // Short integer
    "int",    // Integer
    "long",   // Long integer
    "float",  // Single-precision floating-point
    "double", // Double-precision floating-point
    "void",   // Represents no value or type

    // Modifiers (Can be combined with core types)
    "signed", "unsigned",

    // C99 and later
    "_Bool",     // Boolean (true or false)
    "_Complex",  // Complex number representation
    "_Imaginary" // Imaginary number representation
};

char* cpp_keywords_array[] = {
    // C++98 Keywords (includes most C keywords)
    "asm",      // Inline assembly
    "class",    // Define a class
    "new",      // Dynamic memory allocation
    "delete",   // Dynamic memory deallocation
    "template", // Templates (generic programming)
    "typename", // Used in templates
    "this",     // Pointer to current object
    "try",      // Start of try block (exception handling)
    "catch",    // Start of catch block (exception handling)
    "throw",    // Throw an exception

    // C++11 Keywords
    "nullptr",   // Null pointer literal
    "constexpr", // Constant expressions
    "decltype",  // Deduce type
    "noexcept",  // Function doesn't throw exceptions
    "override",  // Explicitly override virtual function
    "final",     // Prevent virtual function overriding/class inheritance
    "default",   // Used in switch, or to request default constructor/destructor
    "auto",      // Automatic type deduction

    // C++14 Keywords
    "declval", // Used in SFINAE (complicated, advanced usage)

    // C++17 Keywords
    "static_assert", // Compile-time assertions
    "inline",        // Can now be used for variables

    // C++20 Keywords
    "char8_t",   // Type for UTF-8 characters
    "concept",   // Define constraints for templates
    "requires",  // Specify requirements for template parameters
    "coroutine", // Keyword related to coroutines (advanced)
    "co_await",  // Used with coroutines
    "co_yield",  // Used with coroutines
    "co_return", // Used with coroutines
};

string_hashtable_t* c_keywords_ht = NULL;

string_hashtable_t* c_builtin_types_ht = NULL;

string_hashtable_t* cpp_keywords_ht = NULL;

string_hashtable_t* cpp_builtin_types_ht = NULL;

string_hashtable_t* oc_keywords_ht = NULL;

string_hashtable_t* oc_builtin_types_ht = NULL;

char* formatted_snippet = "======================================================================\n" "{error-prefix-lines}" "{error-highlight-on}" "{error-current-line}" "{error-highlight-off}" "{error-suffix-lines}" "\n======================================================================" "\n";

char* parse_error_unknown = "A parse error has occurred but the error message is unavailable.\n" "{formatted_snippet}";

char* error_field_width_or_semicolon = "A parse error has occurred while trying to read a structure or union " "field.\n" "{formatted_snippet}" "Expected a field width or a semi-colon.";

char* error_open_brace_expected = "A parse error has occurred while trying to parse after an opening '[' " "character. Expected a closing ']'.\n" "{formatted_snippet}";

char* error_open_semicolon_expected = "A parse error has occurred since a semicolon was expected.\n" "{formatted_snippet}";

char* error_unrecognized_top_level_declaration = "Unable to parse a top-level declaration.\n" "{formatted_snippet}" "\nVariables, functions, " "structures, unions, and typedefs are currently supported.\n\n" "(Additionally, C pre-processor directives are allowed but " "are currently skipped before parsing.)";

char* error_conflicting_storage_class_specifier = "Conflicting storage class specifiers.\n" "{formatted_snippet}" "\nA storage class specifier is either static, extern, auto, or " "register\n" "For each 'declaration', only one of them is allowed.";

char* error_conflicting_function_specifier = "Conflicting function specifier.\n" "{formatted_snippet}" "\nThe only known function specifier is inline so you likely repeated " "it.";

char* error_expected_open_paren_for_underscore_attribute = "This doesn't look like a valid attribute specification.\n" "{formatted_snippet}" "\nAttributes roughly look like __attribute__((...))\n" "\nYou might have unbalanced parens...";

char* error_expected_matching_close_paren_after_underscore_attribute = "This doesn't look like a valid attribute specification.\n" "{formatted_snippet}" "\nAttributes roughly look like __attribute__((...))\n" "\nYou might have unbalanced parens...";

char* c_punctuation[] = {
    "!=",
    "%=",
    "&&",
    "&=",
    "*=",
    "++",
    "+=",
    "--",
    "-=",
    "->",
    "...",
    "/=",
    "::",
    "<<=",
    "<=",
    "==",
    ">=",
    ">>=",
    "[[",
    "]]",
    "^=",
    "|=",
    "||",
    "<<",
    ">>",

    // Single character tokens should come after any two character
    // token that starts with the same character.

    "!",
    "%",
    "&",
    "(",
    ")",
    "*",
    "+",
    ",",
    "-",
    ".",
    "/",
    ":",
    ";",
    ";",
    "<",
    "=",
    ">",
    "?",
    "[",
    "]",
    "^",
    "{",
    "|",
    "}",
    "~",

    // *****************************************************************
    // Hopefully enough to parse C preprocessor stuff (after going
    // through the token-transformer...)
    // *****************************************************************
    "#",
    // This won't work forever... We special case this in
    // transform_tokens()...
    "\\\n",
};

value_array_t* FLAG_files = NULL;

char* FLAG_command = NULL;

boolean_t FLAG_print_command_line = true;

boolean_t FLAG_include_unnamed_nodes = false;

boolean_t FLAG_print_tokens_show_tokens = false;

boolean_t FLAG_print_tokens_include_whitespace = false;

boolean_t FLAG_print_tokens_include_comments = false;

boolean_t FLAG_print_tokens_parse_and_print = true;

boolean_t FLAG_print_tokens_show_appended_tokens = true;

char* FLAG_ouput_file = NULL;

boolean_t FLAG_generate_enum_convertors = true;

char* FLAG_expression = NULL;

char* FLAG_statement = NULL;

boolean_t FLAG_dump_symbol_table = false;

boolean_t FLAG_use_statement_parser = false;

boolean_t FLAG_to_c = false;

boolean_t FLAG_omit_c_armyknife_include = false;

boolean_t FLAG_unique_prototype_header_files = false;

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
void initialize_keyword_maps(void);
boolean_t is_reserved_word(input_mode_t mode, char* str);
boolean_t is_builtin_type_name(input_mode_t mode, char* str);
value_array_t* read_files(value_array_t* files);
file_t* read_file(char* file_name);
buffer_t* buffer_append_human_readable_error(buffer_t* buffer, compiler_error_t* error);
src_code_snippets_t get_source_code_snippet(buffer_t* buffer, uint64_t location, int before_lines, int after_lines);
char* do_common_replacements(char* template, compiler_error_t* error);
buffer_t* buffer_append_human_readable_tokenizer_error(buffer_t* buffer, compiler_error_t* error);
buffer_t* buffer_append_human_readable_parser_error(buffer_t* buffer, compiler_error_t* error);
char* token_type_to_string(token_type_t type);
char* token_to_string(token_t* token);
token_t* make_derived_token(token_t* source_token);
__attribute__((warn_unused_result)) buffer_t* append_token_debug_string(buffer_t* buffer, token_t token);
buffer_t* buffer_append_token_string(buffer_t* buffer, token_t* token);
token_or_error_t tokenize_whitespace(buffer_t* buffer, uint64_t start_position);
boolean_t is_identifier_start(uint32_t code_point);
token_or_error_t tokenize_identifier(buffer_t* buffer, uint64_t start_position);
token_or_error_t tokenize_numeric(buffer_t* buffer, uint64_t start_position);
boolean_t can_extend_number(numeric_literal_encoding_t encoding, uint32_t code_point, uint32_t previous_code_point);
token_or_error_t tokenize_punctuation(buffer_t* buffer, uint64_t start_position);
boolean_t is_comment_start(buffer_t* buffer, uint64_t position);
token_or_error_t tokenize_comment(buffer_t* buffer, uint64_t start_position);
boolean_t is_string_literal_start(buffer_t* buffer, uint64_t position);
token_or_error_t tokenize_quoted_literal_common(buffer_t* buffer, uint64_t start_position, char* opening_sequence, char* quoted_closing_sequence, char* closing_sequence, token_type_t token_type, tokenizer_error_t unterminated_error_code);
token_or_error_t tokenize_string_literal(buffer_t* buffer, uint64_t start_position);
boolean_t is_character_literal_start(buffer_t* buffer, uint64_t position);
token_or_error_t tokenize_character_literal(buffer_t* buffer, uint64_t start_position);
tokenizer_result_t tokenize(buffer_t* buffer);
value_array_t* transform_tokens(value_array_t* tokens, token_transformer_options_t xform_options);
pstatus_t pstate_error(pstate_t* pstate, uint64_t saved_position, parse_error_code_t parse_error_code);
pstate_t* pstate_ignore_error(pstate_t* pstate);
pstatus_t pstate_propagate_error(pstate_t* pstate, uint64_t saved_position);
pstatus_t pstate_set_result_token(pstate_t* pstate, token_t* token);
pstatus_t pstate_set_result_node(pstate_t* pstate, parse_node_t* node);
token_t* pstate_get_result_token(pstate_t* pstate);
parse_node_t* pstate_get_result_node(pstate_t* pstate);
parse_node_t* pstate_get_optional_result_node(pstate_t* pstate);
token_t* pstate_peek(pstate_t* pstate, int offset);
token_t* pstate_advance(pstate_t* pstate);
boolean_t pstate_match_token_string(pstate_t* pstate, char* token_string);
pstatus_t pstate_expect_token_string(pstate_t* pstate, char* token_string);
pstatus_t pstate_expect_token_type(pstate_t* pstate, token_type_t token_type);
pstatus_t parse_declarations(pstate_t* pstate);
pstatus_t parse_declaration(pstate_t* pstate);
pstatus_t parse_enum_node_declaration(pstate_t* pstate);
pstatus_t parse_structure_node_declaration(pstate_t* pstate);
pstatus_t parse_union_node_declaration(pstate_t* pstate);
pstatus_t parse_attribute_node(pstate_t* pstate);
pstatus_t parse_function_node(pstate_t* pstate);
pstatus_t parse_function_argument_node(pstate_t* pstate);
pstatus_t parse_function_body_node(pstate_t* pstate);
pstatus_t parse_typedef_node(pstate_t* pstate);
void buffer_append_dbg_parse_node(cdl_printer_t* printer, parse_node_t* node);
void buffer_append_dbg_node_list(cdl_printer_t* printer, node_list_t list);
void buffer_append_dbg_tokens(cdl_printer_t* printer, value_array_t* tokens, char* field_name);
void buffer_append_dbg_declarations(cdl_printer_t* printer, declarations_node_t* node);
void buffer_append_dbg_enum(cdl_printer_t* printer, enum_node_t* node);
void buffer_append_dbg_struct_node(cdl_printer_t* printer, struct_node_t* node);
void buffer_append_dbg_enum_element(cdl_printer_t* printer, enum_element_t* node);
void buffer_append_dbg_field_node(cdl_printer_t* printer, field_node_t* node);
void buffer_append_dbg_type_node(cdl_printer_t* printer, type_node_t* node);
void buffer_append_dbg_literal_node(cdl_printer_t* printer, literal_node_t* node);
void buffer_append_dbg_function_node(cdl_printer_t* printer, function_node_t* node);
void buffer_append_dbg_function_argument_node(cdl_printer_t* printer, function_argument_node_t* node);
void buffer_append_dbg_balanced_construct_node(cdl_printer_t* printer, balanced_construct_node_t* node);
void buffer_append_dbg_typedef_node(cdl_printer_t* printer, typedef_node_t* node);
void buffer_append_dbg_variable_definition_node(cdl_printer_t* printer, variable_definition_node_t* node);
void buffer_append_dbg_attribute_node(cdl_printer_t* printer, attribute_node_t* node);
void buffer_append_dbg_empty_statement_node(cdl_printer_t* printer, empty_statement_node_t* node);
void buffer_append_dbg_block_node(cdl_printer_t* printer, block_node_t* node);
void buffer_append_dbg_if_node(cdl_printer_t* printer, if_statement_node_t* node);
void buffer_append_dbg_while_node(cdl_printer_t* printer, while_statement_node_t* node);
void buffer_append_dbg_for_node(cdl_printer_t* printer, for_statement_node_t* node);
void buffer_append_dbg_do_node(cdl_printer_t* printer, do_statement_node_t* node);
void buffer_append_dbg_break_statement_node(cdl_printer_t* printer, break_statement_node_t* node);
void buffer_append_dbg_continue_statement_node(cdl_printer_t* printer, continue_statement_node_t* node);
void buffer_append_dbg_label_statement_node(cdl_printer_t* printer, label_statement_node_t* node);
void buffer_append_dbg_case_label_node(cdl_printer_t* printer, case_label_node_t* node);
void buffer_append_dbg_default_label_node(cdl_printer_t* printer, default_label_node_t* node);
void buffer_append_dbg_return_statement_node(cdl_printer_t* printer, return_statement_node_t* node);
void buffer_append_dbg_expression_statement_node(cdl_printer_t* printer, expression_statement_node_t* node);
void buffer_append_dbg_identifier_node(cdl_printer_t* printer, identifier_node_t* node);
void buffer_append_dbg_operator_node(cdl_printer_t* printer, operator_node_t* node);
void buffer_append_dbg_call_node(cdl_printer_t* printer, call_node_t* node);
void buffer_append_dbg_conditional_node(cdl_printer_t* printer, conditional_node_t* node);
void buffer_append_dbg_switch_node(cdl_printer_t* printer, switch_statement_node_t* node);
void debug_append_tokens(buffer_t* buffer, value_array_t* tokens);
printer_t* append_parse_node(printer_t* printer, parse_node_t* node);
printer_t* append_c_function_node_prefix(printer_t* printer, function_node_t* node);
printer_t* append_c_function_node_prototype(printer_t* printer, function_node_t* node);
printer_t* append_balanced_construct_node(printer_t* printer, balanced_construct_node_t* node);
printer_t* append_c_function_node_and_body(printer_t* printer, function_node_t* node);
printer_t* append_c_function_argument_node(printer_t* printer, function_argument_node_t* node);
printer_t* append_type_node(printer_t* printer, type_node_t* node);
printer_t* append_fn_type_node(printer_t* printer, type_node_t* node);
printer_t* append_c_attribute_node(printer_t* printer, attribute_node_t* node);
printer_t* append_c_raw_token_span(printer_t* printer, token_t* start_token, token_t* end_token);
printer_t* append_enum_node(printer_t* printer, enum_node_t* node);
printer_t* append_enum_element(printer_t* printer, enum_element_t* node);
printer_t* append_enum_to_string(printer_t* printer, enum_node_t* node, char* to_string_fn_prefix, char* type_string);
printer_t* append_string_to_enum(printer_t* printer, enum_node_t* node, char* to_string_fn_prefix, char* type_string);
printer_t* append_field_node(printer_t* printer, field_node_t* node);
printer_t* append_struct_node(printer_t* printer, struct_node_t* node);
printer_t* append_typedef_node(printer_t* printer, typedef_node_t* node);
printer_t* append_cpp_include_node(printer_t* printer, cpp_include_node_t* node);
printer_t* append_cpp_define_node(printer_t* printer, cpp_define_node_t* node);
printer_t* append_variable_definition_node(printer_t* printer, variable_definition_node_t* node, boolean_t is_library);
printer_t* append_literal_node(printer_t* printer, literal_node_t* node);
printer_t* append_identifier_node(printer_t* printer, identifier_node_t* node);
printer_t* append_empty_statement_node(printer_t* printer, empty_statement_node_t* node);
printer_t* append_break_statement_node(printer_t* printer, break_statement_node_t* node);
printer_t* append_continue_statement_node(printer_t* printer, continue_statement_node_t* node);
printer_t* append_label_statement_node(printer_t* printer, label_statement_node_t* node);
printer_t* append_case_label_node(printer_t* printer, case_label_node_t* node);
printer_t* append_default_label_node(printer_t* printer, default_label_node_t* node);
printer_t* append_expression_statement_node(printer_t* printer, expression_statement_node_t* node);
printer_t* append_block_node(printer_t* printer, block_node_t* node);
printer_t* append_if_statement_node(printer_t* printer, if_statement_node_t* node);
printer_t* append_while_statement_node(printer_t* printer, while_statement_node_t* node);
printer_t* append_switch_statement_node(printer_t* printer, switch_statement_node_t* node);
printer_t* append_for_statement_node(printer_t* printer, for_statement_node_t* node);
printer_t* append_do_statement_node(printer_t* printer, do_statement_node_t* node);
printer_t* append_return_statement_node(printer_t* printer, return_statement_node_t* node);
printer_t* append_operator_node(printer_t* printer, operator_node_t* node);
printer_t* append_conditional_node(printer_t* printer, conditional_node_t* node);
printer_t* append_call_node(printer_t* printer, call_node_t* node);
buffer_t* buffer_append_enum_metadata(buffer_t* buffer, enum_node_t* node, char* fn_prefix, char* type_string);
symbol_table_map_t* make_symbol_table_map(void);
symbol_table_t* make_symbol_table(void);
symbol_table_binding_t* symbol_table_map_get(symbol_table_map_t* map, char* key_string);
parse_node_t* symbol_table_map_get_only_definition(symbol_table_map_t* map, char* key_string);
void symbol_table_add_declaration_node(symbol_table_map_t* map, char* key_string, parse_node_t* node);
void symbol_table_add_declartions(symbol_table_t* symbol_table, declarations_node_t* root);
buffer_t* symbol_table_stats(buffer_t* buffer, symbol_table_t* symbol_table);
buffer_t* buffer_append_dgb_binding(buffer_t* buffer, symbol_table_binding_t* binding);
buffer_t* buffer_appennd_dbg_symbol_table_map(buffer_t* buffer, symbol_table_map_t* symbol_table_map);
buffer_t* buffer_append_dgb_symbol_table(buffer_t* buffer, symbol_table_t* symbol_table);
token_t* generate_struct_name_from_typedef_name(token_t* name);
void split_structure_typedefs(symbol_table_t* symbol_table);
void reorder_symbol_table_typedefs(symbol_table_t* symbol_table);
void reorder_symbol_table_typedefs__process_binding(symbol_table_map_t* typedefs, symbol_table_binding_t* binding, value_array_t* reordered_bindings);
struct_node_t* get_full_structure_definition_node(symbol_table_binding_t* binding);
symbol_table_binding_t* resolve_typename_to_structure_binding(symbol_table_t* symbol_table, type_node_t* type_node);
void reorder_symbol_table_structures_process_binding(symbol_table_t* symbol_table, symbol_table_binding_t* binding, value_array_t* reordered_bindings);
void reorder_symbol_table_structures(symbol_table_t* symbol_table);
void convert_nullptr_to_null(value_array_t* tokens);
c_preprocessor_directive_range_t mark_c_preprocessor_directive(c_preprocess_options_t options, value_array_t* tokens, uint64_t start_position);
uint64_t handle_c_preprocessor_directive(c_preprocess_options_t options, symbol_table_t* symbol_table, value_array_t* tokens, uint64_t start_position);
void handle_c_preprocessor_directives(c_preprocess_options_t options, symbol_table_t* symbol_table, value_array_t* tokens);
__attribute__((warn_unused_result)) buffer_t* extract_enums_process_declarations(buffer_t* output, declarations_node_t* root);
__attribute__((warn_unused_result)) buffer_t* extract_prototypes_process_declarations(buffer_t* output, declarations_node_t* root);
void parse_and_add_top_level_definitions(symbol_table_t* symbol_table, value_array_t* file_names, boolean_t use_statement_parser);
void symbol_table_parse_buffer(symbol_table_t* symbol_table, buffer_t* buffer, char* file_name, boolean_t use_statement_parser);
void srcgen_enum_to_string_converters(symbol_table_t* symbol_table);
pstatus_t parse_block(pstate_t* pstate);
pstatus_t pratt_parse_expression(pstate_t* pstate, int precedence);
pstatus_t pratt_handle_instruction(pstate_t* pstate, pratt_parser_instruction_t instruction, parse_node_t* left);
pratt_parser_instruction_t get_prefix_instruction(token_t* token);
pratt_parser_instruction_t get_infix_instruction(token_t* token);
associativity_t precedence_to_associativity(precedence_t precedence);
pstatus_t parse_statement(pstate_t* pstate);
pstatus_t parse_return_statement(pstate_t* pstate);
pstatus_t parse_if_statement(pstate_t* pstate);
pstatus_t parse_while_statement(pstate_t* pstate);
pstatus_t parse_do_statement(pstate_t* pstate);
pstatus_t parse_for_statement(pstate_t* pstate);
pstatus_t parse_switch_statement(pstate_t* pstate);
pstatus_t parse_case_label(pstate_t* pstate);
pstatus_t parse_expression_statement(pstate_t* pstate);
pstatus_t parse_goto_statement(pstate_t* pstate);
pstatus_t parse_break_statement(pstate_t* pstate);
pstatus_t parse_continue_statement(pstate_t* pstate);
pstatus_t parse_label_statement(pstate_t* pstate);
pstatus_t parse_default_label(pstate_t* pstate);
pstatus_t parse_empty_statement(pstate_t* pstate);
break_statement_node_t* make_break_statement(token_t* break_keyword_token);
break_statement_node_t* to_break_statement_node(parse_node_t* ptr);
continue_statement_node_t* make_continue_statement(token_t* keyword_token);
continue_statement_node_t* to_continue_statement_node(parse_node_t* ptr);
label_statement_node_t* make_label_statement(token_t* label);
label_statement_node_t* to_label_statement_node(parse_node_t* ptr);
goto_statement_node_t* make_goto_statement(token_t* label);
goto_statement_node_t* to_goto_statement_node(parse_node_t* ptr);
empty_statement_node_t* make_empty_statement(token_t* semi_colon_token);
empty_statement_node_t* to_empty_statement_node(parse_node_t* ptr);
switch_statement_node_t* make_switch_statement(parse_node_t* expression, parse_node_t* block);
switch_statement_node_t* to_switch_statement_node(parse_node_t* ptr);
case_label_node_t* make_case_label(parse_node_t* expression);
case_label_node_t* to_case_label_node(parse_node_t* ptr);
default_label_node_t* make_default_label(token_t* default_token);
default_label_node_t* to_default_label_node(parse_node_t* ptr);
block_node_t* make_block_node();
block_node_t* to_block_node(parse_node_t* ptr);
for_statement_node_t* make_for_statement(parse_node_t* for_init, parse_node_t* for_test, parse_node_t* for_increment, parse_node_t* for_body);
for_statement_node_t* to_for_statement_node(parse_node_t* ptr);
if_statement_node_t* make_if_statement(parse_node_t* if_condition, parse_node_t* if_true, parse_node_t* if_else);
if_statement_node_t* to_if_statement_node(parse_node_t* ptr);
expression_statement_node_t* make_expression_statement_node(parse_node_t* expression);
expression_statement_node_t* to_expression_statement_node(parse_node_t* ptr);
return_statement_node_t* make_return_statement(parse_node_t* expression);
return_statement_node_t* to_return_statement_node(parse_node_t* ptr);
while_statement_node_t* make_while_statement(parse_node_t* condition, parse_node_t* body);
while_statement_node_t* to_while_statement_node(parse_node_t* ptr);
do_statement_node_t* make_do_statement(parse_node_t* body, parse_node_t* condition);
do_statement_node_t* to_do_statement_node(parse_node_t* ptr);
pstatus_t parse_typeof_node(pstate_t* pstate);
pstatus_t parse_user_type_node(pstate_t* pstate);
pstatus_t parse_enum_node(pstate_t* pstate);
pstatus_t parse_structure_node(pstate_t* pstate);
pstatus_t parse_union_node(pstate_t* pstate);
pstatus_t parse_type_node(pstate_t* pstate);
canonical_type_result_t make_type_token_result(char* str, int consumed_tokens);
canonical_type_result_t parse_canonical_type(pstate_t* pstate);
pstatus_t parse_function_type(pstate_t* pstate);
pstatus_t parse_function_type_argument(pstate_t* pstate);
pstatus_t parse_field_node(pstate_t* pstate);
pstatus_t parse_enum_element_node(pstate_t* pstate);
pstatus_t parse_expression(pstate_t* pstate);
pstatus_t parse_initializer(pstate_t* pstate);
pstatus_t parse_variable_definition_node(pstate_t* pstate);
pstatus_t parse_literal_node(pstate_t* pstate);
pstatus_t parse_balanced_construct(pstate_t* pstate);
printer_t* make_printer(buffer_t* buffer, int indent_width);
printer_t* append_string(printer_t* printer, char* string);
printer_t* append_token(printer_t* printer, token_t* token);
printer_t* printer_newline(printer_t* printer);
printer_t* printer_space(printer_t* printer);
printer_t* printer_indent(printer_t* printer);
printer_t* printer_increase_indent(printer_t* printer);
printer_t* printer_decrease_indent(printer_t* printer);
void do_print_tokens(value_array_t* tokens, char* message);
void print_tokens(void);
void extract_command(char* command);
void configure_flags(void);
void configure_parse_expression(void);
void configure_parse_statement(void);
void configure_print_tokens_command(void);
void configure_extract_enums_command(void);
void configure_extract_prototypes_command(void);
void configure_generate_c_output_file(void);
boolean_t is_inlined_function(function_node_t* node);
void dump_symbol_table(char* phase_name, symbol_table_t* symbol_table);
buffer_t* get_reflection_header_buffer(void);
char* include_node_to_string(cpp_include_node_t* node);
void generate_c_output_file(boolean_t is_library);
void parse_expression_string_and_print_parse_tree(char* expression);
void parse_statement_string_and_print_parse_tree(char* expression);
int main(int argc, char** argv);
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
char* input_mode_to_string(input_mode_t value);
input_mode_t string_to_input_mode(char* value);
enum_metadata_t* input_mode_metadata();
char* output_mode_to_string(output_mode_t value);
output_mode_t string_to_output_mode(char* value);
enum_metadata_t* output_mode_metadata();
char* file_tag_to_string(file_tag_t value);
file_tag_t string_to_file_tag(char* value);
enum_metadata_t* file_tag_metadata();
char* tokenizer_error_to_string(tokenizer_error_t value);
tokenizer_error_t string_to_tokenizer_error(char* value);
enum_metadata_t* tokenizer_error_metadata();
char* parse_error_code_to_string(parse_error_code_t value);
parse_error_code_t string_to_parse_error_code(char* value);
enum_metadata_t* parse_error_code_metadata();
token_type_t string_to_token_type(char* value);
enum_metadata_t* token_type_metadata();
char* numeric_literal_encoding_to_string(numeric_literal_encoding_t value);
numeric_literal_encoding_t string_to_numeric_literal_encoding(char* value);
enum_metadata_t* numeric_literal_encoding_metadata();
char* parse_node_type_to_string(parse_node_type_t value);
parse_node_type_t string_to_parse_node_type(char* value);
enum_metadata_t* parse_node_type_metadata();
char* pratt_parser_operation_to_string(pratt_parser_operation_t value);
pratt_parser_operation_t string_to_pratt_parser_operation(char* value);
enum_metadata_t* pratt_parser_operation_metadata();
char* associativity_to_string(associativity_t value);
associativity_t string_to_associativity(char* value);
enum_metadata_t* associativity_metadata();
char* precedence_to_string(precedence_t value);
precedence_t string_to_precedence(char* value);
enum_metadata_t* precedence_metadata();
char* type_qualifier_to_string(type_qualifier_t value);
type_qualifier_t string_to_type_qualifier(char* value);
enum_metadata_t* type_qualifier_metadata();
char* type_node_kind_to_string(type_node_kind_t value);
type_node_kind_t string_to_type_node_kind(char* value);
enum_metadata_t* type_node_kind_metadata();

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

static inline void maybe_initialize_keyword_maps(void){
  if ((c_keywords_ht==NULL))
  {
    initialize_keyword_maps();
  }
}

static inline token_t* token_at(value_array_t* tokens, uint64_t position){
  if ((position>=(tokens->length)))
  {
    return NULL;
  }
  return value_array_get_ptr(tokens, position, typeof(token_t*));
}

static inline boolean_t token_matches(token_t* token, char* str){
  if ((token==NULL))
  {
    return false;
  }
  int str_len = strlen(str);
  return ((str_len==((token->end)-(token->start)))&&buffer_match_string_at((token->buffer), (token->start), str));
}

static inline boolean_t token_starts_with(token_t* token, char* str){
  return buffer_match_string_at((token->buffer), (token->start), str);
}

static inline boolean_t token_contains(token_t* token, char* str){
  return buffer_region_contains((token->buffer), (token->start), (token->end), str);
}

static inline token_t* heap_allocate_token(token_t token){
  token_t* result = malloc_struct(token_t);
  ((*result)=token);
  return result;
}

static inline void token_list_add(token_list_t* token_list, token_t* token){
  if ((token==NULL))
  {
    fatal_error(ERROR_ILLEGAL_STATE);
  }
  if (((token_list->list)==NULL))
  {
    ((token_list->list)=make_value_array(2));
  }
  value_array_add((token_list->list), ptr_to_value(token));
}

static inline uint64_t token_list_length(token_list_t token_list){
  if (((token_list.list)==NULL))
  {
    return 0;
  }
  return ((token_list.list)->length);
}

static inline boolean_t token_list_is_empty(token_list_t token_list){
  return (token_list_length(token_list)==0);
}

static inline token_t* token_list_get(token_list_t token_list, uint64_t index){
  if (((token_list.list)==NULL))
  {
    fatal_error(ERROR_ACCESS_OUT_OF_BOUNDS);
  }
  return value_array_get_ptr((token_list.list), index, typeof(token_t*));
}

static inline boolean_t token_list_contains(token_list_t token_list, char* token_string){
  for (
    int i = 0;
    (i<token_list_length(token_list));
    (i++))
  {
    if (token_matches(token_list_get(token_list, i), token_string))
    {
      return true;
    }
  }
  return false;
}

static inline parse_node_t* to_node(void* ptr){
  return (/*CAST*/(parse_node_t*) ptr);
}

static inline declarations_node_t* to_declarations_node(parse_node_t* ptr){
  if (((ptr==NULL)||((ptr->tag)!=PARSE_NODE_DECLARATIONS)))
  {
    fatal_error(ERROR_ILLEGAL_STATE);
  }
  return (/*CAST*/(declarations_node_t*) ptr);
}

static inline literal_node_t* to_literal_node(parse_node_t* ptr){
  if (((ptr==NULL)||((ptr->tag)!=PARSE_NODE_LITERAL)))
  {
    fatal_error(ERROR_ILLEGAL_STATE);
  }
  return (/*CAST*/(literal_node_t*) ptr);
}

static inline function_node_t* to_function_node(parse_node_t* ptr){
  if (((ptr==NULL)||((ptr->tag)!=PARSE_NODE_FUNCTION)))
  {
    fatal_error(ERROR_ILLEGAL_STATE);
  }
  return (/*CAST*/(function_node_t*) ptr);
}

static inline function_argument_node_t* to_function_argument_node(parse_node_t* ptr){
  if (((ptr==NULL)||((ptr->tag)!=PARSE_NODE_FUNCTION_ARGUMENT)))
  {
    fatal_error(ERROR_ILLEGAL_STATE);
  }
  return (/*CAST*/(function_argument_node_t*) ptr);
}

static inline typedef_node_t* to_typedef_node(parse_node_t* ptr){
  if (((ptr==NULL)||((ptr->tag)!=PARSE_NODE_TYPEDEF)))
  {
    fatal_error(ERROR_ILLEGAL_STATE);
  }
  return (/*CAST*/(typedef_node_t*) ptr);
}

static inline unparsed_expression_t* to_unparsed_expression(parse_node_t* ptr){
  if (((ptr==NULL)||((ptr->tag)!=PARSE_NODE_UNPARSED_EXPRESSION)))
  {
    fatal_error(ERROR_ILLEGAL_STATE);
  }
  return (/*CAST*/(unparsed_expression_t*) ptr);
}

static inline attribute_node_t* to_attribute_node(parse_node_t* ptr){
  if (((ptr==NULL)||((ptr->tag)!=PARSE_NODE_ATTRIBUTE)))
  {
    fatal_error(ERROR_ILLEGAL_STATE);
  }
  return (/*CAST*/(attribute_node_t*) ptr);
}

static inline cpp_define_node_t* to_cpp_define_node(parse_node_t* ptr){
  if (((ptr==NULL)||((ptr->tag)!=PARSE_NODE_CPP_DEFINE)))
  {
    fatal_error(ERROR_ILLEGAL_STATE);
  }
  return (/*CAST*/(cpp_define_node_t*) ptr);
}

static inline cpp_include_node_t* to_cpp_include_node(parse_node_t* ptr){
  if (((ptr==NULL)||((ptr->tag)!=PARSE_NODE_CPP_INCLUDE)))
  {
    fatal_error(ERROR_ILLEGAL_STATE);
  }
  return (/*CAST*/(cpp_include_node_t*) ptr);
}

static inline declarations_node_t* malloc_declarations(void){
  declarations_node_t* result = malloc_struct(declarations_node_t);
  ((result->tag)=PARSE_NODE_DECLARATIONS);
  return result;
}

static inline function_node_t* malloc_function_node(void){
  function_node_t* result = malloc_struct(function_node_t);
  ((result->tag)=PARSE_NODE_FUNCTION);
  return result;
}

static inline function_argument_node_t* malloc_function_argument_node(void){
  function_argument_node_t* result = malloc_struct(function_argument_node_t);
  ((result->tag)=PARSE_NODE_FUNCTION_ARGUMENT);
  return result;
}

static inline typedef_node_t* malloc_typedef_node(void){
  typedef_node_t* result = malloc_struct(typedef_node_t);
  ((result->tag)=PARSE_NODE_TYPEDEF);
  return result;
}

static inline attribute_node_t* malloc_attribute_node(void){
  attribute_node_t* result = malloc_struct(attribute_node_t);
  ((result->tag)=PARSE_NODE_ATTRIBUTE);
  return result;
}

static inline cpp_include_node_t* malloc_cpp_include_node(void){
  cpp_include_node_t* result = malloc_struct(cpp_include_node_t);
  ((result->tag)=PARSE_NODE_CPP_INCLUDE);
  return result;
}

static inline cpp_define_node_t* malloc_cpp_define_node(void){
  cpp_define_node_t* result = malloc_struct(cpp_define_node_t);
  ((result->tag)=PARSE_NODE_CPP_DEFINE);
  return result;
}

static inline void node_list_add_node(node_list_t* node_list, struct parse_node_S* oc_node){
  if ((oc_node==NULL))
  {
    fatal_error(ERROR_ILLEGAL_STATE);
  }
  if (((node_list->list)==NULL))
  {
    ((node_list->list)=make_value_array(2));
  }
  value_array_add((node_list->list), ptr_to_value(oc_node));
}

static inline uint64_t node_list_length(node_list_t node_list){
  if (((node_list.list)==NULL))
  {
    return 0;
  }
  return ((node_list.list)->length);
}

static inline boolean_t node_list_is_empty(node_list_t node_list){
  return (node_list_length(node_list)==0);
}

static inline struct parse_node_S* node_list_get(node_list_t node_list, uint64_t index){
  if (((node_list.list)==NULL))
  {
    fatal_error(ERROR_ACCESS_OUT_OF_BOUNDS);
  }
  return value_array_get_ptr((node_list.list), index, typeof(struct parse_node_S*));
}

static inline char* remove_type_suffix(char* typename){
  if (string_ends_with(typename, "_t"))
  {
    return string_substring(typename, 0, (strlen(typename)-2));
  }
  else
  {
    return typename;
  }
}

static inline char* remove_type_suffix_1(char* typename){
  if (string_ends_with(typename, "_t"))
  {
    return string_substring(typename, 0, (strlen(typename)-2));
  }
  else
  {
    return typename;
  }
}

static inline operator_node_t* malloc_operator_node(void){
  operator_node_t* result = malloc_struct(operator_node_t);
  ((result->tag)=PARSE_NODE_OPERATOR);
  return result;
}

static inline identifier_node_t* malloc_identifier_node(void){
  identifier_node_t* result = malloc_struct(identifier_node_t);
  ((result->tag)=PARSE_NODE_IDENTIFIER);
  return result;
}

static inline call_node_t* malloc_call_node(void){
  call_node_t* result = malloc_struct(call_node_t);
  ((result->tag)=PARSE_NODE_CALL);
  return result;
}

static inline conditional_node_t* malloc_conditional_node(void){
  conditional_node_t* result = malloc_struct(conditional_node_t);
  ((result->tag)=PARSE_NODE_CONDITIONAL);
  return result;
}

static inline identifier_node_t* to_identifier_node(parse_node_t* ptr){
  if (((ptr==NULL)||((ptr->tag)!=PARSE_NODE_IDENTIFIER)))
  {
    fatal_error(ERROR_ILLEGAL_STATE);
  }
  return (/*CAST*/(identifier_node_t*) ptr);
}

static inline operator_node_t* to_operator_node(parse_node_t* ptr){
  if (((ptr==NULL)||((ptr->tag)!=PARSE_NODE_OPERATOR)))
  {
    fatal_error(ERROR_ILLEGAL_STATE);
  }
  return (/*CAST*/(operator_node_t*) ptr);
}

static inline call_node_t* to_call_node(parse_node_t* ptr){
  if (((ptr==NULL)||((ptr->tag)!=PARSE_NODE_CALL)))
  {
    fatal_error(ERROR_ILLEGAL_STATE);
  }
  return (/*CAST*/(call_node_t*) ptr);
}

static inline conditional_node_t* to_conditional_node(parse_node_t* ptr){
  if (((ptr==NULL)||((ptr->tag)!=PARSE_NODE_CONDITIONAL)))
  {
    fatal_error(ERROR_ILLEGAL_STATE);
  }
  return (/*CAST*/(conditional_node_t*) ptr);
}

static inline pratt_parser_instruction_t make_parser_instruction(token_t* token, pratt_parser_operation_t operation, precedence_t precedence){
  return ((pratt_parser_instruction_t) {.token = token, .operation = operation, .precedence = precedence});
}

static inline type_node_t* malloc_type_node(void){
  type_node_t* result = malloc_struct(type_node_t);
  ((result->tag)=PARSE_NODE_TYPE);
  return result;
}

static inline type_node_t* to_type_node(parse_node_t* ptr){
  if (((ptr==NULL)||((ptr->tag)!=PARSE_NODE_TYPE)))
  {
    fatal_error(ERROR_ILLEGAL_STATE);
  }
  return (/*CAST*/(type_node_t*) ptr);
}

static inline boolean_t is_struct_node(parse_node_t* ptr){
  return ((ptr!=NULL)&&((ptr->tag)==PARSE_NODE_STRUCT));
}

static inline boolean_t is_enum_node(parse_node_t* ptr){
  return ((ptr!=NULL)&&((ptr->tag)==PARSE_NODE_ENUM));
}

static inline struct_node_t* to_struct_node(parse_node_t* ptr){
  if (((ptr==NULL)||(!(((ptr->tag)==PARSE_NODE_STRUCT)||((ptr->tag)==PARSE_NODE_UNION)))))
  {
    fatal_error(ERROR_ILLEGAL_STATE);
  }
  return (/*CAST*/(struct_node_t*) ptr);
}

static inline field_node_t* to_field_node(parse_node_t* ptr){
  if (((ptr==NULL)||((ptr->tag)!=PARSE_NODE_FIELD)))
  {
    fatal_error(ERROR_ILLEGAL_STATE);
  }
  return (/*CAST*/(field_node_t*) ptr);
}

static inline enum_node_t* to_enum_node(parse_node_t* ptr){
  if (((ptr==NULL)||((ptr->tag)!=PARSE_NODE_ENUM)))
  {
    fatal_error(ERROR_ILLEGAL_STATE);
  }
  return (/*CAST*/(enum_node_t*) ptr);
}

static inline enum_element_t* to_enum_element_node(parse_node_t* ptr){
  if (((ptr==NULL)||((ptr->tag)!=PARSE_NODE_ENUM_ELEMENT)))
  {
    fatal_error(ERROR_ILLEGAL_STATE);
  }
  return (/*CAST*/(enum_element_t*) ptr);
}

static inline enum_node_t* malloc_enum_node(void){
  enum_node_t* result = malloc_struct(enum_node_t);
  ((result->tag)=PARSE_NODE_ENUM);
  return result;
}

static inline enum_element_t* malloc_enum_element(void){
  enum_element_t* result = malloc_struct(enum_element_t);
  ((result->tag)=PARSE_NODE_ENUM_ELEMENT);
  return result;
}

static inline struct_node_t* malloc_struct_node(void){
  struct_node_t* result = malloc_struct(struct_node_t);
  ((result->tag)=PARSE_NODE_STRUCT);
  return result;
}

static inline union_node_t* malloc_union_node(void){
  union_node_t* result = malloc_struct(union_node_t);
  ((result->tag)=PARSE_NODE_UNION);
  return result;
}

static inline field_node_t* malloc_field_node(void){
  field_node_t* result = malloc_struct(field_node_t);
  ((result->tag)=PARSE_NODE_FIELD);
  return result;
}

static inline variable_definition_node_t* to_variable_definition_node(parse_node_t* ptr){
  if (((ptr==NULL)||((ptr->tag)!=PARSE_NODE_VARIABLE_DEFINITION)))
  {
    fatal_error(ERROR_ILLEGAL_STATE);
  }
  return (/*CAST*/(variable_definition_node_t*) ptr);
}

static inline variable_definition_node_t* malloc_variable_definition_node(void){
  variable_definition_node_t* result = malloc_struct(variable_definition_node_t);
  ((result->tag)=PARSE_NODE_VARIABLE_DEFINITION);
  return result;
}

static inline literal_node_t* malloc_literal_node(void){
  literal_node_t* result = malloc_struct(literal_node_t);
  ((result->tag)=PARSE_NODE_LITERAL);
  return result;
}

static inline balanced_construct_node_t* to_balanced_construct_node(parse_node_t* ptr){
  if (((ptr==NULL)||((ptr->tag)!=PARSE_NODE_BALANCED_CONSTRUCT)))
  {
    fatal_error(ERROR_ILLEGAL_STATE);
  }
  return (/*CAST*/(balanced_construct_node_t*) ptr);
}

static inline balanced_construct_node_t* malloc_balanced_construct_node(){
  balanced_construct_node_t* result = malloc_struct(balanced_construct_node_t);
  ((result->tag)=PARSE_NODE_BALANCED_CONSTRUCT);
  return result;
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
void initialize_keyword_maps(void){
  int num_keywords = ((sizeof(c_keywords_array))/(sizeof((c_keywords_array[0]))));
  (c_keywords_ht=make_string_hashtable((2*num_keywords)));
  (cpp_keywords_ht=make_string_hashtable((2*num_keywords)));
  for (
    int i = 0;
    (i<num_keywords);
    (i++))
  {
    (c_keywords_ht=string_ht_insert(c_keywords_ht, (c_keywords_array[i]), u64_to_value(1)));
    (oc_keywords_ht=string_ht_insert(c_keywords_ht, (c_keywords_array[i]), u64_to_value(1)));
    (cpp_keywords_ht=string_ht_insert(cpp_keywords_ht, (c_keywords_array[i]), u64_to_value(1)));
  }
  int num_types = ((sizeof(c_builtin_types_array))/(sizeof((c_builtin_types_array[0]))));
  (c_builtin_types_ht=make_string_hashtable((2*num_types)));
  (cpp_builtin_types_ht=make_string_hashtable((2*num_types)));
  for (
    int i = 0;
    (i<num_types);
    (i++))
  {
    (c_builtin_types_ht=string_ht_insert(c_builtin_types_ht, (c_builtin_types_array[i]), u64_to_value(1)));
    (oc_builtin_types_ht=string_ht_insert(c_builtin_types_ht, (c_builtin_types_array[i]), u64_to_value(1)));
    (cpp_builtin_types_ht=string_ht_insert(cpp_builtin_types_ht, (c_builtin_types_array[i]), u64_to_value(1)));
  }
  (cpp_builtin_types_ht=string_ht_insert(cpp_builtin_types_ht, "char8_t", u64_to_value(1)));
}

/* i=211 j=0 */
boolean_t is_reserved_word(input_mode_t mode, char* str){
  maybe_initialize_keyword_maps();
  switch (mode)
  {
    case INPUT_MODE_OMNI_C:
    return is_ok(string_ht_find(oc_keywords_ht, str));
    case INPUT_MODE_STANDARD_C:
    return is_ok(string_ht_find(c_keywords_ht, str));
    case INPUT_MODE_C_PLUS_PLUS:
    return is_ok(string_ht_find(cpp_keywords_ht, str));
    default:
    break;
  }
  fatal_error(ERROR_ILLEGAL_STATE);
}

/* i=212 j=0 */
boolean_t is_builtin_type_name(input_mode_t mode, char* str){
  maybe_initialize_keyword_maps();
  switch (mode)
  {
    case INPUT_MODE_OMNI_C:
    return is_ok(string_ht_find(oc_builtin_types_ht, str));
    case INPUT_MODE_STANDARD_C:
    return is_ok(string_ht_find(c_builtin_types_ht, str));
    case INPUT_MODE_C_PLUS_PLUS:
    return is_ok(string_ht_find(cpp_builtin_types_ht, str));
    default:
    break;
  }
  fatal_error(ERROR_ILLEGAL_STATE);
}

/* i=213 j=0 */
value_array_t* read_files(value_array_t* files){
  fprintf(stderr, "Parsing %d files...\n", (files->length));
  value_array_t* result = make_value_array((files->length));
  for (
    int i = 0;
    (i<(files->length));
    (i++))
  {
    char* file_name = (value_array_get(files, i).str);
    fprintf(stderr, "Reading %s\n", file_name);
    value_array_add(result, ptr_to_value(read_file(file_name)));
  }
  return result;
}

/* i=214 j=0 */
file_t* read_file(char* file_name){
  file_t* result = malloc_struct(file_t);
  buffer_t* buffer = make_buffer((1024*8));
  (buffer=buffer_append_file_contents(buffer, file_name));
  ((result->tag)=STD_C_SOURCE_FILE);
  ((result->file_name)=file_name);
  ((result->data)=buffer);
  return result;
}

/* i=215 j=0 */
buffer_t* buffer_append_human_readable_error(buffer_t* buffer, compiler_error_t* error){
  if (((error->tokenizer_error_code)!=TOKENIZER_ERROR_UNKNOWN))
  {
    (buffer=buffer_append_human_readable_tokenizer_error(buffer, error));
  }
  if (((error->parse_error_code)!=PARSE_ERROR_UNKNOWN))
  {
    (buffer=buffer_append_human_readable_parser_error(buffer, error));
  }
  return buffer;
}

/* i=216 j=0 */
src_code_snippets_t get_source_code_snippet(buffer_t* buffer, uint64_t location, int before_lines, int after_lines){
  src_code_snippets_t result = {0};
  uint64_t current_begin = buffer_beginning_of_line(buffer, location);
  uint64_t current_end = buffer_end_of_line(buffer, location);
  ((result.current_line)=buffer_c_substring(buffer, current_begin, current_end));
  uint64_t prefix_begin = current_begin;
  for (
    int i = 0;
    ((i<before_lines)&&(prefix_begin>0));
    (i++))
  {
    (prefix_begin=buffer_beginning_of_line(buffer, (prefix_begin-1)));
  }
  ((result.previous_lines)=buffer_c_substring(buffer, prefix_begin, current_begin));
  uint64_t suffix_end = current_end;
  for (
    int i = 0;
    ((i<after_lines)&&(suffix_end<(buffer->length)));
    (i++))
  {
    (suffix_end=buffer_end_of_line(buffer, (suffix_end+1)));
  }
  ((result.next_lines)=buffer_c_substring(buffer, current_end, suffix_end));
  return result;
}

/* i=217 j=0 */
char* do_common_replacements(char* template, compiler_error_t* error){
  buffer_t* buffer = make_buffer(256);
  char* file_name = (error->file_name);
  src_code_snippets_t snippet = {0};
  if ((error->error_token))
  {
    line_and_column_t position_info = buffer_position_to_line_and_column(((error->error_token)->buffer), ((error->error_token)->start));
    (buffer=buffer_printf(buffer, "%s:%d.%d: ", file_name, ((position_info.line)&0xffffffff), ((position_info.column)&0xffffffff)));
    (snippet=get_source_code_snippet(((error->error_token)->buffer), ((error->error_token)->start), 5, 3));
  }
  else
  {
    (buffer=buffer_printf(buffer, "%s:%d.%d: ", file_name, 0, 0));
  }
  (buffer=buffer_append_string(buffer, template));
  (buffer=buffer_replace_all(buffer, "{formatted_snippet}", formatted_snippet));
  (buffer=buffer_replace_all(buffer, "{error-highlight-on}", "\033[1m"));
  (buffer=buffer_replace_all(buffer, "{error-highlight-off}", "\033[0m"));
  (buffer=buffer_replace_all(buffer, "{error-prefix-lines}", (snippet.previous_lines)));
  (buffer=buffer_replace_all(buffer, "{error-current-line}", (snippet.current_line)));
  (buffer=buffer_replace_all(buffer, "{error-suffix-lines}", (snippet.next_lines)));
  return buffer_to_c_string(buffer);
}

/* i=218 j=0 */
buffer_t* buffer_append_human_readable_tokenizer_error(buffer_t* buffer, compiler_error_t* error){
  (buffer=buffer_printf(buffer, "\nlexer error code = %d\n", (error->tokenizer_error_code)));
  return buffer;
}

/* i=219 j=0 */
buffer_t* buffer_append_human_readable_parser_error(buffer_t* buffer, compiler_error_t* error){
  (buffer=buffer_printf(buffer, "\nparser error code = %d\n", (error->parse_error_code)));
  char* template = NULL;
  switch ((error->parse_error_code))
  {
    case PARSE_ERROR_EXPECTED_FIELD_WIDTH_OR_SEMICOLON:
    (template=error_field_width_or_semicolon);
    break;
    case PARSE_ERROR_CLOSE_BRACKET_EXPECTED:
    (template=error_open_brace_expected);
    break;
    case PARSE_ERROR_UNRECOGNIZED_TOP_LEVEL_DECLARATION:
    (template=error_unrecognized_top_level_declaration);
    break;
    case PARSE_ERROR_SEMICOLON_EXPECTED:
    (template=error_open_semicolon_expected);
    break;
    case PARSE_ERROR_CONFLICTING_STORAGE_CLASS_SPECIFIER:
    (template=error_conflicting_storage_class_specifier);
    break;
    case PARSE_ERROR_CONFLICTING_FUNCTION_SPECIFIER:
    (template=error_conflicting_function_specifier);
    break;
    case PARSE_ERROR_EXPECTED_OPEN_PAREN_AFTER_UNDERSCORE_ATTRIBUTE:
    (template=error_expected_open_paren_for_underscore_attribute);
    break;
    case PARSE_ERROR_EXPECTED_MATCHING_CLOSE_PAREN_AFTER_UNDERSCORE_ATTRIBUTE:
    (template=error_expected_matching_close_paren_after_underscore_attribute);
    break;
    default:
    (template=parse_error_unknown);
    break;
  }
  char* template_string = do_common_replacements(template, error);
  return buffer_append_string(buffer, template_string);
}

/* i=224 j=1 */
char* token_type_to_string(token_type_t value){
  switch (value) {
    case TOKEN_TYPE_UNKNOWN:
return "TOKEN_TYPE_UNKNOWN";
  case TOKEN_TYPE_WHITESPACE:
return "TOKEN_TYPE_WHITESPACE";
  case TOKEN_TYPE_COMMENT:
return "TOKEN_TYPE_COMMENT";
  case TOKEN_TYPE_IDENTIFIER:
return "TOKEN_TYPE_IDENTIFIER";
  case TOKEN_TYPE_PUNCTUATION:
return "TOKEN_TYPE_PUNCTUATION";
  case TOKEN_TYPE_INTEGER_LITERAL:
return "TOKEN_TYPE_INTEGER_LITERAL";
  case TOKEN_TYPE_FLOAT_LITERAL:
return "TOKEN_TYPE_FLOAT_LITERAL";
  case TOKEN_TYPE_STRING_LITERAL:
return "TOKEN_TYPE_STRING_LITERAL";
  case TOKEN_TYPE_CHARACTER_LITERAL:
return "TOKEN_TYPE_CHARACTER_LITERAL";
  default:
    return "<<unknown-token_type>>";
  }
}
/* i=225 j=0 */
char* token_to_string(token_t* token){
  return buffer_c_substring((token->buffer), (token->start), (token->end));
}

/* i=226 j=0 */
token_t* make_derived_token(token_t* source_token){
  token_t* result = (/*CAST*/(token_t*) malloc_copy_of((/*CAST*/(uint8_t*) source_token), (sizeof(token_t))));
  buffer_t* buffer = make_buffer(((source_token->end)-(source_token->start)));
  (buffer=buffer_append_sub_buffer(buffer, (source_token->start), (source_token->end), (result->buffer)));
  ((result->buffer)=buffer);
  ((result->start)=0);
  ((result->end)=(buffer->length));
  return result;
}

/* i=227 j=0 */
__attribute__((warn_unused_result)) buffer_t* append_token_debug_string(buffer_t* buffer, token_t token){
  char* str = token_to_string((&token));
  (buffer=buffer_printf(buffer, "type: %s start: %d end: %d line=%d column=%d str: %s", token_type_to_string((token.type)), (token.start), (token.end), (token.line_number), (token.column_number), str));
  free_bytes(str);
  return buffer;
}

/* i=228 j=0 */
buffer_t* buffer_append_token_string(buffer_t* buffer, token_t* token){
  char* str = token_to_string(token);
  (buffer=buffer_printf(buffer, "%s", str));
  free_bytes(str);
  return buffer;
}

/* i=229 j=0 */
token_or_error_t tokenize_whitespace(buffer_t* buffer, uint64_t start_position){
  uint64_t pos = start_position;
  while ((pos<buffer_length(buffer)))
  {
    utf8_decode_result_t decode_result = buffer_utf8_decode(buffer, pos);
    if ((decode_result.error))
    {
      return ((token_or_error_t) {.error_code = TOKENIZER_ERROR_UTF_DECODE_ERROR,
                               .error_position = pos});
    }
    uint32_t code_point = (decode_result.code_point);
    if ((!isspace(code_point)))
    {
      break;
    }
    (pos+=(decode_result.num_bytes));
    if ((code_point=='\n'))
    {
      break;
    }
  }
  return ((token_or_error_t) {.token = compound_literal(token_t, {.buffer = buffer,
                                           .type = TOKEN_TYPE_WHITESPACE,
                                           .start = start_position,
                                           .end = pos})});
}

/* i=230 j=0 */
boolean_t is_identifier_start(uint32_t code_point){
  switch (code_point)
  {
    case '$':
    return true;
    case '_':
    return true;
    default:
    return isalpha(code_point);
  }
}

/* i=231 j=0 */
token_or_error_t tokenize_identifier(buffer_t* buffer, uint64_t start_position){
  uint64_t pos = start_position;
  while ((pos<buffer_length(buffer)))
  {
    utf8_decode_result_t decode_result = buffer_utf8_decode(buffer, pos);
    if ((decode_result.error))
    {
      return ((token_or_error_t) {.error_code = TOKENIZER_ERROR_UTF_DECODE_ERROR,
                               .error_position = pos});
    }
    uint32_t code_point = (decode_result.code_point);
    if ((!(is_identifier_start(code_point)||isdigit(code_point))))
    {
      break;
    }
    (pos+=(decode_result.num_bytes));
  }
  return ((token_or_error_t) {.token = compound_literal(token_t, {.buffer = buffer,
                                           .type = TOKEN_TYPE_IDENTIFIER,
                                           .start = start_position,
                                           .end = pos})});
}

/* i=232 j=0 */
token_or_error_t tokenize_numeric(buffer_t* buffer, uint64_t start_position){
  numeric_literal_encoding_t encoding = NUMERIC_LITERAL_ENCODING_UNDECIDED;
  uint32_t previous_code_point = 0;
  token_type_t token_type = TOKEN_TYPE_INTEGER_LITERAL;
  uint64_t offset = 0;
  uint64_t pos = start_position;
  while ((pos<buffer_length(buffer)))
  {
    utf8_decode_result_t decode_result = buffer_utf8_decode(buffer, pos);
    if ((decode_result.error))
    {
      return ((token_or_error_t) {.error_code = TOKENIZER_ERROR_UTF_DECODE_ERROR,
                               .error_position = pos});
    }
    uint32_t code_point = (decode_result.code_point);
    if (((offset==0)&&(code_point!='0')))
    {
      (encoding=NUMERIC_LITERAL_ENCODING_FLOAT_OR_DECIMAL);
    }
    if (((offset==1)&&(encoding==NUMERIC_LITERAL_ENCODING_UNDECIDED)))
    {
      boolean_t changed = false;
      if ((code_point=='x'))
      {
        (encoding=NUMERIC_LITERAL_ENCODING_HEX);
        (changed=true);
      }
      else
      if ((code_point=='o'))
      {
        (encoding=NUMERIC_LITERAL_ENCODING_OCTAL);
        (changed=true);
      }
      else
      if ((code_point=='b'))
      {
        (encoding=NUMERIC_LITERAL_ENCODING_HEX);
        (changed=true);
      }
      if (changed)
      {
        (pos+=(decode_result.num_bytes));
        (previous_code_point=code_point);
        (offset+=1);
        continue;
      }
    }
    if (((code_point=='.')&&((encoding==NUMERIC_LITERAL_ENCODING_UNDECIDED)||(encoding==NUMERIC_LITERAL_ENCODING_FLOAT_OR_DECIMAL))))
    {
      (token_type=TOKEN_TYPE_FLOAT_LITERAL);
      (encoding=NUMERIC_LITERAL_ENCODING_FLOAT);
      (pos+=(decode_result.num_bytes));
      (previous_code_point=code_point);
      (offset+=1);
      continue;
    }
    if ((!can_extend_number(encoding, code_point, previous_code_point)))
    {
      break;
    }
    (pos+=(decode_result.num_bytes));
    (previous_code_point=code_point);
    (offset+=1);
  }
  return ((token_or_error_t) {.token = compound_literal(token_t, {.buffer = buffer,
                                           .type = token_type,
                                           .start = start_position,
                                           .end = pos})});
}

/* i=233 j=0 */
boolean_t can_extend_number(numeric_literal_encoding_t encoding, uint32_t code_point, uint32_t previous_code_point){
  switch (encoding)
  {
    case NUMERIC_LITERAL_ENCODING_UNDECIDED:
    return string_contains_char("0123456789.eEobxLlUuFf", code_point);
    case NUMERIC_LITERAL_ENCODING_FLOAT_OR_DECIMAL:
    return string_contains_char("0123456789.eELlUuFf", code_point);
    case NUMERIC_LITERAL_ENCODING_BINARY:
    return string_contains_char("01LlUu", code_point);
    case NUMERIC_LITERAL_ENCODING_OCTAL:
    return string_contains_char("01234567LlUu", code_point);
    case NUMERIC_LITERAL_ENCODING_HEX:
    return string_contains_char("0123456789abcdefABCDEFLlUu", code_point);
    case NUMERIC_LITERAL_ENCODING_DECIMAL:
    return string_contains_char("0123456789LlUu", code_point);
    case NUMERIC_LITERAL_ENCODING_FLOAT:
    return (string_contains_char("0123456789FfeE", code_point)||(((previous_code_point=='e')||(previous_code_point=='E'))&&string_contains_char("+-", code_point)));
    default:
    fatal_error(ERROR_ILLEGAL_STATE);
    return false;
  }
  return false;
}

/* i=234 j=0 */
token_or_error_t tokenize_punctuation(buffer_t* buffer, uint64_t start_position){
  int num_elements = ((sizeof(c_punctuation))/(sizeof((c_punctuation[0]))));
  for (
    int i = 0;
    (i<num_elements);
    (i++))
  {
    if (buffer_match_string_at(buffer, start_position, (c_punctuation[i])))
    {
      return ((token_or_error_t) {.token = compound_literal(
               token_t, {.buffer = buffer,
                         .type = TOKEN_TYPE_PUNCTUATION,
                         .start = start_position,
                         .end = start_position + strlen(c_punctuation[i])})});
    }
  }
  return ((token_or_error_t) {.error_code = TOKENIZER_ERROR_UNRECOGNIZED_PUNCTUATION,
                         .error_position = start_position});
}

/* i=235 j=0 */
boolean_t is_comment_start(buffer_t* buffer, uint64_t position){
  return (buffer_match_string_at(buffer, position, "//")||buffer_match_string_at(buffer, position, "/*"));
}

/* i=236 j=0 */
token_or_error_t tokenize_comment(buffer_t* buffer, uint64_t start_position){
  if (buffer_match_string_at(buffer, start_position, "//"))
  {
    for (
      uint64_t position = (start_position+2);
      (position<(buffer->length));
      (position++))
    {
      if (buffer_match_string_at(buffer, position, "\n"))
      {
        return ((token_or_error_t) {.token = compound_literal(token_t, {.buffer = buffer,
                                                 .type = TOKEN_TYPE_COMMENT,
                                                 .start = start_position,
                                                 .end = position + 1})});
      }
    }
  }
  else
  {
    for (
      uint64_t position = (start_position+2);
      (position<(buffer->length));
      (position++))
    {
      if (buffer_match_string_at(buffer, position, "*/"))
      {
        return ((token_or_error_t) {.token = compound_literal(token_t, {.buffer = buffer,
                                                 .type = TOKEN_TYPE_COMMENT,
                                                 .start = start_position,
                                                 .end = position + 2})});
      }
    }
  }
  return ((token_or_error_t) {.error_code = TOKENIZER_ERROR_UNTERMINATED_COMMENT,
                           .error_position = start_position});
}

/* i=237 j=0 */
boolean_t is_string_literal_start(buffer_t* buffer, uint64_t position){
  return buffer_match_string_at(buffer, position, "\"");
}

/* i=238 j=0 */
token_or_error_t tokenize_quoted_literal_common(buffer_t* buffer, uint64_t start_position, char* opening_sequence, char* quoted_closing_sequence, char* closing_sequence, token_type_t token_type, tokenizer_error_t unterminated_error_code){
  if ((!buffer_match_string_at(buffer, start_position, opening_sequence)))
  {
    fatal_error(ERROR_ILLEGAL_STATE);
  }
  for (
    uint64_t position = (start_position+strlen(opening_sequence));
    (position<(buffer->length));
    )
  {
    if (buffer_match_string_at(buffer, position, quoted_closing_sequence))
    {
      (position+=strlen(quoted_closing_sequence));
    }
    else
    if (buffer_match_string_at(buffer, position, closing_sequence))
    {
      return ((token_or_error_t) {.token = compound_literal(
               token_t, {.buffer = buffer,
                         .type = token_type,
                         .start = start_position,
                         .end = position + strlen(closing_sequence)})});
    }
    else
    {
      (position+=1);
    }
  }
  return ((token_or_error_t) {.error_code = unterminated_error_code,
                           .error_position = start_position});
}

/* i=239 j=0 */
token_or_error_t tokenize_string_literal(buffer_t* buffer, uint64_t start_position){
  return tokenize_quoted_literal_common(buffer, start_position, "\"", "\\\"", "\"", TOKEN_TYPE_STRING_LITERAL, TOKENIZER_ERROR_UNTERMINATED_STRING_LITERAL);
}

/* i=240 j=0 */
boolean_t is_character_literal_start(buffer_t* buffer, uint64_t position){
  return buffer_match_string_at(buffer, position, "'");
}

/* i=241 j=0 */
token_or_error_t tokenize_character_literal(buffer_t* buffer, uint64_t start_position){
  return tokenize_quoted_literal_common(buffer, start_position, "'", "\\'", "'", TOKEN_TYPE_CHARACTER_LITERAL, TOKENIZER_ERROR_UNTERMINATED_CHARACTER_LITERL);
}

/* i=243 j=0 */
tokenizer_result_t tokenize(buffer_t* buffer){
  tokenizer_result_t result = {0};
  value_array_t* result_tokens = make_value_array(1024);
  uint64_t line_number = 1;
  uint64_t column_number = 1;
  uint32_t pos = 0;
  while ((pos<buffer_length(buffer)))
  {
    utf8_decode_result_t decode_result = buffer_utf8_decode(buffer, pos);
    if ((decode_result.error))
    {
      return ((tokenizer_result_t) {.tokenizer_error_code = TOKENIZER_ERROR_UTF_DECODE_ERROR});
    }
    uint32_t code_point = (decode_result.code_point);
    token_t* token = NULL;
    if (isspace(code_point))
    {
      read_token(tokenize_whitespace);
    }
    else
    if (is_identifier_start(code_point))
    {
      read_token(tokenize_identifier);
    }
    else
    if (isdigit(code_point))
    {
      read_token(tokenize_numeric);
    }
    else
    if (is_comment_start(buffer, pos))
    {
      read_token(tokenize_comment);
    }
    else
    if (is_string_literal_start(buffer, pos))
    {
      read_token(tokenize_string_literal);
    }
    else
    if (is_character_literal_start(buffer, pos))
    {
      read_token(tokenize_character_literal);
    }
    else
    {
      read_token(tokenize_punctuation);
    }
    if ((token!=NULL))
    {
      ((token->line_number)=line_number);
      ((token->column_number)=column_number);
      for (
        int i = (token->start);
        (i<(token->end));
        (i++))
      {
        uint8_t ch = buffer_get((token->buffer), i);
        if ((ch=='\n'))
        {
          (line_number++);
          (column_number=1);
        }
        else
        {
          (column_number++);
        }
      }
    }
  }
  ((result.tokens)=result_tokens);
  return result;
}

/* i=249 j=0 */
value_array_t* transform_tokens(value_array_t* tokens, token_transformer_options_t xform_options){
  value_array_t* result = make_value_array((tokens->length));
  for (
    int position = 0;
    (position<(tokens->length));
    (position++))
  {
    token_t* token = token_at(tokens, position);
    if (((token->is_cpp_token)&&(!(xform_options.keep_c_preprocessor_lines))))
    {
      continue;
    }
    if ((((token->type)==TOKEN_TYPE_WHITESPACE)&&(!(xform_options.keep_whitespace))))
    {
      continue;
    }
    if (((token->type)==TOKEN_TYPE_COMMENT))
    {
      if (token_starts_with(token, "/**"))
      {
        if ((!(xform_options.keep_javadoc_comments)))
        {
          continue;
        }
      }
      else
      if ((!(xform_options.keep_comments)))
      {
        continue;
      }
    }
    value_array_add(result, ptr_to_value(token));
  }
  return result;
}

/* i=251 j=0 */
pstatus_t pstate_error(pstate_t* pstate, uint64_t saved_position, parse_error_code_t parse_error_code){
  ((pstate->result_token)=NULL);
  ((pstate->result_node)=NULL);
  (((pstate->error).parse_error_code)=parse_error_code);
  (((pstate->error).error_position)=(pstate->position));
  (((pstate->error).error_token)=token_at((pstate->tokens), (pstate->position)));
  ((pstate->position)=saved_position);
  return false;
}

/* i=252 j=0 */
pstate_t* pstate_ignore_error(pstate_t* pstate){
  ((pstate->error)=((compiler_error_t) {0}));
  return pstate;
}

/* i=253 j=0 */
pstatus_t pstate_propagate_error(pstate_t* pstate, uint64_t saved_position){
  ((pstate->position)=saved_position);
  if ((!((pstate->error).parse_error_code)))
  {
    fatal_error(ERROR_ILLEGAL_STATE);
  }
  return false;
}

/* i=254 j=0 */
pstatus_t pstate_set_result_token(pstate_t* pstate, token_t* token){
  ((pstate->error)=((compiler_error_t) {0}));
  ((pstate->result_node)=NULL);
  ((pstate->result_token)=token);
  return true;
}

/* i=255 j=0 */
pstatus_t pstate_set_result_node(pstate_t* pstate, parse_node_t* node){
  ((pstate->error)=((compiler_error_t) {0}));
  ((pstate->result_node)=node);
  ((pstate->result_token)=NULL);
  return true;
}

/* i=256 j=0 */
token_t* pstate_get_result_token(pstate_t* pstate){
  if ((((pstate->error).parse_error_code)!=PARSE_ERROR_UNKNOWN))
  {
    log_debug("error code is not zero");
    ((pstate->error)=((compiler_error_t) {0}));
  }
  token_t* token = (pstate->result_token);
  ((pstate->result_token)=NULL);
  return token;
}

/* i=257 j=0 */
parse_node_t* pstate_get_result_node(pstate_t* pstate){
  if ((((pstate->error).parse_error_code)!=PARSE_ERROR_UNKNOWN))
  {
    log_debug("error code is not zero");
    ((pstate->error)=((compiler_error_t) {0}));
  }
  parse_node_t* result = (pstate->result_node);
  if ((result==NULL))
  {
    fatal_error(ERROR_ILLEGAL_STATE);
  }
  ((pstate->result_node)=NULL);
  return result;
}

/* i=258 j=0 */
parse_node_t* pstate_get_optional_result_node(pstate_t* pstate){
  ((pstate->error)=((compiler_error_t) {0}));
  parse_node_t* result = (pstate->result_node);
  ((pstate->result_node)=NULL);
  return result;
}

/* i=259 j=0 */
token_t* pstate_peek(pstate_t* pstate, int offset){
  return token_at((pstate->tokens), ((pstate->position)+offset));
}

/* i=260 j=0 */
token_t* pstate_advance(pstate_t* pstate){
  if (((pstate->error).parse_error_code))
  {
    log_debug("error code is not zero");
    ((pstate->error)=((compiler_error_t) {0}));
  }
  token_t* token = pstate_peek(pstate, 0);
  ((pstate->position)++);
  return token;
}

/* i=261 j=0 */
boolean_t pstate_match_token_string(pstate_t* pstate, char* token_string){
  token_t* token = pstate_peek(pstate, 0);
  return token_matches(token, token_string);
}

/* i=262 j=0 */
pstatus_t pstate_expect_token_string(pstate_t* pstate, char* token_string){
  token_t* token = pstate_peek(pstate, 0);
  if (token_matches(token, token_string))
  {
    ((pstate->result_node)=NULL);
    ((pstate->result_token)=token);
    ((pstate->position)+=1);
    return true;
  }
  (((pstate->error).parse_error_code)=PARSE_ERROR_EXPECTED_TOKEN);
  return false;
}

/* i=263 j=0 */
pstatus_t pstate_expect_token_type(pstate_t* pstate, token_type_t token_type){
  token_t* token = pstate_peek(pstate, 0);
  if ((token_type==(token->type)))
  {
    ((pstate->result_node)=NULL);
    ((pstate->result_token)=token);
    ((pstate->position)+=1);
    return true;
  }
  (((pstate->error).parse_error_code)=PARSE_ERROR_EXPECTED_TOKEN_TYPE);
  return false;
}

/* i=280 j=0 */
pstatus_t parse_declarations(pstate_t* pstate){
  uint64_t saved_position = (pstate->position);
  declarations_node_t* result = malloc_declarations();
  while (((pstate->position)<((pstate->tokens)->length)))
  {
    if ((!parse_declaration(pstate)))
    {
      return pstate_propagate_error(pstate, saved_position);
    }
    node_list_add_node((&(result->declarations)), pstate_get_result_node(pstate));
  }
  return pstate_set_result_node(pstate, to_node(result));
}

/* i=281 j=0 */
pstatus_t parse_declaration(pstate_t* pstate){
  uint64_t saved_position = (pstate->position);
  if ((((((parse_function_node(pstate)||parse_typedef_node(pstate_ignore_error(pstate)))||parse_enum_node_declaration(pstate_ignore_error(pstate)))||parse_variable_definition_node(pstate_ignore_error(pstate)))||parse_structure_node_declaration(pstate_ignore_error(pstate)))||parse_union_node_declaration(pstate_ignore_error(pstate))))
  {
    return true;
  }
  return pstate_error(pstate, saved_position, PARSE_ERROR_UNRECOGNIZED_TOP_LEVEL_DECLARATION);
}

/* i=282 j=0 */
pstatus_t parse_enum_node_declaration(pstate_t* pstate){
  uint64_t saved_position = (pstate->position);
  if ((!parse_enum_node(pstate)))
  {
    return pstate_propagate_error(pstate, saved_position);
  }
  parse_node_t* result = pstate_get_result_node(pstate);
  if ((!pstate_expect_token_string(pstate, ";")))
  {
    return pstate_propagate_error(pstate, saved_position);
  }
  return pstate_set_result_node(pstate, result);
}

/* i=283 j=0 */
pstatus_t parse_structure_node_declaration(pstate_t* pstate){
  uint64_t saved_position = (pstate->position);
  if ((!parse_structure_node(pstate)))
  {
    return pstate_propagate_error(pstate, saved_position);
  }
  parse_node_t* result = pstate_get_result_node(pstate);
  if ((!pstate_expect_token_string(pstate, ";")))
  {
    return pstate_propagate_error(pstate, saved_position);
  }
  return pstate_set_result_node(pstate, result);
}

/* i=284 j=0 */
pstatus_t parse_union_node_declaration(pstate_t* pstate){
  uint64_t saved_position = (pstate->position);
  if ((!parse_union_node(pstate)))
  {
    return pstate_propagate_error(pstate, saved_position);
  }
  parse_node_t* result = pstate_get_result_node(pstate);
  if ((!pstate_expect_token_string(pstate, ";")))
  {
    return pstate_propagate_error(pstate, saved_position);
  }
  return pstate_set_result_node(pstate, result);
}

/* i=285 j=0 */
pstatus_t parse_attribute_node(pstate_t* pstate){
  uint64_t saved_position = (pstate->position);
  if (((!pstate_expect_token_string(pstate, "__attribute__"))||(!parse_balanced_construct(pstate))))
  {
    return pstate_propagate_error(pstate, saved_position);
  }
  attribute_node_t* result = malloc_attribute_node();
  ((result->inner_start_token)=token_at((pstate->tokens), saved_position));
  ((result->inner_end_token)=pstate_peek(pstate, (-1)));
  return pstate_set_result_node(pstate, to_node(result));
}

/* i=286 j=0 */
pstatus_t parse_function_node(pstate_t* pstate){
  uint64_t saved_position = (pstate->position);
  token_t* storage_class_specifier = NULL;
  token_list_t function_specifiers = {0};
  node_list_t attributes = {0};
  while (1)
  {
    if ((((pstate_expect_token_string(pstate, "static")||pstate_expect_token_string(pstate_ignore_error(pstate), "extern"))||pstate_expect_token_string(pstate_ignore_error(pstate), "auto"))||pstate_expect_token_string(pstate_ignore_error(pstate), "register")))
    {
      if ((storage_class_specifier==NULL))
      {
        (storage_class_specifier=pstate_get_result_token(pstate));
      }
      else
      {
        return pstate_error(pstate, saved_position, PARSE_ERROR_CONFLICTING_STORAGE_CLASS_SPECIFIER);
      }
    }
    else
    if ((pstate_expect_token_string(pstate_ignore_error(pstate), "inline")||pstate_expect_token_string(pstate_ignore_error(pstate), "_Noreturn")))
    {
      token_list_add((&function_specifiers), pstate_get_result_token(pstate));
    }
    else
    if (parse_attribute_node(pstate_ignore_error(pstate)))
    {
      node_list_add_node((&attributes), pstate_get_result_node(pstate));
    }
    else
    {
      pstate_ignore_error(pstate);
      break;
    }
  }
  if ((!parse_type_node(pstate)))
  {
    return pstate_propagate_error(pstate, saved_position);
  }
  type_node_t* return_type = to_type_node(pstate_get_result_node(pstate));
  if ((!pstate_expect_token_type(pstate, TOKEN_TYPE_IDENTIFIER)))
  {
    return pstate_propagate_error(pstate, saved_position);
  }
  token_t* fn_name = pstate_get_result_token(pstate);
  if ((!pstate_expect_token_string(pstate, "(")))
  {
    return pstate_propagate_error(pstate, saved_position);
  }
  function_node_t* fn_node = malloc_function_node();
  ((fn_node->attributes)=attributes);
  ((fn_node->storage_class_specifier)=storage_class_specifier);
  ((fn_node->function_specifiers)=function_specifiers);
  ((fn_node->return_type)=return_type);
  ((fn_node->function_name)=fn_name);
  while (parse_function_argument_node(pstate))
  {
    node_list_add_node((&(fn_node->function_args)), pstate_get_result_node(pstate));
  }
  pstate_ignore_error(pstate);
  if ((!pstate_expect_token_string(pstate, ")")))
  {
    return pstate_propagate_error(pstate, saved_position);
  }
  if (parse_function_body_node(pstate))
  {
    ((fn_node->body)=pstate_get_result_node(pstate));
  }
  else
  {
    pstate_ignore_error(pstate);
    if ((!pstate_expect_token_string(pstate, ";")))
    {
      return pstate_propagate_error(pstate, saved_position);
    }
  }
  return pstate_set_result_node(pstate, to_node(fn_node));
}

/* i=287 j=0 */
pstatus_t parse_function_argument_node(pstate_t* pstate){
  uint64_t saved_position = (pstate->position);
  function_argument_node_t* result = malloc_function_argument_node();
  if (pstate_match_token_string(pstate, "..."))
  {
    pstate_advance(pstate);
    ((result->is_var_args)=true);
  }
  else
  {
    if ((!parse_type_node(pstate)))
    {
      return pstate_propagate_error(pstate, saved_position);
    }
    ((result->arg_type)=to_type_node(pstate_get_result_node(pstate)));
    if (pstate_expect_token_type(pstate, TOKEN_TYPE_IDENTIFIER))
    {
      ((result->arg_name)=pstate_get_result_token(pstate));
    }
    else
    {
      pstate_ignore_error(pstate);
    }
  }
  if (pstate_match_token_string(pstate, ","))
  {
    pstate_advance(pstate);
  }
  return pstate_set_result_node(pstate, to_node(result));
}

/* i=288 j=0 */
pstatus_t parse_function_body_node(pstate_t* pstate){
  if ((pstate->use_statement_parser))
  {
    log_info("USING STATEMENT PARSER");
    return parse_block(pstate);
  }
  else
  {
    return parse_balanced_construct(pstate);
  }
}

/* i=289 j=0 */
pstatus_t parse_typedef_node(pstate_t* pstate){
  uint64_t saved_position = (pstate->position);
  if ((!pstate_expect_token_string(pstate, "typedef")))
  {
    return pstate_propagate_error(pstate, saved_position);
  }
  if ((!parse_type_node(pstate)))
  {
    return pstate_propagate_error(pstate, saved_position);
  }
  type_node_t* type_node = to_type_node(pstate_get_result_node(pstate));
  if ((!pstate_expect_token_type(pstate, TOKEN_TYPE_IDENTIFIER)))
  {
    return pstate_propagate_error(pstate, saved_position);
  }
  token_t* name = pstate_get_result_token(pstate);
  if ((!pstate_expect_token_string(pstate, ";")))
  {
    return pstate_propagate_error(pstate, saved_position);
  }
  typedef_node_t* result = malloc_typedef_node();
  ((result->type_node)=type_node);
  ((result->name)=name);
  return pstate_set_result_node(pstate, to_node(result));
}

/* i=294 j=0 */
void buffer_append_dbg_parse_node(cdl_printer_t* printer, parse_node_t* node){
  switch ((node->tag))
  {
    case PARSE_NODE_DECLARATIONS:
    buffer_append_dbg_declarations(printer, to_declarations_node(node));
    break;
    case PARSE_NODE_ENUM:
    buffer_append_dbg_enum(printer, to_enum_node(node));
    break;
    case PARSE_NODE_ENUM_ELEMENT:
    buffer_append_dbg_enum_element(printer, to_enum_element_node(node));
    break;
    case PARSE_NODE_STRUCT:
    buffer_append_dbg_struct_node(printer, to_struct_node(node));
    break;
    case PARSE_NODE_FIELD:
    buffer_append_dbg_field_node(printer, to_field_node(node));
    break;
    case PARSE_NODE_TYPE:
    buffer_append_dbg_type_node(printer, to_type_node(node));
    break;
    ;
    case PARSE_NODE_LITERAL:
    buffer_append_dbg_literal_node(printer, to_literal_node(node));
    break;
    case PARSE_NODE_FUNCTION:
    buffer_append_dbg_function_node(printer, to_function_node(node));
    break;
    case PARSE_NODE_FUNCTION_ARGUMENT:
    buffer_append_dbg_function_argument_node(printer, to_function_argument_node(node));
    break;
    case PARSE_NODE_BALANCED_CONSTRUCT:
    buffer_append_dbg_balanced_construct_node(printer, to_balanced_construct_node(node));
    break;
    case PARSE_NODE_TYPEDEF:
    buffer_append_dbg_typedef_node(printer, to_typedef_node(node));
    break;
    case PARSE_NODE_VARIABLE_DEFINITION:
    buffer_append_dbg_variable_definition_node(printer, to_variable_definition_node(node));
    break;
    case PARSE_NODE_ATTRIBUTE:
    buffer_append_dbg_attribute_node(printer, to_attribute_node(node));
    break;
    case PARSE_NODE_IDENTIFIER:
    buffer_append_dbg_identifier_node(printer, to_identifier_node(node));
    break;
    case PARSE_NODE_OPERATOR:
    buffer_append_dbg_operator_node(printer, to_operator_node(node));
    break;
    case PARSE_NODE_BREAK_STATEMENT:
    buffer_append_dbg_break_statement_node(printer, to_break_statement_node(node));
    break;
    case PARSE_NODE_CONTINUE_STATEMENT:
    buffer_append_dbg_continue_statement_node(printer, to_continue_statement_node(node));
    break;
    case PARSE_NODE_LABEL_STATEMENT:
    buffer_append_dbg_label_statement_node(printer, to_label_statement_node(node));
    break;
    case PARSE_NODE_CASE_LABEL:
    buffer_append_dbg_case_label_node(printer, to_case_label_node(node));
    break;
    case PARSE_NODE_DEFAULT_LABEL:
    buffer_append_dbg_default_label_node(printer, to_default_label_node(node));
    break;
    case PARSE_NODE_CALL:
    buffer_append_dbg_call_node(printer, to_call_node(node));
    break;
    case PARSE_NODE_BLOCK:
    buffer_append_dbg_block_node(printer, to_block_node(node));
    break;
    case PARSE_NODE_WHILE_STATEMENT:
    buffer_append_dbg_while_node(printer, to_while_statement_node(node));
    break;
    case PARSE_NODE_FOR_STATEMENT:
    buffer_append_dbg_for_node(printer, to_for_statement_node(node));
    break;
    case PARSE_NODE_DO_STATEMENT:
    buffer_append_dbg_do_node(printer, to_do_statement_node(node));
    break;
    case PARSE_NODE_IF_STATEMENT:
    buffer_append_dbg_if_node(printer, to_if_statement_node(node));
    break;
    case PARSE_NODE_EMPTY_STATEMENT:
    buffer_append_dbg_empty_statement_node(printer, to_empty_statement_node(node));
    break;
    case PARSE_NODE_RETURN_STATEMENT:
    buffer_append_dbg_return_statement_node(printer, to_return_statement_node(node));
    break;
    case PARSE_NODE_EXPRESSION_STATEMENT:
    buffer_append_dbg_expression_statement_node(printer, to_expression_statement_node(node));
    break;
    case PARSE_NODE_SWITCH_STATEMENT:
    buffer_append_dbg_switch_node(printer, to_switch_statement_node(node));
    break;
    case PARSE_NODE_CONDITIONAL:
    buffer_append_dbg_conditional_node(printer, to_conditional_node(node));
    break;
    default:
    log_fatal("No debug printer for %s", parse_node_type_to_string((node->tag)));
    fatal_error(ERROR_ILLEGAL_STATE);
    break;
  }
}

/* i=295 j=0 */
void buffer_append_dbg_node_list(cdl_printer_t* printer, node_list_t list){
  cdl_start_array(printer);
  uint64_t length = node_list_length(list);
  for (
    uint64_t i = 0;
    (i<length);
    (i++))
  {
    buffer_append_dbg_parse_node(printer, node_list_get(list, i));
  }
  cdl_end_array(printer);
}

/* i=296 j=0 */
void buffer_append_dbg_tokens(cdl_printer_t* printer, value_array_t* tokens, char* field_name){
  cdl_key(printer, field_name);
  cdl_start_array(printer);
  uint64_t length = (tokens->length);
  for (
    uint64_t i = 0;
    (i<length);
    (i++))
  {
    token_t* token = value_array_get_ptr(tokens, i, typeof(token_t*));
    cdl_string(printer, token_to_string(token));
  }
  cdl_end_array(printer);
}

/* i=297 j=0 */
void buffer_append_dbg_declarations(cdl_printer_t* printer, declarations_node_t* node){
  buffer_append_dbg_node_list(printer, (node->declarations));
}

/* i=298 j=0 */
void buffer_append_dbg_enum(cdl_printer_t* printer, enum_node_t* node){
  cdl_start_table(printer);
  cdl_key(printer, "tag");
  cdl_string(printer, "PARSE_NODE_ENUM");
  if (((node->name)!=NULL))
  {
    cdl_key(printer, "name");
    cdl_string(printer, token_to_string((node->name)));
  }
  cdl_key(printer, "elements");
  buffer_append_dbg_node_list(printer, (node->elements));
  cdl_key(printer, "partial_definition");
  cdl_boolean(printer, (node->partial_definition));
  cdl_end_table(printer);
}

/* i=299 j=0 */
void buffer_append_dbg_struct_node(cdl_printer_t* printer, struct_node_t* node){
  cdl_start_table(printer);
  cdl_key(printer, "tag");
  cdl_string(printer, "PARSE_NODE_STRUCT");
  if (((node->name)!=NULL))
  {
    cdl_key(printer, "name");
    cdl_string(printer, token_to_string((node->name)));
  }
  cdl_key(printer, "partial_definition");
  cdl_boolean(printer, (node->partial_definition));
  cdl_key(printer, "fields");
  buffer_append_dbg_node_list(printer, (node->fields));
  cdl_end_table(printer);
}

/* i=300 j=0 */
void buffer_append_dbg_enum_element(cdl_printer_t* printer, enum_element_t* node){
  cdl_start_table(printer);
  cdl_key(printer, "tag");
  cdl_string(printer, "PARSE_NODE_ENUM_ELEMENT");
  if (((node->name)!=NULL))
  {
    cdl_key(printer, "name");
    cdl_string(printer, token_to_string((node->name)));
  }
  if (((node->value)!=NULL))
  {
    cdl_key(printer, "value");
    cdl_string(printer, token_to_string((node->value)));
  }
  cdl_end_table(printer);
}

/* i=301 j=0 */
void buffer_append_dbg_field_node(cdl_printer_t* printer, field_node_t* node){
  cdl_start_table(printer);
  cdl_key(printer, "tag");
  cdl_string(printer, "PARSE_NODE_FIELD");
  if (((node->name)!=NULL))
  {
    cdl_key(printer, "name");
    cdl_string(printer, token_to_string((node->name)));
  }
  if (((node->type)!=NULL))
  {
    cdl_key(printer, "type");
    buffer_append_dbg_type_node(printer, (node->type));
  }
  cdl_end_table(printer);
}

/* i=302 j=0 */
void buffer_append_dbg_type_node(cdl_printer_t* printer, type_node_t* node){
  cdl_start_table(printer);
  cdl_key(printer, "tag");
  cdl_string(printer, "PARSE_NODE_TYPE");
  cdl_key(printer, "type_node_kind");
  cdl_string(printer, type_node_kind_to_string((node->type_node_kind)));
  if (((node->qualifiers)>0))
  {
    cdl_key(printer, "qualifiers");
    cdl_start_array(printer);
    if ((((node->qualifiers)&TYPE_QUALIFIER_CONST)==TYPE_QUALIFIER_CONST))
    {
      cdl_string(printer, "const");
    }
    if ((((node->qualifiers)&TYPE_QUALIFIER_VOLATILE)==TYPE_QUALIFIER_VOLATILE))
    {
      cdl_string(printer, "volatile");
    }
    if ((((node->qualifiers)&TYPE_QUALIFIER_RESTRICT)==TYPE_QUALIFIER_RESTRICT))
    {
      cdl_string(printer, "restrict");
    }
    cdl_end_array(printer);
  }
  if (((node->type_name)!=NULL))
  {
    cdl_key(printer, "type_name");
    cdl_string(printer, token_to_string((node->type_name)));
  }
  if (((node->user_type)!=NULL))
  {
    cdl_key(printer, "user_type");
    buffer_append_dbg_parse_node(printer, (node->user_type));
  }
  cdl_key(printer, "type_args");
  buffer_append_dbg_node_list(printer, (node->type_args));
  cdl_end_table(printer);
}

/* i=303 j=0 */
void buffer_append_dbg_literal_node(cdl_printer_t* printer, literal_node_t* node){
  cdl_start_table(printer);
  cdl_key(printer, "tag");
  cdl_string(printer, "PARSE_NODE_LITERAL");
  if (((node->token)!=NULL))
  {
    cdl_key(printer, "token");
    cdl_string(printer, token_to_string((node->token)));
  }
  if (((node->tokens)!=NULL))
  {
    buffer_append_dbg_tokens(printer, (node->tokens), "tokens");
  }
  if (((node->initializer_node)!=NULL))
  {
    cdl_key(printer, "initializer_node");
    buffer_append_dbg_parse_node(printer, (node->initializer_node));
  }
  if (((node->initializer_type)!=NULL))
  {
    cdl_key(printer, "initializer_type");
    buffer_append_dbg_parse_node(printer, (node->initializer_type));
  }
  cdl_end_table(printer);
}

/* i=304 j=0 */
void buffer_append_dbg_function_node(cdl_printer_t* printer, function_node_t* node){
  cdl_start_table(printer);
  cdl_key(printer, "tag");
  cdl_string(printer, "PARSE_NODE_FUNCTION");
  cdl_key(printer, "attributes");
  buffer_append_dbg_node_list(printer, (node->attributes));
  if (((node->storage_class_specifier)!=NULL))
  {
    cdl_key(printer, "storage_class_specifier");
    cdl_string(printer, token_to_string((node->storage_class_specifier)));
  }
  if (((node->return_type)!=NULL))
  {
    cdl_key(printer, "return_type");
    buffer_append_dbg_type_node(printer, (node->return_type));
  }
  if (((node->function_name)!=NULL))
  {
    cdl_key(printer, "function_name");
    cdl_string(printer, token_to_string((node->function_name)));
  }
  cdl_key(printer, "function_args");
  buffer_append_dbg_node_list(printer, (node->function_args));
  if (((node->body)!=NULL))
  {
    cdl_key(printer, "body");
    buffer_append_dbg_parse_node(printer, (node->body));
  }
  cdl_end_table(printer);
}

/* i=305 j=0 */
void buffer_append_dbg_function_argument_node(cdl_printer_t* printer, function_argument_node_t* node){
  cdl_start_table(printer);
  cdl_key(printer, "tag");
  cdl_string(printer, "PARSE_NODE_FUNCTION_ARGUEMENT");
  if (((node->arg_type)!=NULL))
  {
    cdl_key(printer, "arg_type");
    buffer_append_dbg_type_node(printer, (node->arg_type));
  }
  if (((node->arg_name)!=NULL))
  {
    cdl_key(printer, "arg_name");
    cdl_string(printer, token_to_string((node->arg_name)));
  }
  cdl_end_table(printer);
}

/* i=306 j=0 */
void buffer_append_dbg_balanced_construct_node(cdl_printer_t* printer, balanced_construct_node_t* node){
  cdl_start_table(printer);
  cdl_key(printer, "tag");
  cdl_string(printer, "PARSE_NODE_BALANCED_CONSTRUCT");
  if (((node->start_token)!=NULL))
  {
    cdl_key(printer, "start_token");
    cdl_string(printer, token_to_string((node->start_token)));
  }
  if (((node->end_token)!=NULL))
  {
    cdl_key(printer, "end_token");
    cdl_string(printer, token_to_string((node->end_token)));
  }
  cdl_end_table(printer);
}

/* i=307 j=0 */
void buffer_append_dbg_typedef_node(cdl_printer_t* printer, typedef_node_t* node){
  cdl_start_table(printer);
  cdl_key(printer, "tag");
  cdl_string(printer, "PARSE_NODE_TYPEDEF");
  if (((node->name)!=NULL))
  {
    cdl_key(printer, "name");
    cdl_string(printer, token_to_string((node->name)));
  }
  if (((node->type_node)!=NULL))
  {
    cdl_key(printer, "type_node");
    buffer_append_dbg_type_node(printer, (node->type_node));
  }
  cdl_end_table(printer);
}

/* i=308 j=0 */
void buffer_append_dbg_variable_definition_node(cdl_printer_t* printer, variable_definition_node_t* node){
  cdl_start_table(printer);
  cdl_key(printer, "tag");
  cdl_string(printer, "PARSE_NODE_VARIABLE_DEFINITION");
  if (((node->name)!=NULL))
  {
    cdl_key(printer, "name");
    cdl_string(printer, token_to_string((node->name)));
  }
  if (((node->type)!=NULL))
  {
    cdl_key(printer, "type");
    buffer_append_dbg_type_node(printer, (node->type));
  }
  if (((node->value)!=NULL))
  {
    cdl_key(printer, "value");
    buffer_append_dbg_parse_node(printer, (node->value));
  }
  if (((node->storage_class_specifier)!=NULL))
  {
    cdl_key(printer, "storage_class_specifier");
    cdl_string(printer, token_to_string((node->storage_class_specifier)));
  }
  if (((node->suffixes)!=NULL))
  {
    cdl_key(printer, "suffixes");
    cdl_start_array(printer);
    for (
      uint64_t i = 0;
      (i<((node->suffixes)->length));
      (i++))
    {
      parse_node_t* suffix = value_array_get_ptr((node->suffixes), i, typeof(parse_node_t*));
      buffer_append_dbg_parse_node(printer, suffix);
    }
    cdl_end_array(printer);
  }
  cdl_end_table(printer);
}

/* i=309 j=0 */
void buffer_append_dbg_attribute_node(cdl_printer_t* printer, attribute_node_t* node){
  cdl_start_table(printer);
  cdl_key(printer, "tag");
  cdl_string(printer, "PARSE_NODE_ATTRIBUTE");
  if (((node->inner_start_token)!=NULL))
  {
    cdl_key(printer, "inner_start_token");
    cdl_string(printer, token_to_string((node->inner_start_token)));
  }
  if (((node->inner_end_token)!=NULL))
  {
    cdl_key(printer, "inner_end_token");
    cdl_string(printer, token_to_string((node->inner_end_token)));
  }
  cdl_end_table(printer);
}

/* i=310 j=0 */
void buffer_append_dbg_empty_statement_node(cdl_printer_t* printer, empty_statement_node_t* node){
  cdl_start_table(printer);
  cdl_key(printer, "tag");
  cdl_string(printer, "PARSE_NODE_EMPTY_STATEMENT");
  cdl_end_table(printer);
}

/* i=311 j=0 */
void buffer_append_dbg_block_node(cdl_printer_t* printer, block_node_t* node){
  cdl_start_table(printer);
  cdl_key(printer, "tag");
  cdl_string(printer, "PARSE_NODE_BLOCK");
  cdl_key(printer, "statements");
  buffer_append_dbg_node_list(printer, (node->statements));
  cdl_end_table(printer);
}

/* i=312 j=0 */
void buffer_append_dbg_if_node(cdl_printer_t* printer, if_statement_node_t* node){
  cdl_start_table(printer);
  cdl_key(printer, "tag");
  cdl_string(printer, "PARSE_NODE_IF_STATEMENT");
  if (((node->if_condition)!=NULL))
  {
    cdl_key(printer, "if_condition");
    buffer_append_dbg_parse_node(printer, (node->if_condition));
  }
  if (((node->if_true)!=NULL))
  {
    cdl_key(printer, "if_true");
    buffer_append_dbg_parse_node(printer, (node->if_true));
  }
  if (((node->if_else)!=NULL))
  {
    cdl_key(printer, "if_else");
    buffer_append_dbg_parse_node(printer, (node->if_else));
  }
  cdl_end_table(printer);
}

/* i=313 j=0 */
void buffer_append_dbg_while_node(cdl_printer_t* printer, while_statement_node_t* node){
  cdl_start_table(printer);
  cdl_key(printer, "tag");
  cdl_string(printer, "PARSE_NODE_WHILE_STATEMENT");
  if (((node->condition)!=NULL))
  {
    cdl_key(printer, "condition");
    buffer_append_dbg_parse_node(printer, (node->condition));
  }
  if (((node->body)!=NULL))
  {
    cdl_key(printer, "body");
    buffer_append_dbg_parse_node(printer, (node->body));
  }
  cdl_end_table(printer);
}

/* i=314 j=0 */
void buffer_append_dbg_for_node(cdl_printer_t* printer, for_statement_node_t* node){
  cdl_start_table(printer);
  cdl_key(printer, "tag");
  cdl_string(printer, "PARSE_NODE_FOR_STATEMENT");
  if (((node->for_init)!=NULL))
  {
    cdl_key(printer, "for_init");
    buffer_append_dbg_parse_node(printer, (node->for_init));
  }
  if (((node->for_test)!=NULL))
  {
    cdl_key(printer, "for_test");
    buffer_append_dbg_parse_node(printer, (node->for_test));
  }
  if (((node->for_increment)!=NULL))
  {
    cdl_key(printer, "for_increment");
    buffer_append_dbg_parse_node(printer, (node->for_increment));
  }
  if (((node->for_body)!=NULL))
  {
    cdl_key(printer, "for_body");
    buffer_append_dbg_parse_node(printer, (node->for_body));
  }
  cdl_end_table(printer);
}

/* i=315 j=0 */
void buffer_append_dbg_do_node(cdl_printer_t* printer, do_statement_node_t* node){
  cdl_start_table(printer);
  cdl_key(printer, "tag");
  cdl_string(printer, "PARSE_NODE_DO_STATEMENT");
  if (((node->body)!=NULL))
  {
    cdl_key(printer, "body");
    buffer_append_dbg_parse_node(printer, (node->body));
  }
  if (((node->condition)!=NULL))
  {
    cdl_key(printer, "condition");
    buffer_append_dbg_parse_node(printer, (node->condition));
  }
  cdl_end_table(printer);
}

/* i=316 j=0 */
void buffer_append_dbg_break_statement_node(cdl_printer_t* printer, break_statement_node_t* node){
  cdl_start_table(printer);
  cdl_key(printer, "tag");
  cdl_string(printer, "PARSE_NODE_BREAK_STATEMENT");
  if (((node->break_keyword_token)!=NULL))
  {
    cdl_key(printer, "break_keyword_token");
    cdl_string(printer, token_to_string((node->break_keyword_token)));
  }
  cdl_end_table(printer);
}

/* i=317 j=0 */
void buffer_append_dbg_continue_statement_node(cdl_printer_t* printer, continue_statement_node_t* node){
  cdl_start_table(printer);
  cdl_key(printer, "tag");
  cdl_string(printer, "PARSE_NODE_CONTINUE_STATEMENT");
  if (((node->continue_keyword_token)!=NULL))
  {
    cdl_key(printer, "continue_keyword_token");
    cdl_string(printer, token_to_string((node->continue_keyword_token)));
  }
  cdl_end_table(printer);
}

/* i=318 j=0 */
void buffer_append_dbg_label_statement_node(cdl_printer_t* printer, label_statement_node_t* node){
  cdl_start_table(printer);
  cdl_key(printer, "tag");
  cdl_string(printer, "PARSE_NODE_LABEL_STATEMENT");
  if (((node->label)!=NULL))
  {
    cdl_key(printer, "label");
    cdl_string(printer, token_to_string((node->label)));
  }
  cdl_end_table(printer);
}

/* i=319 j=0 */
void buffer_append_dbg_case_label_node(cdl_printer_t* printer, case_label_node_t* node){
  cdl_start_table(printer);
  cdl_key(printer, "tag");
  cdl_string(printer, "PARSE_NODE_CASE_LABEL");
  if (((node->expression)!=NULL))
  {
    cdl_key(printer, "expression");
    buffer_append_dbg_parse_node(printer, (node->expression));
  }
  cdl_end_table(printer);
}

/* i=320 j=0 */
void buffer_append_dbg_default_label_node(cdl_printer_t* printer, default_label_node_t* node){
  cdl_start_table(printer);
  cdl_key(printer, "tag");
  cdl_string(printer, "PARSE_NODE_DEFAULT_LABEL");
  cdl_end_table(printer);
}

/* i=321 j=0 */
void buffer_append_dbg_return_statement_node(cdl_printer_t* printer, return_statement_node_t* node){
  cdl_start_table(printer);
  cdl_key(printer, "tag");
  cdl_string(printer, "PARSE_NODE_RETURN_STATEMENT");
  if (((node->expression)!=NULL))
  {
    cdl_key(printer, "expression");
    buffer_append_dbg_parse_node(printer, (node->expression));
  }
  cdl_end_table(printer);
}

/* i=322 j=0 */
void buffer_append_dbg_expression_statement_node(cdl_printer_t* printer, expression_statement_node_t* node){
  cdl_start_table(printer);
  cdl_key(printer, "tag");
  cdl_string(printer, "PARSE_NODE_EXPRESSION_STATEMENT");
  if (((node->expression)!=NULL))
  {
    cdl_key(printer, "expression");
    buffer_append_dbg_parse_node(printer, (node->expression));
  }
  cdl_end_table(printer);
}

/* i=323 j=0 */
void buffer_append_dbg_identifier_node(cdl_printer_t* printer, identifier_node_t* node){
  cdl_start_table(printer);
  cdl_key(printer, "tag");
  cdl_string(printer, "PARSE_NODE_IDENTIFIER");
  cdl_key(printer, "token");
  cdl_string(printer, token_to_string((node->token)));
  cdl_end_table(printer);
}

/* i=324 j=0 */
void buffer_append_dbg_operator_node(cdl_printer_t* printer, operator_node_t* node){
  cdl_start_table(printer);
  cdl_key(printer, "tag");
  cdl_string(printer, "PARSE_NODE_OPERATOR");
  cdl_key(printer, "operator");
  cdl_string(printer, token_to_string((node->operator)));
  if (((node->left)!=NULL))
  {
    cdl_key(printer, "left");
    buffer_append_dbg_parse_node(printer, (node->left));
  }
  if (((node->right)!=NULL))
  {
    cdl_key(printer, "right");
    buffer_append_dbg_parse_node(printer, (node->right));
  }
  cdl_end_table(printer);
}

/* i=325 j=0 */
void buffer_append_dbg_call_node(cdl_printer_t* printer, call_node_t* node){
  cdl_start_table(printer);
  cdl_key(printer, "tag");
  cdl_string(printer, "PARSE_NODE_CALL");
  cdl_key(printer, "function");
  buffer_append_dbg_parse_node(printer, (node->function));
  cdl_key(printer, "args");
  buffer_append_dbg_node_list(printer, (node->args));
  cdl_end_table(printer);
}

/* i=326 j=0 */
void buffer_append_dbg_conditional_node(cdl_printer_t* printer, conditional_node_t* node){
  cdl_start_table(printer);
  cdl_key(printer, "tag");
  cdl_string(printer, "PARSE_NODE_CONDITIONAL");
  if (((node->condition)!=NULL))
  {
    cdl_key(printer, "condition");
    buffer_append_dbg_parse_node(printer, (node->condition));
  }
  if (((node->expr_if_true)!=NULL))
  {
    cdl_key(printer, "expr_if_true");
    buffer_append_dbg_parse_node(printer, (node->expr_if_true));
  }
  if (((node->expr_if_false)!=NULL))
  {
    cdl_key(printer, "expr_if_false");
    buffer_append_dbg_parse_node(printer, (node->expr_if_false));
  }
  cdl_end_table(printer);
}

/* i=327 j=0 */
void buffer_append_dbg_switch_node(cdl_printer_t* printer, switch_statement_node_t* node){
  cdl_start_table(printer);
  cdl_key(printer, "tag");
  cdl_string(printer, "PARSE_NODE_SWITCH_STATEMENT");
  if (((node->expression)!=NULL))
  {
    cdl_key(printer, "expression");
    buffer_append_dbg_parse_node(printer, (node->expression));
  }
  if (((node->block)!=NULL))
  {
    cdl_key(printer, "block");
    buffer_append_dbg_parse_node(printer, (node->block));
  }
  cdl_end_table(printer);
}

/* i=328 j=0 */
void debug_append_tokens(buffer_t* buffer, value_array_t* tokens){
  for (
    int i = 0;
    (i<(tokens->length));
    (i++))
  {
    token_t* token = token_at(tokens, i);
    buffer_append_sub_buffer(buffer, (token->start), (token->end), (token->buffer));
  }
}

/* i=329 j=0 */
printer_t* append_parse_node(printer_t* printer, parse_node_t* node){
  switch ((node->tag))
  {
    case PARSE_NODE_ENUM:
    return append_enum_node(printer, to_enum_node(node));
    case PARSE_NODE_STRUCT:
    case PARSE_NODE_UNION:
    return append_struct_node(printer, to_struct_node(node));
    case PARSE_NODE_TYPE:
    return append_type_node(printer, to_type_node(node));
    case PARSE_NODE_LITERAL:
    return append_literal_node(printer, to_literal_node(node));
    case PARSE_NODE_IDENTIFIER:
    return append_identifier_node(printer, to_identifier_node(node));
    case PARSE_NODE_TYPEDEF:
    return append_typedef_node(printer, to_typedef_node(node));
    case PARSE_NODE_BLOCK:
    return append_block_node(printer, to_block_node(node));
    case PARSE_NODE_BALANCED_CONSTRUCT:
    return append_balanced_construct_node(printer, to_balanced_construct_node(node));
    case PARSE_NODE_VARIABLE_DEFINITION:
    return append_variable_definition_node(printer, to_variable_definition_node(node), true);
    case PARSE_NODE_WHILE_STATEMENT:
    return append_while_statement_node(printer, to_while_statement_node(node));
    case PARSE_NODE_FOR_STATEMENT:
    return append_for_statement_node(printer, to_for_statement_node(node));
    case PARSE_NODE_DO_STATEMENT:
    return append_do_statement_node(printer, to_do_statement_node(node));
    case PARSE_NODE_EMPTY_STATEMENT:
    return append_empty_statement_node(printer, to_empty_statement_node(node));
    case PARSE_NODE_IF_STATEMENT:
    return append_if_statement_node(printer, to_if_statement_node(node));
    case PARSE_NODE_RETURN_STATEMENT:
    return append_return_statement_node(printer, to_return_statement_node(node));
    case PARSE_NODE_BREAK_STATEMENT:
    return append_break_statement_node(printer, to_break_statement_node(node));
    case PARSE_NODE_CONTINUE_STATEMENT:
    return append_continue_statement_node(printer, to_continue_statement_node(node));
    case PARSE_NODE_LABEL_STATEMENT:
    return append_label_statement_node(printer, to_label_statement_node(node));
    case PARSE_NODE_CASE_LABEL:
    return append_case_label_node(printer, to_case_label_node(node));
    case PARSE_NODE_DEFAULT_LABEL:
    return append_default_label_node(printer, to_default_label_node(node));
    case PARSE_NODE_EXPRESSION_STATEMENT:
    return append_expression_statement_node(printer, to_expression_statement_node(node));
    case PARSE_NODE_SWITCH_STATEMENT:
    return append_switch_statement_node(printer, to_switch_statement_node(node));
    case PARSE_NODE_OPERATOR:
    return append_operator_node(printer, to_operator_node(node));
    case PARSE_NODE_CONDITIONAL:
    return append_conditional_node(printer, to_conditional_node(node));
    case PARSE_NODE_CALL:
    return append_call_node(printer, to_call_node(node));
    default:
    break;
  }
  log_fatal("No C file appender for %s", parse_node_type_to_string((node->tag)));
  fatal_error(ERROR_ILLEGAL_STATE);
}

/* i=330 j=0 */
printer_t* append_c_function_node_prefix(printer_t* printer, function_node_t* node){
  for (
    int i = 0;
    (i<node_list_length((node->attributes)));
    (i++))
  {
    append_c_attribute_node(printer, to_attribute_node(node_list_get((node->attributes), i)));
    append_string(printer, " ");
  }
  if (((node->storage_class_specifier)!=NULL))
  {
    append_token(printer, (node->storage_class_specifier));
    append_string(printer, " ");
  }
  for (
    int i = 0;
    (i<token_list_length((node->function_specifiers)));
    (i++))
  {
    append_token(printer, token_list_get((node->function_specifiers), i));
    append_string(printer, " ");
  }
  append_type_node(printer, (node->return_type));
  printer_space(printer);
  append_token(printer, (node->function_name));
  append_string(printer, "(");
  for (
    int i = 0;
    (i<node_list_length((node->function_args)));
    (i++))
  {
    if ((i>0))
    {
      append_string(printer, ", ");
    }
    function_argument_node_t* arg_node = to_function_argument_node(node_list_get((node->function_args), i));
    append_c_function_argument_node(printer, arg_node);
  }
  append_string(printer, ")");
  return printer;
}

/* i=331 j=0 */
printer_t* append_c_function_node_prototype(printer_t* printer, function_node_t* node){
  append_c_function_node_prefix(printer, node);
  append_string(printer, ";\n");
  return printer;
}

/* i=332 j=0 */
printer_t* append_balanced_construct_node(printer_t* printer, balanced_construct_node_t* node){
  uint64_t start = ((node->start_token)->start);
  uint64_t end = ((node->end_token)->end);
  buffer_append_sub_buffer((printer->buffer), start, end, ((node->start_token)->buffer));
  return printer;
}

/* i=333 j=0 */
printer_t* append_c_function_node_and_body(printer_t* printer, function_node_t* node){
  append_c_function_node_prefix(printer, node);
  append_parse_node(printer, (node->body));
  printer_newline(printer);
  return printer;
}

/* i=334 j=0 */
printer_t* append_c_function_argument_node(printer_t* printer, function_argument_node_t* node){
  if ((node->is_var_args))
  {
    append_string(printer, "...");
  }
  else
  {
    append_type_node(printer, (node->arg_type));
    if (((node->arg_name)!=NULL))
    {
      printer_space(printer);
      append_token(printer, (node->arg_name));
    }
  }
  return printer;
}

/* i=335 j=0 */
printer_t* append_type_node(printer_t* printer, type_node_t* node){
  if ((((node->qualifiers)&TYPE_QUALIFIER_CONST)==TYPE_QUALIFIER_CONST))
  {
    append_string(printer, "const ");
  }
  if ((((node->qualifiers)&TYPE_QUALIFIER_VOLATILE)==TYPE_QUALIFIER_VOLATILE))
  {
    append_string(printer, "volatile ");
  }
  if ((((node->qualifiers)&TYPE_QUALIFIER_RESTRICT)==TYPE_QUALIFIER_RESTRICT))
  {
    append_string(printer, "restrict ");
  }
  switch ((node->type_node_kind))
  {
    case TYPE_NODE_KIND_POINTER:
    append_type_node(printer, to_type_node(node_list_get((node->type_args), 0)));
    append_string(printer, "*");
    break;
    case TYPE_NODE_KIND_PRIMITIVE_TYPENAME:
    case TYPE_NODE_KIND_TYPENAME:
    if (((node->type_name)!=NULL))
    {
      append_token(printer, (node->type_name));
    }
    break;
    case TYPE_NODE_KIND_TYPE_EXPRESSION:
    if (token_matches((node->type_name), "fn_t"))
    {
      append_fn_type_node(printer, node);
    }
    else
    {
      append_parse_node(printer, (node->user_type));
    }
    break;
    case TYPE_NODE_KIND_ARRAY:
    append_string(printer, "typeof(");
    append_type_node(printer, to_type_node(node_list_get((node->type_args), 0)));
    append_string(printer, "[])");
    break;
    case TYPE_NODE_KIND_TYPEOF:
    append_string(printer, "typeof(");
    append_type_node(printer, to_type_node((node->user_type)));
    append_string(printer, ")");
    break;
    default:
    log_fatal("type_node_kind is not expected %s", type_node_kind_to_string((node->type_node_kind)));
    fatal_error(ERROR_ILLEGAL_STATE);
    break;
  }
  return printer;
}

/* i=336 j=0 */
printer_t* append_fn_type_node(printer_t* printer, type_node_t* node){
  append_token(printer, (node->type_name));
  append_string(printer, "(");
  for (
    int i = 0;
    (i<node_list_length((node->type_args)));
    (i++))
  {
    if ((i>0))
    {
      append_string(printer, ", ");
    }
    append_parse_node(printer, node_list_get((node->type_args), i));
  }
  append_string(printer, ")");
  return printer;
}

/* i=337 j=0 */
printer_t* append_c_attribute_node(printer_t* printer, attribute_node_t* node){
  append_c_raw_token_span(printer, (node->inner_start_token), (node->inner_end_token));
  return printer;
}

/* i=338 j=0 */
printer_t* append_c_raw_token_span(printer_t* printer, token_t* start_token, token_t* end_token){
  if (((start_token->buffer)!=(end_token->buffer)))
  {
    fatal_error(ERROR_ILLEGAL_STATE);
  }
  buffer_append_sub_buffer((printer->buffer), (start_token->start), (end_token->end), (start_token->buffer));
  return printer;
}

/* i=339 j=0 */
printer_t* append_enum_node(printer_t* printer, enum_node_t* node){
  append_string(printer, "enum ");
  if (((node->name)!=NULL))
  {
    append_token(printer, (node->name));
    printer_newline(printer);
  }
  if ((node->partial_definition))
  {
    return printer;
  }
  append_string(printer, "{\n");
  printer_increase_indent(printer);
  for (
    int i = 0;
    (i<node_list_length((node->elements)));
    (i++))
  {
    printer_indent(printer);
    append_enum_element(printer, to_enum_element_node(node_list_get((node->elements), i)));
    append_string(printer, ",\n");
  }
  append_string(printer, "}");
  printer_decrease_indent(printer);
  return printer;
}

/* i=340 j=0 */
printer_t* append_enum_element(printer_t* printer, enum_element_t* node){
  append_token(printer, (node->name));
  if (((node->value)!=NULL))
  {
    append_string(printer, " = ");
    append_token(printer, (node->value));
  }
  return printer;
}

/* i=341 j=0 */
printer_t* append_enum_to_string(printer_t* printer, enum_node_t* node, char* to_string_fn_prefix, char* type_string){
  append_string(printer, "char* ");
  append_string(printer, to_string_fn_prefix);
  append_string(printer, "_to_string(");
  append_string(printer, type_string);
  append_string(printer, " value) {\n");
  printer_increase_indent(printer);
  printer_indent(printer);
  append_string(printer, "switch (value) {\n");
  printer_indent(printer);
  for (
    int i = 0;
    (i<node_list_length((node->elements)));
    (i++))
  {
    enum_element_t* element = to_enum_element_node(node_list_get((node->elements), i));
    printer_indent(printer);
    append_string(printer, "case ");
    append_token(printer, (element->name));
    append_string(printer, ":\n");
    printer_increase_indent(printer);
    append_string(printer, "return \"");
    append_token(printer, (element->name));
    append_string(printer, "\";\n");
    printer_decrease_indent(printer);
  }
  printer_indent(printer);
  append_string(printer, "default:\n");
  printer_increase_indent(printer);
  printer_indent(printer);
  append_string(printer, "return \"<<unknown-");
  append_string(printer, to_string_fn_prefix);
  append_string(printer, ">>\";\n");
  printer_decrease_indent(printer);
  printer_indent(printer);
  append_string(printer, "}\n");
  printer_decrease_indent(printer);
  append_string(printer, "}\n\n");
  return printer;
}

/* i=342 j=0 */
printer_t* append_string_to_enum(printer_t* printer, enum_node_t* node, char* to_string_fn_prefix, char* type_string){
  append_string(printer, type_string);
  append_string(printer, " string_to_");
  append_string(printer, to_string_fn_prefix);
  append_string(printer, "(char* value) {\n");
  printer_increase_indent(printer);
  for (
    int i = 0;
    (i<node_list_length((node->elements)));
    (i++))
  {
    enum_element_t* element = to_enum_element_node(node_list_get((node->elements), i));
    printer_indent(printer);
    append_string(printer, "if (strcmp(value, \"");
    append_token(printer, (element->name));
    append_string(printer, "\") == 0) {\n");
    printer_increase_indent(printer);
    append_string(printer, "return ");
    append_token(printer, (element->name));
    append_string(printer, ";\n");
    printer_decrease_indent(printer);
    printer_indent(printer);
    append_string(printer, "}\n");
  }
  printer_indent(printer);
  append_string(printer, "return 0;\n");
  printer_decrease_indent(printer);
  append_string(printer, "}\n\n");
  return printer;
}

/* i=343 j=0 */
printer_t* append_field_node(printer_t* printer, field_node_t* node){
  append_type_node(printer, (node->type));
  append_string(printer, " ");
  if (((node->name)!=NULL))
  {
    append_token(printer, (node->name));
  }
  return printer;
}

/* i=344 j=0 */
printer_t* append_struct_node(printer_t* printer, struct_node_t* node){
  append_string(printer, (((node->tag)==PARSE_NODE_UNION) ? "union " : "struct "));
  if (((node->name)!=NULL))
  {
    append_token(printer, (node->name));
  }
  if ((!(node->partial_definition)))
  {
    append_string(printer, " {\n");
    printer_increase_indent(printer);
    for (
      int i = 0;
      (i<node_list_length((node->fields)));
      (i++))
    {
      printer_indent(printer);
      append_field_node(printer, to_field_node(node_list_get((node->fields), i)));
      append_string(printer, ";\n");
    }
    printer_decrease_indent(printer);
    append_string(printer, "}");
  }
  return printer;
}

/* i=345 j=0 */
printer_t* append_typedef_node(printer_t* printer, typedef_node_t* node){
  append_string(printer, "typedef ");
  append_type_node(printer, (node->type_node));
  append_string(printer, " ");
  append_token(printer, (node->name));
  append_string(printer, ";\n");
  return printer;
}

/* i=346 j=0 */
printer_t* append_cpp_include_node(printer_t* printer, cpp_include_node_t* node){
  append_string(printer, (node->text));
  return printer;
}

/* i=347 j=0 */
printer_t* append_cpp_define_node(printer_t* printer, cpp_define_node_t* node){
  append_string(printer, (node->text));
  return printer;
}

/* i=348 j=0 */
printer_t* append_variable_definition_node(printer_t* printer, variable_definition_node_t* node, boolean_t is_library){
  printer_indent(printer);
  boolean_t is_header_file = (!is_library);
  if (((node->storage_class_specifier)!=NULL))
  {
    append_token(printer, (node->storage_class_specifier));
    append_string(printer, " ");
  }
  else
  if (is_header_file)
  {
    append_string(printer, "extern ");
  }
  append_type_node(printer, (node->type));
  append_string(printer, " ");
  append_token(printer, (node->name));
  if ((node->suffixes))
  {
    for (
      int i = 0;
      (i<((node->suffixes)->length));
      (i++))
    {
      append_parse_node(printer, value_array_get_ptr((node->suffixes), i, typeof(parse_node_t*)));
    }
  }
  if ((is_library&&((node->value)!=NULL)))
  {
    append_string(printer, " = ");
    append_parse_node(printer, (node->value));
  }
  append_string(printer, ";\n");
  return printer;
}

/* i=349 j=0 */
printer_t* append_literal_node(printer_t* printer, literal_node_t* node){
  if (((node->token)!=NULL))
  {
    append_token(printer, (node->token));
  }
  else
  if (((node->initializer_node)!=NULL))
  {
    if (((node->initializer_type)!=NULL))
    {
      append_string(printer, "((");
      append_parse_node(printer, (node->initializer_type));
      append_string(printer, ") ");
    }
    append_balanced_construct_node(printer, to_balanced_construct_node((node->initializer_node)));
    if (((node->initializer_type)!=NULL))
    {
      append_string(printer, ")");
    }
  }
  else
  if ((((node->tokens)!=NULL)&&(((node->tokens)->length)>0)))
  {
    for (
      uint64_t i = 0;
      (i<((node->tokens)->length));
      (i++))
    {
      if ((i>0))
      {
        append_string(printer, " ");
      }
      token_t* token = value_array_get_ptr((node->tokens), i, typeof(token_t*));
      append_token(printer, token);
    }
  }
  else
  {
    append_string(printer, "FIXME");
  }
  return printer;
}

/* i=350 j=0 */
printer_t* append_identifier_node(printer_t* printer, identifier_node_t* node){
  if (((node->token)==NULL))
  {
    fatal_error(ERROR_ILLEGAL_STATE);
  }
  append_token(printer, (node->token));
  return printer;
}

/* i=351 j=0 */
printer_t* append_empty_statement_node(printer_t* printer, empty_statement_node_t* node){
  printer_indent(printer);
  append_string(printer, ";\n");
  return printer;
}

/* i=352 j=0 */
printer_t* append_break_statement_node(printer_t* printer, break_statement_node_t* node){
  printer_indent(printer);
  append_string(printer, "break;\n");
  return printer;
}

/* i=353 j=0 */
printer_t* append_continue_statement_node(printer_t* printer, continue_statement_node_t* node){
  printer_indent(printer);
  append_string(printer, "continue;\n");
  return printer;
}

/* i=354 j=0 */
printer_t* append_label_statement_node(printer_t* printer, label_statement_node_t* node){
  printer_indent(printer);
  append_token(printer, (node->label));
  append_string(printer, ":\n");
  return printer;
}

/* i=355 j=0 */
printer_t* append_case_label_node(printer_t* printer, case_label_node_t* node){
  printer_indent(printer);
  append_string(printer, "case ");
  append_parse_node(printer, (node->expression));
  append_string(printer, ":\n");
  return printer;
}

/* i=356 j=0 */
printer_t* append_default_label_node(printer_t* printer, default_label_node_t* node){
  printer_indent(printer);
  append_string(printer, "default:\n");
  return printer;
}

/* i=357 j=0 */
printer_t* append_expression_statement_node(printer_t* printer, expression_statement_node_t* node){
  printer_indent(printer);
  append_parse_node(printer, (node->expression));
  append_string(printer, ";\n");
  return printer;
}

/* i=358 j=0 */
printer_t* append_block_node(printer_t* printer, block_node_t* node){
  printer_indent(printer);
  append_string(printer, "{\n");
  printer_increase_indent(printer);
  uint64_t length = node_list_length((node->statements));
  for (
    uint64_t i = 0;
    (i<length);
    (i++))
  {
    append_parse_node(printer, node_list_get((node->statements), i));
  }
  printer_decrease_indent(printer);
  printer_indent(printer);
  append_string(printer, "}\n");
  return printer;
}

/* i=359 j=0 */
printer_t* append_if_statement_node(printer_t* printer, if_statement_node_t* node){
  printer_indent(printer);
  append_string(printer, "if (");
  append_parse_node(printer, (node->if_condition));
  append_string(printer, ")\n");
  append_parse_node(printer, (node->if_true));
  if ((node->if_else))
  {
    printer_indent(printer);
    append_string(printer, "else\n");
    append_parse_node(printer, (node->if_else));
  }
  return printer;
}

/* i=360 j=0 */
printer_t* append_while_statement_node(printer_t* printer, while_statement_node_t* node){
  printer_indent(printer);
  append_string(printer, "while (");
  append_parse_node(printer, (node->condition));
  append_string(printer, ")\n");
  append_parse_node(printer, (node->body));
  return printer;
}

/* i=361 j=0 */
printer_t* append_switch_statement_node(printer_t* printer, switch_statement_node_t* node){
  printer_indent(printer);
  append_string(printer, "switch (");
  append_parse_node(printer, (node->expression));
  append_string(printer, ")\n");
  append_parse_node(printer, (node->block));
  return printer;
}

/* i=362 j=0 */
printer_t* append_for_statement_node(printer_t* printer, for_statement_node_t* node){
  printer_indent(printer);
  append_string(printer, "for (\n");
  printer_increase_indent(printer);
  if (((node->for_init)!=NULL))
  {
    append_parse_node(printer, (node->for_init));
  }
  else
  {
    printer_indent(printer);
    append_string(printer, ";\n");
  }
  printer_indent(printer);
  if (((node->for_test)!=NULL))
  {
    append_parse_node(printer, (node->for_test));
  }
  append_string(printer, ";");
  printer_newline(printer);
  printer_indent(printer);
  if (((node->for_increment)!=NULL))
  {
    append_parse_node(printer, (node->for_increment));
  }
  append_string(printer, ")\n");
  printer_decrease_indent(printer);
  append_parse_node(printer, (node->for_body));
  return printer;
}

/* i=363 j=0 */
printer_t* append_do_statement_node(printer_t* printer, do_statement_node_t* node){
  printer_indent(printer);
  append_string(printer, "do");
  append_parse_node(printer, (node->body));
  append_string(printer, "while (");
  append_parse_node(printer, (node->condition));
  append_string(printer, ");\n");
  return printer;
}

/* i=364 j=0 */
printer_t* append_return_statement_node(printer_t* printer, return_statement_node_t* node){
  printer_indent(printer);
  append_string(printer, "return");
  if (((node->expression)!=NULL))
  {
    append_string(printer, " ");
    append_parse_node(printer, (node->expression));
  }
  append_string(printer, ";\n");
  return printer;
}

/* i=365 j=0 */
printer_t* append_operator_node(printer_t* printer, operator_node_t* node){
  if (token_matches((node->operator), "cast"))
  {
    append_string(printer, "(/*CAST*/(");
    append_parse_node(printer, (node->left));
    append_string(printer, ") ");
    append_parse_node(printer, (node->right));
    append_string(printer, ")");
    return printer;
  }
  if (token_matches((node->operator), "block_expr"))
  {
    return append_parse_node(printer, (node->left));
  }
  if (token_matches((node->operator), "typeof"))
  {
    append_string(printer, "typeof(");
    append_parse_node(printer, (node->left));
    append_string(printer, ")");
    return printer;
  }
  append_string(printer, "(");
  if (((node->left)!=NULL))
  {
    append_parse_node(printer, (node->left));
  }
  append_token(printer, (node->operator));
  if (token_matches((node->operator), "sizeof"))
  {
    append_string(printer, "(");
  }
  if (((node->right)!=NULL))
  {
    append_parse_node(printer, (node->right));
  }
  if (token_matches((node->operator), "sizeof"))
  {
    append_string(printer, ")");
  }
  else
  if (token_matches((node->operator), "["))
  {
    append_string(printer, "]");
  }
  append_string(printer, ")");
  return printer;
}

/* i=366 j=0 */
printer_t* append_conditional_node(printer_t* printer, conditional_node_t* node){
  append_string(printer, "(");
  if (((node->condition)!=NULL))
  {
    append_parse_node(printer, (node->condition));
  }
  append_string(printer, " ? ");
  if (((node->expr_if_true)!=NULL))
  {
    append_parse_node(printer, (node->expr_if_true));
  }
  append_string(printer, " : ");
  if (((node->expr_if_false)!=NULL))
  {
    append_parse_node(printer, (node->expr_if_false));
  }
  append_string(printer, ")");
  return printer;
}

/* i=367 j=0 */
printer_t* append_call_node(printer_t* printer, call_node_t* node){
  append_parse_node(printer, (node->function));
  append_string(printer, "(");
  for (
    int i = 0;
    (i<node_list_length((node->args)));
    (i++))
  {
    if ((i>0))
    {
      append_string(printer, ", ");
    }
    append_parse_node(printer, node_list_get((node->args), i));
  }
  append_string(printer, ")");
  return printer;
}

/* i=368 j=0 */
buffer_t* buffer_append_enum_metadata(buffer_t* buffer, enum_node_t* node, char* fn_prefix, char* type_string){
  char* code_template = "enum_metadata_t* ${fn_prefix}_metadata() {\n" "${element_constructions}" "    static enum_metadata_t enum_metadata_result = (enum_metadata_t) {\n" "        .name = \"${enum_name}\",\n" "        .elements = ${previous_var_address}\n" "    };\n" "    return &enum_metadata_result;\n" "}\n\n";
  char* field_template = "    static enum_element_metadata_t ${var_id} = (enum_element_metadata_t) {\n" "        .next = ${previous_var_address},\n" "        .name = \"${element_name}\",\n" "        .value = ${element_name}\n" "    };\n";
  buffer_t* element_constructions = make_buffer(128);
  buffer_t* buf = make_buffer(128);
  char* previous_var_address = "NULL";
  for (
    int i = 0;
    (i<node_list_length((node->elements)));
    (i++))
  {
    enum_element_t* element = to_enum_element_node(node_list_get((node->elements), i));
    char* var_id = string_printf("var_%d", i);
    char* element_name = token_to_string((element->name));
    buffer_clear(buf);
    buffer_append_string(buf, field_template);
    buffer_replace_all(buf, "${var_id}", var_id);
    buffer_replace_all(buf, "${previous_var_address}", previous_var_address);
    buffer_replace_all(buf, "${element_name}", element_name);
    buffer_append_buffer(element_constructions, buf);
    (previous_var_address=string_printf("&%s", var_id));
  }
  buffer_t* code = buffer_append_string(make_buffer(128), code_template);
  buffer_replace_all(code, "${fn_prefix}", fn_prefix);
  buffer_replace_all(code, "${enum_name}", type_string);
  buffer_replace_all(code, "${previous_var_address}", previous_var_address);
  buffer_replace_all(code, "${element_constructions}", buffer_to_c_string(element_constructions));
  return buffer_append_buffer(buffer, code);
}

/* i=369 j=0 */
symbol_table_map_t* make_symbol_table_map(void){
  symbol_table_map_t* result = malloc_struct(symbol_table_map_t);
  ((result->ht)=make_string_hashtable(16));
  ((result->ordered_bindings)=make_value_array(16));
  return result;
}

/* i=370 j=0 */
symbol_table_t* make_symbol_table(void){
  symbol_table_t* result = malloc_struct(symbol_table_t);
  ((result->system_includes)=make_value_array(16));
  ((result->user_includes)=make_value_array(16));
  ((result->defines)=make_value_array(16));
  ((result->enums)=make_symbol_table_map());
  ((result->typedefs)=make_symbol_table_map());
  ((result->structures)=make_symbol_table_map());
  ((result->variables)=make_symbol_table_map());
  ((result->functions)=make_symbol_table_map());
  return result;
}

/* i=371 j=0 */
symbol_table_binding_t* symbol_table_map_get(symbol_table_map_t* map, char* key_string){
  value_result_t result = string_ht_find((map->ht), key_string);
  if (is_ok(result))
  {
    return (/*CAST*/(symbol_table_binding_t*) (result.ptr));
  }
  return NULL;
}

/* i=372 j=0 */
parse_node_t* symbol_table_map_get_only_definition(symbol_table_map_t* map, char* key_string){
  value_result_t result = string_ht_find((map->ht), key_string);
  if (is_ok(result))
  {
    symbol_table_binding_t* binding = (/*CAST*/(symbol_table_binding_t*) (result.ptr));
    if ((((binding->definition_nodes)->length)!=1))
    {
      fatal_error(ERROR_ILLEGAL_STATE);
    }
    return value_array_get_ptr((binding->definition_nodes), 0, typeof(parse_node_t*));
  }
  return NULL;
}

/* i=373 j=0 */
void symbol_table_add_declaration_node(symbol_table_map_t* map, char* key_string, parse_node_t* node){
  value_result_t previous_binding = string_ht_find((map->ht), key_string);
  if (is_ok(previous_binding))
  {
    symbol_table_binding_t* binding = (/*CAST*/(symbol_table_binding_t*) (previous_binding.ptr));
    value_array_add((binding->definition_nodes), ptr_to_value(node));
    return;
  }
  symbol_table_binding_t* binding = malloc_struct(symbol_table_binding_t);
  ((binding->key_string)=key_string);
  ((binding->definition_nodes)=make_value_array(2));
  value_array_add((binding->definition_nodes), ptr_to_value(node));
  ((map->ht)=string_ht_insert((map->ht), (binding->key_string), ptr_to_value(binding)));
  value_array_add((map->ordered_bindings), ptr_to_value(binding));
}

/* i=374 j=0 */
void symbol_table_add_declartions(symbol_table_t* symbol_table, declarations_node_t* root){
  uint64_t length = node_list_length((root->declarations));
  for (
    uint64_t i = 0;
    (i<length);
    (i++))
  {
    parse_node_t* node = node_list_get((root->declarations), i);
    switch ((node->tag))
    {
      case PARSE_NODE_ENUM:
      symbol_table_add_declaration_node((symbol_table->enums), token_to_string((to_enum_node(node)->name)), node);
      break;
      case PARSE_NODE_FUNCTION:
      symbol_table_add_declaration_node((symbol_table->functions), token_to_string((to_function_node(node)->function_name)), node);
      break;
      case PARSE_NODE_VARIABLE_DEFINITION:
      symbol_table_add_declaration_node((symbol_table->variables), token_to_string((to_variable_definition_node(node)->name)), node);
      break;
      case PARSE_NODE_STRUCT:
      case PARSE_NODE_UNION:
      symbol_table_add_declaration_node((symbol_table->structures), token_to_string((to_struct_node(node)->name)), node);
      break;
      case PARSE_NODE_TYPEDEF:
      symbol_table_add_declaration_node((symbol_table->typedefs), token_to_string((to_typedef_node(node)->name)), node);
      break;
      default:
      fatal_error(ERROR_ILLEGAL_STATE);
      break;
    }
  }
}

/* i=375 j=0 */
buffer_t* symbol_table_stats(buffer_t* buffer, symbol_table_t* symbol_table){
  (buffer=buffer_printf(buffer, "Symbol Table Stats\n"));
  (buffer=buffer_printf(buffer, "#enums %d\n", string_ht_num_entries(((symbol_table->enums)->ht))));
  (buffer=buffer_printf(buffer, "#typedefs %d\n", string_ht_num_entries(((symbol_table->typedefs)->ht))));
  (buffer=buffer_printf(buffer, "#structures %d\n", string_ht_num_entries(((symbol_table->structures)->ht))));
  (buffer=buffer_printf(buffer, "#variables %d\n", string_ht_num_entries(((symbol_table->variables)->ht))));
  (buffer=buffer_printf(buffer, "#functions %d\n", string_ht_num_entries(((symbol_table->functions)->ht))));
  return buffer;
}

/* i=376 j=0 */
buffer_t* buffer_append_dgb_binding(buffer_t* buffer, symbol_table_binding_t* binding){
  buffer_printf(buffer, "%s:\n", (binding->key_string));
  buffer_append_dbg_parse_node(make_cdl_printer(buffer), value_array_get_ptr((binding->definition_nodes), 0, typeof(parse_node_t*)));
  return buffer;
}

/* i=377 j=0 */
buffer_t* buffer_appennd_dbg_symbol_table_map(buffer_t* buffer, symbol_table_map_t* symbol_table_map){
  for (
    int i = 0;
    (i<((symbol_table_map->ordered_bindings)->length));
    (i++))
  {
    if ((i>0))
    {
      (buffer=buffer_printf(buffer, "\n"));
    }
    symbol_table_binding_t* binding = value_array_get_ptr((symbol_table_map->ordered_bindings), i, typeof(symbol_table_binding_t*));
    buffer_append_dgb_binding(buffer, binding);
  }
  return buffer;
}

/* i=378 j=0 */
buffer_t* buffer_append_dgb_symbol_table(buffer_t* buffer, symbol_table_t* symbol_table){
  (buffer=buffer_printf(buffer, "\n========================= Begin Symbol Table " "=========================\n"));
  (buffer=buffer_printf(buffer, "*** Symbol Table Enumerations ***\n"));
  (buffer=buffer_appennd_dbg_symbol_table_map(buffer, (symbol_table->enums)));
  (buffer=buffer_printf(buffer, "\n*** Symbol Table Typedefs ***\n"));
  (buffer=buffer_appennd_dbg_symbol_table_map(buffer, (symbol_table->typedefs)));
  (buffer=buffer_printf(buffer, "\n*** Symbol Table Structures ***\n"));
  (buffer=buffer_appennd_dbg_symbol_table_map(buffer, (symbol_table->structures)));
  (buffer=buffer_printf(buffer, "\n*** Symbol Table Variables ***\n"));
  (buffer=buffer_appennd_dbg_symbol_table_map(buffer, (symbol_table->variables)));
  (buffer=buffer_printf(buffer, "\n*** Symbol Table Functions ***\n"));
  (buffer=buffer_appennd_dbg_symbol_table_map(buffer, (symbol_table->functions)));
  (buffer=buffer_printf(buffer, "========================= End Symbol Table " "=========================\n\n"));
  return buffer;
}

/* i=379 j=0 */
token_t* generate_struct_name_from_typedef_name(token_t* name){
  token_t* generated = make_derived_token(name);
  buffer_append_string((generated->buffer), "__generated_S");
  ((generated->end)=((generated->buffer)->length));
  return generated;
}

/* i=380 j=0 */
void split_structure_typedefs(symbol_table_t* symbol_table){
  for (
    int i = 0;
    (i<(((symbol_table->typedefs)->ordered_bindings)->length));
    (i++))
  {
    symbol_table_binding_t* binding = (/*CAST*/(symbol_table_binding_t*) (value_array_get(((symbol_table->typedefs)->ordered_bindings), i).ptr));
    if ((((binding->definition_nodes)->length)!=1))
    {
      fatal_error(ERROR_ILLEGAL_STATE);
    }
    typedef_node_t* node = to_typedef_node((/*CAST*/(parse_node_t*) (value_array_get((binding->definition_nodes), 0).ptr)));
    if (((((node->type_node)->type_node_kind)==TYPE_NODE_KIND_TYPE_EXPRESSION)&&is_struct_node(((node->type_node)->user_type))))
    {
      struct_node_t* struct_node = to_struct_node(((node->type_node)->user_type));
      if ((!(struct_node->partial_definition)))
      {
        if (((struct_node->name)==NULL))
        {
          ((struct_node->name)=generate_struct_name_from_typedef_name((node->name)));
        }
        log_debug("Splitting %s off from %s", token_to_string((node->name)), token_to_string((struct_node->name)));
        struct_node_t* partial_definition = malloc_struct_node();
        ((partial_definition->partial_definition)=true);
        ((partial_definition->name)=(struct_node->name));
        (((node->type_node)->user_type)=(/*CAST*/(parse_node_t*) partial_definition));
        symbol_table_add_declaration_node((symbol_table->structures), token_to_string((struct_node->name)), (/*CAST*/(parse_node_t*) struct_node));
      }
    }
  }
}

/* i=381 j=0 */
void reorder_symbol_table_typedefs(symbol_table_t* symbol_table){
  value_array_t* bindings = ((symbol_table->typedefs)->ordered_bindings);
  value_array_t* reordered_bindings = make_value_array((bindings->length));
  for (
    int i = 0;
    (i<(bindings->length));
    (i++))
  {
    symbol_table_binding_t* binding = (/*CAST*/(symbol_table_binding_t*) (value_array_get(bindings, i).ptr));
    reorder_symbol_table_typedefs__process_binding((symbol_table->typedefs), binding, reordered_bindings);
  }
  (((symbol_table->typedefs)->ordered_bindings)=reordered_bindings);
}

/* i=382 j=0 */
void reorder_symbol_table_typedefs__process_binding(symbol_table_map_t* typedefs, symbol_table_binding_t* binding, value_array_t* reordered_bindings){
  log_debug("processing binding %s", (binding->key_string));
  if ((!(binding->visited)))
  {
    if ((((binding->definition_nodes)->length)!=1))
    {
      fatal_error(ERROR_ILLEGAL_STATE);
    }
    parse_node_t* node = (/*CAST*/(parse_node_t*) (value_array_get((binding->definition_nodes), 0).ptr));
    typedef_node_t* typedef_node = to_typedef_node(node);
    type_node_t* type_node = (typedef_node->type_node);
    while (((type_node->type_node_kind)==TYPE_NODE_KIND_POINTER))
    {
      (type_node=to_type_node(node_list_get((type_node->type_args), 0)));
    }
    if (((type_node->type_node_kind)==TYPE_NODE_KIND_TYPENAME))
    {
      char* type_name = token_to_string((type_node->type_name));
      symbol_table_binding_t* dependent_binding = symbol_table_map_get(typedefs, type_name);
      if ((dependent_binding!=NULL))
      {
        reorder_symbol_table_typedefs__process_binding(typedefs, dependent_binding, reordered_bindings);
      }
    }
    else
    if (((type_node->type_node_kind)==TYPE_NODE_KIND_TYPE_EXPRESSION))
    {
    }
    else
    if (((type_node->type_node_kind)!=TYPE_NODE_KIND_PRIMITIVE_TYPENAME))
    {
      fatal_error(ERROR_ILLEGAL_STATE);
    }
    log_debug("adding binding %s at position %d", (binding->key_string), (reordered_bindings->length));
    value_array_add(reordered_bindings, ptr_to_value(binding));
    ((binding->visited)=true);
  }
}

/* i=383 j=0 */
struct_node_t* get_full_structure_definition_node(symbol_table_binding_t* binding){
  for (
    uint64_t i = 0;
    (i<((binding->definition_nodes)->length));
    (i++))
  {
    parse_node_t* node = (/*CAST*/(parse_node_t*) (value_array_get((binding->definition_nodes), i).ptr));
    struct_node_t* structure_node = to_struct_node(node);
    if ((!(structure_node->partial_definition)))
    {
      return structure_node;
    }
  }
  return NULL;
}

/* i=384 j=0 */
symbol_table_binding_t* resolve_typename_to_structure_binding(symbol_table_t* symbol_table, type_node_t* type_node){
  if (((type_node->type_node_kind)==TYPE_NODE_KIND_POINTER))
  {
    log_debug("resolve_typename_to_structure_binding -- not looking through pointers " "%p", type_node);
    return NULL;
  }
  if (((type_node->type_node_kind)==TYPE_NODE_KIND_TYPE_EXPRESSION))
  {
    parse_node_t* user_type = (type_node->user_type);
    if (is_struct_node(user_type))
    {
      struct_node_t* struct_node = to_struct_node(user_type);
      if (((struct_node->name)!=NULL))
      {
        char* key_name = token_to_string((struct_node->name));
        symbol_table_binding_t* binding = symbol_table_map_get((symbol_table->structures), key_name);
        char* dbg_binding = buffer_to_c_string(buffer_append_dgb_binding(make_buffer(10), binding));
        log_debug("resolve_typename_to_structure_binding -- returning binding %p %s", binding, dbg_binding);
        return binding;
      }
    }
    return NULL;
  }
  char* key_string = token_to_string((type_node->type_name));
  log_debug("resolve_typename_to_structure_binding -- %d %s", (type_node->tag), key_string);
  symbol_table_binding_t* typedef_binding = symbol_table_map_get((symbol_table->typedefs), key_string);
  if ((typedef_binding!=NULL))
  {
    if ((((typedef_binding->definition_nodes)->length)!=1))
    {
      fatal_error(ERROR_ILLEGAL_STATE);
    }
    typedef_node_t* typedef_node = to_typedef_node(value_array_get_ptr((typedef_binding->definition_nodes), 0, typeof(parse_node_t*)));
    return resolve_typename_to_structure_binding(symbol_table, (typedef_node->type_node));
  }
  else
  {
    return symbol_table_map_get((symbol_table->structures), key_string);
  }
}

/* i=385 j=0 */
void reorder_symbol_table_structures_process_binding(symbol_table_t* symbol_table, symbol_table_binding_t* binding, value_array_t* reordered_bindings){
  log_debug("processing %s", (binding->key_string));
  if ((!(binding->visited)))
  {
    ((binding->visited)=true);
    struct_node_t* structure_node = get_full_structure_definition_node(binding);
    if ((structure_node==NULL))
    {
      return;
    }
    uint64_t length = node_list_length((structure_node->fields));
    for (
      uint64_t i = 0;
      (i<length);
      (i++))
    {
      field_node_t* field = to_field_node(node_list_get((structure_node->fields), i));
      type_node_t* type_node = (field->type);
      if ((type_node!=NULL))
      {
        symbol_table_binding_t* field_type_binding = resolve_typename_to_structure_binding(symbol_table, type_node);
        if (((field_type_binding!=NULL)&&(!(field_type_binding->visited))))
        {
          reorder_symbol_table_structures_process_binding(symbol_table, field_type_binding, reordered_bindings);
        }
      }
    }
    value_array_add(reordered_bindings, ptr_to_value(binding));
  }
}

/* i=386 j=0 */
void reorder_symbol_table_structures(symbol_table_t* symbol_table){
  value_array_t* bindings = ((symbol_table->structures)->ordered_bindings);
  value_array_t* reordered_bindings = make_value_array((bindings->length));
  for (
    int i = 0;
    (i<(bindings->length));
    (i++))
  {
    symbol_table_binding_t* binding = (/*CAST*/(symbol_table_binding_t*) (value_array_get(bindings, i).ptr));
    reorder_symbol_table_structures_process_binding(symbol_table, binding, reordered_bindings);
  }
  (((symbol_table->structures)->ordered_bindings)=reordered_bindings);
}

/* i=387 j=0 */
void convert_nullptr_to_null(value_array_t* tokens){
  buffer_t* null_token = buffer_append_string(make_buffer(1), "NULL");
  for (
    int i = 0;
    (i<(tokens->length));
    (i++))
  {
    token_t* token = token_at(tokens, i);
    if (token_matches(token, "nullptr"))
    {
      ((token->start)=0);
      ((token->end)=(null_token->length));
      ((token->buffer)=null_token);
    }
  }
}

/* i=388 j=0 */
c_preprocessor_directive_range_t mark_c_preprocessor_directive(c_preprocess_options_t options, value_array_t* tokens, uint64_t start_position){
  c_preprocessor_directive_range_t result = {0};
  uint64_t position = start_position;
  for (
    ;
    (position<(tokens->length));
    (position++))
  {
    token_t* token = token_at(tokens, position);
    ((token->is_cpp_token)=true);
    if ((position==start_position))
    {
      ((result.token_start_position)=start_position);
      ((result.buffer_start_position)=(token->start));
      ((result.buffer)=(token->buffer));
    }
    if (((result.buffer)!=(token->buffer)))
    {
      fatal_error(ERROR_ILLEGAL_STATE);
    }
    ((result.token_end_position)=(position+1));
    ((result.buffer_end_position)=(token->end));
    if ((((token->type)==TOKEN_TYPE_WHITESPACE)&&token_contains(token, "\n")))
    {
      break;
    }
  }
  return result;
}

/* i=389 j=0 */
uint64_t handle_c_preprocessor_directive(c_preprocess_options_t options, symbol_table_t* symbol_table, value_array_t* tokens, uint64_t start_position){
  c_preprocessor_directive_range_t range = mark_c_preprocessor_directive(options, tokens, start_position);
  token_t* directive_name = token_at(tokens, (start_position+1));
  if (token_matches(directive_name, "include"))
  {
    cpp_include_node_t* node = malloc_cpp_include_node();
    ((node->text)=buffer_c_substring((range.buffer), (range.buffer_start_position), (range.buffer_end_position)));
    if (string_contains_char((node->text), '<'))
    {
      value_array_add((symbol_table->system_includes), ptr_to_value(node));
    }
    else
    {
      value_array_add((symbol_table->user_includes), ptr_to_value(node));
    }
  }
  else
  if (token_matches(directive_name, "define"))
  {
    cpp_define_node_t* node = malloc_cpp_define_node();
    ((node->text)=buffer_c_substring((range.buffer), (range.buffer_start_position), (range.buffer_end_position)));
    value_array_add((symbol_table->defines), ptr_to_value(node));
  }
  return (range.token_end_position);
}

/* i=390 j=0 */
void handle_c_preprocessor_directives(c_preprocess_options_t options, symbol_table_t* symbol_table, value_array_t* tokens){
  for (
    uint64_t position = 0;
    (position<(tokens->length));
    )
  {
    token_t* token = token_at(tokens, position);
    if (token_matches(token, "#"))
    {
      (position=handle_c_preprocessor_directive(options, symbol_table, tokens, position));
    }
    else
    {
      (position++);
    }
  }
}

/* i=392 j=0 */
__attribute__((warn_unused_result)) buffer_t* extract_enums_process_declarations(buffer_t* output, declarations_node_t* root){
  printer_t* printer = make_printer(output, 2);
  uint64_t length = node_list_length((root->declarations));
  for (
    uint64_t i = 0;
    (i<length);
    (i++))
  {
    parse_node_t* node = node_list_get((root->declarations), i);
    if (((node->tag)==PARSE_NODE_TYPEDEF))
    {
      typedef_node_t* typedef_node = to_typedef_node(node);
      if (is_enum_node(((typedef_node->type_node)->user_type)))
      {
        enum_node_t* enum_node = to_enum_node(((typedef_node->type_node)->user_type));
        buffer_printf(output, "typedef ");
        buffer_printf(output, "%s ", token_to_string((typedef_node->name)));
        append_enum_node(printer, enum_node);
        buffer_printf(output, ";\n\n");
        char* enum_node_name = token_to_string((typedef_node->name));
        char* to_string_prefix = remove_type_suffix(enum_node_name);
        char* enum_node_type_string = enum_node_name;
        append_enum_to_string(printer, enum_node, to_string_prefix, enum_node_type_string);
        append_string_to_enum(printer, enum_node, to_string_prefix, enum_node_type_string);
      }
    }
    else
    if (((node->tag)==PARSE_NODE_ENUM))
    {
      enum_node_t* enum_node = to_enum_node(node);
      append_enum_node(printer, enum_node);
      buffer_printf(output, ";\n\n");
      char* enum_node_name = token_to_string((enum_node->name));
      char* to_string_prefix = remove_type_suffix(enum_node_name);
      char* enum_node_type_string = string_printf("enum %s", enum_node_name);
      append_enum_to_string(printer, enum_node, to_string_prefix, enum_node_type_string);
      append_string_to_enum(printer, enum_node, to_string_prefix, enum_node_type_string);
    }
  }
  return output;
}

/* i=393 j=0 */
__attribute__((warn_unused_result)) buffer_t* extract_prototypes_process_declarations(buffer_t* output, declarations_node_t* root){
  printer_t* printer = make_printer(output, 2);
  uint64_t length = node_list_length((root->declarations));
  for (
    uint64_t i = 0;
    (i<length);
    (i++))
  {
    parse_node_t* node = node_list_get((root->declarations), i);
    if (((node->tag)==PARSE_NODE_FUNCTION))
    {
      function_node_t* fn_node = to_function_node(node);
      if (((fn_node->body)==NULL))
      {
        continue;
      }
      if (token_list_contains((fn_node->function_specifiers), "inline"))
      {
        continue;
      }
      if ((((fn_node->storage_class_specifier)!=NULL)&&token_matches((fn_node->storage_class_specifier), "static")))
      {
        continue;
      }
      append_c_function_node_prototype(printer, fn_node);
      buffer_printf(output, "\n");
    }
  }
  return output;
}

/* i=394 j=0 */
void parse_and_add_top_level_definitions(symbol_table_t* symbol_table, value_array_t* file_names, boolean_t use_statement_parser){
  value_array_t* files = read_files(file_names);
  for (
    int i = 0;
    (i<(files->length));
    (i++))
  {
    file_t* file = (/*CAST*/(file_t*) (value_array_get(files, i).ptr));
    symbol_table_parse_buffer(symbol_table, (file->data), (file->file_name), use_statement_parser);
  }
}

/* i=395 j=0 */
void symbol_table_parse_buffer(symbol_table_t* symbol_table, buffer_t* buffer, char* file_name, boolean_t use_statement_parser){
  tokenizer_result_t tokenizer_result = tokenize(buffer);
  if ((tokenizer_result.tokenizer_error_code))
  {
    log_warn("Tokenizer error: \"%s\"::%d -- %d", file_name, (tokenizer_result.tokenizer_error_position), (tokenizer_result.tokenizer_error_code));
    fatal_error(ERROR_ILLEGAL_INPUT);
  }
  value_array_t* tokens = (tokenizer_result.tokens);
  handle_c_preprocessor_directives(((c_preprocess_options_t) {
                           .keep_system_includes = true,
                           .keep_user_includes = false,
                       }), symbol_table, tokens);
  (tokens=transform_tokens(tokens, ((token_transformer_options_t) {
                                   .keep_whitespace = false,
                                   .keep_comments = false,
                                   .keep_javadoc_comments = false,
                                   .keep_c_preprocessor_lines = false,
                               })));
  pstate_t pstate = ((pstate_t) {.tokens = tokens, .use_statement_parser = use_statement_parser});
  if ((!parse_declarations((&pstate))))
  {
    (((pstate.error).file_name)=file_name);
    buffer_t* buffer = make_buffer(1);
    (buffer=buffer_append_human_readable_error(buffer, (&(pstate.error))));
    log_fatal("%s", buffer_to_c_string(buffer));
    fatal_error(ERROR_ILLEGAL_INPUT);
  }
  declarations_node_t* root = to_declarations_node(pstate_get_result_node((&pstate)));
  symbol_table_add_declartions(symbol_table, root);
}

/* i=397 j=0 */
void srcgen_enum_to_string_converters(symbol_table_t* symbol_table){
  buffer_t* buffer = make_buffer(1);
  printer_t* printer = make_printer(buffer, 2);
  for (
    int i = 0;
    (i<(((symbol_table->typedefs)->ordered_bindings)->length));
    (i++))
  {
    symbol_table_binding_t* binding = value_array_get_ptr(((symbol_table->typedefs)->ordered_bindings), i, typeof(symbol_table_binding_t*));
    typedef_node_t* typedef_node = to_typedef_node(value_array_get_ptr((binding->definition_nodes), 0, typeof(parse_node_t*)));
    if (is_enum_node(((typedef_node->type_node)->user_type)))
    {
      enum_node_t* enum_node = to_enum_node(((typedef_node->type_node)->user_type));
      char* enum_node_name = token_to_string((typedef_node->name));
      log_debug("Generating converters for %s", enum_node_name);
      char* to_string_prefix = remove_type_suffix_1(enum_node_name);
      char* enum_node_type_string = enum_node_name;
      append_enum_to_string(printer, enum_node, to_string_prefix, enum_node_type_string);
      append_string_to_enum(printer, enum_node, to_string_prefix, enum_node_type_string);
      buffer_append_enum_metadata(buffer, enum_node, to_string_prefix, enum_node_type_string);
    }
  }
  for (
    int i = 0;
    (i<(((symbol_table->enums)->ordered_bindings)->length));
    (i++))
  {
    symbol_table_binding_t* binding = value_array_get_ptr(((symbol_table->enums)->ordered_bindings), i, typeof(symbol_table_binding_t*));
    enum_node_t* enum_node = to_enum_node(value_array_get_ptr((binding->definition_nodes), 0, typeof(parse_node_t*)));
    char* enum_node_name = token_to_string((enum_node->name));
    char* to_string_prefix = remove_type_suffix_1(enum_node_name);
    char* enum_node_type_string = string_printf("enum %s", enum_node_name);
    append_enum_to_string(printer, enum_node, to_string_prefix, enum_node_type_string);
    append_string_to_enum(printer, enum_node, to_string_prefix, enum_node_type_string);
    buffer_append_enum_metadata(buffer, enum_node, to_string_prefix, enum_node_type_string);
  }
  if (((buffer->length)>0))
  {
    symbol_table_parse_buffer(symbol_table, buffer, "#<internal-code-generator>", false);
  }
}

/* i=407 j=1 */
pstatus_t parse_block(pstate_t* pstate){
  uint64_t saved_position = (pstate->position);
  if ((!pstate_expect_token_string(pstate, "{")))
  {
    return pstate_propagate_error(pstate, saved_position);
  }
  block_node_t* result = make_block_node();
  while (parse_statement(pstate))
  {
    node_list_add_node((&(result->statements)), pstate_get_result_node(pstate));
  }
  if ((!pstate_expect_token_string(pstate, "}")))
  {
    return pstate_propagate_error(pstate, saved_position);
  }
  return pstate_set_result_node(pstate, to_node(result));
}

/* i=408 j=0 */
pstatus_t pratt_parse_expression(pstate_t* pstate, int precedence){
  uint64_t saved_position = (pstate->position);
  token_t* token = pstate_peek(pstate, 0);
  if ((token==NULL))
  {
    return pstate_error(pstate, saved_position, PARSE_ERROR_EOF);
  }
  pratt_parser_instruction_t prefix_instruction = get_prefix_instruction(token);
  if (((prefix_instruction.operation)==PRATT_PARSE_UNKNOWN))
  {
    log_debug("(RETURNING ERROR) No prefix for %s\n", token_to_string(token));
    return pstate_error(pstate, saved_position, PARSE_ERROR_EXPECTED_PREFIX_OPERATOR_OR_TERMINAL);
  }
  if ((!pratt_handle_instruction(pstate, prefix_instruction, NULL)))
  {
    log_debug("(RETURNING ERROR) handle instruction\n", token_to_string(token));
    return pstate_propagate_error(pstate, saved_position);
  }
  parse_node_t* left = pstate_get_result_node(pstate);
  while (1)
  {
    pratt_parser_instruction_t infix_instruction = get_infix_instruction(pstate_peek(pstate, 0));
    if ((((infix_instruction.operation)==PRATT_PARSE_UNKNOWN)||(precedence>=(infix_instruction.precedence))))
    {
      return pstate_set_result_node(pstate, left);
    }
    if ((!pratt_handle_instruction(pstate, infix_instruction, left)))
    {
      return pstate_propagate_error(pstate, saved_position);
    }
    (left=pstate_get_result_node(pstate));
  }
  return NULL;
}

/* i=409 j=0 */
pstatus_t pratt_handle_instruction(pstate_t* pstate, pratt_parser_instruction_t instruction, parse_node_t* left){
  uint64_t saved_position = (pstate->position);
  token_t* token = pstate_peek(pstate, 0);
  switch ((instruction.operation))
  {
    case PRATT_PARSE_BINARY_OPERATOR:
    do    {
      pstate_advance(pstate);
      int recursive_precedence = (instruction.precedence);
      if ((precedence_to_associativity(recursive_precedence)==LEFT_TO_RIGHT))
      {
        (recursive_precedence++);
      }
      else
      {
        (recursive_precedence--);
      }
      if ((!pratt_parse_expression(pstate, recursive_precedence)))
      {
        return pstate_propagate_error(pstate, saved_position);
      }
      operator_node_t* result = malloc_operator_node();
      ((result->operator)=token);
      ((result->left)=left);
      ((result->right)=pstate_get_result_node(pstate));
      return pstate_set_result_node(pstate, to_node(result));
    }
while (0);
    case PRATT_PARSE_IDENTIFIER:
    do    {
      pstate_advance(pstate);
      identifier_node_t* result = malloc_identifier_node();
      ((result->token)=token);
      return pstate_set_result_node(pstate, to_node(result));
    }
while (0);
    case PRATT_PARSE_LITERAL:
    return parse_literal_node(pstate);
    case PRATT_PARSE_PREFIX_OPERATOR:
    do    {
      pstate_advance(pstate);
      if ((!pratt_parse_expression(pstate, (instruction.precedence))))
      {
        return pstate_propagate_error(pstate, saved_position);
      }
      operator_node_t* result = malloc_operator_node();
      ((result->operator)=token);
      if ((left!=NULL))
      {
        fatal_error(ERROR_ILLEGAL_STATE);
      }
      ((result->left)=NULL);
      ((result->right)=pstate_get_result_node(pstate));
      return pstate_set_result_node(pstate, to_node(result));
    }
while (0);
    case PRATT_PARSE_POSTFIX_OPERATOR:
    do    {
      pstate_advance(pstate);
      operator_node_t* result = malloc_operator_node();
      ((result->operator)=token);
      ((result->left)=left);
      ((result->right)=NULL);
      return pstate_set_result_node(pstate, to_node(result));
    }
while (0);
    case PRATT_PARSE_SUB_EXPRESSION:
    do    {
      if ((!pstate_expect_token_string(pstate, "(")))
      {
        fatal_error(ERROR_ILLEGAL_STATE);
      }
      if ((!pratt_parse_expression(pstate, 0)))
      {
        return pstate_propagate_error(pstate, saved_position);
      }
      parse_node_t* inner_expression = pstate_get_result_node(pstate);
      if ((!pstate_expect_token_string(pstate, ")")))
      {
        return pstate_propagate_error(pstate, saved_position);
      }
      return pstate_set_result_node(pstate, inner_expression);
    }
while (0);
    break;
    case PRATT_PARSE_INDEX_EXPRESSION:
    do    {
      if ((!pstate_expect_token_string(pstate, "[")))
      {
        fatal_error(ERROR_ILLEGAL_STATE);
      }
      if ((!pratt_parse_expression(pstate, 0)))
      {
        return pstate_propagate_error(pstate, saved_position);
      }
      parse_node_t* inner_expression = pstate_get_result_node(pstate);
      if ((!pstate_expect_token_string(pstate, "]")))
      {
        return pstate_propagate_error(pstate, saved_position);
      }
      operator_node_t* result = malloc_operator_node();
      ((result->operator)=token);
      ((result->left)=left);
      ((result->right)=inner_expression);
      return pstate_set_result_node(pstate, to_node(result));
    }
while (0);
    break;
    case PRATT_PARSE_SIZEOF:
    do    {
      if ((!pstate_expect_token_string(pstate, "sizeof")))
      {
        fatal_error(ERROR_ILLEGAL_STATE);
      }
      if ((!pstate_expect_token_string(pstate, "(")))
      {
        return pstate_propagate_error(pstate, saved_position);
      }
      if ((!pratt_parse_expression(pstate, 0)))
      {
        return pstate_propagate_error(pstate, saved_position);
      }
      parse_node_t* inner_expression = pstate_get_result_node(pstate);
      if ((!pstate_expect_token_string(pstate, ")")))
      {
        return pstate_propagate_error(pstate, saved_position);
      }
      operator_node_t* result = malloc_operator_node();
      ((result->operator)=token);
      ((result->right)=inner_expression);
      return pstate_set_result_node(pstate, to_node(result));
    }
while (0);
    break;
    case PRATT_PARSE_CAST_MACRO:
    do    {
      if ((!pstate_expect_token_string(pstate, "cast")))
      {
        fatal_error(ERROR_ILLEGAL_STATE);
      }
      if ((!pstate_expect_token_string(pstate, "(")))
      {
        return pstate_propagate_error(pstate, saved_position);
      }
      if ((!parse_type_node(pstate)))
      {
        return pstate_propagate_error(pstate, saved_position);
      }
      parse_node_t* type_node = pstate_get_result_node(pstate);
      if (((!pstate_expect_token_string(pstate, ","))||(!pratt_parse_expression(pstate, PRECEDENCE_ASSIGNMENT))))
      {
        return pstate_propagate_error(pstate, saved_position);
      }
      parse_node_t* expression = pstate_get_result_node(pstate);
      if ((!pstate_expect_token_string(pstate, ")")))
      {
        return pstate_propagate_error(pstate, saved_position);
      }
      operator_node_t* result = malloc_operator_node();
      ((result->operator)=token);
      ((result->left)=type_node);
      ((result->right)=expression);
      return pstate_set_result_node(pstate, to_node(result));
    }
while (0);
    break;
    case PRATT_PARSE_BLOCK_EXPR:
    do    {
      if ((!pstate_expect_token_string(pstate, "block_expr")))
      {
        fatal_error(ERROR_ILLEGAL_STATE);
      }
      if ((!pstate_expect_token_string(pstate, "(")))
      {
        return pstate_propagate_error(pstate, saved_position);
      }
      if ((!parse_block(pstate)))
      {
        return pstate_propagate_error(pstate, saved_position);
      }
      parse_node_t* block_node = pstate_get_result_node(pstate);
      if ((!pstate_expect_token_string(pstate, ")")))
      {
        return pstate_propagate_error(pstate, saved_position);
      }
      operator_node_t* result = malloc_operator_node();
      ((result->operator)=token);
      ((result->left)=block_node);
      return pstate_set_result_node(pstate, to_node(result));
    }
while (0);
    break;
    case PRATT_PARSE_TYPE_OF:
    do    {
      if ((!pstate_expect_token_string(pstate, "typeof")))
      {
        fatal_error(ERROR_ILLEGAL_STATE);
      }
      if ((!pstate_expect_token_string(pstate, "(")))
      {
        return pstate_propagate_error(pstate, saved_position);
      }
      if ((!parse_type_node(pstate)))
      {
        return pstate_propagate_error(pstate, saved_position);
      }
      parse_node_t* type_node = pstate_get_result_node(pstate);
      if ((!pstate_expect_token_string(pstate, ")")))
      {
        return pstate_propagate_error(pstate, saved_position);
      }
      operator_node_t* result = malloc_operator_node();
      ((result->operator)=token);
      ((result->left)=type_node);
      return pstate_set_result_node(pstate, to_node(result));
    }
while (0);
    break;
    case PRATT_PARSE_CALL:
    do    {
      if ((!pstate_expect_token_string(pstate, "(")))
      {
        fatal_error(ERROR_ILLEGAL_STATE);
      }
      call_node_t* result = malloc_call_node();
      ((result->function)=left);
      boolean_t expect_comma = false;
      while ((!token_matches(pstate_peek(pstate, 0), ")")))
      {
        if (expect_comma)
        {
          if ((!pstate_expect_token_string(pstate, ",")))
          {
            return pstate_propagate_error(pstate, saved_position);
          }
        }
        else
        {
          (expect_comma=true);
        }
        if ((!pratt_parse_expression(pstate, PRECEDENCE_ASSIGNMENT)))
        {
          return pstate_propagate_error(pstate, saved_position);
        }
        node_list_add_node((&(result->args)), pstate_get_result_node(pstate));
      }
      if ((!pstate_expect_token_string(pstate, ")")))
      {
        return pstate_propagate_error(pstate, saved_position);
      }
      return pstate_set_result_node(pstate, to_node(result));
    }
while (0);
    break;
    case PRATT_PARSE_CONDITIONAL:
    do    {
      pstate_advance(pstate);
      int recursive_precedence = (instruction.precedence);
      if ((!pratt_parse_expression(pstate, recursive_precedence)))
      {
        return pstate_propagate_error(pstate, saved_position);
      }
      parse_node_t* expr_if_true = pstate_get_result_node(pstate);
      if (((!pstate_expect_token_string(pstate, ":"))||(!pratt_parse_expression(pstate, recursive_precedence))))
      {
        return pstate_propagate_error(pstate, saved_position);
      }
      parse_node_t* expr_if_false = pstate_get_result_node(pstate);
      conditional_node_t* result = malloc_conditional_node();
      ((result->condition)=left);
      ((result->expr_if_true)=expr_if_true);
      ((result->expr_if_false)=expr_if_false);
      return pstate_set_result_node(pstate, to_node(result));
    }
while (0);
    default:
    break;
  }
  return pstate_error(pstate, saved_position, PARSE_ERROR_UNHANDLED_INSTRUCTION);
}

/* i=410 j=0 */
pratt_parser_instruction_t get_prefix_instruction(token_t* token){
  switch ((token->type))
  {
    case TOKEN_TYPE_IDENTIFIER:
    if (token_matches(token, "sizeof"))
    {
      return make_parser_instruction(token, PRATT_PARSE_SIZEOF, PRECEDENCE_UNARY);
    }
    if (token_matches(token, "cast"))
    {
      return make_parser_instruction(token, PRATT_PARSE_CAST_MACRO, PRECEDENCE_UNARY);
    }
    if (token_matches(token, "typeof"))
    {
      return make_parser_instruction(token, PRATT_PARSE_TYPE_OF, PRECEDENCE_UNARY);
    }
    if (token_matches(token, "block_expr"))
    {
      return make_parser_instruction(token, PRATT_PARSE_BLOCK_EXPR, PRECEDENCE_UNARY);
    }
    if (token_matches(token, "compound_literal"))
    {
      return make_parser_instruction(token, PRATT_PARSE_LITERAL, PRECEDENCE_UNARY);
    }
    return make_parser_instruction(token, PRATT_PARSE_IDENTIFIER, PRECEDENCE_PRIMARY);
    case TOKEN_TYPE_INTEGER_LITERAL:
    case TOKEN_TYPE_FLOAT_LITERAL:
    case TOKEN_TYPE_STRING_LITERAL:
    case TOKEN_TYPE_CHARACTER_LITERAL:
    return make_parser_instruction(token, PRATT_PARSE_LITERAL, PRECEDENCE_PRIMARY);
    case TOKEN_TYPE_PUNCTUATION:
    break;
    default:
    return make_parser_instruction(token, PRATT_PARSE_UNKNOWN, PRECEDENCE_UNKNOWN);
  }
  if (token_matches(token, "("))
  {
    return make_parser_instruction(token, PRATT_PARSE_SUB_EXPRESSION, PRECEDENCE_PRIMARY);
  }
  if (((((((((token_matches(token, "+")||token_matches(token, "-"))||token_matches(token, "~"))||token_matches(token, "!"))||token_matches(token, "!"))||token_matches(token, "++"))||token_matches(token, "--"))||token_matches(token, "*"))||token_matches(token, "&")))
  {
    return make_parser_instruction(token, PRATT_PARSE_PREFIX_OPERATOR, PRECEDENCE_UNARY);
  }
  return ((pratt_parser_instruction_t) {0});
}

/* i=411 j=0 */
pratt_parser_instruction_t get_infix_instruction(token_t* token){
  if ((token_matches(token, "+")||token_matches(token, "-")))
  {
    return make_parser_instruction(token, PRATT_PARSE_BINARY_OPERATOR, PRECEDENCE_ADDITIVE);
  }
  if (((token_matches(token, "*")||token_matches(token, "/"))||token_matches(token, "%")))
  {
    return make_parser_instruction(token, PRATT_PARSE_BINARY_OPERATOR, PRECEDENCE_MULTIPICITIVE);
  }
  if ((token_matches(token, "<<")||token_matches(token, ">>")))
  {
    return make_parser_instruction(token, PRATT_PARSE_BINARY_OPERATOR, PRECEDENCE_SHIFT);
  }
  if (token_matches(token, "||"))
  {
    return make_parser_instruction(token, PRATT_PARSE_BINARY_OPERATOR, PRECEDENCE_LOGICAL_OR);
  }
  if (token_matches(token, "&&"))
  {
    return make_parser_instruction(token, PRATT_PARSE_BINARY_OPERATOR, PRECEDENCE_LOGICAL_AND);
  }
  if (token_matches(token, "|"))
  {
    return make_parser_instruction(token, PRATT_PARSE_BINARY_OPERATOR, PRECEDENCE_OR);
  }
  if (token_matches(token, "^"))
  {
    return make_parser_instruction(token, PRATT_PARSE_BINARY_OPERATOR, PRECEDENCE_XOR);
  }
  if (token_matches(token, "&"))
  {
    return make_parser_instruction(token, PRATT_PARSE_BINARY_OPERATOR, PRECEDENCE_AND);
  }
  if ((token_matches(token, "==")||token_matches(token, "!=")))
  {
    return make_parser_instruction(token, PRATT_PARSE_BINARY_OPERATOR, PRECEDENCE_EQUALITY);
  }
  if ((((token_matches(token, "<")||token_matches(token, "<="))||token_matches(token, ">"))||token_matches(token, ">=")))
  {
    return make_parser_instruction(token, PRATT_PARSE_BINARY_OPERATOR, PRECEDENCE_RELATIONAL);
  }
  if ((token_matches(token, "->")||token_matches(token, ".")))
  {
    return make_parser_instruction(token, PRATT_PARSE_BINARY_OPERATOR, PRECEDENCE_POSTFIX);
  }
  if ((token_matches(token, "++")||token_matches(token, "--")))
  {
    return make_parser_instruction(token, PRATT_PARSE_POSTFIX_OPERATOR, PRECEDENCE_POSTFIX);
  }
  if (((((((((((token_matches(token, "=")||token_matches(token, "+="))||token_matches(token, "-="))||token_matches(token, "*="))||token_matches(token, "/="))||token_matches(token, "%="))||token_matches(token, "&="))||token_matches(token, "^="))||token_matches(token, "|="))||token_matches(token, "<<="))||token_matches(token, ">>=")))
  {
    return make_parser_instruction(token, PRATT_PARSE_BINARY_OPERATOR, PRECEDENCE_ASSIGNMENT);
  }
  if (token_matches(token, "["))
  {
    return make_parser_instruction(token, PRATT_PARSE_INDEX_EXPRESSION, PRECEDENCE_POSTFIX);
  }
  if (token_matches(token, "("))
  {
    return make_parser_instruction(token, PRATT_PARSE_CALL, PRECEDENCE_POSTFIX);
  }
  if (token_matches(token, "?"))
  {
    return make_parser_instruction(token, PRATT_PARSE_CONDITIONAL, PRECEDENCE_CONDITIONAL);
  }
  return ((pratt_parser_instruction_t) {0});
}

/* i=412 j=0 */
associativity_t precedence_to_associativity(precedence_t precedence){
  switch (precedence)
  {
    case PRECEDENCE_PRIMARY:
    return LEFT_TO_RIGHT;
    case PRECEDENCE_POSTFIX:
    return LEFT_TO_RIGHT;
    case PRECEDENCE_UNARY:
    return RIGHT_TO_LEFT;
    case PRECEDENCE_MULTIPICITIVE:
    return LEFT_TO_RIGHT;
    case PRECEDENCE_ADDITIVE:
    return LEFT_TO_RIGHT;
    case PRECEDENCE_SHIFT:
    return LEFT_TO_RIGHT;
    case PRECEDENCE_RELATIONAL:
    return LEFT_TO_RIGHT;
    case PRECEDENCE_EQUALITY:
    return LEFT_TO_RIGHT;
    case PRECEDENCE_AND:
    return LEFT_TO_RIGHT;
    case PRECEDENCE_XOR:
    return LEFT_TO_RIGHT;
    case PRECEDENCE_OR:
    return LEFT_TO_RIGHT;
    case PRECEDENCE_LOGICAL_AND:
    return LEFT_TO_RIGHT;
    case PRECEDENCE_LOGICAL_OR:
    return LEFT_TO_RIGHT;
    case PRECEDENCE_CONDITIONAL:
    return RIGHT_TO_LEFT;
    case PRECEDENCE_ASSIGNMENT:
    return RIGHT_TO_LEFT;
    case PRECEDENCE_COMMA:
    return LEFT_TO_RIGHT;
    default:
    fatal_error(ERROR_ILLEGAL_STATE);
  }
  return 0;
}

/* i=413 j=0 */
pstatus_t parse_statement(pstate_t* pstate){
  uint64_t saved_position = (pstate->position);
  if ((((((((((((((((parse_block(pstate)||parse_break_statement(pstate_ignore_error(pstate)))||parse_return_statement(pstate_ignore_error(pstate)))||parse_if_statement(pstate_ignore_error(pstate)))||parse_while_statement(pstate_ignore_error(pstate)))||parse_do_statement(pstate_ignore_error(pstate)))||parse_for_statement(pstate_ignore_error(pstate)))||parse_switch_statement(pstate_ignore_error(pstate)))||parse_case_label(pstate_ignore_error(pstate)))||parse_default_label(pstate_ignore_error(pstate)))||parse_continue_statement(pstate_ignore_error(pstate)))||parse_goto_statement(pstate_ignore_error(pstate)))||parse_label_statement(pstate_ignore_error(pstate)))||parse_variable_definition_node(pstate_ignore_error(pstate)))||parse_expression_statement(pstate_ignore_error(pstate)))||parse_empty_statement(pstate_ignore_error(pstate))))
  {
    return true;
  }
  return pstate_error(pstate, saved_position, PARSE_ERROR_EXPECTED_STATEMENT);
}

/* i=414 j=0 */
pstatus_t parse_return_statement(pstate_t* pstate){
  uint64_t saved_position = (pstate->position);
  if ((!pstate_expect_token_string(pstate, "return")))
  {
    return pstate_propagate_error(pstate, saved_position);
  }
  parse_expression(pstate);
  parse_node_t* expr = pstate_get_optional_result_node(pstate);
  if ((!pstate_expect_token_string(pstate, ";")))
  {
    return pstate_propagate_error(pstate, saved_position);
  }
  return pstate_set_result_node(pstate, to_node(make_return_statement(expr)));
}

/* i=415 j=0 */
pstatus_t parse_if_statement(pstate_t* pstate){
  uint64_t saved_position = (pstate->position);
  if ((((!pstate_expect_token_string(pstate, "if"))||(!pstate_expect_token_string(pstate, "(")))||(!parse_expression(pstate))))
  {
    return pstate_propagate_error(pstate, saved_position);
  }
  parse_node_t* if_test = pstate_get_result_node(pstate);
  if (((!pstate_expect_token_string(pstate, ")"))||(!parse_statement(pstate))))
  {
    return pstate_propagate_error(pstate, saved_position);
  }
  parse_node_t* if_true = pstate_get_result_node(pstate);
  parse_node_t* if_false = NULL;
  if (pstate_match_token_string(pstate, "else"))
  {
    pstate_advance(pstate);
    if ((!parse_statement(pstate)))
    {
      return pstate_propagate_error(pstate, saved_position);
    }
    (if_false=pstate_get_result_node(pstate));
  }
  return pstate_set_result_node(pstate, to_node(make_if_statement(if_test, if_true, if_false)));
}

/* i=416 j=0 */
pstatus_t parse_while_statement(pstate_t* pstate){
  uint64_t saved_position = (pstate->position);
  if ((((!pstate_expect_token_string(pstate, "while"))||(!pstate_expect_token_string(pstate, "(")))||(!parse_expression(pstate))))
  {
    return pstate_propagate_error(pstate, saved_position);
  }
  parse_node_t* while_test = pstate_get_result_node(pstate);
  if (((!pstate_expect_token_string(pstate, ")"))||(!parse_statement(pstate))))
  {
    return pstate_propagate_error(pstate, saved_position);
  }
  parse_node_t* while_body = pstate_get_result_node(pstate);
  return pstate_set_result_node(pstate, to_node(make_while_statement(while_test, while_body)));
}

/* i=417 j=0 */
pstatus_t parse_do_statement(pstate_t* pstate){
  uint64_t saved_position = (pstate->position);
  if (((!pstate_expect_token_string(pstate, "do"))||(!parse_statement(pstate))))
  {
    return pstate_propagate_error(pstate, saved_position);
  }
  parse_node_t* do_while_body = pstate_get_result_node(pstate);
  if (((!pstate_expect_token_string(pstate, "while"))||(!pstate_expect_token_string(pstate, "("))))
  {
    return pstate_propagate_error(pstate, saved_position);
  }
  if ((!parse_expression(pstate)))
  {
    return pstate_propagate_error(pstate, saved_position);
  }
  parse_node_t* do_while_condition = pstate_get_result_node(pstate);
  if (((!pstate_expect_token_string(pstate, ")"))||(!pstate_expect_token_string(pstate, ";"))))
  {
    return pstate_propagate_error(pstate, saved_position);
  }
  return pstate_set_result_node(pstate, to_node(make_do_statement(do_while_body, do_while_condition)));
}

/* i=418 j=0 */
pstatus_t parse_for_statement(pstate_t* pstate){
  uint64_t saved_position = (pstate->position);
  if (((!pstate_expect_token_string(pstate, "for"))||(!pstate_expect_token_string(pstate, "("))))
  {
    return pstate_propagate_error(pstate, saved_position);
  }
  if ((!parse_statement(pstate)))
  {
    return pstate_propagate_error(pstate, saved_position);
  }
  parse_node_t* for_init = pstate_get_result_node(pstate);
  parse_node_t* for_test = NULL;
  if (parse_expression(pstate))
  {
    (for_test=pstate_get_result_node(pstate));
  }
  if ((!pstate_expect_token_string(pstate, ";")))
  {
    return pstate_propagate_error(pstate, saved_position);
  }
  parse_node_t* for_increment = NULL;
  if (parse_expression(pstate))
  {
    (for_increment=pstate_get_result_node(pstate));
  }
  if ((!pstate_expect_token_string(pstate, ")")))
  {
    return pstate_propagate_error(pstate, saved_position);
  }
  if ((!parse_statement(pstate)))
  {
    return pstate_propagate_error(pstate, saved_position);
  }
  parse_node_t* for_body = pstate_get_result_node(pstate);
  return pstate_set_result_node(pstate, to_node(make_for_statement(for_init, for_test, for_increment, for_body)));
}

/* i=419 j=0 */
pstatus_t parse_switch_statement(pstate_t* pstate){
  uint64_t saved_position = (pstate->position);
  if ((((!pstate_expect_token_string(pstate, "switch"))||(!pstate_expect_token_string(pstate, "(")))||(!parse_expression(pstate))))
  {
    return pstate_propagate_error(pstate, saved_position);
  }
  parse_node_t* switch_item = pstate_get_result_node(pstate);
  if (((!pstate_expect_token_string(pstate, ")"))||(!parse_block(pstate))))
  {
    return pstate_propagate_error(pstate, saved_position);
  }
  parse_node_t* block = pstate_get_result_node(pstate);
  return pstate_set_result_node(pstate, to_node(make_switch_statement(switch_item, block)));
}

/* i=420 j=0 */
pstatus_t parse_case_label(pstate_t* pstate){
  uint64_t saved_position = (pstate->position);
  if (((!pstate_expect_token_string(pstate, "case"))||(!parse_expression(pstate))))
  {
    return pstate_propagate_error(pstate, saved_position);
  }
  parse_node_t* case_expr = pstate_get_result_node(pstate);
  if ((!pstate_expect_token_string(pstate, ":")))
  {
    return pstate_propagate_error(pstate, saved_position);
  }
  return pstate_set_result_node(pstate, to_node(make_case_label(case_expr)));
}

/* i=421 j=0 */
pstatus_t parse_expression_statement(pstate_t* pstate){
  uint64_t saved_position = (pstate->position);
  if ((!parse_expression(pstate)))
  {
    return pstate_propagate_error(pstate, saved_position);
  }
  parse_node_t* expr = pstate_get_result_node(pstate);
  if ((!pstate_expect_token_string(pstate, ";")))
  {
    return pstate_propagate_error(pstate, saved_position);
  }
  return pstate_set_result_node(pstate, to_node(make_expression_statement_node(expr)));
}

/* i=422 j=0 */
pstatus_t parse_goto_statement(pstate_t* pstate){
  uint64_t saved_position = (pstate->position);
  token_t* label_token = pstate_peek(pstate, 1);
  if ((((!pstate_expect_token_string(pstate, "goto"))||(!pstate_expect_token_type(pstate, TOKEN_TYPE_IDENTIFIER)))||(!pstate_expect_token_string(pstate, ";"))))
  {
    return pstate_propagate_error(pstate, saved_position);
  }
  return pstate_set_result_node(pstate, to_node(make_goto_statement(label_token)));
}

/* i=423 j=0 */
pstatus_t parse_break_statement(pstate_t* pstate){
  uint64_t saved_position = (pstate->position);
  token_t* keyword_token = pstate_peek(pstate, 0);
  if (((!pstate_expect_token_string(pstate, "break"))||(!pstate_expect_token_string(pstate, ";"))))
  {
    return pstate_propagate_error(pstate, saved_position);
  }
  return pstate_set_result_node(pstate, to_node(make_break_statement(keyword_token)));
}

/* i=424 j=0 */
pstatus_t parse_continue_statement(pstate_t* pstate){
  uint64_t saved_position = (pstate->position);
  token_t* keyword_token = pstate_peek(pstate, 0);
  if (((!pstate_expect_token_string(pstate, "continue"))||(!pstate_expect_token_string(pstate, ";"))))
  {
    return pstate_propagate_error(pstate, saved_position);
  }
  return pstate_set_result_node(pstate, to_node(make_continue_statement(keyword_token)));
}

/* i=425 j=0 */
pstatus_t parse_label_statement(pstate_t* pstate){
  uint64_t saved_position = (pstate->position);
  token_t* label_token = pstate_peek(pstate, 0);
  if (((!pstate_expect_token_type(pstate, TOKEN_TYPE_IDENTIFIER))||(!pstate_expect_token_string(pstate, ":"))))
  {
    return pstate_propagate_error(pstate, saved_position);
  }
  return pstate_set_result_node(pstate, to_node(make_label_statement(label_token)));
}

/* i=426 j=0 */
pstatus_t parse_default_label(pstate_t* pstate){
  uint64_t saved_position = (pstate->position);
  token_t* default_token = pstate_peek(pstate, 0);
  if (((!pstate_expect_token_string(pstate, "default"))||(!pstate_expect_token_string(pstate, ":"))))
  {
    return pstate_propagate_error(pstate, saved_position);
  }
  return pstate_set_result_node(pstate, to_node(make_default_label(default_token)));
}

/* i=427 j=0 */
pstatus_t parse_empty_statement(pstate_t* pstate){
  uint64_t saved_position = (pstate->position);
  token_t* semi_colon_token = pstate_peek(pstate, 0);
  if ((!pstate_expect_token_string(pstate, ";")))
  {
    return pstate_propagate_error(pstate, saved_position);
  }
  return pstate_set_result_node(pstate, to_node(make_empty_statement(semi_colon_token)));
}

/* i=428 j=0 */
break_statement_node_t* make_break_statement(token_t* break_keyword_token){
  break_statement_node_t* result = malloc_struct(break_statement_node_t);
  ((result->tag)=PARSE_NODE_BREAK_STATEMENT);
  ((result->break_keyword_token)=break_keyword_token);
  return result;
}

/* i=429 j=0 */
break_statement_node_t* to_break_statement_node(parse_node_t* ptr){
  if (((ptr==NULL)||((ptr->tag)!=PARSE_NODE_BREAK_STATEMENT)))
  {
    fatal_error(ERROR_ILLEGAL_STATE);
  }
  return (/*CAST*/(break_statement_node_t*) ptr);
}

/* i=430 j=0 */
continue_statement_node_t* make_continue_statement(token_t* keyword_token){
  continue_statement_node_t* result = malloc_struct(continue_statement_node_t);
  ((result->tag)=PARSE_NODE_CONTINUE_STATEMENT);
  ((result->continue_keyword_token)=keyword_token);
  return result;
}

/* i=431 j=0 */
continue_statement_node_t* to_continue_statement_node(parse_node_t* ptr){
  if (((ptr==NULL)||((ptr->tag)!=PARSE_NODE_CONTINUE_STATEMENT)))
  {
    fatal_error(ERROR_ILLEGAL_STATE);
  }
  return (/*CAST*/(continue_statement_node_t*) ptr);
}

/* i=432 j=0 */
label_statement_node_t* make_label_statement(token_t* label){
  label_statement_node_t* result = malloc_struct(label_statement_node_t);
  ((result->tag)=PARSE_NODE_LABEL_STATEMENT);
  ((result->label)=label);
  return result;
}

/* i=433 j=0 */
label_statement_node_t* to_label_statement_node(parse_node_t* ptr){
  if (((ptr==NULL)||((ptr->tag)!=PARSE_NODE_LABEL_STATEMENT)))
  {
    fatal_error(ERROR_ILLEGAL_STATE);
  }
  return (/*CAST*/(label_statement_node_t*) ptr);
}

/* i=434 j=0 */
goto_statement_node_t* make_goto_statement(token_t* label){
  goto_statement_node_t* result = malloc_struct(goto_statement_node_t);
  ((result->tag)=PARSE_NODE_GOTO_STATEMENT);
  ((result->label)=label);
  return result;
}

/* i=435 j=0 */
goto_statement_node_t* to_goto_statement_node(parse_node_t* ptr){
  if (((ptr==NULL)||((ptr->tag)!=PARSE_NODE_GOTO_STATEMENT)))
  {
    fatal_error(ERROR_ILLEGAL_STATE);
  }
  return (/*CAST*/(goto_statement_node_t*) ptr);
}

/* i=436 j=0 */
empty_statement_node_t* make_empty_statement(token_t* semi_colon_token){
  empty_statement_node_t* result = malloc_struct(empty_statement_node_t);
  ((result->tag)=PARSE_NODE_EMPTY_STATEMENT);
  ((result->semi_colon_token)=semi_colon_token);
  return result;
}

/* i=437 j=0 */
empty_statement_node_t* to_empty_statement_node(parse_node_t* ptr){
  if (((ptr==NULL)||((ptr->tag)!=PARSE_NODE_EMPTY_STATEMENT)))
  {
    fatal_error(ERROR_ILLEGAL_STATE);
  }
  return (/*CAST*/(empty_statement_node_t*) ptr);
}

/* i=438 j=0 */
switch_statement_node_t* make_switch_statement(parse_node_t* expression, parse_node_t* block){
  switch_statement_node_t* result = malloc_struct(switch_statement_node_t);
  ((result->tag)=PARSE_NODE_SWITCH_STATEMENT);
  ((result->expression)=expression);
  ((result->block)=block);
  return result;
}

/* i=439 j=0 */
switch_statement_node_t* to_switch_statement_node(parse_node_t* ptr){
  if (((ptr==NULL)||((ptr->tag)!=PARSE_NODE_SWITCH_STATEMENT)))
  {
    fatal_error(ERROR_ILLEGAL_STATE);
  }
  return (/*CAST*/(switch_statement_node_t*) ptr);
}

/* i=440 j=0 */
case_label_node_t* make_case_label(parse_node_t* expression){
  case_label_node_t* result = malloc_struct(case_label_node_t);
  ((result->tag)=PARSE_NODE_CASE_LABEL);
  ((result->expression)=expression);
  return result;
}

/* i=441 j=0 */
case_label_node_t* to_case_label_node(parse_node_t* ptr){
  if (((ptr==NULL)||((ptr->tag)!=PARSE_NODE_CASE_LABEL)))
  {
    fatal_error(ERROR_ILLEGAL_STATE);
  }
  return (/*CAST*/(case_label_node_t*) ptr);
}

/* i=442 j=0 */
default_label_node_t* make_default_label(token_t* default_token){
  default_label_node_t* result = malloc_struct(default_label_node_t);
  ((result->tag)=PARSE_NODE_DEFAULT_LABEL);
  ((result->default_token)=default_token);
  return result;
}

/* i=443 j=0 */
default_label_node_t* to_default_label_node(parse_node_t* ptr){
  if (((ptr==NULL)||((ptr->tag)!=PARSE_NODE_DEFAULT_LABEL)))
  {
    fatal_error(ERROR_ILLEGAL_STATE);
  }
  return (/*CAST*/(default_label_node_t*) ptr);
}

/* i=444 j=0 */
block_node_t* make_block_node(){
  block_node_t* result = malloc_struct(block_node_t);
  ((result->tag)=PARSE_NODE_BLOCK);
  return result;
}

/* i=445 j=0 */
block_node_t* to_block_node(parse_node_t* ptr){
  if (((ptr==NULL)||((ptr->tag)!=PARSE_NODE_BLOCK)))
  {
    fatal_error(ERROR_ILLEGAL_STATE);
  }
  return (/*CAST*/(block_node_t*) ptr);
}

/* i=446 j=0 */
for_statement_node_t* make_for_statement(parse_node_t* for_init, parse_node_t* for_test, parse_node_t* for_increment, parse_node_t* for_body){
  for_statement_node_t* result = malloc_struct(for_statement_node_t);
  ((result->tag)=PARSE_NODE_FOR_STATEMENT);
  ((result->for_init)=for_init);
  ((result->for_test)=for_test);
  ((result->for_increment)=for_increment);
  ((result->for_body)=for_body);
  return result;
}

/* i=447 j=0 */
for_statement_node_t* to_for_statement_node(parse_node_t* ptr){
  if (((ptr==NULL)||((ptr->tag)!=PARSE_NODE_FOR_STATEMENT)))
  {
    fatal_error(ERROR_ILLEGAL_STATE);
  }
  return (/*CAST*/(for_statement_node_t*) ptr);
}

/* i=448 j=0 */
if_statement_node_t* make_if_statement(parse_node_t* if_condition, parse_node_t* if_true, parse_node_t* if_else){
  if_statement_node_t* result = malloc_struct(if_statement_node_t);
  ((result->tag)=PARSE_NODE_IF_STATEMENT);
  ((result->if_condition)=if_condition);
  ((result->if_true)=if_true);
  ((result->if_else)=if_else);
  return result;
}

/* i=449 j=0 */
if_statement_node_t* to_if_statement_node(parse_node_t* ptr){
  if (((ptr==NULL)||((ptr->tag)!=PARSE_NODE_IF_STATEMENT)))
  {
    fatal_error(ERROR_ILLEGAL_STATE);
  }
  return (/*CAST*/(if_statement_node_t*) ptr);
}

/* i=450 j=0 */
expression_statement_node_t* make_expression_statement_node(parse_node_t* expression){
  expression_statement_node_t* result = malloc_struct(expression_statement_node_t);
  ((result->tag)=PARSE_NODE_EXPRESSION_STATEMENT);
  ((result->expression)=expression);
  return result;
}

/* i=451 j=0 */
expression_statement_node_t* to_expression_statement_node(parse_node_t* ptr){
  if (((ptr==NULL)||((ptr->tag)!=PARSE_NODE_EXPRESSION_STATEMENT)))
  {
    fatal_error(ERROR_ILLEGAL_STATE);
  }
  return (/*CAST*/(expression_statement_node_t*) ptr);
}

/* i=452 j=0 */
return_statement_node_t* make_return_statement(parse_node_t* expression){
  return_statement_node_t* result = malloc_struct(return_statement_node_t);
  ((result->tag)=PARSE_NODE_RETURN_STATEMENT);
  ((result->expression)=expression);
  return result;
}

/* i=453 j=0 */
return_statement_node_t* to_return_statement_node(parse_node_t* ptr){
  if (((ptr==NULL)||((ptr->tag)!=PARSE_NODE_RETURN_STATEMENT)))
  {
    fatal_error(ERROR_ILLEGAL_STATE);
  }
  return (/*CAST*/(return_statement_node_t*) ptr);
}

/* i=454 j=0 */
while_statement_node_t* make_while_statement(parse_node_t* condition, parse_node_t* body){
  while_statement_node_t* result = malloc_struct(while_statement_node_t);
  ((result->tag)=PARSE_NODE_WHILE_STATEMENT);
  ((result->condition)=condition);
  ((result->body)=body);
  return result;
}

/* i=455 j=0 */
while_statement_node_t* to_while_statement_node(parse_node_t* ptr){
  if (((ptr==NULL)||((ptr->tag)!=PARSE_NODE_WHILE_STATEMENT)))
  {
    fatal_error(ERROR_ILLEGAL_STATE);
  }
  return (/*CAST*/(while_statement_node_t*) ptr);
}

/* i=456 j=0 */
do_statement_node_t* make_do_statement(parse_node_t* body, parse_node_t* condition){
  do_statement_node_t* result = malloc_struct(do_statement_node_t);
  ((result->tag)=PARSE_NODE_DO_STATEMENT);
  ((result->body)=body);
  ((result->condition)=condition);
  return result;
}

/* i=457 j=0 */
do_statement_node_t* to_do_statement_node(parse_node_t* ptr){
  if (((ptr==NULL)||((ptr->tag)!=PARSE_NODE_DO_STATEMENT)))
  {
    fatal_error(ERROR_ILLEGAL_STATE);
  }
  return (/*CAST*/(do_statement_node_t*) ptr);
}

/* i=460 j=1 */
pstatus_t parse_typeof_node(pstate_t* pstate){
  uint64_t saved_position = (pstate->position);
  if ((!pstate_expect_token_string(pstate, "typeof")))
  {
    return pstate_propagate_error(pstate, saved_position);
  }
  if ((!pstate_expect_token_string(pstate, "(")))
  {
    return pstate_propagate_error(pstate, saved_position);
  }
  if ((!parse_type_node(pstate)))
  {
    return pstate_propagate_error(pstate, saved_position);
  }
  parse_node_t* type_node = pstate_get_result_node(pstate);
  if ((!pstate_expect_token_string(pstate, ")")))
  {
    return pstate_propagate_error(pstate, saved_position);
  }
  type_node_t* result = malloc_type_node();
  ((result->type_node_kind)=TYPE_NODE_KIND_TYPEOF);
  ((result->user_type)=type_node);
  return pstate_set_result_node(pstate, to_node(result));
}

/* i=461 j=1 */
pstatus_t parse_user_type_node(pstate_t* pstate){
  uint64_t saved_position = (pstate->position);
  if (((parse_enum_node(pstate)||parse_structure_node(pstate_ignore_error(pstate)))||parse_union_node(pstate_ignore_error(pstate))))
  {
    return true;
  }
  else
  {
    return pstate_propagate_error(pstate, saved_position);
  }
}

/* i=462 j=1 */
pstatus_t parse_enum_node(pstate_t* pstate){
  uint64_t saved_position = (pstate->position);
  if ((!pstate_expect_token_string(pstate, "enum")))
  {
    return pstate_propagate_error(pstate, saved_position);
  }
  enum_node_t* result = malloc_enum_node();
  if (pstate_expect_token_type(pstate, TOKEN_TYPE_IDENTIFIER))
  {
    ((result->name)=pstate_get_result_token(pstate));
  }
  else
  {
    pstate_ignore_error(pstate);
  }
  if ((!pstate_expect_token_string(pstate, "{")))
  {
    pstate_ignore_error(pstate);
    ((result->partial_definition)=true);
    return pstate_set_result_node(pstate, to_node(result));
  }
  while (true)
  {
    if (pstate_expect_token_string(pstate, "}"))
    {
      break;
    }
    pstate_ignore_error(pstate);
    if ((!parse_enum_element_node(pstate)))
    {
      return pstate_propagate_error(pstate, saved_position);
    }
    node_list_add_node((&(result->elements)), pstate_get_result_node(pstate));
  }
  return pstate_set_result_node(pstate, to_node(result));
}

/* i=463 j=1 */
pstatus_t parse_structure_node(pstate_t* pstate){
  uint64_t saved_position = (pstate->position);
  if ((!pstate_expect_token_string(pstate, "struct")))
  {
    return pstate_propagate_error(pstate, saved_position);
  }
  struct_node_t* result = malloc_struct_node();
  if (pstate_expect_token_type(pstate, TOKEN_TYPE_IDENTIFIER))
  {
    ((result->name)=pstate_get_result_token(pstate));
  }
  else
  {
    pstate_ignore_error(pstate);
  }
  if (pstate_expect_token_string(pstate, "{"))
  {
    ((result->partial_definition)=false);
    while (true)
    {
      if (pstate_expect_token_string(pstate, "}"))
      {
        break;
      }
      pstate_ignore_error(pstate);
      if ((!parse_field_node(pstate)))
      {
        return pstate_propagate_error(pstate, saved_position);
      }
      node_list_add_node((&(result->fields)), pstate_get_result_node(pstate));
    }
  }
  else
  {
    ((result->partial_definition)=true);
    pstate_ignore_error(pstate);
  }
  return pstate_set_result_node(pstate, to_node(result));
}

/* i=464 j=1 */
pstatus_t parse_union_node(pstate_t* pstate){
  uint64_t saved_position = (pstate->position);
  if ((!pstate_expect_token_string(pstate, "union")))
  {
    return pstate_propagate_error(pstate, saved_position);
  }
  union_node_t* result = malloc_union_node();
  if (pstate_expect_token_type(pstate, TOKEN_TYPE_IDENTIFIER))
  {
    ((result->name)=pstate_get_result_token(pstate));
  }
  else
  {
    pstate_ignore_error(pstate);
  }
  if (pstate_expect_token_string(pstate, "{"))
  {
    ((result->partial_definition)=false);
    while (true)
    {
      if (pstate_expect_token_string(pstate, "}"))
      {
        break;
      }
      pstate_ignore_error(pstate);
      if ((!parse_field_node(pstate)))
      {
        return pstate_propagate_error(pstate, saved_position);
      }
      node_list_add_node((&(result->fields)), pstate_get_result_node(pstate));
    }
  }
  else
  {
    ((result->partial_definition)=true);
    pstate_ignore_error(pstate);
  }
  return pstate_set_result_node(pstate, to_node(result));
}

/* i=465 j=0 */
pstatus_t parse_type_node(pstate_t* pstate){
  uint64_t saved_position = (pstate->position);
  if ((parse_typeof_node(pstate)||parse_function_type(pstate)))
  {
    return true;
  }
  type_node_t* result = malloc_type_node();
  type_qualifier_t qualifiers = TYPE_QUALIFIER_NONE;
  if (pstate_match_token_string(pstate, "const"))
  {
    pstate_advance(pstate);
    (qualifiers|=TYPE_QUALIFIER_CONST);
  }
  canonical_type_result_t canonical_type_result = parse_canonical_type(pstate);
  token_t* type_name = (canonical_type_result.canonical_type);
  if ((type_name!=NULL))
  {
    while (((canonical_type_result.consumed_tokens)>0))
    {
      ((canonical_type_result.consumed_tokens)--);
      pstate_advance(pstate);
    }
    ((result->type_node_kind)=TYPE_NODE_KIND_PRIMITIVE_TYPENAME);
    ((result->type_name)=type_name);
  }
  else
  if (parse_user_type_node(pstate))
  {
    ((result->type_node_kind)=TYPE_NODE_KIND_TYPE_EXPRESSION);
    ((result->user_type)=pstate_get_result_node(pstate));
  }
  else
  if (pstate_expect_token_type(pstate, TOKEN_TYPE_IDENTIFIER))
  {
    ((result->type_node_kind)=TYPE_NODE_KIND_TYPENAME);
    ((result->type_name)=pstate_get_result_token(pstate));
  }
  else
  {
    return pstate_propagate_error(pstate, saved_position);
  }
  while (true)
  {
    if (pstate_expect_token_string(pstate, "*"))
    {
      type_node_t* ptr_result = malloc_type_node();
      ((ptr_result->type_node_kind)=TYPE_NODE_KIND_POINTER);
      node_list_add_node((&(ptr_result->type_args)), to_node(result));
      (result=ptr_result);
    }
    else
    if (token_matches(pstate_peek(pstate, 0), "["))
    {
      pstate_advance(pstate);
      type_node_t* array_result = malloc_type_node();
      ((array_result->type_node_kind)=TYPE_NODE_KIND_ARRAY);
      node_list_add_node((&(array_result->type_args)), to_node(result));
      if (pstate_expect_token_type(pstate, TOKEN_TYPE_INTEGER_LITERAL))
      {
        literal_node_t* literal = malloc_literal_node();
        ((literal->token)=pstate_get_result_token(pstate));
        ((array_result->type_node_kind)=TYPE_NODE_KIND_SIZED_ARRAY);
        node_list_add_node((&(array_result->type_args)), to_node(literal));
      }
      else
      {
        pstate_ignore_error(pstate);
      }
      if ((!pstate_expect_token_string(pstate, "]")))
      {
        return pstate_propagate_error(pstate, saved_position);
      }
      (result=array_result);
    }
    else
    {
      pstate_ignore_error(pstate);
      break;
    }
  }
  ((result->qualifiers)=qualifiers);
  return pstate_set_result_node(pstate, to_node(result));
}

/* i=466 j=0 */
canonical_type_result_t make_type_token_result(char* str, int consumed_tokens){
  token_t* canonical_token = malloc_struct(typeof(token_t));
  ((canonical_token->type)=TOKEN_TYPE_IDENTIFIER);
  ((canonical_token->buffer)=buffer_from_string(str));
  ((canonical_token->start)=0);
  ((canonical_token->end)=strlen(str));
  return ((canonical_type_result_t) {.canonical_type = canonical_token, .consumed_tokens = consumed_tokens});
}

/* i=467 j=0 */
canonical_type_result_t parse_canonical_type(pstate_t* pstate){
  token_t* a = pstate_peek(pstate, 0);
  token_t* b = pstate_peek(pstate, 1);
  token_t* c = pstate_peek(pstate, 2);
  if (((token_matches(a, "signed")&&token_matches(b, "short"))&&token_matches(c, "int")))
  {
    return make_type_token_result("short", 3);
  }
  if (((token_matches(a, "unsigned")&&token_matches(b, "short"))&&token_matches(c, "int")))
  {
    return make_type_token_result("unsigned short int", 3);
  }
  if (((token_matches(a, "signed")&&token_matches(b, "long"))&&token_matches(c, "int")))
  {
    return make_type_token_result("long", 3);
  }
  if (((token_matches(a, "unsigned")&&token_matches(b, "long"))&&token_matches(c, "int")))
  {
    return make_type_token_result("unsigned long", 3);
  }
  if ((token_matches(a, "short")&&token_matches(b, "int")))
  {
    return make_type_token_result("short", 2);
  }
  if ((token_matches(a, "signed")&&token_matches(b, "short")))
  {
    return make_type_token_result("short", 2);
  }
  if ((token_matches(a, "signed")&&token_matches(b, "int")))
  {
    return make_type_token_result("int", 2);
  }
  if ((token_matches(a, "long")&&token_matches(b, "int")))
  {
    return make_type_token_result("long", 2);
  }
  if ((token_matches(a, "long")&&token_matches(b, "long")))
  {
    return make_type_token_result("long long", 2);
  }
  if ((token_matches(a, "long")&&token_matches(b, "double")))
  {
    return make_type_token_result("long double", 2);
  }
  if ((token_matches(a, "signed")&&token_matches(b, "long")))
  {
    return make_type_token_result("long", 2);
  }
  if ((token_matches(a, "unsigned")&&token_matches(b, "int")))
  {
    return make_type_token_result("unsigned int", 2);
  }
  if ((token_matches(a, "unsigned")&&token_matches(b, "long")))
  {
    return make_type_token_result("unsigned long", 2);
  }
  if ((token_matches(a, "unsigned")&&token_matches(b, "char")))
  {
    return make_type_token_result("unsigned char", 2);
  }
  if ((token_matches(a, "signed")&&token_matches(b, "char")))
  {
    return make_type_token_result("char", 2);
  }
  if ((token_matches(a, "long")&&token_matches(b, "float")))
  {
    return make_type_token_result("double", 2);
  }
  if (token_matches(a, "signed"))
  {
    return make_type_token_result("int", 1);
  }
  if (token_matches(a, "char"))
  {
    return make_type_token_result("char", 1);
  }
  if (token_matches(a, "int"))
  {
    return make_type_token_result("int", 1);
  }
  if (token_matches(a, "long"))
  {
    return make_type_token_result("long", 1);
  }
  if (token_matches(a, "float"))
  {
    return make_type_token_result("float", 1);
  }
  if (token_matches(a, "double"))
  {
    return make_type_token_result("double", 1);
  }
  return ((canonical_type_result_t) {.canonical_type = NULL, .consumed_tokens = 0});
}

/* i=468 j=0 */
pstatus_t parse_function_type(pstate_t* pstate){
  uint64_t saved_position = (pstate->position);
  token_t* fn_t_token = pstate_peek(pstate, 0);
  if (((!pstate_expect_token_string(pstate, "fn_t"))||(!pstate_expect_token_string(pstate, "("))))
  {
    return pstate_propagate_error(pstate, saved_position);
  }
  type_node_t* result = malloc_type_node();
  ((result->type_node_kind)=TYPE_NODE_KIND_TYPE_EXPRESSION);
  ((result->type_name)=fn_t_token);
  if ((!parse_type_node(pstate)))
  {
    return pstate_propagate_error(pstate, saved_position);
  }
  node_list_add_node((&(result->type_args)), pstate_get_result_node(pstate));
  while (parse_function_type_argument(pstate))
  {
    node_list_add_node((&(result->type_args)), pstate_get_result_node(pstate));
  }
  if ((!pstate_expect_token_string(pstate, ")")))
  {
    return pstate_propagate_error(pstate, saved_position);
  }
  return pstate_set_result_node(pstate, to_node(result));
}

/* i=469 j=0 */
pstatus_t parse_function_type_argument(pstate_t* pstate){
  uint64_t saved_position = (pstate->position);
  if ((!pstate_expect_token_string(pstate, ",")))
  {
    return pstate_propagate_error(pstate, saved_position);
  }
  if ((!parse_type_node(pstate)))
  {
    return pstate_propagate_error(pstate, saved_position);
  }
  token_t* suffix = pstate_peek(pstate, 0);
  if (((suffix->type)==TOKEN_TYPE_IDENTIFIER))
  {
    pstate_advance(pstate);
  }
  return true;
}

/* i=481 j=0 */
pstatus_t parse_field_node(pstate_t* pstate){
  uint64_t saved_position = (pstate->position);
  if ((!parse_type_node(pstate)))
  {
    return pstate_propagate_error(pstate, saved_position);
  }
  type_node_t* field_type = to_type_node(pstate_get_result_node(pstate));
  if ((!pstate_expect_token_type(pstate, TOKEN_TYPE_IDENTIFIER)))
  {
    log_warn("Allowing unnamed field in case the type is a union");
    pstate_ignore_error(pstate);
  }
  token_t* field_name = pstate_get_result_token(pstate);
  if (pstate_expect_token_string(pstate, ":"))
  {
    pstate_advance(pstate);
  }
  else
  {
    pstate_ignore_error(pstate);
  }
  if ((!pstate_expect_token_string(pstate, ";")))
  {
    return pstate_propagate_error(pstate, saved_position);
  }
  field_node_t* result = malloc_field_node();
  ((result->type)=field_type);
  ((result->name)=field_name);
  return pstate_set_result_node(pstate, to_node(result));
}

/* i=482 j=0 */
pstatus_t parse_enum_element_node(pstate_t* pstate){
  uint64_t saved_position = (pstate->position);
  if ((!pstate_expect_token_type(pstate, TOKEN_TYPE_IDENTIFIER)))
  {
    return pstate_propagate_error(pstate, saved_position);
  }
  token_t* name = pstate_get_result_token(pstate);
  token_t* value = NULL;
  if (pstate_expect_token_string(pstate, "="))
  {
    if ((!pstate_expect_token_type(pstate, TOKEN_TYPE_INTEGER_LITERAL)))
    {
      return pstate_propagate_error(pstate, saved_position);
    }
    (value=pstate_get_result_token(pstate));
  }
  if ((!pstate_expect_token_string(pstate, ",")))
  {
    pstate_ignore_error(pstate);
  }
  enum_element_t* result = malloc_enum_element();
  ((result->name)=name);
  ((result->value)=value);
  return pstate_set_result_node(pstate, to_node(result));
}

/* i=485 j=0 */
pstatus_t parse_expression(pstate_t* pstate){
  return pratt_parse_expression(pstate, 0);
}

/* i=486 j=0 */
pstatus_t parse_initializer(pstate_t* pstate){
  uint64_t saved_position = (pstate->position);
  if (token_matches(pstate_peek(pstate, 0), "{"))
  {
    return parse_balanced_construct(pstate);
  }
  return pstate_error(pstate, saved_position, PARSE_ERROR_CLOSE_BRACKET_EXPECTED);
}

/* i=487 j=0 */
pstatus_t parse_variable_definition_node(pstate_t* pstate){
  uint64_t saved_position = (pstate->position);
  token_t* storage_class_specifier = NULL;
  while ((pstate_expect_token_string(pstate, "static")||pstate_expect_token_string(pstate_ignore_error(pstate), "extern")))
  {
    if ((storage_class_specifier!=NULL))
    {
      return pstate_error(pstate, saved_position, PARSE_ERROR_CONFLICTING_STORAGE_CLASS_SPECIFIER);
    }
    (storage_class_specifier=pstate_get_result_token(pstate));
  }
  pstate_ignore_error(pstate);
  if ((!parse_type_node(pstate)))
  {
    return pstate_propagate_error(pstate, saved_position);
  }
  type_node_t* type_node = to_type_node(pstate_get_result_node(pstate));
  if ((!pstate_expect_token_type(pstate, TOKEN_TYPE_IDENTIFIER)))
  {
    return pstate_error(pstate, saved_position, PARSE_ERROR_IDENTIFIER_EXPECTED);
  }
  token_t* name = pstate_get_result_token(pstate);
  variable_definition_node_t* result = malloc_variable_definition_node();
  ((result->type)=type_node);
  ((result->name)=name);
  while (pstate_match_token_string(pstate, "["))
  {
    if ((!parse_balanced_construct(pstate)))
    {
      return pstate_propagate_error(pstate, saved_position);
    }
    if (((result->suffixes)==NULL))
    {
      ((result->suffixes)=make_value_array(1));
    }
    value_array_add((result->suffixes), ptr_to_value(pstate_get_result_node(pstate)));
  }
  if (pstate_expect_token_string(pstate, "="))
  {
    if ((parse_initializer(pstate)||parse_expression(pstate_ignore_error(pstate))))
    {
      ((result->value)=pstate_get_result_node(pstate));
    }
    else
    {
      return pstate_propagate_error(pstate, saved_position);
    }
  }
  else
  {
    pstate_ignore_error(pstate);
  }
  if ((!pstate_expect_token_string(pstate, ";")))
  {
    return pstate_propagate_error(pstate, saved_position);
  }
  return pstate_set_result_node(pstate, to_node(result));
}

/* i=489 j=0 */
pstatus_t parse_literal_node(pstate_t* pstate){
  uint64_t saved_position = (pstate->position);
  if (pstate_expect_token_type(pstate, TOKEN_TYPE_STRING_LITERAL))
  {
    literal_node_t* result = malloc_literal_node();
    ((result->tokens)=make_value_array(1));
    value_array_add((result->tokens), ptr_to_value(pstate_get_result_token(pstate)));
    while (pstate_expect_token_type(pstate, TOKEN_TYPE_STRING_LITERAL))
    {
      value_array_add((result->tokens), ptr_to_value(pstate_get_result_token(pstate)));
    }
    pstate_ignore_error(pstate);
    return pstate_set_result_node(pstate, to_node(result));
  }
  if (((pstate_expect_token_type(pstate_ignore_error(pstate), TOKEN_TYPE_INTEGER_LITERAL)||pstate_expect_token_type(pstate_ignore_error(pstate), TOKEN_TYPE_FLOAT_LITERAL))||pstate_expect_token_type(pstate_ignore_error(pstate), TOKEN_TYPE_CHARACTER_LITERAL)))
  {
    literal_node_t* result = malloc_literal_node();
    ((result->tokens)=make_value_array(1));
    value_array_add((result->tokens), ptr_to_value(pstate_get_result_token(pstate)));
    return pstate_set_result_node(pstate, to_node(result));
  }
  else
  {
    pstate_ignore_error(pstate);
  }
  if (pstate_match_token_string(pstate, "{"))
  {
    if ((!parse_balanced_construct(pstate)))
    {
      return pstate_propagate_error(pstate, saved_position);
    }
    literal_node_t* result = malloc_literal_node();
    ((result->initializer_node)=pstate_get_result_node(pstate));
    return pstate_set_result_node(pstate, to_node(result));
  }
  if (pstate_match_token_string(pstate, "compound_literal"))
  {
    pstate_advance(pstate);
    if (((!pstate_expect_token_string(pstate, "("))||(!parse_type_node(pstate))))
    {
      return pstate_propagate_error(pstate, saved_position);
    }
    parse_node_t* type_node = pstate_get_result_node(pstate);
    if ((!pstate_expect_token_string(pstate, ",")))
    {
      return pstate_propagate_error(pstate, saved_position);
    }
    if ((!parse_balanced_construct(pstate)))
    {
      return pstate_propagate_error(pstate, saved_position);
    }
    parse_node_t* initializer_node = pstate_get_result_node(pstate);
    if ((!pstate_expect_token_string(pstate, ")")))
    {
      return pstate_propagate_error(pstate, saved_position);
    }
    literal_node_t* result = malloc_literal_node();
    ((result->initializer_node)=initializer_node);
    ((result->initializer_type)=type_node);
    return pstate_set_result_node(pstate, to_node(result));
  }
  if (((((((pstate_expect_token_string(pstate, "NULL")||pstate_expect_token_string(pstate_ignore_error(pstate), "nullptr"))||pstate_expect_token_string(pstate_ignore_error(pstate), "true"))||pstate_expect_token_string(pstate_ignore_error(pstate), "false"))||pstate_expect_token_type(pstate_ignore_error(pstate), TOKEN_TYPE_INTEGER_LITERAL))||pstate_expect_token_type(pstate_ignore_error(pstate), TOKEN_TYPE_FLOAT_LITERAL))||pstate_expect_token_type(pstate_ignore_error(pstate), TOKEN_TYPE_CHARACTER_LITERAL)))
  {
    literal_node_t* result = malloc_literal_node();
    ((result->token)=pstate_get_result_token(pstate));
    return pstate_set_result_node(pstate, to_node(result));
  }
  return pstate_error(pstate, saved_position, PARSE_ERROR_NOT_LITERAL_NODE);
}

/* i=492 j=0 */
pstatus_t parse_balanced_construct(pstate_t* pstate){
  uint64_t saved_position = (pstate->position);
  int open_parens = 0;
  int open_brackets = 0;
  int open_braces = 0;
  do  {
    if (pstate_expect_token_string(pstate, "("))
    {
      (open_parens++);
    }
    else
    if (pstate_expect_token_string(pstate_ignore_error(pstate), "["))
    {
      (open_brackets++);
    }
    else
    if (pstate_expect_token_string(pstate_ignore_error(pstate), "{"))
    {
      (open_braces++);
    }
    else
    if (pstate_expect_token_string(pstate_ignore_error(pstate), ")"))
    {
      (open_parens--);
    }
    else
    if (pstate_expect_token_string(pstate_ignore_error(pstate), "]"))
    {
      (open_brackets--);
    }
    else
    if (pstate_expect_token_string(pstate_ignore_error(pstate), "}"))
    {
      (open_braces--);
    }
    else
    {
      pstate_advance(pstate_ignore_error(pstate));
    }
  }
while (((((open_parens+open_brackets)+open_braces)>0)&&((pstate->position)<((pstate->tokens)->length))));
  if (((pstate->position)==(saved_position+1)))
  {
    return pstate_error(pstate, saved_position, PARSE_ERROR_OPEN_BRACE_EXPECTED);
  }
  balanced_construct_node_t* result = malloc_balanced_construct_node();
  ((result->start_token)=token_at((pstate->tokens), saved_position));
  ((result->end_token)=pstate_peek(pstate, (-1)));
  return pstate_set_result_node(pstate, to_node(result));
}

/* i=493 j=0 */
printer_t* make_printer(buffer_t* buffer, int indent_width){
  printer_t* result = malloc_struct(printer_t);
  ((result->buffer)=buffer);
  ((result->indent_width)=indent_width);
  return result;
}

/* i=494 j=0 */
printer_t* append_string(printer_t* printer, char* string){
  buffer_append_string((printer->buffer), string);
  return printer;
}

/* i=495 j=0 */
printer_t* append_token(printer_t* printer, token_t* token){
  buffer_append_sub_buffer((printer->buffer), (token->start), (token->end), (token->buffer));
  return printer;
}

/* i=496 j=0 */
printer_t* printer_newline(printer_t* printer){
  buffer_append_byte((printer->buffer), '\n');
  return printer;
}

/* i=497 j=0 */
printer_t* printer_space(printer_t* printer){
  buffer_append_byte((printer->buffer), ' ');
  return printer;
}

/* i=498 j=0 */
printer_t* printer_indent(printer_t* printer){
  buffer_append_repeated_byte((printer->buffer), ' ', ((printer->indent_width)*(printer->indent_level)));
  return printer;
}

/* i=499 j=0 */
printer_t* printer_increase_indent(printer_t* printer){
  ((printer->indent_level)++);
  return printer;
}

/* i=500 j=0 */
printer_t* printer_decrease_indent(printer_t* printer){
  ((printer->indent_level)--);
  return printer;
}

/* i=501 j=0 */
void do_print_tokens(value_array_t* tokens, char* message){
  if (FLAG_print_tokens_show_tokens)
  {
    buffer_t* buffer = make_buffer(((tokens->length)*60));
    for (
      int j = 0;
      (j<(tokens->length));
      (j++))
    {
      token_t* token = token_at(tokens, j);
      (buffer=append_token_debug_string(buffer, (*token)));
      (buffer=buffer_append_string(buffer, "\n"));
    }
    fprintf(stdout, "** %s **\n%s\n", message, buffer_to_c_string(buffer));
  }
}

/* i=502 j=0 */
void print_tokens(void){
  log_info("print_tokens()");
  value_array_t* files = read_files(FLAG_files);
  for (
    int i = 0;
    (i<(FLAG_files->length));
    (i++))
  {
    file_t* file = value_array_get_ptr(files, i, typeof(file_t*));
    fprintf(stdout, "====================================================\n");
    fprintf(stdout, "====> Processing %s\n", (file->file_name));
    fprintf(stdout, "====================================================\n");
    tokenizer_result_t tokenizer_result = tokenize((file->data));
    if ((tokenizer_result.tokenizer_error_code))
    {
      log_warn("Tokenizer error: \"%s\"::%d -- %d", (value_array_get(FLAG_files, i).str), (tokenizer_result.tokenizer_error_position), (tokenizer_result.tokenizer_error_code));
      continue;
    }
    value_array_t* tokens = (tokenizer_result.tokens);
    convert_nullptr_to_null(tokens);
    if (FLAG_print_tokens_show_appended_tokens)
    {
      buffer_t* appended_tokens = make_buffer(1);
      debug_append_tokens(appended_tokens, tokens);
      fprintf(stdout, "%s", buffer_to_c_string(appended_tokens));
    }
    do_print_tokens(tokens, "before xform tokens");
    (tokens=transform_tokens(tokens, ((token_transformer_options_t) {
                .keep_whitespace = FLAG_print_tokens_include_whitespace,
                .keep_comments = FLAG_print_tokens_include_comments,
                .keep_javadoc_comments = FLAG_print_tokens_include_comments,
                .keep_c_preprocessor_lines = false,
            })));
    do_print_tokens(tokens, "after xform tokens");
    if (FLAG_print_tokens_parse_and_print)
    {
      pstate_t pstate = ((pstate_t) {.tokens = tokens,
                     .use_statement_parser = FLAG_use_statement_parser});
      ;
      if ((!parse_declarations((&pstate))))
      {
        (((pstate.error).file_name)=(file->file_name));
        buffer_t* buffer = make_buffer(1);
        (buffer=buffer_append_human_readable_error(buffer, (&(pstate.error))));
        log_fatal("%s", buffer_to_c_string(buffer));
        fatal_error(ERROR_ILLEGAL_INPUT);
      }
      else
      {
        buffer_t* buffer = make_buffer(1024);
        buffer_append_dbg_parse_node(make_cdl_printer(buffer), pstate_get_result_node((&pstate)));
        fprintf(stdout, "** Parse Nodes %s **\n%s\n", (file->file_name), buffer_to_c_string(buffer));
      }
    }
  }
}

/* i=503 j=0 */
void extract_command(char* command){
  log_info("extract_prototypes(%s)", command);
  buffer_t* prototype_outputs = make_buffer((16*1024));
  buffer_t* enum_outputs = make_buffer((16*1024));
  value_array_t* files = read_files(FLAG_files);
  for (
    int i = 0;
    (i<(FLAG_files->length));
    (i++))
  {
    if (FLAG_unique_prototype_header_files)
    {
      buffer_clear(prototype_outputs);
    }
    file_t* file = value_array_get_ptr(files, i, typeof(file_t*));
    (prototype_outputs=buffer_printf(prototype_outputs, "/* Automatically extracted prototypes from %s */\n\n", (file->file_name)));
    fprintf(stdout, "====================================================\n");
    fprintf(stdout, "====> Processing %s\n", (file->file_name));
    fprintf(stdout, "====================================================\n");
    tokenizer_result_t tokenizer_result = tokenize((file->data));
    if ((tokenizer_result.tokenizer_error_code))
    {
      log_warn("Tokenizer error: \"%s\"::%d -- %d", (value_array_get(FLAG_files, i).str), (tokenizer_result.tokenizer_error_position), (tokenizer_result.tokenizer_error_code));
      fatal_error(ERROR_ILLEGAL_INPUT);
    }
    value_array_t* tokens = (tokenizer_result.tokens);
    symbol_table_t* symbol_table = make_symbol_table();
    handle_c_preprocessor_directives(((c_preprocess_options_t) {
                             .keep_system_includes = false,
                             .keep_user_includes = false,
                         }), symbol_table, tokens);
    (tokens=transform_tokens(tokens, ((token_transformer_options_t) {
                                     .keep_whitespace = false,
                                     .keep_comments = false,
                                     .keep_javadoc_comments = false,
                                     .keep_c_preprocessor_lines = false,
                                 })));
    pstate_t pstate = ((pstate_t) {.tokens = tokens, .use_statement_parser = FLAG_use_statement_parser});
    if ((!parse_declarations((&pstate))))
    {
      (((pstate.error).file_name)=(file->file_name));
      buffer_t* buffer = make_buffer(1);
      (buffer=buffer_append_human_readable_error(buffer, (&(pstate.error))));
      log_fatal("%s", buffer_to_c_string(buffer));
      fatal_error(ERROR_ILLEGAL_INPUT);
    }
    declarations_node_t* root = to_declarations_node(pstate_get_result_node((&pstate)));
    if (string_equal("extract-prototypes", command))
    {
      (prototype_outputs=extract_prototypes_process_declarations(prototype_outputs, root));
    }
    else
    if (string_equal("extract-enums", command))
    {
      (enum_outputs=extract_enums_process_declarations(enum_outputs, root));
    }
    if (FLAG_unique_prototype_header_files)
    {
      if (string_equal("extract-prototypes", command))
      {
        char* prototype_outputs_file_name = string_printf("%s.generated.h", (file->file_name));
        buffer_write_file(prototype_outputs, prototype_outputs_file_name);
        free_bytes(prototype_outputs_file_name);
      }
      else
      if (string_equal("extract-enums", command))
      {
        char* enum_outputs_file_name = string_printf("%s.enum-generated.h", (file->file_name));
        buffer_write_file(enum_outputs, enum_outputs_file_name);
        free_bytes(enum_outputs_file_name);
      }
    }
  }
  if ((!FLAG_unique_prototype_header_files))
  {
    fprintf(stdout, "%s", buffer_to_c_string(prototype_outputs));
  }
}

/* i=504 j=0 */
void configure_flags(void){
  flag_program_name("omni-c");
  flag_description("omni-c is a transpiler for the omni-c language as well as a code " "generation tool for ISO C.");
  flag_boolean("--print-command-line", (&FLAG_print_command_line));
  flag_boolean("--use-statement-parser", (&FLAG_use_statement_parser));
  configure_print_tokens_command();
  configure_extract_prototypes_command();
  configure_extract_enums_command();
  configure_generate_c_output_file();
  configure_parse_expression();
  configure_parse_statement();
}

/* i=505 j=0 */
void configure_parse_expression(void){
  flag_command("parse-expression", (&FLAG_command));
  flag_string("--expression", (&FLAG_expression));
  flag_boolean("--to-c", (&FLAG_to_c));
}

/* i=506 j=0 */
void configure_parse_statement(void){
  flag_command("parse-statement", (&FLAG_command));
  flag_string("--statement", (&FLAG_statement));
}

/* i=507 j=0 */
void configure_print_tokens_command(void){
  flag_command("print-tokens", (&FLAG_command));
  flag_boolean("--show-tokens", (&FLAG_print_tokens_show_tokens));
  flag_boolean("--include-whitespace", (&FLAG_print_tokens_include_whitespace));
  flag_boolean("--include-comments", (&FLAG_print_tokens_include_comments));
  flag_boolean("--parse-and-print", (&FLAG_print_tokens_parse_and_print));
  flag_boolean("--show-appended-tokens", (&FLAG_print_tokens_show_appended_tokens));
  flag_file_args((&FLAG_files));
}

/* i=508 j=0 */
void configure_extract_enums_command(void){
  flag_command("extract-enums", (&FLAG_command));
  flag_string("--output-file", (&FLAG_ouput_file));
  flag_boolean("--unique-prototype-header-files", (&FLAG_unique_prototype_header_files));
  flag_file_args((&FLAG_files));
}

/* i=509 j=0 */
void configure_extract_prototypes_command(void){
  flag_command("extract-prototypes", (&FLAG_command));
  flag_string("--output-file", (&FLAG_ouput_file));
  flag_boolean("--unique-prototype-header-files", (&FLAG_unique_prototype_header_files));
  flag_file_args((&FLAG_files));
}

/* i=510 j=0 */
void configure_generate_c_output_file(void){
  flag_command("generate-header-file", (&FLAG_command));
  flag_string("--output-file", (&FLAG_ouput_file));
  flag_boolean("--generate-enum-convertors", (&FLAG_generate_enum_convertors));
  flag_boolean("--dump-symbol-table", (&FLAG_dump_symbol_table));
  flag_boolean("--use-statement-parser", (&FLAG_use_statement_parser));
  flag_boolean("--omit-c-armyknife-include", (&FLAG_omit_c_armyknife_include));
  flag_file_args((&FLAG_files));
  flag_command("generate-library", (&FLAG_command));
  flag_string("--output-file", (&FLAG_ouput_file));
  flag_boolean("--generate-enum-convertors", (&FLAG_generate_enum_convertors));
  flag_boolean("--dump-symbol-table", (&FLAG_dump_symbol_table));
  flag_boolean("--use-statement-parser", (&FLAG_use_statement_parser));
  flag_boolean("--omit-c-armyknife-include", (&FLAG_omit_c_armyknife_include));
  flag_file_args((&FLAG_files));
}

/* i=511 j=0 */
boolean_t is_inlined_function(function_node_t* node){
  return (token_matches((node->storage_class_specifier), "static")&&token_list_contains((node->function_specifiers), "inline"));
}

/* i=512 j=0 */
void dump_symbol_table(char* phase_name, symbol_table_t* symbol_table){
  if (FLAG_dump_symbol_table)
  {
    fprintf(stderr, "==================================================================" "====\n");
    fprintf(stderr, "%s\n", phase_name);
    fprintf(stderr, "==================================================================" "====\n\n");
    buffer_t* buffer = make_buffer(128);
    (buffer=symbol_table_stats(buffer, symbol_table));
    buffer_append_dgb_symbol_table(buffer, symbol_table);
    fprintf(stderr, "%s", buffer_to_c_string(buffer));
  }
}

/* i=513 j=1 */
buffer_t* get_reflection_header_buffer(void){
  uint8_t reflection_header[] = {
    0x23, 0x69, 0x66, 0x6E, 0x64, 0x65, 0x66, 0x20,
    0x5F, 0x52, 0x45, 0x46, 0x4C, 0x45, 0x43, 0x54,
    0x49, 0x4F, 0x4E, 0x5F, 0x48, 0x5F, 0x0A, 0x23,
    0x64, 0x65, 0x66, 0x69, 0x6E, 0x65, 0x20, 0x5F,
    0x52, 0x45, 0x46, 0x4C, 0x45, 0x43, 0x54, 0x49,
    0x4F, 0x4E, 0x5F, 0x48, 0x5F, 0x0A, 0x0A, 0x23,
    0x69, 0x6E, 0x63, 0x6C, 0x75, 0x64, 0x65, 0x20,
    0x3C, 0x73, 0x74, 0x72, 0x69, 0x6E, 0x67, 0x2E,
    0x68, 0x3E, 0x0A, 0x0A, 0x2F, 0x2A, 0x20, 0x3D,
    0x3D, 0x3D, 0x3D, 0x3D, 0x3D, 0x3D, 0x3D, 0x3D,
    0x3D, 0x3D, 0x3D, 0x3D, 0x3D, 0x3D, 0x3D, 0x3D,
    0x3D, 0x3D, 0x3D, 0x3D, 0x3D, 0x3D, 0x3D, 0x3D,
    0x3D, 0x3D, 0x3D, 0x3D, 0x3D, 0x3D, 0x3D, 0x3D,
    0x3D, 0x3D, 0x3D, 0x3D, 0x3D, 0x3D, 0x3D, 0x3D,
    0x3D, 0x3D, 0x3D, 0x3D, 0x3D, 0x3D, 0x3D, 0x3D,
    0x3D, 0x3D, 0x3D, 0x3D, 0x3D, 0x3D, 0x3D, 0x3D,
    0x3D, 0x3D, 0x3D, 0x3D, 0x3D, 0x3D, 0x3D, 0x3D,
    0x3D, 0x3D, 0x3D, 0x3D, 0x3D, 0x20, 0x2A, 0x2F,
    0x0A, 0x2F, 0x2F, 0x20, 0x54, 0x68, 0x65, 0x73,
    0x65, 0x20, 0x61, 0x72, 0x65, 0x20, 0x74, 0x68,
    0x65, 0x20, 0x72, 0x65, 0x66, 0x6C, 0x65, 0x63,
    0x74, 0x69, 0x6F, 0x6E, 0x20, 0x41, 0x50, 0x49,
    0x20, 0x64, 0x61, 0x74, 0x61, 0x20, 0x73, 0x74,
    0x72, 0x75, 0x63, 0x74, 0x75, 0x72, 0x65, 0x73,
    0x20, 0x66, 0x6F, 0x72, 0x20, 0x61, 0x20, 0x70,
    0x72, 0x6F, 0x67, 0x72, 0x61, 0x6D, 0x20, 0x63,
    0x6F, 0x6D, 0x70, 0x69, 0x6C, 0x65, 0x64, 0x0A,
    0x2F, 0x2F, 0x20, 0x77, 0x69, 0x74, 0x68, 0x20,
    0x4F, 0x6D, 0x6E, 0x69, 0x20, 0x43, 0x2E, 0x20,
    0x57, 0x65, 0x20, 0x75, 0x73, 0x65, 0x20, 0x6C,
    0x69, 0x6E, 0x6B, 0x65, 0x64, 0x20, 0x6C, 0x69,
    0x73, 0x74, 0x73, 0x20, 0x69, 0x6E, 0x73, 0x74,
    0x65, 0x61, 0x64, 0x20, 0x6F, 0x66, 0x20, 0x76,
    0x61, 0x6C, 0x75, 0x65, 0x5F, 0x61, 0x72, 0x72,
    0x61, 0x79, 0x5F, 0x74, 0x20, 0x74, 0x6F, 0x20,
    0x6B, 0x65, 0x65, 0x70, 0x0A, 0x2F, 0x2F, 0x20,
    0x63, 0x6F, 0x6D, 0x70, 0x69, 0x6C, 0x65, 0x64,
    0x20, 0x70, 0x72, 0x6F, 0x67, 0x72, 0x61, 0x6D,
    0x73, 0x20, 0x69, 0x6E, 0x64, 0x65, 0x70, 0x65,
    0x6E, 0x64, 0x65, 0x6E, 0x74, 0x20, 0x6F, 0x66,
    0x20, 0x61, 0x6E, 0x79, 0x20, 0x70, 0x61, 0x72,
    0x74, 0x69, 0x63, 0x75, 0x6C, 0x61, 0x72, 0x20,
    0x6C, 0x69, 0x62, 0x72, 0x61, 0x72, 0x79, 0x0A,
    0x2F, 0x2F, 0x20, 0x64, 0x61, 0x74, 0x61, 0x2D,
    0x73, 0x74, 0x72, 0x75, 0x63, 0x74, 0x75, 0x72,
    0x65, 0x73, 0x2E, 0x0A, 0x2F, 0x2A, 0x20, 0x3D,
    0x3D, 0x3D, 0x3D, 0x3D, 0x3D, 0x3D, 0x3D, 0x3D,
    0x3D, 0x3D, 0x3D, 0x3D, 0x3D, 0x3D, 0x3D, 0x3D,
    0x3D, 0x3D, 0x3D, 0x3D, 0x3D, 0x3D, 0x3D, 0x3D,
    0x3D, 0x3D, 0x3D, 0x3D, 0x3D, 0x3D, 0x3D, 0x3D,
    0x3D, 0x3D, 0x3D, 0x3D, 0x3D, 0x3D, 0x3D, 0x3D,
    0x3D, 0x3D, 0x3D, 0x3D, 0x3D, 0x3D, 0x3D, 0x3D,
    0x3D, 0x3D, 0x3D, 0x3D, 0x3D, 0x3D, 0x3D, 0x3D,
    0x3D, 0x3D, 0x3D, 0x3D, 0x3D, 0x3D, 0x3D, 0x3D,
    0x3D, 0x3D, 0x3D, 0x3D, 0x3D, 0x20, 0x2A, 0x2F,
    0x0A, 0x0A, 0x2F, 0x2F, 0x20, 0x2D, 0x2D, 0x2D,
    0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D,
    0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D,
    0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D,
    0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D,
    0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D,
    0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D,
    0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D,
    0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D,
    0x2D, 0x2D, 0x2D, 0x0A, 0x2F, 0x2F, 0x20, 0x45,
    0x6E, 0x75, 0x6D, 0x65, 0x72, 0x61, 0x74, 0x69,
    0x6F, 0x6E, 0x73, 0x0A, 0x2F, 0x2F, 0x20, 0x2D,
    0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D,
    0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D,
    0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D,
    0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D,
    0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D,
    0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D,
    0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D,
    0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D,
    0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x0A, 0x0A, 0x74,
    0x79, 0x70, 0x65, 0x64, 0x65, 0x66, 0x20, 0x73,
    0x74, 0x72, 0x75, 0x63, 0x74, 0x20, 0x65, 0x6E,
    0x75, 0x6D, 0x5F, 0x65, 0x6C, 0x65, 0x6D, 0x65,
    0x6E, 0x74, 0x5F, 0x6D, 0x65, 0x74, 0x61, 0x64,
    0x61, 0x74, 0x61, 0x5F, 0x53, 0x20, 0x7B, 0x0A,
    0x20, 0x20, 0x73, 0x74, 0x72, 0x75, 0x63, 0x74,
    0x20, 0x65, 0x6E, 0x75, 0x6D, 0x5F, 0x65, 0x6C,
    0x65, 0x6D, 0x65, 0x6E, 0x74, 0x5F, 0x6D, 0x65,
    0x74, 0x61, 0x64, 0x61, 0x74, 0x61, 0x5F, 0x53,
    0x2A, 0x20, 0x6E, 0x65, 0x78, 0x74, 0x3B, 0x0A,
    0x20, 0x20, 0x63, 0x68, 0x61, 0x72, 0x2A, 0x20,
    0x6E, 0x61, 0x6D, 0x65, 0x3B, 0x0A, 0x20, 0x20,
    0x6C, 0x6F, 0x6E, 0x67, 0x20, 0x76, 0x61, 0x6C,
    0x75, 0x65, 0x3B, 0x0A, 0x7D, 0x20, 0x65, 0x6E,
    0x75, 0x6D, 0x5F, 0x65, 0x6C, 0x65, 0x6D, 0x65,
    0x6E, 0x74, 0x5F, 0x6D, 0x65, 0x74, 0x61, 0x64,
    0x61, 0x74, 0x61, 0x5F, 0x74, 0x3B, 0x0A, 0x0A,
    0x2F, 0x2A, 0x2A, 0x0A, 0x20, 0x2A, 0x20, 0x40,
    0x73, 0x74, 0x72, 0x75, 0x63, 0x74, 0x75, 0x72,
    0x65, 0x20, 0x65, 0x6E, 0x75, 0x6D, 0x5F, 0x6D,
    0x65, 0x74, 0x61, 0x64, 0x61, 0x74, 0x61, 0x5F,
    0x74, 0x0A, 0x20, 0x2A, 0x0A, 0x20, 0x2A, 0x20,
    0x54, 0x68, 0x65, 0x20, 0x72, 0x75, 0x6E, 0x74,
    0x69, 0x6D, 0x65, 0x20, 0x6D, 0x65, 0x74, 0x61,
    0x64, 0x61, 0x74, 0x61, 0x20, 0x66, 0x6F, 0x72,
    0x20, 0x61, 0x20, 0x72, 0x65, 0x66, 0x6C, 0x65,
    0x63, 0x74, 0x65, 0x64, 0x20, 0x65, 0x6E, 0x75,
    0x6D, 0x65, 0x72, 0x61, 0x74, 0x69, 0x6F, 0x6E,
    0x2E, 0x0A, 0x20, 0x2A, 0x2F, 0x0A, 0x74, 0x79,
    0x70, 0x65, 0x64, 0x65, 0x66, 0x20, 0x73, 0x74,
    0x72, 0x75, 0x63, 0x74, 0x20, 0x7B, 0x0A, 0x20,
    0x20, 0x63, 0x68, 0x61, 0x72, 0x2A, 0x20, 0x6E,
    0x61, 0x6D, 0x65, 0x3B, 0x0A, 0x20, 0x20, 0x65,
    0x6E, 0x75, 0x6D, 0x5F, 0x65, 0x6C, 0x65, 0x6D,
    0x65, 0x6E, 0x74, 0x5F, 0x6D, 0x65, 0x74, 0x61,
    0x64, 0x61, 0x74, 0x61, 0x5F, 0x74, 0x2A, 0x20,
    0x65, 0x6C, 0x65, 0x6D, 0x65, 0x6E, 0x74, 0x73,
    0x3B, 0x0A, 0x7D, 0x20, 0x65, 0x6E, 0x75, 0x6D,
    0x5F, 0x6D, 0x65, 0x74, 0x61, 0x64, 0x61, 0x74,
    0x61, 0x5F, 0x74, 0x3B, 0x0A, 0x0A, 0x2F, 0x2F,
    0x20, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D,
    0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D,
    0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D,
    0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D,
    0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D,
    0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D,
    0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D,
    0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D,
    0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x0A,
    0x2F, 0x2F, 0x20, 0x53, 0x74, 0x72, 0x75, 0x63,
    0x74, 0x75, 0x72, 0x65, 0x73, 0x0A, 0x2F, 0x2F,
    0x20, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D,
    0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D,
    0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D,
    0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D,
    0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D,
    0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D,
    0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D,
    0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D,
    0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x0A,
    0x0A, 0x74, 0x79, 0x70, 0x65, 0x64, 0x65, 0x66,
    0x20, 0x73, 0x74, 0x72, 0x75, 0x63, 0x74, 0x20,
    0x66, 0x69, 0x65, 0x6C, 0x64, 0x5F, 0x6D, 0x65,
    0x74, 0x61, 0x64, 0x61, 0x74, 0x61, 0x5F, 0x53,
    0x20, 0x7B, 0x0A, 0x20, 0x20, 0x73, 0x74, 0x72,
    0x75, 0x63, 0x74, 0x20, 0x66, 0x69, 0x65, 0x6C,
    0x64, 0x5F, 0x6D, 0x65, 0x74, 0x61, 0x64, 0x61,
    0x74, 0x61, 0x5F, 0x53, 0x2A, 0x20, 0x6E, 0x65,
    0x78, 0x74, 0x3B, 0x0A, 0x20, 0x20, 0x63, 0x68,
    0x61, 0x72, 0x2A, 0x20, 0x6E, 0x61, 0x6D, 0x65,
    0x3B, 0x0A, 0x20, 0x20, 0x63, 0x68, 0x61, 0x72,
    0x2A, 0x20, 0x74, 0x79, 0x70, 0x65, 0x5F, 0x6E,
    0x61, 0x6D, 0x65, 0x5F, 0x73, 0x74, 0x72, 0x69,
    0x6E, 0x67, 0x3B, 0x0A, 0x20, 0x20, 0x69, 0x6E,
    0x74, 0x20, 0x73, 0x74, 0x61, 0x72, 0x74, 0x5F,
    0x6F, 0x66, 0x66, 0x73, 0x65, 0x74, 0x3B, 0x0A,
    0x7D, 0x20, 0x66, 0x69, 0x65, 0x6C, 0x64, 0x5F,
    0x6D, 0x65, 0x74, 0x61, 0x64, 0x61, 0x74, 0x61,
    0x5F, 0x74, 0x3B, 0x0A, 0x0A, 0x2F, 0x2A, 0x2A,
    0x0A, 0x20, 0x2A, 0x20, 0x40, 0x73, 0x74, 0x72,
    0x75, 0x63, 0x74, 0x75, 0x72, 0x65, 0x20, 0x73,
    0x74, 0x72, 0x75, 0x63, 0x74, 0x75, 0x72, 0x65,
    0x5F, 0x6D, 0x65, 0x74, 0x61, 0x64, 0x61, 0x74,
    0x61, 0x5F, 0x74, 0x0A, 0x20, 0x2A, 0x0A, 0x20,
    0x2A, 0x20, 0x54, 0x68, 0x65, 0x20, 0x72, 0x75,
    0x6E, 0x74, 0x69, 0x6D, 0x65, 0x20, 0x6D, 0x65,
    0x74, 0x61, 0x64, 0x61, 0x74, 0x61, 0x20, 0x66,
    0x6F, 0x72, 0x20, 0x61, 0x20, 0x72, 0x65, 0x66,
    0x6C, 0x65, 0x63, 0x74, 0x65, 0x64, 0x20, 0x73,
    0x74, 0x72, 0x75, 0x63, 0x74, 0x75, 0x72, 0x65,
    0x2E, 0x0A, 0x20, 0x2A, 0x2F, 0x0A, 0x74, 0x79,
    0x70, 0x65, 0x64, 0x65, 0x66, 0x20, 0x73, 0x74,
    0x72, 0x75, 0x63, 0x74, 0x20, 0x7B, 0x0A, 0x20,
    0x20, 0x63, 0x68, 0x61, 0x72, 0x2A, 0x20, 0x6E,
    0x61, 0x6D, 0x65, 0x3B, 0x0A, 0x20, 0x20, 0x69,
    0x6E, 0x74, 0x20, 0x73, 0x69, 0x7A, 0x65, 0x3B,
    0x0A, 0x20, 0x20, 0x69, 0x6E, 0x74, 0x20, 0x61,
    0x6C, 0x69, 0x67, 0x6E, 0x6D, 0x65, 0x6E, 0x74,
    0x3B, 0x0A, 0x20, 0x20, 0x66, 0x69, 0x65, 0x6C,
    0x64, 0x5F, 0x6D, 0x65, 0x74, 0x61, 0x64, 0x61,
    0x74, 0x61, 0x5F, 0x74, 0x2A, 0x20, 0x66, 0x69,
    0x65, 0x6C, 0x64, 0x73, 0x3B, 0x0A, 0x7D, 0x20,
    0x73, 0x74, 0x72, 0x75, 0x63, 0x74, 0x75, 0x72,
    0x65, 0x5F, 0x6D, 0x65, 0x74, 0x61, 0x64, 0x61,
    0x74, 0x61, 0x5F, 0x74, 0x3B, 0x0A, 0x0A, 0x2F,
    0x2F, 0x20, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D,
    0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D,
    0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D,
    0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D,
    0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D,
    0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D,
    0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D,
    0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D,
    0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D,
    0x0A, 0x2F, 0x2F, 0x20, 0x55, 0x6E, 0x69, 0x6F,
    0x6E, 0x73, 0x20, 0x0A, 0x2F, 0x2F, 0x20, 0x2D,
    0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D,
    0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D,
    0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D,
    0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D,
    0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D,
    0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D,
    0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D,
    0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D,
    0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x0A, 0x0A, 0x2F,
    0x2A, 0x0A, 0x20, 0x2A, 0x20, 0x55, 0x6E, 0x69,
    0x6F, 0x6E, 0x73, 0x20, 0x63, 0x6F, 0x75, 0x6C,
    0x64, 0x20, 0x62, 0x65, 0x20, 0x74, 0x72, 0x65,
    0x61, 0x74, 0x65, 0x64, 0x20, 0x65, 0x78, 0x61,
    0x63, 0x74, 0x6C, 0x79, 0x20, 0x6C, 0x69, 0x6B,
    0x65, 0x20, 0x73, 0x74, 0x72, 0x75, 0x63, 0x74,
    0x75, 0x72, 0x65, 0x73, 0x20, 0x68, 0x6F, 0x77,
    0x65, 0x76, 0x65, 0x72, 0x20, 0x74, 0x68, 0x65,
    0x72, 0x65, 0x20, 0x69, 0x73, 0x20, 0x6E, 0x6F,
    0x0A, 0x20, 0x2A, 0x20, 0x75, 0x6E, 0x69, 0x66,
    0x6F, 0x72, 0x6D, 0x20, 0x77, 0x61, 0x79, 0x20,
    0x74, 0x6F, 0x20, 0x74, 0x61, 0x67, 0x20, 0x61,
    0x20, 0x75, 0x6E, 0x69, 0x6F, 0x6E, 0x20, 0x73,
    0x6F, 0x20, 0x6D, 0x61, 0x6B, 0x69, 0x6E, 0x67,
    0x20, 0x75, 0x73, 0x65, 0x20, 0x6F, 0x66, 0x20,
    0x74, 0x68, 0x65, 0x20, 0x6D, 0x65, 0x74, 0x61,
    0x64, 0x61, 0x74, 0x61, 0x20, 0x69, 0x73, 0x20,
    0x61, 0x20, 0x62, 0x69, 0x74, 0x0A, 0x20, 0x2A,
    0x20, 0x70, 0x72, 0x6F, 0x62, 0x6C, 0x65, 0x6D,
    0x61, 0x74, 0x69, 0x63, 0x2E, 0x20, 0x46, 0x6F,
    0x72, 0x20, 0x74, 0x68, 0x61, 0x74, 0x20, 0x72,
    0x65, 0x61, 0x73, 0x6F, 0x6E, 0x20, 0x28, 0x61,
    0x6E, 0x64, 0x20, 0x62, 0x65, 0x63, 0x61, 0x75,
    0x73, 0x65, 0x20, 0x6F, 0x6D, 0x6E, 0x69, 0x2D,
    0x63, 0x20, 0x64, 0x6F, 0x65, 0x73, 0x6E, 0x27,
    0x74, 0x20, 0x6E, 0x65, 0x65, 0x64, 0x20, 0x74,
    0x68, 0x65, 0x6D, 0x0A, 0x20, 0x2A, 0x20, 0x79,
    0x65, 0x74, 0x29, 0x2C, 0x20, 0x77, 0x65, 0x20,
    0x61, 0x72, 0x65, 0x20, 0x73, 0x6B, 0x69, 0x70,
    0x70, 0x69, 0x6E, 0x67, 0x20, 0x74, 0x68, 0x65,
    0x6D, 0x20, 0x66, 0x6F, 0x72, 0x20, 0x6E, 0x6F,
    0x77, 0x2E, 0x0A, 0x20, 0x2A, 0x2F, 0x0A, 0x0A,
    0x2F, 0x2F, 0x20, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D,
    0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D,
    0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D,
    0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D,
    0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D,
    0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D,
    0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D,
    0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D,
    0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D,
    0x2D, 0x0A, 0x2F, 0x2F, 0x20, 0x46, 0x75, 0x6E,
    0x63, 0x74, 0x69, 0x6F, 0x6E, 0x73, 0x0A, 0x2F,
    0x2F, 0x20, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D,
    0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D,
    0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D,
    0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D,
    0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D,
    0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D,
    0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D,
    0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D,
    0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D,
    0x0A, 0x0A, 0x74, 0x79, 0x70, 0x65, 0x64, 0x65,
    0x66, 0x20, 0x73, 0x74, 0x72, 0x75, 0x63, 0x74,
    0x20, 0x66, 0x75, 0x6E, 0x63, 0x74, 0x69, 0x6F,
    0x6E, 0x5F, 0x61, 0x72, 0x67, 0x5F, 0x6D, 0x65,
    0x74, 0x61, 0x64, 0x61, 0x74, 0x61, 0x5F, 0x53,
    0x20, 0x7B, 0x0A, 0x20, 0x20, 0x73, 0x74, 0x72,
    0x75, 0x63, 0x74, 0x20, 0x66, 0x75, 0x6E, 0x63,
    0x74, 0x69, 0x6F, 0x6E, 0x5F, 0x61, 0x72, 0x67,
    0x5F, 0x6D, 0x65, 0x74, 0x61, 0x64, 0x61, 0x74,
    0x61, 0x5F, 0x74, 0x2A, 0x20, 0x6E, 0x65, 0x78,
    0x74, 0x3B, 0x0A, 0x20, 0x20, 0x63, 0x68, 0x61,
    0x72, 0x2A, 0x20, 0x6E, 0x61, 0x6D, 0x65, 0x3B,
    0x0A, 0x20, 0x20, 0x63, 0x68, 0x61, 0x72, 0x2A,
    0x20, 0x74, 0x79, 0x70, 0x65, 0x5F, 0x73, 0x74,
    0x72, 0x69, 0x6E, 0x67, 0x3B, 0x0A, 0x7D, 0x20,
    0x66, 0x75, 0x6E, 0x63, 0x74, 0x69, 0x6F, 0x6E,
    0x5F, 0x61, 0x72, 0x67, 0x5F, 0x6D, 0x65, 0x74,
    0x61, 0x64, 0x61, 0x74, 0x61, 0x5F, 0x74, 0x3B,
    0x0A, 0x0A, 0x2F, 0x2A, 0x2A, 0x0A, 0x20, 0x2A,
    0x20, 0x40, 0x73, 0x74, 0x72, 0x75, 0x63, 0x74,
    0x75, 0x72, 0x65, 0x20, 0x66, 0x75, 0x6E, 0x63,
    0x74, 0x69, 0x6F, 0x6E, 0x5F, 0x6D, 0x65, 0x74,
    0x61, 0x64, 0x61, 0x74, 0x61, 0x5F, 0x74, 0x0A,
    0x20, 0x2A, 0x0A, 0x20, 0x2A, 0x20, 0x54, 0x68,
    0x65, 0x20, 0x72, 0x75, 0x6E, 0x74, 0x69, 0x6D,
    0x65, 0x20, 0x6D, 0x65, 0x74, 0x61, 0x64, 0x61,
    0x74, 0x61, 0x20, 0x66, 0x6F, 0x72, 0x20, 0x61,
    0x20, 0x72, 0x65, 0x66, 0x6C, 0x65, 0x63, 0x74,
    0x65, 0x64, 0x20, 0x73, 0x74, 0x72, 0x75, 0x63,
    0x74, 0x75, 0x72, 0x65, 0x2E, 0x0A, 0x20, 0x2A,
    0x2F, 0x0A, 0x74, 0x79, 0x70, 0x65, 0x64, 0x65,
    0x66, 0x20, 0x73, 0x74, 0x72, 0x75, 0x63, 0x74,
    0x20, 0x7B, 0x0A, 0x20, 0x20, 0x63, 0x68, 0x61,
    0x72, 0x2A, 0x20, 0x6E, 0x61, 0x6D, 0x65, 0x3B,
    0x0A, 0x20, 0x20, 0x66, 0x75, 0x6E, 0x63, 0x74,
    0x69, 0x6F, 0x6E, 0x5F, 0x61, 0x72, 0x67, 0x5F,
    0x6D, 0x65, 0x74, 0x61, 0x64, 0x61, 0x74, 0x61,
    0x5F, 0x74, 0x2A, 0x20, 0x61, 0x72, 0x67, 0x75,
    0x6D, 0x65, 0x6E, 0x74, 0x73, 0x3B, 0x0A, 0x7D,
    0x20, 0x66, 0x75, 0x6E, 0x63, 0x74, 0x69, 0x6F,
    0x6E, 0x5F, 0x6D, 0x65, 0x74, 0x61, 0x64, 0x61,
    0x74, 0x61, 0x5F, 0x74, 0x3B, 0x0A, 0x0A, 0x23,
    0x65, 0x6E, 0x64, 0x69, 0x66, 0x20, 0x2F, 0x2A,
    0x20, 0x5F, 0x52, 0x45, 0x46, 0x4C, 0x45, 0x43,
    0x54, 0x49, 0x4F, 0x4E, 0x5F, 0x48, 0x5F, 0x20,
    0x2A, 0x2F, 0x0A,
    };
  buffer_t* result = make_buffer(2387);
  for (
    int i = 0;
    (i<2387);
    (i++))
  {
    buffer_append_byte(result, (reflection_header[i]));
  }
  return result;
}

/* i=514 j=0 */
char* include_node_to_string(cpp_include_node_t* node){
  buffer_t* buffer = make_buffer(32);
  printer_t* printer = make_printer(buffer, 2);
  append_cpp_include_node(printer, node);
  char* include_statement = buffer_to_c_string(buffer);
  return include_statement;
}

/* i=515 j=0 */
void generate_c_output_file(boolean_t is_library){
  boolean_t is_header_file = (!is_library);
  symbol_table_t* symbol_table = make_symbol_table();
  parse_and_add_top_level_definitions(symbol_table, FLAG_files, FLAG_use_statement_parser);
  dump_symbol_table("initial parse", symbol_table);
  if (FLAG_generate_enum_convertors)
  {
    srcgen_enum_to_string_converters(symbol_table);
    dump_symbol_table("enum to string generation", symbol_table);
  }
  split_structure_typedefs(symbol_table);
  dump_symbol_table("split structures", symbol_table);
  reorder_symbol_table_typedefs(symbol_table);
  dump_symbol_table("reorder typedefs", symbol_table);
  reorder_symbol_table_structures(symbol_table);
  dump_symbol_table("reorder structures", symbol_table);
  buffer_t* buffer = make_buffer((1024*8));
  printer_t* printer = make_printer(buffer, 2);
  char* guard_name = "_HEADER_FILE_GUARD_";
  if (is_header_file)
  {
    buffer_printf(buffer, "#ifndef %s\n#define %s\n\n", guard_name, guard_name);
  }
  buffer_append_buffer(buffer, get_reflection_header_buffer());
  boolean_t append_newline_after_system_includes = false;
  buffer_append_string(buffer, "// ========== system includes ==========\n\n");
  string_hashtable_t* system_includes_set = make_string_hashtable(19);
  for (
    uint64_t i = 0;
    (i<((symbol_table->system_includes)->length));
    (i++))
  {
    (append_newline_after_system_includes=true);
    cpp_include_node_t* node = value_array_get_ptr((symbol_table->system_includes), i, typeof(cpp_include_node_t*));
    char* include_statement = include_node_to_string(node);
    if ((FLAG_omit_c_armyknife_include&&string_starts_with(include_statement, "#include <c-armyknife-lib")))
    {
      continue;
    }
    if ((!is_ok(string_ht_find(system_includes_set, include_statement))))
    {
      (system_includes_set=string_ht_insert(system_includes_set, include_statement, boolean_to_value(true)));
      buffer_append_string(buffer, include_statement);
    }
  }
  if (append_newline_after_system_includes)
  {
    buffer_append_string(buffer, "\n");
  }
  buffer_append_string(buffer, "// ========== defines ==========\n\n");
  for (
    uint64_t i = 0;
    (i<((symbol_table->defines)->length));
    (i++))
  {
    cpp_define_node_t* node = value_array_get_ptr((symbol_table->defines), i, typeof(cpp_define_node_t*));
    append_cpp_define_node(printer, node);
    append_string(printer, "\n");
  }
  buffer_append_string(buffer, "// ========== enums ==========\n\n");
  for (
    int i = 0;
    (i<(((symbol_table->enums)->ordered_bindings)->length));
    (i++))
  {
    symbol_table_binding_t* binding = value_array_get_ptr(((symbol_table->enums)->ordered_bindings), i, typeof(symbol_table_binding_t*));
    enum_node_t* enum_node = to_enum_node(value_array_get_ptr((binding->definition_nodes), 0, typeof(parse_node_t*)));
    append_enum_node(printer, enum_node);
    append_string(printer, ";\n\n");
  }
  buffer_append_string(buffer, "// ========== typedefs ==========\n\n");
  for (
    int i = 0;
    (i<(((symbol_table->typedefs)->ordered_bindings)->length));
    (i++))
  {
    symbol_table_binding_t* binding = value_array_get_ptr(((symbol_table->typedefs)->ordered_bindings), i, typeof(symbol_table_binding_t*));
    typedef_node_t* typedef_node = to_typedef_node((/*CAST*/(parse_node_t*) (value_array_get((binding->definition_nodes), 0).ptr)));
    append_typedef_node(printer, typedef_node);
    append_string(printer, "\n");
  }
  buffer_append_string(buffer, "// ========== stuctures/unions ==========\n\n");
  for (
    int i = 0;
    (i<(((symbol_table->structures)->ordered_bindings)->length));
    (i++))
  {
    symbol_table_binding_t* binding = value_array_get_ptr(((symbol_table->structures)->ordered_bindings), i, typeof(symbol_table_binding_t*));
    struct_node_t* struct_node = get_full_structure_definition_node(binding);
    if ((struct_node==NULL))
    {
      (struct_node=value_array_get_ptr((binding->definition_nodes), 0, typeof(struct_node_t*)));
    }
    append_struct_node(printer, struct_node);
    append_string(printer, ";\n\n");
  }
  boolean_t append_newline_after_variables = false;
  buffer_append_string(buffer, "// ========== global variables ==========\n\n");
  for (
    int i = 0;
    (i<(((symbol_table->variables)->ordered_bindings)->length));
    (i++))
  {
    symbol_table_binding_t* binding = value_array_get_ptr(((symbol_table->variables)->ordered_bindings), i, typeof(symbol_table_binding_t*));
    append_variable_definition_node(printer, value_array_get_ptr((binding->definition_nodes), 0, typeof(variable_definition_node_t*)), is_library);
    append_string(printer, "\n");
  }
  if (append_newline_after_variables)
  {
    append_string(printer, "\n");
  }
  boolean_t append_newline_after_prototypes = false;
  buffer_append_string(buffer, "// ========== function prototypes ==========\n\n");
  for (
    int i = 0;
    (i<(((symbol_table->functions)->ordered_bindings)->length));
    (i++))
  {
    symbol_table_binding_t* binding = value_array_get_ptr(((symbol_table->functions)->ordered_bindings), i, typeof(symbol_table_binding_t*));
    function_node_t* function_node = to_function_node((/*CAST*/(parse_node_t*) (value_array_get((binding->definition_nodes), 0).ptr)));
    if ((!is_inlined_function(function_node)))
    {
      (append_newline_after_prototypes=true);
      append_c_function_node_prototype(printer, function_node);
    }
  }
  if (append_newline_after_prototypes)
  {
    append_string(printer, "\n");
  }
  boolean_t append_newline_after_inlines = false;
  buffer_append_string(buffer, "// ========== inlined functions ==========\n\n");
  for (
    int i = 0;
    (i<(((symbol_table->functions)->ordered_bindings)->length));
    (i++))
  {
    symbol_table_binding_t* binding = value_array_get_ptr(((symbol_table->functions)->ordered_bindings), i, typeof(symbol_table_binding_t*));
    function_node_t* function_node = to_function_node((/*CAST*/(parse_node_t*) (value_array_get((binding->definition_nodes), 0).ptr)));
    if (is_inlined_function(function_node))
    {
      (append_newline_after_inlines=true);
      append_c_function_node_and_body(printer, function_node);
    }
  }
  if (append_newline_after_inlines)
  {
    buffer_append_string(buffer, "\n");
  }
  boolean_t append_newline_after_functions = false;
  if (is_library)
  {
    buffer_append_string(buffer, "// ========== functions ==========\n\n");
    for (
      int i = 0;
      (i<(((symbol_table->functions)->ordered_bindings)->length));
      (i++))
    {
      symbol_table_binding_t* binding = value_array_get_ptr(((symbol_table->functions)->ordered_bindings), i, typeof(symbol_table_binding_t*));
      for (
        int j = 0;
        (j<((binding->definition_nodes)->length));
        (j++))
      {
        function_node_t* function_node = to_function_node((/*CAST*/(parse_node_t*) (value_array_get((binding->definition_nodes), j).ptr)));
        if (((!is_inlined_function(function_node))&&((function_node->body)!=NULL)))
        {
          (append_newline_after_functions=true);
          buffer_printf(buffer, "/* i=%d j=%d */\n", i, j);
          append_c_function_node_and_body(printer, function_node);
        }
      }
    }
  }
  if (append_newline_after_functions)
  {
    buffer_append_string(buffer, "\n");
  }
  if (is_header_file)
  {
    buffer_printf(buffer, "\n#endif /* %s */\n", guard_name);
  }
  if ((FLAG_ouput_file==NULL))
  {
    fprintf(stdout, "%s\n", buffer_to_c_string(buffer));
  }
  else
  {
    log_info("Attempting to write buffer to %s", FLAG_ouput_file);
    buffer_write_file(buffer, FLAG_ouput_file);
  }
}

/* i=516 j=0 */
void parse_expression_string_and_print_parse_tree(char* expression){
  if ((expression==NULL))
  {
    log_fatal("Expression not specified!");
    fatal_error(ERROR_ILLEGAL_INPUT);
  }
  tokenizer_result_t tokenizer_result = tokenize(buffer_append_string(make_buffer(1), expression));
  if ((tokenizer_result.tokenizer_error_code))
  {
    fatal_error(ERROR_ILLEGAL_INPUT);
  }
  value_array_t* tokens = (tokenizer_result.tokens);
  (tokens=transform_tokens(tokens, ((token_transformer_options_t) {
                                   .keep_whitespace = false,
                                   .keep_comments = false,
                                   .keep_javadoc_comments = false,
                                   .keep_c_preprocessor_lines = false,
                               })));
  pstate_t pstate = ((pstate_t) {0});
  ((pstate.use_statement_parser)=true);
  ((pstate.tokens)=tokens);
  if ((!parse_expression((&pstate))))
  {
    fprintf(stderr, "FAIL\n");
    exit(1);
  }
  parse_node_t* node = pstate_get_result_node((&pstate));
  if ((!pstate_expect_token_string((&pstate), ";")))
  {
    fprintf(stderr, "FAIL (expected ';')\n");
    exit(1);
  }
  buffer_t* output = make_buffer(1);
  buffer_append_dbg_parse_node(make_cdl_printer(output), node);
  if (FLAG_to_c)
  {
    buffer_append_string(output, "\n// C Output\n");
    printer_t* printer = make_printer(output, 2);
    append_parse_node(printer, node);
  }
  fprintf(stdout, "%s\n", buffer_to_c_string(output));
}

/* i=517 j=0 */
void parse_statement_string_and_print_parse_tree(char* expression){
  tokenizer_result_t tokenizer_result = tokenize(buffer_append_string(make_buffer(1), expression));
  if ((tokenizer_result.tokenizer_error_code))
  {
    fatal_error(ERROR_ILLEGAL_INPUT);
  }
  value_array_t* tokens = (tokenizer_result.tokens);
  (tokens=transform_tokens(tokens, ((token_transformer_options_t) {
                                   .keep_whitespace = false,
                                   .keep_comments = false,
                                   .keep_javadoc_comments = false,
                                   .keep_c_preprocessor_lines = false,
                               })));
  pstate_t state = ((pstate_t) {0});
  ((state.use_statement_parser)=true);
  ((state.tokens)=tokens);
  pstatus_t status = parse_statement((&state));
  if ((!status))
  {
    fprintf(stderr, "FAIL");
    exit(1);
  }
  parse_node_t* node = pstate_get_result_node((&state));
  buffer_t* output = make_buffer(1);
  buffer_append_dbg_parse_node(make_cdl_printer(output), node);
  buffer_append_string(output, "\n// C Output\n");
  printer_t* printer = make_printer(output, 2);
  append_parse_node(printer, node);
  fprintf(stdout, "%s\n", buffer_to_c_string(output));
}

/* i=518 j=0 */
int main(int argc, char** argv){
  configure_fatal_errors(((fatal_error_config_t) {
                                                 .catch_sigsegv = true,
                                             }));
  logger_init();
  configure_flags();
  char* error = flag_parse_command_line(argc, argv);
  if (error)
  {
    flag_print_help(stderr, error);
    exit(1);
  }
  if (FLAG_print_command_line)
  {
    fprintf(stderr, "Command Line:");
    for (
      int i = 0;
      (i<argc);
      (i++))
    {
      fprintf(stderr, " %s", (argv[i]));
    }
    fprintf(stderr, "\n");
  }
  if ((FLAG_command==NULL))
  {
    fatal_error(ERROR_BAD_COMMAND_LINE);
  }
  else
  if (string_equal("generate-header-file", FLAG_command))
  {
    generate_c_output_file(false);
    log_info("Exiting normally.");
    exit(0);
  }
  else
  if (string_equal("generate-library", FLAG_command))
  {
    generate_c_output_file(true);
    log_info("Exiting normally.");
    exit(0);
  }
  else
  if (string_equal("parse-expression", FLAG_command))
  {
    parse_expression_string_and_print_parse_tree(FLAG_expression);
  }
  else
  if (string_equal("parse-statement", FLAG_command))
  {
    parse_statement_string_and_print_parse_tree(FLAG_statement);
  }
  else
  if (string_equal("print-tokens", FLAG_command))
  {
    print_tokens();
  }
  else
  if ((string_equal("extract-enums", FLAG_command)||string_equal("extract-prototypes", FLAG_command)))
  {
    extract_command(FLAG_command);
  }
  else
  {
    fprintf(stderr, "Unknown command: %s\n", FLAG_command);
  }
  exit(0);
}

/* i=519 j=0 */
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
/* i=520 j=0 */
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
/* i=521 j=0 */
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
/* i=522 j=0 */
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
/* i=523 j=0 */
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
/* i=524 j=0 */
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
/* i=525 j=0 */
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
/* i=526 j=0 */
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
/* i=527 j=0 */
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
/* i=528 j=0 */
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
/* i=529 j=0 */
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
/* i=530 j=0 */
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
/* i=531 j=0 */
char* input_mode_to_string(input_mode_t value){
  switch (value) {
    case INPUT_MODE_OMNI_C:
return "INPUT_MODE_OMNI_C";
  case INPUT_MODE_STANDARD_C:
return "INPUT_MODE_STANDARD_C";
  case INPUT_MODE_C_PLUS_PLUS:
return "INPUT_MODE_C_PLUS_PLUS";
  default:
    return "<<unknown-input_mode>>";
  }
}
/* i=532 j=0 */
input_mode_t string_to_input_mode(char* value){
  if (strcmp(value, "INPUT_MODE_OMNI_C") == 0) {
return INPUT_MODE_OMNI_C;
  }
  if (strcmp(value, "INPUT_MODE_STANDARD_C") == 0) {
return INPUT_MODE_STANDARD_C;
  }
  if (strcmp(value, "INPUT_MODE_C_PLUS_PLUS") == 0) {
return INPUT_MODE_C_PLUS_PLUS;
  }
  return 0;
}
/* i=533 j=0 */
enum_metadata_t* input_mode_metadata(){
    static enum_element_metadata_t var_0 = (enum_element_metadata_t) {
        .next = NULL,
        .name = "INPUT_MODE_OMNI_C",
        .value = INPUT_MODE_OMNI_C
    };
    static enum_element_metadata_t var_1 = (enum_element_metadata_t) {
        .next = &var_0,
        .name = "INPUT_MODE_STANDARD_C",
        .value = INPUT_MODE_STANDARD_C
    };
    static enum_element_metadata_t var_2 = (enum_element_metadata_t) {
        .next = &var_1,
        .name = "INPUT_MODE_C_PLUS_PLUS",
        .value = INPUT_MODE_C_PLUS_PLUS
    };
    static enum_metadata_t enum_metadata_result = (enum_metadata_t) {
        .name = "input_mode_t",
        .elements = &var_2
    };
    return &enum_metadata_result;
}
/* i=534 j=0 */
char* output_mode_to_string(output_mode_t value){
  switch (value) {
    case OUTPUT_MODE_STANDARD_C:
return "OUTPUT_MODE_STANDARD_C";
  case OUTPUT_MODE_C_PLUS_PLUS:
return "OUTPUT_MODE_C_PLUS_PLUS";
  default:
    return "<<unknown-output_mode>>";
  }
}
/* i=535 j=0 */
output_mode_t string_to_output_mode(char* value){
  if (strcmp(value, "OUTPUT_MODE_STANDARD_C") == 0) {
return OUTPUT_MODE_STANDARD_C;
  }
  if (strcmp(value, "OUTPUT_MODE_C_PLUS_PLUS") == 0) {
return OUTPUT_MODE_C_PLUS_PLUS;
  }
  return 0;
}
/* i=536 j=0 */
enum_metadata_t* output_mode_metadata(){
    static enum_element_metadata_t var_0 = (enum_element_metadata_t) {
        .next = NULL,
        .name = "OUTPUT_MODE_STANDARD_C",
        .value = OUTPUT_MODE_STANDARD_C
    };
    static enum_element_metadata_t var_1 = (enum_element_metadata_t) {
        .next = &var_0,
        .name = "OUTPUT_MODE_C_PLUS_PLUS",
        .value = OUTPUT_MODE_C_PLUS_PLUS
    };
    static enum_metadata_t enum_metadata_result = (enum_metadata_t) {
        .name = "output_mode_t",
        .elements = &var_1
    };
    return &enum_metadata_result;
}
/* i=537 j=0 */
char* file_tag_to_string(file_tag_t value){
  switch (value) {
    case OMNI_C_SOURCE_FILE:
return "OMNI_C_SOURCE_FILE";
  case STD_C_SOURCE_FILE:
return "STD_C_SOURCE_FILE";
  case DATA_FILE:
return "DATA_FILE";
  default:
    return "<<unknown-file_tag>>";
  }
}
/* i=538 j=0 */
file_tag_t string_to_file_tag(char* value){
  if (strcmp(value, "OMNI_C_SOURCE_FILE") == 0) {
return OMNI_C_SOURCE_FILE;
  }
  if (strcmp(value, "STD_C_SOURCE_FILE") == 0) {
return STD_C_SOURCE_FILE;
  }
  if (strcmp(value, "DATA_FILE") == 0) {
return DATA_FILE;
  }
  return 0;
}
/* i=539 j=0 */
enum_metadata_t* file_tag_metadata(){
    static enum_element_metadata_t var_0 = (enum_element_metadata_t) {
        .next = NULL,
        .name = "OMNI_C_SOURCE_FILE",
        .value = OMNI_C_SOURCE_FILE
    };
    static enum_element_metadata_t var_1 = (enum_element_metadata_t) {
        .next = &var_0,
        .name = "STD_C_SOURCE_FILE",
        .value = STD_C_SOURCE_FILE
    };
    static enum_element_metadata_t var_2 = (enum_element_metadata_t) {
        .next = &var_1,
        .name = "DATA_FILE",
        .value = DATA_FILE
    };
    static enum_metadata_t enum_metadata_result = (enum_metadata_t) {
        .name = "file_tag_t",
        .elements = &var_2
    };
    return &enum_metadata_result;
}
/* i=540 j=0 */
char* tokenizer_error_to_string(tokenizer_error_t value){
  switch (value) {
    case TOKENIZER_ERROR_UNKNOWN:
return "TOKENIZER_ERROR_UNKNOWN";
  case TOKENIZER_ERROR_UTF_DECODE_ERROR:
return "TOKENIZER_ERROR_UTF_DECODE_ERROR";
  case TOKENIZER_ERROR_UNRECOGNIZED_PUNCTUATION:
return "TOKENIZER_ERROR_UNRECOGNIZED_PUNCTUATION";
  case TOKENIZER_ERROR_UNTERMINATED_COMMENT:
return "TOKENIZER_ERROR_UNTERMINATED_COMMENT";
  case TOKENIZER_ERROR_UNTERMINATED_STRING_LITERAL:
return "TOKENIZER_ERROR_UNTERMINATED_STRING_LITERAL";
  case TOKENIZER_ERROR_UNTERMINATED_CHARACTER_LITERL:
return "TOKENIZER_ERROR_UNTERMINATED_CHARACTER_LITERL";
  default:
    return "<<unknown-tokenizer_error>>";
  }
}
/* i=541 j=0 */
tokenizer_error_t string_to_tokenizer_error(char* value){
  if (strcmp(value, "TOKENIZER_ERROR_UNKNOWN") == 0) {
return TOKENIZER_ERROR_UNKNOWN;
  }
  if (strcmp(value, "TOKENIZER_ERROR_UTF_DECODE_ERROR") == 0) {
return TOKENIZER_ERROR_UTF_DECODE_ERROR;
  }
  if (strcmp(value, "TOKENIZER_ERROR_UNRECOGNIZED_PUNCTUATION") == 0) {
return TOKENIZER_ERROR_UNRECOGNIZED_PUNCTUATION;
  }
  if (strcmp(value, "TOKENIZER_ERROR_UNTERMINATED_COMMENT") == 0) {
return TOKENIZER_ERROR_UNTERMINATED_COMMENT;
  }
  if (strcmp(value, "TOKENIZER_ERROR_UNTERMINATED_STRING_LITERAL") == 0) {
return TOKENIZER_ERROR_UNTERMINATED_STRING_LITERAL;
  }
  if (strcmp(value, "TOKENIZER_ERROR_UNTERMINATED_CHARACTER_LITERL") == 0) {
return TOKENIZER_ERROR_UNTERMINATED_CHARACTER_LITERL;
  }
  return 0;
}
/* i=542 j=0 */
enum_metadata_t* tokenizer_error_metadata(){
    static enum_element_metadata_t var_0 = (enum_element_metadata_t) {
        .next = NULL,
        .name = "TOKENIZER_ERROR_UNKNOWN",
        .value = TOKENIZER_ERROR_UNKNOWN
    };
    static enum_element_metadata_t var_1 = (enum_element_metadata_t) {
        .next = &var_0,
        .name = "TOKENIZER_ERROR_UTF_DECODE_ERROR",
        .value = TOKENIZER_ERROR_UTF_DECODE_ERROR
    };
    static enum_element_metadata_t var_2 = (enum_element_metadata_t) {
        .next = &var_1,
        .name = "TOKENIZER_ERROR_UNRECOGNIZED_PUNCTUATION",
        .value = TOKENIZER_ERROR_UNRECOGNIZED_PUNCTUATION
    };
    static enum_element_metadata_t var_3 = (enum_element_metadata_t) {
        .next = &var_2,
        .name = "TOKENIZER_ERROR_UNTERMINATED_COMMENT",
        .value = TOKENIZER_ERROR_UNTERMINATED_COMMENT
    };
    static enum_element_metadata_t var_4 = (enum_element_metadata_t) {
        .next = &var_3,
        .name = "TOKENIZER_ERROR_UNTERMINATED_STRING_LITERAL",
        .value = TOKENIZER_ERROR_UNTERMINATED_STRING_LITERAL
    };
    static enum_element_metadata_t var_5 = (enum_element_metadata_t) {
        .next = &var_4,
        .name = "TOKENIZER_ERROR_UNTERMINATED_CHARACTER_LITERL",
        .value = TOKENIZER_ERROR_UNTERMINATED_CHARACTER_LITERL
    };
    static enum_metadata_t enum_metadata_result = (enum_metadata_t) {
        .name = "tokenizer_error_t",
        .elements = &var_5
    };
    return &enum_metadata_result;
}
/* i=543 j=0 */
char* parse_error_code_to_string(parse_error_code_t value){
  switch (value) {
    case PARSE_ERROR_UNKNOWN:
return "PARSE_ERROR_UNKNOWN";
  case PARSE_ERROR_COMMA_OR_EQUAL_SIGN_EXPECTED:
return "PARSE_ERROR_COMMA_OR_EQUAL_SIGN_EXPECTED";
  case PARSE_ERROR_EXPECTED_FIELD_WIDTH_OR_SEMICOLON:
return "PARSE_ERROR_EXPECTED_FIELD_WIDTH_OR_SEMICOLON";
  case PARSE_ERROR_EXPECTED_SEMICOLON:
return "PARSE_ERROR_EXPECTED_SEMICOLON";
  case PARSE_ERROR_IDENTIFIER_EXPECTED:
return "PARSE_ERROR_IDENTIFIER_EXPECTED";
  case PARSE_ERROR_NOT_LITERAL_NODE:
return "PARSE_ERROR_NOT_LITERAL_NODE";
  case PARSE_ERROR_INTEGER_LITERAL_EXPECTED:
return "PARSE_ERROR_INTEGER_LITERAL_EXPECTED";
  case PARSE_ERROR_OPEN_BRACE_EXPECTED:
return "PARSE_ERROR_OPEN_BRACE_EXPECTED";
  case PARSE_ERROR_CLOSE_BRACKET_EXPECTED:
return "PARSE_ERROR_CLOSE_BRACKET_EXPECTED";
  case PARSE_ERROR_UNRECOGNIZED_TOP_LEVEL_DECLARATION:
return "PARSE_ERROR_UNRECOGNIZED_TOP_LEVEL_DECLARATION";
  case PARSE_ERROR_SEMICOLON_EXPECTED:
return "PARSE_ERROR_SEMICOLON_EXPECTED";
  case PARSE_ERROR_CONFLICTING_STORAGE_CLASS_SPECIFIER:
return "PARSE_ERROR_CONFLICTING_STORAGE_CLASS_SPECIFIER";
  case PARSE_ERROR_CONFLICTING_FUNCTION_SPECIFIER:
return "PARSE_ERROR_CONFLICTING_FUNCTION_SPECIFIER";
  case PARSE_ERROR_EXPECTED_OPEN_PAREN_AFTER_UNDERSCORE_ATTRIBUTE:
return "PARSE_ERROR_EXPECTED_OPEN_PAREN_AFTER_UNDERSCORE_ATTRIBUTE";
  case PARSE_ERROR_EXPECTED_MATCHING_CLOSE_PAREN_AFTER_UNDERSCORE_ATTRIBUTE:
return "PARSE_ERROR_EXPECTED_MATCHING_CLOSE_PAREN_AFTER_UNDERSCORE_ATTRIBUTE";
  case PARSE_ERROR_EXPECTED_PREFIX_OPERATOR_OR_TERMINAL:
return "PARSE_ERROR_EXPECTED_PREFIX_OPERATOR_OR_TERMINAL";
  case PARSE_ERROR_EXPECTED_STATEMENT:
return "PARSE_ERROR_EXPECTED_STATEMENT";
  case PARSE_ERROR_EXPECTED_TOKEN:
return "PARSE_ERROR_EXPECTED_TOKEN";
  case PARSE_ERROR_EXPECTED_TOKEN_TYPE:
return "PARSE_ERROR_EXPECTED_TOKEN_TYPE";
  case PARSE_ERROR_EOF:
return "PARSE_ERROR_EOF";
  case PARSE_ERROR_UNHANDLED_INSTRUCTION:
return "PARSE_ERROR_UNHANDLED_INSTRUCTION";
  default:
    return "<<unknown-parse_error_code>>";
  }
}
/* i=544 j=0 */
parse_error_code_t string_to_parse_error_code(char* value){
  if (strcmp(value, "PARSE_ERROR_UNKNOWN") == 0) {
return PARSE_ERROR_UNKNOWN;
  }
  if (strcmp(value, "PARSE_ERROR_COMMA_OR_EQUAL_SIGN_EXPECTED") == 0) {
return PARSE_ERROR_COMMA_OR_EQUAL_SIGN_EXPECTED;
  }
  if (strcmp(value, "PARSE_ERROR_EXPECTED_FIELD_WIDTH_OR_SEMICOLON") == 0) {
return PARSE_ERROR_EXPECTED_FIELD_WIDTH_OR_SEMICOLON;
  }
  if (strcmp(value, "PARSE_ERROR_EXPECTED_SEMICOLON") == 0) {
return PARSE_ERROR_EXPECTED_SEMICOLON;
  }
  if (strcmp(value, "PARSE_ERROR_IDENTIFIER_EXPECTED") == 0) {
return PARSE_ERROR_IDENTIFIER_EXPECTED;
  }
  if (strcmp(value, "PARSE_ERROR_NOT_LITERAL_NODE") == 0) {
return PARSE_ERROR_NOT_LITERAL_NODE;
  }
  if (strcmp(value, "PARSE_ERROR_INTEGER_LITERAL_EXPECTED") == 0) {
return PARSE_ERROR_INTEGER_LITERAL_EXPECTED;
  }
  if (strcmp(value, "PARSE_ERROR_OPEN_BRACE_EXPECTED") == 0) {
return PARSE_ERROR_OPEN_BRACE_EXPECTED;
  }
  if (strcmp(value, "PARSE_ERROR_CLOSE_BRACKET_EXPECTED") == 0) {
return PARSE_ERROR_CLOSE_BRACKET_EXPECTED;
  }
  if (strcmp(value, "PARSE_ERROR_UNRECOGNIZED_TOP_LEVEL_DECLARATION") == 0) {
return PARSE_ERROR_UNRECOGNIZED_TOP_LEVEL_DECLARATION;
  }
  if (strcmp(value, "PARSE_ERROR_SEMICOLON_EXPECTED") == 0) {
return PARSE_ERROR_SEMICOLON_EXPECTED;
  }
  if (strcmp(value, "PARSE_ERROR_CONFLICTING_STORAGE_CLASS_SPECIFIER") == 0) {
return PARSE_ERROR_CONFLICTING_STORAGE_CLASS_SPECIFIER;
  }
  if (strcmp(value, "PARSE_ERROR_CONFLICTING_FUNCTION_SPECIFIER") == 0) {
return PARSE_ERROR_CONFLICTING_FUNCTION_SPECIFIER;
  }
  if (strcmp(value, "PARSE_ERROR_EXPECTED_OPEN_PAREN_AFTER_UNDERSCORE_ATTRIBUTE") == 0) {
return PARSE_ERROR_EXPECTED_OPEN_PAREN_AFTER_UNDERSCORE_ATTRIBUTE;
  }
  if (strcmp(value, "PARSE_ERROR_EXPECTED_MATCHING_CLOSE_PAREN_AFTER_UNDERSCORE_ATTRIBUTE") == 0) {
return PARSE_ERROR_EXPECTED_MATCHING_CLOSE_PAREN_AFTER_UNDERSCORE_ATTRIBUTE;
  }
  if (strcmp(value, "PARSE_ERROR_EXPECTED_PREFIX_OPERATOR_OR_TERMINAL") == 0) {
return PARSE_ERROR_EXPECTED_PREFIX_OPERATOR_OR_TERMINAL;
  }
  if (strcmp(value, "PARSE_ERROR_EXPECTED_STATEMENT") == 0) {
return PARSE_ERROR_EXPECTED_STATEMENT;
  }
  if (strcmp(value, "PARSE_ERROR_EXPECTED_TOKEN") == 0) {
return PARSE_ERROR_EXPECTED_TOKEN;
  }
  if (strcmp(value, "PARSE_ERROR_EXPECTED_TOKEN_TYPE") == 0) {
return PARSE_ERROR_EXPECTED_TOKEN_TYPE;
  }
  if (strcmp(value, "PARSE_ERROR_EOF") == 0) {
return PARSE_ERROR_EOF;
  }
  if (strcmp(value, "PARSE_ERROR_UNHANDLED_INSTRUCTION") == 0) {
return PARSE_ERROR_UNHANDLED_INSTRUCTION;
  }
  return 0;
}
/* i=545 j=0 */
enum_metadata_t* parse_error_code_metadata(){
    static enum_element_metadata_t var_0 = (enum_element_metadata_t) {
        .next = NULL,
        .name = "PARSE_ERROR_UNKNOWN",
        .value = PARSE_ERROR_UNKNOWN
    };
    static enum_element_metadata_t var_1 = (enum_element_metadata_t) {
        .next = &var_0,
        .name = "PARSE_ERROR_COMMA_OR_EQUAL_SIGN_EXPECTED",
        .value = PARSE_ERROR_COMMA_OR_EQUAL_SIGN_EXPECTED
    };
    static enum_element_metadata_t var_2 = (enum_element_metadata_t) {
        .next = &var_1,
        .name = "PARSE_ERROR_EXPECTED_FIELD_WIDTH_OR_SEMICOLON",
        .value = PARSE_ERROR_EXPECTED_FIELD_WIDTH_OR_SEMICOLON
    };
    static enum_element_metadata_t var_3 = (enum_element_metadata_t) {
        .next = &var_2,
        .name = "PARSE_ERROR_EXPECTED_SEMICOLON",
        .value = PARSE_ERROR_EXPECTED_SEMICOLON
    };
    static enum_element_metadata_t var_4 = (enum_element_metadata_t) {
        .next = &var_3,
        .name = "PARSE_ERROR_IDENTIFIER_EXPECTED",
        .value = PARSE_ERROR_IDENTIFIER_EXPECTED
    };
    static enum_element_metadata_t var_5 = (enum_element_metadata_t) {
        .next = &var_4,
        .name = "PARSE_ERROR_NOT_LITERAL_NODE",
        .value = PARSE_ERROR_NOT_LITERAL_NODE
    };
    static enum_element_metadata_t var_6 = (enum_element_metadata_t) {
        .next = &var_5,
        .name = "PARSE_ERROR_INTEGER_LITERAL_EXPECTED",
        .value = PARSE_ERROR_INTEGER_LITERAL_EXPECTED
    };
    static enum_element_metadata_t var_7 = (enum_element_metadata_t) {
        .next = &var_6,
        .name = "PARSE_ERROR_OPEN_BRACE_EXPECTED",
        .value = PARSE_ERROR_OPEN_BRACE_EXPECTED
    };
    static enum_element_metadata_t var_8 = (enum_element_metadata_t) {
        .next = &var_7,
        .name = "PARSE_ERROR_CLOSE_BRACKET_EXPECTED",
        .value = PARSE_ERROR_CLOSE_BRACKET_EXPECTED
    };
    static enum_element_metadata_t var_9 = (enum_element_metadata_t) {
        .next = &var_8,
        .name = "PARSE_ERROR_UNRECOGNIZED_TOP_LEVEL_DECLARATION",
        .value = PARSE_ERROR_UNRECOGNIZED_TOP_LEVEL_DECLARATION
    };
    static enum_element_metadata_t var_10 = (enum_element_metadata_t) {
        .next = &var_9,
        .name = "PARSE_ERROR_SEMICOLON_EXPECTED",
        .value = PARSE_ERROR_SEMICOLON_EXPECTED
    };
    static enum_element_metadata_t var_11 = (enum_element_metadata_t) {
        .next = &var_10,
        .name = "PARSE_ERROR_CONFLICTING_STORAGE_CLASS_SPECIFIER",
        .value = PARSE_ERROR_CONFLICTING_STORAGE_CLASS_SPECIFIER
    };
    static enum_element_metadata_t var_12 = (enum_element_metadata_t) {
        .next = &var_11,
        .name = "PARSE_ERROR_CONFLICTING_FUNCTION_SPECIFIER",
        .value = PARSE_ERROR_CONFLICTING_FUNCTION_SPECIFIER
    };
    static enum_element_metadata_t var_13 = (enum_element_metadata_t) {
        .next = &var_12,
        .name = "PARSE_ERROR_EXPECTED_OPEN_PAREN_AFTER_UNDERSCORE_ATTRIBUTE",
        .value = PARSE_ERROR_EXPECTED_OPEN_PAREN_AFTER_UNDERSCORE_ATTRIBUTE
    };
    static enum_element_metadata_t var_14 = (enum_element_metadata_t) {
        .next = &var_13,
        .name = "PARSE_ERROR_EXPECTED_MATCHING_CLOSE_PAREN_AFTER_UNDERSCORE_ATTRIBUTE",
        .value = PARSE_ERROR_EXPECTED_MATCHING_CLOSE_PAREN_AFTER_UNDERSCORE_ATTRIBUTE
    };
    static enum_element_metadata_t var_15 = (enum_element_metadata_t) {
        .next = &var_14,
        .name = "PARSE_ERROR_EXPECTED_PREFIX_OPERATOR_OR_TERMINAL",
        .value = PARSE_ERROR_EXPECTED_PREFIX_OPERATOR_OR_TERMINAL
    };
    static enum_element_metadata_t var_16 = (enum_element_metadata_t) {
        .next = &var_15,
        .name = "PARSE_ERROR_EXPECTED_STATEMENT",
        .value = PARSE_ERROR_EXPECTED_STATEMENT
    };
    static enum_element_metadata_t var_17 = (enum_element_metadata_t) {
        .next = &var_16,
        .name = "PARSE_ERROR_EXPECTED_TOKEN",
        .value = PARSE_ERROR_EXPECTED_TOKEN
    };
    static enum_element_metadata_t var_18 = (enum_element_metadata_t) {
        .next = &var_17,
        .name = "PARSE_ERROR_EXPECTED_TOKEN_TYPE",
        .value = PARSE_ERROR_EXPECTED_TOKEN_TYPE
    };
    static enum_element_metadata_t var_19 = (enum_element_metadata_t) {
        .next = &var_18,
        .name = "PARSE_ERROR_EOF",
        .value = PARSE_ERROR_EOF
    };
    static enum_element_metadata_t var_20 = (enum_element_metadata_t) {
        .next = &var_19,
        .name = "PARSE_ERROR_UNHANDLED_INSTRUCTION",
        .value = PARSE_ERROR_UNHANDLED_INSTRUCTION
    };
    static enum_metadata_t enum_metadata_result = (enum_metadata_t) {
        .name = "parse_error_code_t",
        .elements = &var_20
    };
    return &enum_metadata_result;
}
/* i=546 j=0 */
token_type_t string_to_token_type(char* value){
  if (strcmp(value, "TOKEN_TYPE_UNKNOWN") == 0) {
return TOKEN_TYPE_UNKNOWN;
  }
  if (strcmp(value, "TOKEN_TYPE_WHITESPACE") == 0) {
return TOKEN_TYPE_WHITESPACE;
  }
  if (strcmp(value, "TOKEN_TYPE_COMMENT") == 0) {
return TOKEN_TYPE_COMMENT;
  }
  if (strcmp(value, "TOKEN_TYPE_IDENTIFIER") == 0) {
return TOKEN_TYPE_IDENTIFIER;
  }
  if (strcmp(value, "TOKEN_TYPE_PUNCTUATION") == 0) {
return TOKEN_TYPE_PUNCTUATION;
  }
  if (strcmp(value, "TOKEN_TYPE_INTEGER_LITERAL") == 0) {
return TOKEN_TYPE_INTEGER_LITERAL;
  }
  if (strcmp(value, "TOKEN_TYPE_FLOAT_LITERAL") == 0) {
return TOKEN_TYPE_FLOAT_LITERAL;
  }
  if (strcmp(value, "TOKEN_TYPE_STRING_LITERAL") == 0) {
return TOKEN_TYPE_STRING_LITERAL;
  }
  if (strcmp(value, "TOKEN_TYPE_CHARACTER_LITERAL") == 0) {
return TOKEN_TYPE_CHARACTER_LITERAL;
  }
  return 0;
}
/* i=547 j=0 */
enum_metadata_t* token_type_metadata(){
    static enum_element_metadata_t var_0 = (enum_element_metadata_t) {
        .next = NULL,
        .name = "TOKEN_TYPE_UNKNOWN",
        .value = TOKEN_TYPE_UNKNOWN
    };
    static enum_element_metadata_t var_1 = (enum_element_metadata_t) {
        .next = &var_0,
        .name = "TOKEN_TYPE_WHITESPACE",
        .value = TOKEN_TYPE_WHITESPACE
    };
    static enum_element_metadata_t var_2 = (enum_element_metadata_t) {
        .next = &var_1,
        .name = "TOKEN_TYPE_COMMENT",
        .value = TOKEN_TYPE_COMMENT
    };
    static enum_element_metadata_t var_3 = (enum_element_metadata_t) {
        .next = &var_2,
        .name = "TOKEN_TYPE_IDENTIFIER",
        .value = TOKEN_TYPE_IDENTIFIER
    };
    static enum_element_metadata_t var_4 = (enum_element_metadata_t) {
        .next = &var_3,
        .name = "TOKEN_TYPE_PUNCTUATION",
        .value = TOKEN_TYPE_PUNCTUATION
    };
    static enum_element_metadata_t var_5 = (enum_element_metadata_t) {
        .next = &var_4,
        .name = "TOKEN_TYPE_INTEGER_LITERAL",
        .value = TOKEN_TYPE_INTEGER_LITERAL
    };
    static enum_element_metadata_t var_6 = (enum_element_metadata_t) {
        .next = &var_5,
        .name = "TOKEN_TYPE_FLOAT_LITERAL",
        .value = TOKEN_TYPE_FLOAT_LITERAL
    };
    static enum_element_metadata_t var_7 = (enum_element_metadata_t) {
        .next = &var_6,
        .name = "TOKEN_TYPE_STRING_LITERAL",
        .value = TOKEN_TYPE_STRING_LITERAL
    };
    static enum_element_metadata_t var_8 = (enum_element_metadata_t) {
        .next = &var_7,
        .name = "TOKEN_TYPE_CHARACTER_LITERAL",
        .value = TOKEN_TYPE_CHARACTER_LITERAL
    };
    static enum_metadata_t enum_metadata_result = (enum_metadata_t) {
        .name = "token_type_t",
        .elements = &var_8
    };
    return &enum_metadata_result;
}
/* i=548 j=0 */
char* numeric_literal_encoding_to_string(numeric_literal_encoding_t value){
  switch (value) {
    case NUMERIC_LITERAL_ENCODING_UNDECIDED:
return "NUMERIC_LITERAL_ENCODING_UNDECIDED";
  case NUMERIC_LITERAL_ENCODING_FLOAT_OR_DECIMAL:
return "NUMERIC_LITERAL_ENCODING_FLOAT_OR_DECIMAL";
  case NUMERIC_LITERAL_ENCODING_BINARY:
return "NUMERIC_LITERAL_ENCODING_BINARY";
  case NUMERIC_LITERAL_ENCODING_OCTAL:
return "NUMERIC_LITERAL_ENCODING_OCTAL";
  case NUMERIC_LITERAL_ENCODING_HEX:
return "NUMERIC_LITERAL_ENCODING_HEX";
  case NUMERIC_LITERAL_ENCODING_DECIMAL:
return "NUMERIC_LITERAL_ENCODING_DECIMAL";
  case NUMERIC_LITERAL_ENCODING_FLOAT:
return "NUMERIC_LITERAL_ENCODING_FLOAT";
  default:
    return "<<unknown-numeric_literal_encoding>>";
  }
}
/* i=549 j=0 */
numeric_literal_encoding_t string_to_numeric_literal_encoding(char* value){
  if (strcmp(value, "NUMERIC_LITERAL_ENCODING_UNDECIDED") == 0) {
return NUMERIC_LITERAL_ENCODING_UNDECIDED;
  }
  if (strcmp(value, "NUMERIC_LITERAL_ENCODING_FLOAT_OR_DECIMAL") == 0) {
return NUMERIC_LITERAL_ENCODING_FLOAT_OR_DECIMAL;
  }
  if (strcmp(value, "NUMERIC_LITERAL_ENCODING_BINARY") == 0) {
return NUMERIC_LITERAL_ENCODING_BINARY;
  }
  if (strcmp(value, "NUMERIC_LITERAL_ENCODING_OCTAL") == 0) {
return NUMERIC_LITERAL_ENCODING_OCTAL;
  }
  if (strcmp(value, "NUMERIC_LITERAL_ENCODING_HEX") == 0) {
return NUMERIC_LITERAL_ENCODING_HEX;
  }
  if (strcmp(value, "NUMERIC_LITERAL_ENCODING_DECIMAL") == 0) {
return NUMERIC_LITERAL_ENCODING_DECIMAL;
  }
  if (strcmp(value, "NUMERIC_LITERAL_ENCODING_FLOAT") == 0) {
return NUMERIC_LITERAL_ENCODING_FLOAT;
  }
  return 0;
}
/* i=550 j=0 */
enum_metadata_t* numeric_literal_encoding_metadata(){
    static enum_element_metadata_t var_0 = (enum_element_metadata_t) {
        .next = NULL,
        .name = "NUMERIC_LITERAL_ENCODING_UNDECIDED",
        .value = NUMERIC_LITERAL_ENCODING_UNDECIDED
    };
    static enum_element_metadata_t var_1 = (enum_element_metadata_t) {
        .next = &var_0,
        .name = "NUMERIC_LITERAL_ENCODING_FLOAT_OR_DECIMAL",
        .value = NUMERIC_LITERAL_ENCODING_FLOAT_OR_DECIMAL
    };
    static enum_element_metadata_t var_2 = (enum_element_metadata_t) {
        .next = &var_1,
        .name = "NUMERIC_LITERAL_ENCODING_BINARY",
        .value = NUMERIC_LITERAL_ENCODING_BINARY
    };
    static enum_element_metadata_t var_3 = (enum_element_metadata_t) {
        .next = &var_2,
        .name = "NUMERIC_LITERAL_ENCODING_OCTAL",
        .value = NUMERIC_LITERAL_ENCODING_OCTAL
    };
    static enum_element_metadata_t var_4 = (enum_element_metadata_t) {
        .next = &var_3,
        .name = "NUMERIC_LITERAL_ENCODING_HEX",
        .value = NUMERIC_LITERAL_ENCODING_HEX
    };
    static enum_element_metadata_t var_5 = (enum_element_metadata_t) {
        .next = &var_4,
        .name = "NUMERIC_LITERAL_ENCODING_DECIMAL",
        .value = NUMERIC_LITERAL_ENCODING_DECIMAL
    };
    static enum_element_metadata_t var_6 = (enum_element_metadata_t) {
        .next = &var_5,
        .name = "NUMERIC_LITERAL_ENCODING_FLOAT",
        .value = NUMERIC_LITERAL_ENCODING_FLOAT
    };
    static enum_metadata_t enum_metadata_result = (enum_metadata_t) {
        .name = "numeric_literal_encoding_t",
        .elements = &var_6
    };
    return &enum_metadata_result;
}
/* i=551 j=0 */
char* parse_node_type_to_string(parse_node_type_t value){
  switch (value) {
    case PARSE_NODE_UNKNOWN:
return "PARSE_NODE_UNKNOWN";
  case PARSE_NODE_DECLARATIONS:
return "PARSE_NODE_DECLARATIONS";
  case PARSE_NODE_ENUM:
return "PARSE_NODE_ENUM";
  case PARSE_NODE_ENUM_ELEMENT:
return "PARSE_NODE_ENUM_ELEMENT";
  case PARSE_NODE_FIELD:
return "PARSE_NODE_FIELD";
  case PARSE_NODE_GLOBAL_FUNCTION:
return "PARSE_NODE_GLOBAL_FUNCTION";
  case PARSE_NODE_VARIABLE_DEFINITION:
return "PARSE_NODE_VARIABLE_DEFINITION";
  case PARSE_NODE_LIST_OF_NODES:
return "PARSE_NODE_LIST_OF_NODES";
  case PARSE_NODE_STRUCT:
return "PARSE_NODE_STRUCT";
  case PARSE_NODE_UNION:
return "PARSE_NODE_UNION";
  case PARSE_NODE_TYPE:
return "PARSE_NODE_TYPE";
  case PARSE_NODE_LITERAL:
return "PARSE_NODE_LITERAL";
  case PARSE_NODE_FUNCTION:
return "PARSE_NODE_FUNCTION";
  case PARSE_NODE_FUNCTION_ARGUMENT:
return "PARSE_NODE_FUNCTION_ARGUMENT";
  case PARSE_NODE_TYPEDEF:
return "PARSE_NODE_TYPEDEF";
  case PARSE_NODE_UNPARSED_EXPRESSION:
return "PARSE_NODE_UNPARSED_EXPRESSION";
  case PARSE_NODE_ATTRIBUTE:
return "PARSE_NODE_ATTRIBUTE";
  case PARSE_NODE_CPP_INCLUDE:
return "PARSE_NODE_CPP_INCLUDE";
  case PARSE_NODE_CPP_DEFINE:
return "PARSE_NODE_CPP_DEFINE";
  case PARSE_NODE_OPERATOR:
return "PARSE_NODE_OPERATOR";
  case PARSE_NODE_IDENTIFIER:
return "PARSE_NODE_IDENTIFIER";
  case PARSE_NODE_IF_STATEMENT:
return "PARSE_NODE_IF_STATEMENT";
  case PARSE_NODE_FOR_STATEMENT:
return "PARSE_NODE_FOR_STATEMENT";
  case PARSE_NODE_DO_STATEMENT:
return "PARSE_NODE_DO_STATEMENT";
  case PARSE_NODE_WHILE_STATEMENT:
return "PARSE_NODE_WHILE_STATEMENT";
  case PARSE_NODE_EMPTY_STATEMENT:
return "PARSE_NODE_EMPTY_STATEMENT";
  case PARSE_NODE_BLOCK:
return "PARSE_NODE_BLOCK";
  case PARSE_NODE_RETURN_STATEMENT:
return "PARSE_NODE_RETURN_STATEMENT";
  case PARSE_NODE_SWITCH_STATEMENT:
return "PARSE_NODE_SWITCH_STATEMENT";
  case PARSE_NODE_CASE_LABEL:
return "PARSE_NODE_CASE_LABEL";
  case PARSE_NODE_DEFAULT_LABEL:
return "PARSE_NODE_DEFAULT_LABEL";
  case PARSE_NODE_GOTO_STATEMENT:
return "PARSE_NODE_GOTO_STATEMENT";
  case PARSE_NODE_BREAK_STATEMENT:
return "PARSE_NODE_BREAK_STATEMENT";
  case PARSE_NODE_CONTINUE_STATEMENT:
return "PARSE_NODE_CONTINUE_STATEMENT";
  case PARSE_NODE_LABEL_STATEMENT:
return "PARSE_NODE_LABEL_STATEMENT";
  case PARSE_NODE_VARIABLE_STATEMENT:
return "PARSE_NODE_VARIABLE_STATEMENT";
  case PARSE_NODE_EXPRESSION_STATEMENT:
return "PARSE_NODE_EXPRESSION_STATEMENT";
  case PARSE_NODE_BALANCED_CONSTRUCT:
return "PARSE_NODE_BALANCED_CONSTRUCT";
  case PARSE_NODE_CALL:
return "PARSE_NODE_CALL";
  case PARSE_NODE_CONDITIONAL:
return "PARSE_NODE_CONDITIONAL";
  default:
    return "<<unknown-parse_node_type>>";
  }
}
/* i=552 j=0 */
parse_node_type_t string_to_parse_node_type(char* value){
  if (strcmp(value, "PARSE_NODE_UNKNOWN") == 0) {
return PARSE_NODE_UNKNOWN;
  }
  if (strcmp(value, "PARSE_NODE_DECLARATIONS") == 0) {
return PARSE_NODE_DECLARATIONS;
  }
  if (strcmp(value, "PARSE_NODE_ENUM") == 0) {
return PARSE_NODE_ENUM;
  }
  if (strcmp(value, "PARSE_NODE_ENUM_ELEMENT") == 0) {
return PARSE_NODE_ENUM_ELEMENT;
  }
  if (strcmp(value, "PARSE_NODE_FIELD") == 0) {
return PARSE_NODE_FIELD;
  }
  if (strcmp(value, "PARSE_NODE_GLOBAL_FUNCTION") == 0) {
return PARSE_NODE_GLOBAL_FUNCTION;
  }
  if (strcmp(value, "PARSE_NODE_VARIABLE_DEFINITION") == 0) {
return PARSE_NODE_VARIABLE_DEFINITION;
  }
  if (strcmp(value, "PARSE_NODE_LIST_OF_NODES") == 0) {
return PARSE_NODE_LIST_OF_NODES;
  }
  if (strcmp(value, "PARSE_NODE_STRUCT") == 0) {
return PARSE_NODE_STRUCT;
  }
  if (strcmp(value, "PARSE_NODE_UNION") == 0) {
return PARSE_NODE_UNION;
  }
  if (strcmp(value, "PARSE_NODE_TYPE") == 0) {
return PARSE_NODE_TYPE;
  }
  if (strcmp(value, "PARSE_NODE_LITERAL") == 0) {
return PARSE_NODE_LITERAL;
  }
  if (strcmp(value, "PARSE_NODE_FUNCTION") == 0) {
return PARSE_NODE_FUNCTION;
  }
  if (strcmp(value, "PARSE_NODE_FUNCTION_ARGUMENT") == 0) {
return PARSE_NODE_FUNCTION_ARGUMENT;
  }
  if (strcmp(value, "PARSE_NODE_TYPEDEF") == 0) {
return PARSE_NODE_TYPEDEF;
  }
  if (strcmp(value, "PARSE_NODE_UNPARSED_EXPRESSION") == 0) {
return PARSE_NODE_UNPARSED_EXPRESSION;
  }
  if (strcmp(value, "PARSE_NODE_ATTRIBUTE") == 0) {
return PARSE_NODE_ATTRIBUTE;
  }
  if (strcmp(value, "PARSE_NODE_CPP_INCLUDE") == 0) {
return PARSE_NODE_CPP_INCLUDE;
  }
  if (strcmp(value, "PARSE_NODE_CPP_DEFINE") == 0) {
return PARSE_NODE_CPP_DEFINE;
  }
  if (strcmp(value, "PARSE_NODE_OPERATOR") == 0) {
return PARSE_NODE_OPERATOR;
  }
  if (strcmp(value, "PARSE_NODE_IDENTIFIER") == 0) {
return PARSE_NODE_IDENTIFIER;
  }
  if (strcmp(value, "PARSE_NODE_IF_STATEMENT") == 0) {
return PARSE_NODE_IF_STATEMENT;
  }
  if (strcmp(value, "PARSE_NODE_FOR_STATEMENT") == 0) {
return PARSE_NODE_FOR_STATEMENT;
  }
  if (strcmp(value, "PARSE_NODE_DO_STATEMENT") == 0) {
return PARSE_NODE_DO_STATEMENT;
  }
  if (strcmp(value, "PARSE_NODE_WHILE_STATEMENT") == 0) {
return PARSE_NODE_WHILE_STATEMENT;
  }
  if (strcmp(value, "PARSE_NODE_EMPTY_STATEMENT") == 0) {
return PARSE_NODE_EMPTY_STATEMENT;
  }
  if (strcmp(value, "PARSE_NODE_BLOCK") == 0) {
return PARSE_NODE_BLOCK;
  }
  if (strcmp(value, "PARSE_NODE_RETURN_STATEMENT") == 0) {
return PARSE_NODE_RETURN_STATEMENT;
  }
  if (strcmp(value, "PARSE_NODE_SWITCH_STATEMENT") == 0) {
return PARSE_NODE_SWITCH_STATEMENT;
  }
  if (strcmp(value, "PARSE_NODE_CASE_LABEL") == 0) {
return PARSE_NODE_CASE_LABEL;
  }
  if (strcmp(value, "PARSE_NODE_DEFAULT_LABEL") == 0) {
return PARSE_NODE_DEFAULT_LABEL;
  }
  if (strcmp(value, "PARSE_NODE_GOTO_STATEMENT") == 0) {
return PARSE_NODE_GOTO_STATEMENT;
  }
  if (strcmp(value, "PARSE_NODE_BREAK_STATEMENT") == 0) {
return PARSE_NODE_BREAK_STATEMENT;
  }
  if (strcmp(value, "PARSE_NODE_CONTINUE_STATEMENT") == 0) {
return PARSE_NODE_CONTINUE_STATEMENT;
  }
  if (strcmp(value, "PARSE_NODE_LABEL_STATEMENT") == 0) {
return PARSE_NODE_LABEL_STATEMENT;
  }
  if (strcmp(value, "PARSE_NODE_VARIABLE_STATEMENT") == 0) {
return PARSE_NODE_VARIABLE_STATEMENT;
  }
  if (strcmp(value, "PARSE_NODE_EXPRESSION_STATEMENT") == 0) {
return PARSE_NODE_EXPRESSION_STATEMENT;
  }
  if (strcmp(value, "PARSE_NODE_BALANCED_CONSTRUCT") == 0) {
return PARSE_NODE_BALANCED_CONSTRUCT;
  }
  if (strcmp(value, "PARSE_NODE_CALL") == 0) {
return PARSE_NODE_CALL;
  }
  if (strcmp(value, "PARSE_NODE_CONDITIONAL") == 0) {
return PARSE_NODE_CONDITIONAL;
  }
  return 0;
}
/* i=553 j=0 */
enum_metadata_t* parse_node_type_metadata(){
    static enum_element_metadata_t var_0 = (enum_element_metadata_t) {
        .next = NULL,
        .name = "PARSE_NODE_UNKNOWN",
        .value = PARSE_NODE_UNKNOWN
    };
    static enum_element_metadata_t var_1 = (enum_element_metadata_t) {
        .next = &var_0,
        .name = "PARSE_NODE_DECLARATIONS",
        .value = PARSE_NODE_DECLARATIONS
    };
    static enum_element_metadata_t var_2 = (enum_element_metadata_t) {
        .next = &var_1,
        .name = "PARSE_NODE_ENUM",
        .value = PARSE_NODE_ENUM
    };
    static enum_element_metadata_t var_3 = (enum_element_metadata_t) {
        .next = &var_2,
        .name = "PARSE_NODE_ENUM_ELEMENT",
        .value = PARSE_NODE_ENUM_ELEMENT
    };
    static enum_element_metadata_t var_4 = (enum_element_metadata_t) {
        .next = &var_3,
        .name = "PARSE_NODE_FIELD",
        .value = PARSE_NODE_FIELD
    };
    static enum_element_metadata_t var_5 = (enum_element_metadata_t) {
        .next = &var_4,
        .name = "PARSE_NODE_GLOBAL_FUNCTION",
        .value = PARSE_NODE_GLOBAL_FUNCTION
    };
    static enum_element_metadata_t var_6 = (enum_element_metadata_t) {
        .next = &var_5,
        .name = "PARSE_NODE_VARIABLE_DEFINITION",
        .value = PARSE_NODE_VARIABLE_DEFINITION
    };
    static enum_element_metadata_t var_7 = (enum_element_metadata_t) {
        .next = &var_6,
        .name = "PARSE_NODE_LIST_OF_NODES",
        .value = PARSE_NODE_LIST_OF_NODES
    };
    static enum_element_metadata_t var_8 = (enum_element_metadata_t) {
        .next = &var_7,
        .name = "PARSE_NODE_STRUCT",
        .value = PARSE_NODE_STRUCT
    };
    static enum_element_metadata_t var_9 = (enum_element_metadata_t) {
        .next = &var_8,
        .name = "PARSE_NODE_UNION",
        .value = PARSE_NODE_UNION
    };
    static enum_element_metadata_t var_10 = (enum_element_metadata_t) {
        .next = &var_9,
        .name = "PARSE_NODE_TYPE",
        .value = PARSE_NODE_TYPE
    };
    static enum_element_metadata_t var_11 = (enum_element_metadata_t) {
        .next = &var_10,
        .name = "PARSE_NODE_LITERAL",
        .value = PARSE_NODE_LITERAL
    };
    static enum_element_metadata_t var_12 = (enum_element_metadata_t) {
        .next = &var_11,
        .name = "PARSE_NODE_FUNCTION",
        .value = PARSE_NODE_FUNCTION
    };
    static enum_element_metadata_t var_13 = (enum_element_metadata_t) {
        .next = &var_12,
        .name = "PARSE_NODE_FUNCTION_ARGUMENT",
        .value = PARSE_NODE_FUNCTION_ARGUMENT
    };
    static enum_element_metadata_t var_14 = (enum_element_metadata_t) {
        .next = &var_13,
        .name = "PARSE_NODE_TYPEDEF",
        .value = PARSE_NODE_TYPEDEF
    };
    static enum_element_metadata_t var_15 = (enum_element_metadata_t) {
        .next = &var_14,
        .name = "PARSE_NODE_UNPARSED_EXPRESSION",
        .value = PARSE_NODE_UNPARSED_EXPRESSION
    };
    static enum_element_metadata_t var_16 = (enum_element_metadata_t) {
        .next = &var_15,
        .name = "PARSE_NODE_ATTRIBUTE",
        .value = PARSE_NODE_ATTRIBUTE
    };
    static enum_element_metadata_t var_17 = (enum_element_metadata_t) {
        .next = &var_16,
        .name = "PARSE_NODE_CPP_INCLUDE",
        .value = PARSE_NODE_CPP_INCLUDE
    };
    static enum_element_metadata_t var_18 = (enum_element_metadata_t) {
        .next = &var_17,
        .name = "PARSE_NODE_CPP_DEFINE",
        .value = PARSE_NODE_CPP_DEFINE
    };
    static enum_element_metadata_t var_19 = (enum_element_metadata_t) {
        .next = &var_18,
        .name = "PARSE_NODE_OPERATOR",
        .value = PARSE_NODE_OPERATOR
    };
    static enum_element_metadata_t var_20 = (enum_element_metadata_t) {
        .next = &var_19,
        .name = "PARSE_NODE_IDENTIFIER",
        .value = PARSE_NODE_IDENTIFIER
    };
    static enum_element_metadata_t var_21 = (enum_element_metadata_t) {
        .next = &var_20,
        .name = "PARSE_NODE_IF_STATEMENT",
        .value = PARSE_NODE_IF_STATEMENT
    };
    static enum_element_metadata_t var_22 = (enum_element_metadata_t) {
        .next = &var_21,
        .name = "PARSE_NODE_FOR_STATEMENT",
        .value = PARSE_NODE_FOR_STATEMENT
    };
    static enum_element_metadata_t var_23 = (enum_element_metadata_t) {
        .next = &var_22,
        .name = "PARSE_NODE_DO_STATEMENT",
        .value = PARSE_NODE_DO_STATEMENT
    };
    static enum_element_metadata_t var_24 = (enum_element_metadata_t) {
        .next = &var_23,
        .name = "PARSE_NODE_WHILE_STATEMENT",
        .value = PARSE_NODE_WHILE_STATEMENT
    };
    static enum_element_metadata_t var_25 = (enum_element_metadata_t) {
        .next = &var_24,
        .name = "PARSE_NODE_EMPTY_STATEMENT",
        .value = PARSE_NODE_EMPTY_STATEMENT
    };
    static enum_element_metadata_t var_26 = (enum_element_metadata_t) {
        .next = &var_25,
        .name = "PARSE_NODE_BLOCK",
        .value = PARSE_NODE_BLOCK
    };
    static enum_element_metadata_t var_27 = (enum_element_metadata_t) {
        .next = &var_26,
        .name = "PARSE_NODE_RETURN_STATEMENT",
        .value = PARSE_NODE_RETURN_STATEMENT
    };
    static enum_element_metadata_t var_28 = (enum_element_metadata_t) {
        .next = &var_27,
        .name = "PARSE_NODE_SWITCH_STATEMENT",
        .value = PARSE_NODE_SWITCH_STATEMENT
    };
    static enum_element_metadata_t var_29 = (enum_element_metadata_t) {
        .next = &var_28,
        .name = "PARSE_NODE_CASE_LABEL",
        .value = PARSE_NODE_CASE_LABEL
    };
    static enum_element_metadata_t var_30 = (enum_element_metadata_t) {
        .next = &var_29,
        .name = "PARSE_NODE_DEFAULT_LABEL",
        .value = PARSE_NODE_DEFAULT_LABEL
    };
    static enum_element_metadata_t var_31 = (enum_element_metadata_t) {
        .next = &var_30,
        .name = "PARSE_NODE_GOTO_STATEMENT",
        .value = PARSE_NODE_GOTO_STATEMENT
    };
    static enum_element_metadata_t var_32 = (enum_element_metadata_t) {
        .next = &var_31,
        .name = "PARSE_NODE_BREAK_STATEMENT",
        .value = PARSE_NODE_BREAK_STATEMENT
    };
    static enum_element_metadata_t var_33 = (enum_element_metadata_t) {
        .next = &var_32,
        .name = "PARSE_NODE_CONTINUE_STATEMENT",
        .value = PARSE_NODE_CONTINUE_STATEMENT
    };
    static enum_element_metadata_t var_34 = (enum_element_metadata_t) {
        .next = &var_33,
        .name = "PARSE_NODE_LABEL_STATEMENT",
        .value = PARSE_NODE_LABEL_STATEMENT
    };
    static enum_element_metadata_t var_35 = (enum_element_metadata_t) {
        .next = &var_34,
        .name = "PARSE_NODE_VARIABLE_STATEMENT",
        .value = PARSE_NODE_VARIABLE_STATEMENT
    };
    static enum_element_metadata_t var_36 = (enum_element_metadata_t) {
        .next = &var_35,
        .name = "PARSE_NODE_EXPRESSION_STATEMENT",
        .value = PARSE_NODE_EXPRESSION_STATEMENT
    };
    static enum_element_metadata_t var_37 = (enum_element_metadata_t) {
        .next = &var_36,
        .name = "PARSE_NODE_BALANCED_CONSTRUCT",
        .value = PARSE_NODE_BALANCED_CONSTRUCT
    };
    static enum_element_metadata_t var_38 = (enum_element_metadata_t) {
        .next = &var_37,
        .name = "PARSE_NODE_CALL",
        .value = PARSE_NODE_CALL
    };
    static enum_element_metadata_t var_39 = (enum_element_metadata_t) {
        .next = &var_38,
        .name = "PARSE_NODE_CONDITIONAL",
        .value = PARSE_NODE_CONDITIONAL
    };
    static enum_metadata_t enum_metadata_result = (enum_metadata_t) {
        .name = "parse_node_type_t",
        .elements = &var_39
    };
    return &enum_metadata_result;
}
/* i=554 j=0 */
char* pratt_parser_operation_to_string(pratt_parser_operation_t value){
  switch (value) {
    case PRATT_PARSE_UNKNOWN:
return "PRATT_PARSE_UNKNOWN";
  case PRATT_PARSE_PREFIX_OPERATOR:
return "PRATT_PARSE_PREFIX_OPERATOR";
  case PRATT_PARSE_BINARY_OPERATOR:
return "PRATT_PARSE_BINARY_OPERATOR";
  case PRATT_PARSE_POSTFIX_OPERATOR:
return "PRATT_PARSE_POSTFIX_OPERATOR";
  case PRATT_PARSE_CONDITIONAL:
return "PRATT_PARSE_CONDITIONAL";
  case PRATT_PARSE_IDENTIFIER:
return "PRATT_PARSE_IDENTIFIER";
  case PRATT_PARSE_LITERAL:
return "PRATT_PARSE_LITERAL";
  case PRATT_PARSE_SUB_EXPRESSION:
return "PRATT_PARSE_SUB_EXPRESSION";
  case PRATT_PARSE_INDEX_EXPRESSION:
return "PRATT_PARSE_INDEX_EXPRESSION";
  case PRATT_PARSE_SIZEOF:
return "PRATT_PARSE_SIZEOF";
  case PRATT_PARSE_CAST_MACRO:
return "PRATT_PARSE_CAST_MACRO";
  case PRATT_PARSE_TYPE_OF:
return "PRATT_PARSE_TYPE_OF";
  case PRATT_PARSE_BLOCK_EXPR:
return "PRATT_PARSE_BLOCK_EXPR";
  case PRATT_PARSE_CALL:
return "PRATT_PARSE_CALL";
  default:
    return "<<unknown-pratt_parser_operation>>";
  }
}
/* i=555 j=0 */
pratt_parser_operation_t string_to_pratt_parser_operation(char* value){
  if (strcmp(value, "PRATT_PARSE_UNKNOWN") == 0) {
return PRATT_PARSE_UNKNOWN;
  }
  if (strcmp(value, "PRATT_PARSE_PREFIX_OPERATOR") == 0) {
return PRATT_PARSE_PREFIX_OPERATOR;
  }
  if (strcmp(value, "PRATT_PARSE_BINARY_OPERATOR") == 0) {
return PRATT_PARSE_BINARY_OPERATOR;
  }
  if (strcmp(value, "PRATT_PARSE_POSTFIX_OPERATOR") == 0) {
return PRATT_PARSE_POSTFIX_OPERATOR;
  }
  if (strcmp(value, "PRATT_PARSE_CONDITIONAL") == 0) {
return PRATT_PARSE_CONDITIONAL;
  }
  if (strcmp(value, "PRATT_PARSE_IDENTIFIER") == 0) {
return PRATT_PARSE_IDENTIFIER;
  }
  if (strcmp(value, "PRATT_PARSE_LITERAL") == 0) {
return PRATT_PARSE_LITERAL;
  }
  if (strcmp(value, "PRATT_PARSE_SUB_EXPRESSION") == 0) {
return PRATT_PARSE_SUB_EXPRESSION;
  }
  if (strcmp(value, "PRATT_PARSE_INDEX_EXPRESSION") == 0) {
return PRATT_PARSE_INDEX_EXPRESSION;
  }
  if (strcmp(value, "PRATT_PARSE_SIZEOF") == 0) {
return PRATT_PARSE_SIZEOF;
  }
  if (strcmp(value, "PRATT_PARSE_CAST_MACRO") == 0) {
return PRATT_PARSE_CAST_MACRO;
  }
  if (strcmp(value, "PRATT_PARSE_TYPE_OF") == 0) {
return PRATT_PARSE_TYPE_OF;
  }
  if (strcmp(value, "PRATT_PARSE_BLOCK_EXPR") == 0) {
return PRATT_PARSE_BLOCK_EXPR;
  }
  if (strcmp(value, "PRATT_PARSE_CALL") == 0) {
return PRATT_PARSE_CALL;
  }
  return 0;
}
/* i=556 j=0 */
enum_metadata_t* pratt_parser_operation_metadata(){
    static enum_element_metadata_t var_0 = (enum_element_metadata_t) {
        .next = NULL,
        .name = "PRATT_PARSE_UNKNOWN",
        .value = PRATT_PARSE_UNKNOWN
    };
    static enum_element_metadata_t var_1 = (enum_element_metadata_t) {
        .next = &var_0,
        .name = "PRATT_PARSE_PREFIX_OPERATOR",
        .value = PRATT_PARSE_PREFIX_OPERATOR
    };
    static enum_element_metadata_t var_2 = (enum_element_metadata_t) {
        .next = &var_1,
        .name = "PRATT_PARSE_BINARY_OPERATOR",
        .value = PRATT_PARSE_BINARY_OPERATOR
    };
    static enum_element_metadata_t var_3 = (enum_element_metadata_t) {
        .next = &var_2,
        .name = "PRATT_PARSE_POSTFIX_OPERATOR",
        .value = PRATT_PARSE_POSTFIX_OPERATOR
    };
    static enum_element_metadata_t var_4 = (enum_element_metadata_t) {
        .next = &var_3,
        .name = "PRATT_PARSE_CONDITIONAL",
        .value = PRATT_PARSE_CONDITIONAL
    };
    static enum_element_metadata_t var_5 = (enum_element_metadata_t) {
        .next = &var_4,
        .name = "PRATT_PARSE_IDENTIFIER",
        .value = PRATT_PARSE_IDENTIFIER
    };
    static enum_element_metadata_t var_6 = (enum_element_metadata_t) {
        .next = &var_5,
        .name = "PRATT_PARSE_LITERAL",
        .value = PRATT_PARSE_LITERAL
    };
    static enum_element_metadata_t var_7 = (enum_element_metadata_t) {
        .next = &var_6,
        .name = "PRATT_PARSE_SUB_EXPRESSION",
        .value = PRATT_PARSE_SUB_EXPRESSION
    };
    static enum_element_metadata_t var_8 = (enum_element_metadata_t) {
        .next = &var_7,
        .name = "PRATT_PARSE_INDEX_EXPRESSION",
        .value = PRATT_PARSE_INDEX_EXPRESSION
    };
    static enum_element_metadata_t var_9 = (enum_element_metadata_t) {
        .next = &var_8,
        .name = "PRATT_PARSE_SIZEOF",
        .value = PRATT_PARSE_SIZEOF
    };
    static enum_element_metadata_t var_10 = (enum_element_metadata_t) {
        .next = &var_9,
        .name = "PRATT_PARSE_CAST_MACRO",
        .value = PRATT_PARSE_CAST_MACRO
    };
    static enum_element_metadata_t var_11 = (enum_element_metadata_t) {
        .next = &var_10,
        .name = "PRATT_PARSE_TYPE_OF",
        .value = PRATT_PARSE_TYPE_OF
    };
    static enum_element_metadata_t var_12 = (enum_element_metadata_t) {
        .next = &var_11,
        .name = "PRATT_PARSE_BLOCK_EXPR",
        .value = PRATT_PARSE_BLOCK_EXPR
    };
    static enum_element_metadata_t var_13 = (enum_element_metadata_t) {
        .next = &var_12,
        .name = "PRATT_PARSE_CALL",
        .value = PRATT_PARSE_CALL
    };
    static enum_metadata_t enum_metadata_result = (enum_metadata_t) {
        .name = "pratt_parser_operation_t",
        .elements = &var_13
    };
    return &enum_metadata_result;
}
/* i=557 j=0 */
char* associativity_to_string(associativity_t value){
  switch (value) {
    case LEFT_TO_RIGHT:
return "LEFT_TO_RIGHT";
  case RIGHT_TO_LEFT:
return "RIGHT_TO_LEFT";
  default:
    return "<<unknown-associativity>>";
  }
}
/* i=558 j=0 */
associativity_t string_to_associativity(char* value){
  if (strcmp(value, "LEFT_TO_RIGHT") == 0) {
return LEFT_TO_RIGHT;
  }
  if (strcmp(value, "RIGHT_TO_LEFT") == 0) {
return RIGHT_TO_LEFT;
  }
  return 0;
}
/* i=559 j=0 */
enum_metadata_t* associativity_metadata(){
    static enum_element_metadata_t var_0 = (enum_element_metadata_t) {
        .next = NULL,
        .name = "LEFT_TO_RIGHT",
        .value = LEFT_TO_RIGHT
    };
    static enum_element_metadata_t var_1 = (enum_element_metadata_t) {
        .next = &var_0,
        .name = "RIGHT_TO_LEFT",
        .value = RIGHT_TO_LEFT
    };
    static enum_metadata_t enum_metadata_result = (enum_metadata_t) {
        .name = "associativity_t",
        .elements = &var_1
    };
    return &enum_metadata_result;
}
/* i=560 j=0 */
char* precedence_to_string(precedence_t value){
  switch (value) {
    case PRECEDENCE_UNKNOWN:
return "PRECEDENCE_UNKNOWN";
  case PRECEDENCE_COMMA:
return "PRECEDENCE_COMMA";
  case PRECEDENCE_ASSIGNMENT:
return "PRECEDENCE_ASSIGNMENT";
  case PRECEDENCE_CONDITIONAL:
return "PRECEDENCE_CONDITIONAL";
  case PRECEDENCE_LOGICAL_OR:
return "PRECEDENCE_LOGICAL_OR";
  case PRECEDENCE_LOGICAL_AND:
return "PRECEDENCE_LOGICAL_AND";
  case PRECEDENCE_OR:
return "PRECEDENCE_OR";
  case PRECEDENCE_XOR:
return "PRECEDENCE_XOR";
  case PRECEDENCE_AND:
return "PRECEDENCE_AND";
  case PRECEDENCE_EQUALITY:
return "PRECEDENCE_EQUALITY";
  case PRECEDENCE_RELATIONAL:
return "PRECEDENCE_RELATIONAL";
  case PRECEDENCE_SHIFT:
return "PRECEDENCE_SHIFT";
  case PRECEDENCE_ADDITIVE:
return "PRECEDENCE_ADDITIVE";
  case PRECEDENCE_MULTIPICITIVE:
return "PRECEDENCE_MULTIPICITIVE";
  case PRECEDENCE_UNARY:
return "PRECEDENCE_UNARY";
  case PRECEDENCE_POSTFIX:
return "PRECEDENCE_POSTFIX";
  case PRECEDENCE_PRIMARY:
return "PRECEDENCE_PRIMARY";
  default:
    return "<<unknown-precedence>>";
  }
}
/* i=561 j=0 */
precedence_t string_to_precedence(char* value){
  if (strcmp(value, "PRECEDENCE_UNKNOWN") == 0) {
return PRECEDENCE_UNKNOWN;
  }
  if (strcmp(value, "PRECEDENCE_COMMA") == 0) {
return PRECEDENCE_COMMA;
  }
  if (strcmp(value, "PRECEDENCE_ASSIGNMENT") == 0) {
return PRECEDENCE_ASSIGNMENT;
  }
  if (strcmp(value, "PRECEDENCE_CONDITIONAL") == 0) {
return PRECEDENCE_CONDITIONAL;
  }
  if (strcmp(value, "PRECEDENCE_LOGICAL_OR") == 0) {
return PRECEDENCE_LOGICAL_OR;
  }
  if (strcmp(value, "PRECEDENCE_LOGICAL_AND") == 0) {
return PRECEDENCE_LOGICAL_AND;
  }
  if (strcmp(value, "PRECEDENCE_OR") == 0) {
return PRECEDENCE_OR;
  }
  if (strcmp(value, "PRECEDENCE_XOR") == 0) {
return PRECEDENCE_XOR;
  }
  if (strcmp(value, "PRECEDENCE_AND") == 0) {
return PRECEDENCE_AND;
  }
  if (strcmp(value, "PRECEDENCE_EQUALITY") == 0) {
return PRECEDENCE_EQUALITY;
  }
  if (strcmp(value, "PRECEDENCE_RELATIONAL") == 0) {
return PRECEDENCE_RELATIONAL;
  }
  if (strcmp(value, "PRECEDENCE_SHIFT") == 0) {
return PRECEDENCE_SHIFT;
  }
  if (strcmp(value, "PRECEDENCE_ADDITIVE") == 0) {
return PRECEDENCE_ADDITIVE;
  }
  if (strcmp(value, "PRECEDENCE_MULTIPICITIVE") == 0) {
return PRECEDENCE_MULTIPICITIVE;
  }
  if (strcmp(value, "PRECEDENCE_UNARY") == 0) {
return PRECEDENCE_UNARY;
  }
  if (strcmp(value, "PRECEDENCE_POSTFIX") == 0) {
return PRECEDENCE_POSTFIX;
  }
  if (strcmp(value, "PRECEDENCE_PRIMARY") == 0) {
return PRECEDENCE_PRIMARY;
  }
  return 0;
}
/* i=562 j=0 */
enum_metadata_t* precedence_metadata(){
    static enum_element_metadata_t var_0 = (enum_element_metadata_t) {
        .next = NULL,
        .name = "PRECEDENCE_UNKNOWN",
        .value = PRECEDENCE_UNKNOWN
    };
    static enum_element_metadata_t var_1 = (enum_element_metadata_t) {
        .next = &var_0,
        .name = "PRECEDENCE_COMMA",
        .value = PRECEDENCE_COMMA
    };
    static enum_element_metadata_t var_2 = (enum_element_metadata_t) {
        .next = &var_1,
        .name = "PRECEDENCE_ASSIGNMENT",
        .value = PRECEDENCE_ASSIGNMENT
    };
    static enum_element_metadata_t var_3 = (enum_element_metadata_t) {
        .next = &var_2,
        .name = "PRECEDENCE_CONDITIONAL",
        .value = PRECEDENCE_CONDITIONAL
    };
    static enum_element_metadata_t var_4 = (enum_element_metadata_t) {
        .next = &var_3,
        .name = "PRECEDENCE_LOGICAL_OR",
        .value = PRECEDENCE_LOGICAL_OR
    };
    static enum_element_metadata_t var_5 = (enum_element_metadata_t) {
        .next = &var_4,
        .name = "PRECEDENCE_LOGICAL_AND",
        .value = PRECEDENCE_LOGICAL_AND
    };
    static enum_element_metadata_t var_6 = (enum_element_metadata_t) {
        .next = &var_5,
        .name = "PRECEDENCE_OR",
        .value = PRECEDENCE_OR
    };
    static enum_element_metadata_t var_7 = (enum_element_metadata_t) {
        .next = &var_6,
        .name = "PRECEDENCE_XOR",
        .value = PRECEDENCE_XOR
    };
    static enum_element_metadata_t var_8 = (enum_element_metadata_t) {
        .next = &var_7,
        .name = "PRECEDENCE_AND",
        .value = PRECEDENCE_AND
    };
    static enum_element_metadata_t var_9 = (enum_element_metadata_t) {
        .next = &var_8,
        .name = "PRECEDENCE_EQUALITY",
        .value = PRECEDENCE_EQUALITY
    };
    static enum_element_metadata_t var_10 = (enum_element_metadata_t) {
        .next = &var_9,
        .name = "PRECEDENCE_RELATIONAL",
        .value = PRECEDENCE_RELATIONAL
    };
    static enum_element_metadata_t var_11 = (enum_element_metadata_t) {
        .next = &var_10,
        .name = "PRECEDENCE_SHIFT",
        .value = PRECEDENCE_SHIFT
    };
    static enum_element_metadata_t var_12 = (enum_element_metadata_t) {
        .next = &var_11,
        .name = "PRECEDENCE_ADDITIVE",
        .value = PRECEDENCE_ADDITIVE
    };
    static enum_element_metadata_t var_13 = (enum_element_metadata_t) {
        .next = &var_12,
        .name = "PRECEDENCE_MULTIPICITIVE",
        .value = PRECEDENCE_MULTIPICITIVE
    };
    static enum_element_metadata_t var_14 = (enum_element_metadata_t) {
        .next = &var_13,
        .name = "PRECEDENCE_UNARY",
        .value = PRECEDENCE_UNARY
    };
    static enum_element_metadata_t var_15 = (enum_element_metadata_t) {
        .next = &var_14,
        .name = "PRECEDENCE_POSTFIX",
        .value = PRECEDENCE_POSTFIX
    };
    static enum_element_metadata_t var_16 = (enum_element_metadata_t) {
        .next = &var_15,
        .name = "PRECEDENCE_PRIMARY",
        .value = PRECEDENCE_PRIMARY
    };
    static enum_metadata_t enum_metadata_result = (enum_metadata_t) {
        .name = "precedence_t",
        .elements = &var_16
    };
    return &enum_metadata_result;
}
/* i=563 j=0 */
char* type_qualifier_to_string(type_qualifier_t value){
  switch (value) {
    case TYPE_QUALIFIER_NONE:
return "TYPE_QUALIFIER_NONE";
  case TYPE_QUALIFIER_CONST:
return "TYPE_QUALIFIER_CONST";
  case TYPE_QUALIFIER_VOLATILE:
return "TYPE_QUALIFIER_VOLATILE";
  case TYPE_QUALIFIER_RESTRICT:
return "TYPE_QUALIFIER_RESTRICT";
  default:
    return "<<unknown-type_qualifier>>";
  }
}
/* i=564 j=0 */
type_qualifier_t string_to_type_qualifier(char* value){
  if (strcmp(value, "TYPE_QUALIFIER_NONE") == 0) {
return TYPE_QUALIFIER_NONE;
  }
  if (strcmp(value, "TYPE_QUALIFIER_CONST") == 0) {
return TYPE_QUALIFIER_CONST;
  }
  if (strcmp(value, "TYPE_QUALIFIER_VOLATILE") == 0) {
return TYPE_QUALIFIER_VOLATILE;
  }
  if (strcmp(value, "TYPE_QUALIFIER_RESTRICT") == 0) {
return TYPE_QUALIFIER_RESTRICT;
  }
  return 0;
}
/* i=565 j=0 */
enum_metadata_t* type_qualifier_metadata(){
    static enum_element_metadata_t var_0 = (enum_element_metadata_t) {
        .next = NULL,
        .name = "TYPE_QUALIFIER_NONE",
        .value = TYPE_QUALIFIER_NONE
    };
    static enum_element_metadata_t var_1 = (enum_element_metadata_t) {
        .next = &var_0,
        .name = "TYPE_QUALIFIER_CONST",
        .value = TYPE_QUALIFIER_CONST
    };
    static enum_element_metadata_t var_2 = (enum_element_metadata_t) {
        .next = &var_1,
        .name = "TYPE_QUALIFIER_VOLATILE",
        .value = TYPE_QUALIFIER_VOLATILE
    };
    static enum_element_metadata_t var_3 = (enum_element_metadata_t) {
        .next = &var_2,
        .name = "TYPE_QUALIFIER_RESTRICT",
        .value = TYPE_QUALIFIER_RESTRICT
    };
    static enum_metadata_t enum_metadata_result = (enum_metadata_t) {
        .name = "type_qualifier_t",
        .elements = &var_3
    };
    return &enum_metadata_result;
}
/* i=566 j=0 */
char* type_node_kind_to_string(type_node_kind_t value){
  switch (value) {
    case TYPE_NODE_KIND_UNKNOWN:
return "TYPE_NODE_KIND_UNKNOWN";
  case TYPE_NODE_KIND_POINTER:
return "TYPE_NODE_KIND_POINTER";
  case TYPE_NODE_KIND_ARRAY:
return "TYPE_NODE_KIND_ARRAY";
  case TYPE_NODE_KIND_SIZED_ARRAY:
return "TYPE_NODE_KIND_SIZED_ARRAY";
  case TYPE_NODE_KIND_VARIABLE_SIZED_ARRAY:
return "TYPE_NODE_KIND_VARIABLE_SIZED_ARRAY";
  case TYPE_NODE_KIND_PRIMITIVE_TYPENAME:
return "TYPE_NODE_KIND_PRIMITIVE_TYPENAME";
  case TYPE_NODE_KIND_TYPENAME:
return "TYPE_NODE_KIND_TYPENAME";
  case TYPE_NODE_KIND_TYPE_EXPRESSION:
return "TYPE_NODE_KIND_TYPE_EXPRESSION";
  case TYPE_NODE_KIND_TYPEOF:
return "TYPE_NODE_KIND_TYPEOF";
  default:
    return "<<unknown-type_node_kind>>";
  }
}
/* i=567 j=0 */
type_node_kind_t string_to_type_node_kind(char* value){
  if (strcmp(value, "TYPE_NODE_KIND_UNKNOWN") == 0) {
return TYPE_NODE_KIND_UNKNOWN;
  }
  if (strcmp(value, "TYPE_NODE_KIND_POINTER") == 0) {
return TYPE_NODE_KIND_POINTER;
  }
  if (strcmp(value, "TYPE_NODE_KIND_ARRAY") == 0) {
return TYPE_NODE_KIND_ARRAY;
  }
  if (strcmp(value, "TYPE_NODE_KIND_SIZED_ARRAY") == 0) {
return TYPE_NODE_KIND_SIZED_ARRAY;
  }
  if (strcmp(value, "TYPE_NODE_KIND_VARIABLE_SIZED_ARRAY") == 0) {
return TYPE_NODE_KIND_VARIABLE_SIZED_ARRAY;
  }
  if (strcmp(value, "TYPE_NODE_KIND_PRIMITIVE_TYPENAME") == 0) {
return TYPE_NODE_KIND_PRIMITIVE_TYPENAME;
  }
  if (strcmp(value, "TYPE_NODE_KIND_TYPENAME") == 0) {
return TYPE_NODE_KIND_TYPENAME;
  }
  if (strcmp(value, "TYPE_NODE_KIND_TYPE_EXPRESSION") == 0) {
return TYPE_NODE_KIND_TYPE_EXPRESSION;
  }
  if (strcmp(value, "TYPE_NODE_KIND_TYPEOF") == 0) {
return TYPE_NODE_KIND_TYPEOF;
  }
  return 0;
}
/* i=568 j=0 */
enum_metadata_t* type_node_kind_metadata(){
    static enum_element_metadata_t var_0 = (enum_element_metadata_t) {
        .next = NULL,
        .name = "TYPE_NODE_KIND_UNKNOWN",
        .value = TYPE_NODE_KIND_UNKNOWN
    };
    static enum_element_metadata_t var_1 = (enum_element_metadata_t) {
        .next = &var_0,
        .name = "TYPE_NODE_KIND_POINTER",
        .value = TYPE_NODE_KIND_POINTER
    };
    static enum_element_metadata_t var_2 = (enum_element_metadata_t) {
        .next = &var_1,
        .name = "TYPE_NODE_KIND_ARRAY",
        .value = TYPE_NODE_KIND_ARRAY
    };
    static enum_element_metadata_t var_3 = (enum_element_metadata_t) {
        .next = &var_2,
        .name = "TYPE_NODE_KIND_SIZED_ARRAY",
        .value = TYPE_NODE_KIND_SIZED_ARRAY
    };
    static enum_element_metadata_t var_4 = (enum_element_metadata_t) {
        .next = &var_3,
        .name = "TYPE_NODE_KIND_VARIABLE_SIZED_ARRAY",
        .value = TYPE_NODE_KIND_VARIABLE_SIZED_ARRAY
    };
    static enum_element_metadata_t var_5 = (enum_element_metadata_t) {
        .next = &var_4,
        .name = "TYPE_NODE_KIND_PRIMITIVE_TYPENAME",
        .value = TYPE_NODE_KIND_PRIMITIVE_TYPENAME
    };
    static enum_element_metadata_t var_6 = (enum_element_metadata_t) {
        .next = &var_5,
        .name = "TYPE_NODE_KIND_TYPENAME",
        .value = TYPE_NODE_KIND_TYPENAME
    };
    static enum_element_metadata_t var_7 = (enum_element_metadata_t) {
        .next = &var_6,
        .name = "TYPE_NODE_KIND_TYPE_EXPRESSION",
        .value = TYPE_NODE_KIND_TYPE_EXPRESSION
    };
    static enum_element_metadata_t var_8 = (enum_element_metadata_t) {
        .next = &var_7,
        .name = "TYPE_NODE_KIND_TYPEOF",
        .value = TYPE_NODE_KIND_TYPEOF
    };
    static enum_metadata_t enum_metadata_result = (enum_metadata_t) {
        .name = "type_node_kind_t",
        .elements = &var_8
    };
    return &enum_metadata_result;
}

