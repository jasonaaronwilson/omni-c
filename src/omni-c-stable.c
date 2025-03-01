// -*- buffer-read-only: t -*-
//
// This is a generated file, so you generally don't want to edit it!
// The bottom of the file has more information about it's creation.


#ifndef _REFLECTION_H_
#define _REFLECTION_H_

#define fn_t(return_type, ...) typeof(return_type(*)(__VA_ARGS__))

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
#include <gc.h>
#include <stdarg.h>
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
#include <ctype.h>

// ========== defines ==========

#define _OMNI_C_H_

#define cast(type, expr) ((type) (expr))

#define block_expr(block) block

#define _MIN_MAX_H_

#define min(a, b) ((a) < (b) ? (a) : (b))

#define max(a, b) ((a) > (b) ? (a) : (b))

#define compound_literal(type, ...) ((type) __VA_ARGS__)

#define _FN_H_

#define fn_t(return_type, ...) typeof(return_type(*)(__VA_ARGS__))

#define _LEB128_H_

#define ERROR_INSUFFICIENT_INPUT -1

#define ERROR_TOO_BIG -2

#define _FATAL_ERROR_H_

#define fatal_error(code) fatal_error_impl(__FILE__, __LINE__, code)

#define boolean_to_value(x) compound_literal(value_t, {.u64 = x})

#define u64_to_value(x) compound_literal(value_t, {.u64 = x})

#define i64_to_value(x) compound_literal(value_t, {.i64 = x})

#define str_to_value(x) compound_literal(value_t, {.str = x})

#define ptr_to_value(x) compound_literal(value_t, {.ptr = x})

#define dbl_to_value(x) compound_literal(value_t, {.dbl = x})

#define malloc_bytes(amount) (checked_malloc(__FILE__, __LINE__, amount))

#define free_bytes(ptr) (checked_free(__FILE__, __LINE__, ptr))

#define malloc_struct(struct_name)                                             \
  ((struct_name*) (checked_malloc(__FILE__, __LINE__, sizeof(struct_name))))

#define malloc_copy_of(source, number_of_bytes)                                \
  (checked_malloc_copy_of(__FILE__, __LINE__, source, number_of_bytes))

#define ARMYKNIFE_MEMORY_ALLOCATION_MAXIMUM_AMOUNT (1L << 48)

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

#define _SUB_PROCESS_H_

#define _SPLITJOIN_H_

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

typedef struct logger_state_S logger_state_t;

typedef struct utf8_decode_result_S utf8_decode_result_t;

typedef struct buffer_t__generated_S buffer_t;

typedef struct line_and_column_t__generated_S line_and_column_t;

typedef struct byte_stream_source_t__generated_S byte_stream_source_t;

typedef struct byte_stream_target_t__generated_S byte_stream_target_t;

typedef struct buffer_byte_stream_source_data_t__generated_S buffer_byte_stream_source_data_t;

typedef struct cstring_byte_stream_source_data_t__generated_S cstring_byte_stream_source_data_t;

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

typedef fn_t(boolean_t, FILE*, string_tree_t*, int64_t, void*) oarchive_stream_headers_callback_t;

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
  PARSE_NODE_COMPOUND_LITERAL,
  PARSE_NODE_DESIGNATED_INITIALIZER,
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

typedef struct literal_node_t__generated_S literal_node_t;

typedef struct compound_literal_node_t__generated_S compound_literal_node_t;

typedef struct designated_initializer_node_t__generated_S designated_initializer_node_t;

typedef struct balanced_construct_node_S balanced_construct_node_t;

typedef struct printer_S printer_t;

// ========== stuctures/unions ==========

struct fatal_error_config_S {
  boolean_t catch_sigsegv;
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

struct buffer_t__generated_S {
  uint32_t length;
  uint32_t capacity;
  uint8_t* elements;
};

struct line_and_column_t__generated_S {
  uint64_t line;
  uint64_t column;
};

struct byte_stream_source_t__generated_S {
  fn_t(uint8_t, byte_stream_source_t*, boolean_t*) read_byte;
  void* data;
};

struct byte_stream_target_t__generated_S {
  fn_t(byte_stream_target_t*, byte_stream_target_t*, uint8_t) write_byte;
  void* data;
};

struct buffer_byte_stream_source_data_t__generated_S {
  buffer_t* buffer;
  uint64_t position;
};

struct cstring_byte_stream_source_data_t__generated_S {
  char* string;
  uint64_t length;
  uint64_t position;
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

struct parse_node_S {
  parse_node_type_t tag;
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
  symbol_table_t* parent;
  value_array_t* files;
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
  token_t* first_token;
  parse_node_t* if_condition;
  parse_node_t* if_true;
  parse_node_t* if_else;
};

struct for_statement_node_t__generated_S {
  parse_node_type_t tag;
  token_t* first_token;
  parse_node_t* for_init;
  parse_node_t* for_test;
  parse_node_t* for_increment;
  parse_node_t* for_body;
};

struct do_statement_node_t__generated_S {
  parse_node_type_t tag;
  token_t* first_token;
  parse_node_t* body;
  parse_node_t* condition;
};

struct while_statement_node_t__generated_S {
  parse_node_type_t tag;
  token_t* first_token;
  parse_node_t* condition;
  parse_node_t* body;
};

struct empty_statement_node_t__generated_S {
  parse_node_type_t tag;
  token_t* semi_colon_token;
};

struct block_node_t__generated_S {
  parse_node_type_t tag;
  token_t* first_token;
  node_list_t statements;
};

struct return_statement_node_t__generated_S {
  parse_node_type_t tag;
  token_t* first_token;
  parse_node_t* expression;
};

struct switch_statement_node_t__generated_S {
  parse_node_type_t tag;
  token_t* first_token;
  parse_node_t* expression;
  parse_node_t* block;
};

struct case_label_node_t__generated_S {
  parse_node_type_t tag;
  token_t* first_token;
  parse_node_t* expression;
};

struct default_label_node_t__generated_S {
  parse_node_type_t tag;
  token_t* default_token;
};

struct goto_statement_node_t__generated_S {
  parse_node_type_t tag;
  token_t* first_token;
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
  token_t* first_token;
  parse_node_t* expression;
};

struct expression_statement_node_t__generated_S {
  parse_node_type_t tag;
  token_t* first_token;
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
  value_array_t* suffixes;
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
  parse_node_t* value_expr;
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

struct literal_node_t__generated_S {
  parse_node_type_t tag;
  token_t* token;
  value_array_t* tokens;
  parse_node_t* initializer_node;
  parse_node_t* initializer_type;
};

struct compound_literal_node_t__generated_S {
  parse_node_type_t tag;
  parse_node_t* type_node;
  node_list_t initializers;
};

struct designated_initializer_node_t__generated_S {
  parse_node_type_t tag;
  parse_node_t* index_expression;
  token_t* member_name;
  parse_node_t* value;
};

struct balanced_construct_node_S {
  parse_node_type_t tag;
  token_t* start_token;
  token_t* end_token;
};

struct printer_S {
  symbol_table_t* symbol_table;
  buffer_t* buffer;
  uint32_t indent_width;
  uint32_t indent_level;
  boolean_t convert_nullptr;
  boolean_t output_line_directives;
};

// ========== global variables ==========

fatal_error_config_t fatal_error_config = {0};

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

string_hashtable_t* c_keywords_ht = ((void *)0);

string_hashtable_t* c_builtin_types_ht = ((void *)0);

string_hashtable_t* cpp_keywords_ht = ((void *)0);

string_hashtable_t* cpp_builtin_types_ht = ((void *)0);

string_hashtable_t* oc_keywords_ht = ((void *)0);

string_hashtable_t* oc_builtin_types_ht = ((void *)0);

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

    // Since we match from top-to-bottom, we generally want the
    // longest tokens first.

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

value_array_t* FLAG_files = ((void *)0);

char* FLAG_command = ((void *)0);

boolean_t FLAG_print_command_line = true;

boolean_t FLAG_include_unnamed_nodes = false;

boolean_t FLAG_print_tokens_show_tokens = false;

boolean_t FLAG_print_tokens_include_whitespace = false;

boolean_t FLAG_print_tokens_include_comments = false;

boolean_t FLAG_print_tokens_parse_and_print = true;

boolean_t FLAG_print_tokens_show_appended_tokens = true;

char* FLAG_c_output_file = ((void *)0);

char* FLAG_binary_output_file = ((void *)0);

char* FLAG_archive_output_file = ((void *)0);

boolean_t FLAG_generate_enum_convertors = true;

char* FLAG_expression = ((void *)0);

char* FLAG_statement = ((void *)0);

boolean_t FLAG_dump_symbol_table = false;

boolean_t FLAG_use_statement_parser = true;

boolean_t FLAG_to_c = false;

boolean_t FLAG_omit_c_armyknife_include = false;

char* FLAG_c_compiler = "clang";

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
buffer_t* make_buffer(uint64_t initial_capacity);
uint64_t buffer_length(buffer_t* array);
void buffer_clear(buffer_t* buffer);
uint8_t buffer_get(buffer_t* buffer, uint64_t position);
char* buffer_c_substring(buffer_t* buffer, uint64_t start, uint64_t end);
char* buffer_to_c_string(buffer_t* buffer);
buffer_t* buffer_append_byte(buffer_t* buffer, uint8_t element);
buffer_t* buffer_append_bytes(buffer_t* buffer, uint8_t* bytes, uint64_t n_bytes);
buffer_t* buffer_append_string(buffer_t* buffer, const char* str);
extern buffer_t* buffer_increase_capacity(buffer_t* buffer, uint64_t capacity);
__attribute__((format(printf, 2, 3))) buffer_t* buffer_printf(buffer_t* buffer, char* format, ...);
extern buffer_t* buffer_append_repeated_byte(buffer_t* buffer, uint8_t byte, int count);
utf8_decode_result_t buffer_utf8_decode(buffer_t* buffer, uint64_t position);
extern buffer_t* buffer_append_code_point(buffer_t* buffer, uint32_t code_point);
boolean_t buffer_match_string_at(buffer_t* buffer, uint64_t start_position, char* str);
buffer_t* buffer_from_string(char* string);
buffer_t* buffer_adjust_region(buffer_t* buffer, uint64_t start, uint64_t end, uint64_t new_width);
buffer_t* buffer_replace_all(buffer_t* buffer, char* original_text, char* replacement_text);
line_and_column_t buffer_position_to_line_and_column(buffer_t* buffer, uint64_t position);
boolean_t buffer_region_contains(buffer_t* buffer, uint64_t start, uint64_t end, char* text);
buffer_t* buffer_replace_matching_byte(buffer_t* buffer, uint8_t original, uint8_t replacement);
uint64_t buffer_beginning_of_line(buffer_t* buffer, uint64_t start);
uint64_t buffer_end_of_line(buffer_t* buffer, uint64_t start);
extern buffer_t* buffer_append_buffer(buffer_t* buffer, buffer_t* src_buffer);
extern buffer_t* buffer_append_sub_buffer(buffer_t* buffer, uint64_t start_position, uint64_t end_position, buffer_t* src_buffer);
buffer_t* buffer_to_uppercase(buffer_t* buffer);
buffer_t* buffer_to_lowercase(buffer_t* buffer);
byte_stream_source_t* buffer_to_byte_source(buffer_t* buffer);
uint8_t buffer_stream_source_read(byte_stream_source_t* source, boolean_t* has_byte);
byte_stream_source_t* cstring_to_byte_source(char* string);
uint8_t cstring_stream_source_read(byte_stream_source_t* source, boolean_t* has_byte);
byte_stream_target_t* buffer_to_byte_target(buffer_t* buffer);
byte_stream_target_t* buffer_stream_target_write(byte_stream_target_t* target, uint8_t byte);
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
void make_file_read_only(char* file_name);
void make_writable_if_exists(const char* file_name);
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
void cdl_indent(cdl_printer_t* printer);
boolean_t is_safe_string(char* string);
void cdl_output_token(cdl_printer_t* printer, char* string);
void cdl_boolean(cdl_printer_t* printer, boolean_t boolean);
void cdl_string(cdl_printer_t* printer, char* string);
void cdl_int64(cdl_printer_t* printer, int64_t number);
void cdl_uint64(cdl_printer_t* printer, uint64_t number);
void cdl_double(cdl_printer_t* printer, double number);
void cdl_start_array(cdl_printer_t* printer);
void cdl_end_array(cdl_printer_t* printer);
void cdl_start_table(cdl_printer_t* printer);
void cdl_key(cdl_printer_t* printer, char* key);
void cdl_end_table(cdl_printer_t* printer);
sub_process_t* make_sub_process(value_array_t* argv);
boolean_t sub_process_launch(sub_process_t* sub_process);
uint64_t sub_process_write(sub_process_t* sub_process, buffer_t* data, uint64_t start_position);
void sub_process_close_stdin(sub_process_t* sub_process);
void sub_process_read(sub_process_t* sub_process, buffer_t* stdout, buffer_t* stderr);
void sub_process_wait(sub_process_t* sub_process);
void sub_process_launch_and_wait(sub_process_t* sub_process, buffer_t* child_stdin, buffer_t* child_stdout, buffer_t* child_stderr);
void sub_process_record_exit_status(sub_process_t* sub_process, pid_t pid, int status);
boolean_t is_sub_process_running(sub_process_t* sub_process);
buffer_t* join_array_of_strings(value_array_t* array_of_strings, char* separator);
void oarchive_append_header_and_file_contents(FILE* out, char* filename);
string_tree_t* oarchive_read_header(FILE* in);
void oarchive_stream_members(FILE* in, oarchive_stream_headers_callback_t callback, void* callback_data);
__attribute__((format(printf, 3, 4))) void test_fail_and_exit(char* file_name, int line_number, char* format, ...);
void initialize_keyword_maps(void);
boolean_t is_reserved_word(input_mode_t mode, char* str);
boolean_t is_builtin_type_name(input_mode_t mode, char* str);
value_array_t* read_files(value_array_t* files);
file_t* read_file(char* file_name);
void add_all_oarchive_members(value_array_t* result, char* archive_file_name);
boolean_t add_orachive_file(FILE* input, string_tree_t* metadata, int64_t size, void* callback_data);
buffer_t* buffer_append_human_readable_error(buffer_t* buffer, compiler_error_t* error);
src_code_snippets_t get_source_code_snippet(buffer_t* buffer, uint64_t location, int before_lines, int after_lines);
char* do_common_replacements(char* template, compiler_error_t* error);
buffer_t* buffer_append_human_readable_tokenizer_error(buffer_t* buffer, compiler_error_t* error);
buffer_t* buffer_append_human_readable_parser_error(buffer_t* buffer, compiler_error_t* error);
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
boolean_t pstate_is_eof(pstate_t* pstate);
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
pstatus_t parse_improved_typedef_node(pstate_t* pstate);
void buffer_append_dbg_parse_node(cdl_printer_t* printer, parse_node_t* node);
void buffer_append_dbg_node_list(cdl_printer_t* printer, node_list_t list);
void buffer_append_dbg_tokens(cdl_printer_t* printer, value_array_t* tokens, char* field_name);
void buffer_append_dbg_declarations(cdl_printer_t* printer, declarations_node_t* node);
void buffer_append_dbg_enum(cdl_printer_t* printer, enum_node_t* node);
void buffer_append_dbg_struct_node(cdl_printer_t* printer, struct_node_t* node);
void buffer_append_dbg_union_node(cdl_printer_t* printer, union_node_t* node);
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
printer_t* append_line_directive(printer_t* printer, token_t* token);
symbol_table_map_t* make_symbol_table_map(void);
symbol_table_t* make_symbol_table(void);
symbol_table_binding_t* symbol_table_map_get(symbol_table_map_t* map, char* key_string);
parse_node_t* symbol_table_map_get_only_definition(symbol_table_map_t* map, char* key_string);
void symbol_table_add_declaration_node(symbol_table_map_t* map, char* key_string, parse_node_t* node);
void symbol_table_add_declartions(symbol_table_t* symbol_table, declarations_node_t* root);
buffer_t* symbol_table_stats(buffer_t* buffer, symbol_table_t* symbol_table);
void buffer_append_dgb_binding(cdl_printer_t* printer, symbol_table_binding_t* binding);
void buffer_appennd_dbg_symbol_table_map(cdl_printer_t* printer, symbol_table_map_t* symbol_table_map);
void buffer_append_dgb_symbol_table(cdl_printer_t* printer, symbol_table_t* symbol_table);
token_t* generate_struct_name_from_typedef_name(token_t* name);
void split_structure_typedefs(symbol_table_t* symbol_table);
void reorder_symbol_table_typedefs(symbol_table_t* symbol_table);
void reorder_symbol_table_typedefs__process_binding(symbol_table_map_t* typedefs, symbol_table_binding_t* binding, value_array_t* reordered_bindings);
struct_node_t* get_full_structure_definition_node(symbol_table_binding_t* binding);
symbol_table_binding_t* resolve_typename_to_structure_binding(symbol_table_t* symbol_table, type_node_t* type_node);
void reorder_symbol_table_structures_process_binding(symbol_table_t* symbol_table, symbol_table_binding_t* binding, value_array_t* reordered_bindings);
void reorder_symbol_table_structures(symbol_table_t* symbol_table);
c_preprocessor_directive_range_t mark_c_preprocessor_directive(c_preprocess_options_t options, value_array_t* tokens, uint64_t start_position);
uint64_t handle_c_preprocessor_directive(c_preprocess_options_t options, symbol_table_t* symbol_table, value_array_t* tokens, uint64_t start_position);
void handle_c_preprocessor_directives(c_preprocess_options_t options, symbol_table_t* symbol_table, value_array_t* tokens);
void parse_and_add_top_level_definitions(symbol_table_t* symbol_table, value_array_t* file_names, boolean_t use_statement_parser);
void symbol_table_parse_buffer(symbol_table_t* symbol_table, buffer_t* buffer, char* file_name, boolean_t use_statement_parser);
file_t* symbol_table_token_to_file(symbol_table_t* symbol_table, token_t* token);
void srcgen_enum_to_string_converters(symbol_table_t* symbol_table);
pstatus_t pratt_parse_expression(pstate_t* pstate, int precedence);
pstatus_t pratt_handle_instruction(pstate_t* pstate, pratt_parser_instruction_t instruction, parse_node_t* left);
pratt_parser_instruction_t get_prefix_instruction(token_t* token);
pratt_parser_instruction_t get_infix_instruction(token_t* token);
associativity_t precedence_to_associativity(precedence_t precedence);
pstatus_t parse_statement(pstate_t* pstate);
pstatus_t parse_block(pstate_t* pstate);
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
goto_statement_node_t* make_goto_statement(token_t* first_token, token_t* label);
goto_statement_node_t* to_goto_statement_node(parse_node_t* ptr);
empty_statement_node_t* make_empty_statement(token_t* semi_colon_token);
empty_statement_node_t* to_empty_statement_node(parse_node_t* ptr);
switch_statement_node_t* make_switch_statement(token_t* first_token, parse_node_t* expression, parse_node_t* block);
switch_statement_node_t* to_switch_statement_node(parse_node_t* ptr);
case_label_node_t* make_case_label(token_t* first_token, parse_node_t* expression);
case_label_node_t* to_case_label_node(parse_node_t* ptr);
default_label_node_t* make_default_label(token_t* default_token);
default_label_node_t* to_default_label_node(parse_node_t* ptr);
block_node_t* make_block_node(token_t* first_token);
block_node_t* to_block_node(parse_node_t* ptr);
for_statement_node_t* make_for_statement(token_t* first_token, parse_node_t* for_init, parse_node_t* for_test, parse_node_t* for_increment, parse_node_t* for_body);
for_statement_node_t* to_for_statement_node(parse_node_t* ptr);
if_statement_node_t* make_if_statement(token_t* first_token, parse_node_t* if_condition, parse_node_t* if_true, parse_node_t* if_else);
if_statement_node_t* to_if_statement_node(parse_node_t* ptr);
expression_statement_node_t* make_expression_statement_node(token_t* first_token, parse_node_t* expression);
expression_statement_node_t* to_expression_statement_node(parse_node_t* ptr);
return_statement_node_t* make_return_statement(token_t* first_token, parse_node_t* expression);
return_statement_node_t* to_return_statement_node(parse_node_t* ptr);
while_statement_node_t* make_while_statement(token_t* first_token, parse_node_t* condition, parse_node_t* body);
while_statement_node_t* to_while_statement_node(parse_node_t* ptr);
do_statement_node_t* make_do_statement(token_t* first_token, parse_node_t* body, parse_node_t* condition);
do_statement_node_t* to_do_statement_node(parse_node_t* ptr);
pstatus_t parse_type_node(pstate_t* pstate);
canonical_type_result_t make_type_token_result(char* str, int consumed_tokens);
pstatus_t parse_typeof_node(pstate_t* pstate);
canonical_type_result_t parse_canonical_type(pstate_t* pstate);
pstatus_t parse_function_type(pstate_t* pstate);
pstatus_t parse_function_type_argument(pstate_t* pstate);
pstatus_t parse_structure_node(pstate_t* pstate);
pstatus_t parse_field_node(pstate_t* pstate);
pstatus_t parse_union_node(pstate_t* pstate);
pstatus_t parse_user_type_node(pstate_t* pstate);
pstatus_t parse_enum_node(pstate_t* pstate);
pstatus_t parse_enum_element_node(pstate_t* pstate);
pstatus_t parse_expression(pstate_t* pstate);
pstatus_t parse_initializer(pstate_t* pstate);
pstatus_t parse_variable_definition_node(pstate_t* pstate);
pstatus_t parse_literal_node(pstate_t* pstate);
pstatus_t parse_compound_literal(pstate_t* pstate);
pstatus_t parse_designated_initializer_node(pstate_t* pstate);
pstatus_t parse_balanced_construct(pstate_t* pstate);
printer_t* make_printer(buffer_t* buffer, symbol_table_t* symbol_table, int indent_width);
printer_t* append_string(printer_t* printer, char* string);
printer_t* append_token(printer_t* printer, token_t* token);
printer_t* printer_newline(printer_t* printer);
printer_t* printer_space(printer_t* printer);
printer_t* printer_indent(printer_t* printer);
printer_t* printer_increase_indent(printer_t* printer);
printer_t* printer_decrease_indent(printer_t* printer);
void do_print_tokens(value_array_t* tokens, char* message);
void print_tokens(void);
void configure_flags(void);
void configure_parse_expression(void);
void configure_parse_statement(void);
void configure_print_tokens_command(void);
void configure_regular_commands(void);
boolean_t is_inlined_function(function_node_t* node);
void dump_symbol_table(char* phase_name, symbol_table_t* symbol_table);
char* include_node_to_string(cpp_include_node_t* node);
void generate_c_output_file(boolean_t is_library, buffer_t* command_line_overview_comment);
void add_generated_c_file_header(buffer_t* buffer);
void parse_expression_string_and_print_parse_tree(char* expression);
void parse_statement_string_and_print_parse_tree(char* expression);
buffer_t* git_hash_object(char* filename);
buffer_t* command_line_args_to_buffer(int argc, char** argv);
value_array_t* c_compiler_command_line(char* input_file, char* output_file);
int invoke_c_compiler(char* input_file, char* output_file);
void generate_archive_file(void);
int main(int argc, char** argv);
buffer_t* get_reflection_header_buffer(void);
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
char* token_type_to_string(token_type_t value);
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


# 139 "lib/value.c"
static inline boolean_t is_ok(value_result_t value)
# 139 "lib/value.c"
{

# 140 "lib/value.c"
  return ((value.nf_error)==NF_OK);
}


# 149 "lib/value.c"
static inline boolean_t is_not_ok(value_result_t value)
# 149 "lib/value.c"
{

# 150 "lib/value.c"
  return ((value.nf_error)!=NF_OK);
}


# 205 "lib/string-util.c"
static inline boolean_t is_hex_digit(char ch)
# 205 "lib/string-util.c"
{

# 206 "lib/string-util.c"
  return (((ch>='0')&&(ch<='9'))||((ch>='a')&&(ch<='f')));
}


# 209 "lib/string-util.c"
static inline uint64_t hex_digit_to_value(char ch)
# 209 "lib/string-util.c"
{

# 210 "lib/string-util.c"
  if (((ch>='0')&&(ch<='9')))

# 210 "lib/string-util.c"
  {

# 211 "lib/string-util.c"
    return (ch-'0');
  }
  else

# 212 "lib/string-util.c"
  {

# 213 "lib/string-util.c"
    return ((ch-'a')+10);
  }
}


# 487 "lib/string-util.c"
static inline uint64_t mix(uint64_t h)
# 487 "lib/string-util.c"
{

# 488 "lib/string-util.c"
  (h^=(h>>23));

# 489 "lib/string-util.c"
  (h*=0x2127599bf4325c37ULL);

# 490 "lib/string-util.c"
  (h^=(h>>47));

# 491 "lib/string-util.c"
  return h;
}


# 176 "lib/logger.c"
static inline boolean_t should_log_info()
# 176 "lib/logger.c"
{

# 177 "lib/logger.c"
  return ((global_logger_state.level)<=LOGGER_INFO);
}


# 24 "lib/string-alist.c"
static inline value_result_t alist_find(string_alist_t* list, char* key)
# 24 "lib/string-alist.c"
{

# 25 "lib/string-alist.c"
  return value_alist_find((/*CAST*/(value_alist_t*) list), cmp_string_values, str_to_value(key));
}


# 35 "lib/string-alist.c"
__attribute__((warn_unused_result)) static inline string_alist_t* alist_insert(string_alist_t* list, char* key, value_t value)
# 35 "lib/string-alist.c"
{

# 36 "lib/string-alist.c"
  return (/*CAST*/(string_alist_t*) value_alist_insert((/*CAST*/(value_alist_t*) list), cmp_string_values, str_to_value(key), value));
}


# 49 "lib/string-alist.c"
__attribute__((warn_unused_result)) static inline string_alist_t* alist_delete(string_alist_t* list, char* key)
# 49 "lib/string-alist.c"
{

# 50 "lib/string-alist.c"
  return (/*CAST*/(string_alist_t*) value_alist_delete((/*CAST*/(value_alist_t*) list), cmp_string_values, str_to_value(key)));
}


# 63 "lib/string-alist.c"
__attribute__((warn_unused_result)) static inline uint64_t alist_length(string_alist_t* list)
# 63 "lib/string-alist.c"
{

# 64 "lib/string-alist.c"
  return value_alist_length((/*CAST*/(value_alist_t*) list));
}


# 70 "lib/value-hashtable.c"
static inline uint64_t value_ht_num_entries(value_hashtable_t* ht)
# 70 "lib/value-hashtable.c"
{

# 71 "lib/value-hashtable.c"
  return (ht->n_entries);
}


# 17 "lib/string-hashtable.c"
static inline value_hashtable_t* to_value_hashtable(string_hashtable_t* ht)
# 17 "lib/string-hashtable.c"
{

# 18 "lib/string-hashtable.c"
  return (/*CAST*/(value_hashtable_t*) ht);
}


# 30 "lib/string-hashtable.c"
static inline string_hashtable_t* make_string_hashtable(uint64_t n_buckets)
# 30 "lib/string-hashtable.c"
{

# 31 "lib/string-hashtable.c"
  return (/*CAST*/(string_hashtable_t*) make_value_hashtable(n_buckets));
}


# 40 "lib/string-hashtable.c"
__attribute__((warn_unused_result)) static inline string_hashtable_t* string_ht_insert(string_hashtable_t* ht, char* key, value_t value)
# 40 "lib/string-hashtable.c"
{

# 41 "lib/string-hashtable.c"
  return (/*CAST*/(string_hashtable_t*) value_ht_insert(to_value_hashtable(ht), hash_string_value, cmp_string_values, str_to_value(key), value));
}


# 53 "lib/string-hashtable.c"
__attribute__((warn_unused_result)) static inline string_hashtable_t* string_ht_delete(string_hashtable_t* ht, char* key)
# 53 "lib/string-hashtable.c"
{

# 54 "lib/string-hashtable.c"
  return (/*CAST*/(string_hashtable_t*) value_ht_delete(to_value_hashtable(ht), hash_string_value, cmp_string_values, str_to_value(key)));
}


# 64 "lib/string-hashtable.c"
static inline value_result_t string_ht_find(string_hashtable_t* ht, char* key)
# 64 "lib/string-hashtable.c"
{

# 65 "lib/string-hashtable.c"
  return value_ht_find(to_value_hashtable(ht), hash_string_value, cmp_string_values, str_to_value(key));
}


# 74 "lib/string-hashtable.c"
static inline uint64_t string_ht_num_entries(string_hashtable_t* ht)
# 74 "lib/string-hashtable.c"
{

# 75 "lib/string-hashtable.c"
  return value_ht_num_entries(to_value_hashtable(ht));
}


# 183 "lib/value-tree.c"
static inline uint64_t value_tree_min_level(uint32_t a, uint32_t b)
# 183 "lib/value-tree.c"
{

# 184 "lib/value-tree.c"
  return ((a<b) ? a : b);
}


# 217 "lib/value-tree.c"
static inline boolean_t value_tree_is_leaf(value_tree_t* t)
# 217 "lib/value-tree.c"
{

# 218 "lib/value-tree.c"
  return (((t->left)==NULL)&&((t->right)==NULL));
}


# 22 "lib/string-tree.c"
static inline value_result_t string_tree_find(string_tree_t* t, char* key)
# 22 "lib/string-tree.c"
{

# 23 "lib/string-tree.c"
  return value_tree_find((/*CAST*/(value_tree_t*) t), cmp_string_values, str_to_value(key));
}


# 34 "lib/string-tree.c"
__attribute__((warn_unused_result)) static inline string_tree_t* string_tree_insert(string_tree_t* t, char* key, value_t value)
# 34 "lib/string-tree.c"
{

# 35 "lib/string-tree.c"
  return (/*CAST*/(string_tree_t*) value_tree_insert((/*CAST*/(value_tree_t*) t), cmp_string_values, str_to_value(key), value));
}


# 47 "lib/string-tree.c"
__attribute__((warn_unused_result)) static inline string_tree_t* string_tree_delete(string_tree_t* t, char* key)
# 47 "lib/string-tree.c"
{

# 48 "lib/string-tree.c"
  return (/*CAST*/(string_tree_t*) value_tree_delete((/*CAST*/(value_tree_t*) t), cmp_string_values, str_to_value(key)));
}


# 63 "lib/random.c"
static inline uint64_t rotl(uint64_t x, int k)
# 63 "lib/random.c"
{

# 64 "lib/random.c"
  return ((x<<k)|(x>>(64-k)));
}


# 123 "keywords.c"
static inline void maybe_initialize_keyword_maps(void)
# 123 "keywords.c"
{

# 124 "keywords.c"
  if ((c_keywords_ht==((void *)0)))

# 124 "keywords.c"
  {

# 125 "keywords.c"
    initialize_keyword_maps();
  }
}


# 40 "lexer.c"
static inline token_t* token_at(value_array_t* tokens, uint64_t position)
# 40 "lexer.c"
{

# 41 "lexer.c"
  if ((position>=(tokens->length)))

# 41 "lexer.c"
  {

# 42 "lexer.c"
    return ((void *)0);
  }

# 44 "lexer.c"
  return value_array_get_ptr(tokens, position, typeof(token_t*));
}


# 47 "lexer.c"
static inline boolean_t token_matches(token_t* token, char* str)
# 47 "lexer.c"
{

# 48 "lexer.c"
  if ((token==((void *)0)))

# 48 "lexer.c"
  {

# 49 "lexer.c"
    return false;
  }
  int str_len = strlen(str);

# 52 "lexer.c"
  return ((str_len==((token->end)-(token->start)))&&buffer_match_string_at((token->buffer), (token->start), str));
}


# 56 "lexer.c"
static inline boolean_t token_starts_with(token_t* token, char* str)
# 56 "lexer.c"
{

# 57 "lexer.c"
  return buffer_match_string_at((token->buffer), (token->start), str);
}


# 60 "lexer.c"
static inline boolean_t token_contains(token_t* token, char* str)
# 60 "lexer.c"
{

# 61 "lexer.c"
  return buffer_region_contains((token->buffer), (token->start), (token->end), str);
}


# 572 "lexer.c"
static inline token_t* heap_allocate_token(token_t token)
# 572 "lexer.c"
{
  token_t* result = malloc_struct(token_t);

# 574 "lexer.c"
  ((*result)=token);

# 575 "lexer.c"
  return result;
}


# 21 "token-list.c"
static inline void token_list_add(token_list_t* token_list, token_t* token)
# 21 "token-list.c"
{

# 22 "token-list.c"
  if ((token==((void *)0)))

# 22 "token-list.c"
  {

# 23 "token-list.c"
    fatal_error(ERROR_ILLEGAL_STATE);
  }

# 25 "token-list.c"
  if (((token_list->list)==((void *)0)))

# 25 "token-list.c"
  {

# 26 "token-list.c"
    ((token_list->list)=make_value_array(2));
  }

# 28 "token-list.c"
  value_array_add((token_list->list), ptr_to_value(token));
}


# 36 "token-list.c"
static inline uint64_t token_list_length(token_list_t token_list)
# 36 "token-list.c"
{

# 37 "token-list.c"
  if (((token_list.list)==((void *)0)))

# 37 "token-list.c"
  {

# 38 "token-list.c"
    return 0;
  }

# 40 "token-list.c"
  return ((token_list.list)->length);
}


# 48 "token-list.c"
static inline boolean_t token_list_is_empty(token_list_t token_list)
# 48 "token-list.c"
{

# 49 "token-list.c"
  return (token_list_length(token_list)==0);
}


# 57 "token-list.c"
static inline token_t* token_list_get(token_list_t token_list, uint64_t index)
# 57 "token-list.c"
{

# 58 "token-list.c"
  if (((token_list.list)==((void *)0)))

# 58 "token-list.c"
  {

# 59 "token-list.c"
    fatal_error(ERROR_ACCESS_OUT_OF_BOUNDS);
  }

# 61 "token-list.c"
  return value_array_get_ptr((token_list.list), index, typeof(token_t*));
}


# 69 "token-list.c"
static inline boolean_t token_list_contains(token_list_t token_list, char* token_string)
# 70 "token-list.c"
{

# 71 "token-list.c"
  for (
    int i = 0;
    (i<token_list_length(token_list));
    (i++))

# 71 "token-list.c"
  {

# 72 "token-list.c"
    if (token_matches(token_list_get(token_list, i), token_string))

# 72 "token-list.c"
    {

# 73 "token-list.c"
      return true;
    }
  }

# 76 "token-list.c"
  return false;
}


# 104 "parser.c"
static inline parse_node_t* to_node(void* ptr)
# 104 "parser.c"
{

# 105 "parser.c"
  return (/*CAST*/(parse_node_t*) ptr);
}


# 139 "declaration-parser.c"
static inline declarations_node_t* to_declarations_node(parse_node_t* ptr)
# 139 "declaration-parser.c"
{

# 140 "declaration-parser.c"
  if (((ptr==((void *)0))||((ptr->tag)!=PARSE_NODE_DECLARATIONS)))

# 140 "declaration-parser.c"
  {

# 141 "declaration-parser.c"
    fatal_error(ERROR_ILLEGAL_STATE);
  }

# 143 "declaration-parser.c"
  return (/*CAST*/(declarations_node_t*) ptr);
}


# 152 "declaration-parser.c"
static inline literal_node_t* to_literal_node(parse_node_t* ptr)
# 152 "declaration-parser.c"
{

# 153 "declaration-parser.c"
  if (((ptr==((void *)0))||((ptr->tag)!=PARSE_NODE_LITERAL)))

# 153 "declaration-parser.c"
  {

# 154 "declaration-parser.c"
    fatal_error(ERROR_ILLEGAL_STATE);
  }

# 156 "declaration-parser.c"
  return (/*CAST*/(literal_node_t*) ptr);
}


# 165 "declaration-parser.c"
static inline function_node_t* to_function_node(parse_node_t* ptr)
# 165 "declaration-parser.c"
{

# 166 "declaration-parser.c"
  if (((ptr==((void *)0))||((ptr->tag)!=PARSE_NODE_FUNCTION)))

# 166 "declaration-parser.c"
  {

# 167 "declaration-parser.c"
    fatal_error(ERROR_ILLEGAL_STATE);
  }

# 169 "declaration-parser.c"
  return (/*CAST*/(function_node_t*) ptr);
}


# 179 "declaration-parser.c"
static inline function_argument_node_t* to_function_argument_node(parse_node_t* ptr)
# 179 "declaration-parser.c"
{

# 180 "declaration-parser.c"
  if (((ptr==((void *)0))||((ptr->tag)!=PARSE_NODE_FUNCTION_ARGUMENT)))

# 180 "declaration-parser.c"
  {

# 181 "declaration-parser.c"
    fatal_error(ERROR_ILLEGAL_STATE);
  }

# 183 "declaration-parser.c"
  return (/*CAST*/(function_argument_node_t*) ptr);
}


# 192 "declaration-parser.c"
static inline typedef_node_t* to_typedef_node(parse_node_t* ptr)
# 192 "declaration-parser.c"
{

# 193 "declaration-parser.c"
  if (((ptr==((void *)0))||((ptr->tag)!=PARSE_NODE_TYPEDEF)))

# 193 "declaration-parser.c"
  {

# 194 "declaration-parser.c"
    fatal_error(ERROR_ILLEGAL_STATE);
  }

# 196 "declaration-parser.c"
  return (/*CAST*/(typedef_node_t*) ptr);
}


# 205 "declaration-parser.c"
static inline unparsed_expression_t* to_unparsed_expression(parse_node_t* ptr)
# 205 "declaration-parser.c"
{

# 206 "declaration-parser.c"
  if (((ptr==((void *)0))||((ptr->tag)!=PARSE_NODE_UNPARSED_EXPRESSION)))

# 206 "declaration-parser.c"
  {

# 207 "declaration-parser.c"
    fatal_error(ERROR_ILLEGAL_STATE);
  }

# 209 "declaration-parser.c"
  return (/*CAST*/(unparsed_expression_t*) ptr);
}


# 218 "declaration-parser.c"
static inline attribute_node_t* to_attribute_node(parse_node_t* ptr)
# 218 "declaration-parser.c"
{

# 219 "declaration-parser.c"
  if (((ptr==((void *)0))||((ptr->tag)!=PARSE_NODE_ATTRIBUTE)))

# 219 "declaration-parser.c"
  {

# 220 "declaration-parser.c"
    fatal_error(ERROR_ILLEGAL_STATE);
  }

# 222 "declaration-parser.c"
  return (/*CAST*/(attribute_node_t*) ptr);
}


# 231 "declaration-parser.c"
static inline cpp_define_node_t* to_cpp_define_node(parse_node_t* ptr)
# 231 "declaration-parser.c"
{

# 232 "declaration-parser.c"
  if (((ptr==((void *)0))||((ptr->tag)!=PARSE_NODE_CPP_DEFINE)))

# 232 "declaration-parser.c"
  {

# 233 "declaration-parser.c"
    fatal_error(ERROR_ILLEGAL_STATE);
  }

# 235 "declaration-parser.c"
  return (/*CAST*/(cpp_define_node_t*) ptr);
}


# 244 "declaration-parser.c"
static inline cpp_include_node_t* to_cpp_include_node(parse_node_t* ptr)
# 244 "declaration-parser.c"
{

# 245 "declaration-parser.c"
  if (((ptr==((void *)0))||((ptr->tag)!=PARSE_NODE_CPP_INCLUDE)))

# 245 "declaration-parser.c"
  {

# 246 "declaration-parser.c"
    fatal_error(ERROR_ILLEGAL_STATE);
  }

# 248 "declaration-parser.c"
  return (/*CAST*/(cpp_include_node_t*) ptr);
}


# 255 "declaration-parser.c"
static inline declarations_node_t* malloc_declarations(void)
# 255 "declaration-parser.c"
{
  declarations_node_t* result = malloc_struct(declarations_node_t);

# 257 "declaration-parser.c"
  ((result->tag)=PARSE_NODE_DECLARATIONS);

# 258 "declaration-parser.c"
  return result;
}


# 261 "declaration-parser.c"
static inline function_node_t* malloc_function_node(void)
# 261 "declaration-parser.c"
{
  function_node_t* result = malloc_struct(function_node_t);

# 263 "declaration-parser.c"
  ((result->tag)=PARSE_NODE_FUNCTION);

# 264 "declaration-parser.c"
  return result;
}


# 267 "declaration-parser.c"
static inline function_argument_node_t* malloc_function_argument_node(void)
# 267 "declaration-parser.c"
{
  function_argument_node_t* result = malloc_struct(function_argument_node_t);

# 269 "declaration-parser.c"
  ((result->tag)=PARSE_NODE_FUNCTION_ARGUMENT);

# 270 "declaration-parser.c"
  return result;
}


# 273 "declaration-parser.c"
static inline typedef_node_t* malloc_typedef_node(void)
# 273 "declaration-parser.c"
{
  typedef_node_t* result = malloc_struct(typedef_node_t);

# 275 "declaration-parser.c"
  ((result->tag)=PARSE_NODE_TYPEDEF);

# 276 "declaration-parser.c"
  return result;
}


# 279 "declaration-parser.c"
static inline attribute_node_t* malloc_attribute_node(void)
# 279 "declaration-parser.c"
{
  attribute_node_t* result = malloc_struct(attribute_node_t);

# 281 "declaration-parser.c"
  ((result->tag)=PARSE_NODE_ATTRIBUTE);

# 282 "declaration-parser.c"
  return result;
}


# 285 "declaration-parser.c"
static inline cpp_include_node_t* malloc_cpp_include_node(void)
# 285 "declaration-parser.c"
{
  cpp_include_node_t* result = malloc_struct(cpp_include_node_t);

# 287 "declaration-parser.c"
  ((result->tag)=PARSE_NODE_CPP_INCLUDE);

# 288 "declaration-parser.c"
  return result;
}


# 291 "declaration-parser.c"
static inline cpp_define_node_t* malloc_cpp_define_node(void)
# 291 "declaration-parser.c"
{
  cpp_define_node_t* result = malloc_struct(cpp_define_node_t);

# 293 "declaration-parser.c"
  ((result->tag)=PARSE_NODE_CPP_DEFINE);

# 294 "declaration-parser.c"
  return result;
}


# 31 "node-list.c"
static inline void node_list_add_node(node_list_t* node_list, parse_node_t* oc_node)
# 32 "node-list.c"
{

# 33 "node-list.c"
  if ((oc_node==((void *)0)))

# 33 "node-list.c"
  {

# 34 "node-list.c"
    fatal_error(ERROR_ILLEGAL_STATE);
  }

# 36 "node-list.c"
  if (((node_list->list)==((void *)0)))

# 36 "node-list.c"
  {

# 37 "node-list.c"
    ((node_list->list)=make_value_array(2));
  }

# 39 "node-list.c"
  value_array_add((node_list->list), ptr_to_value(oc_node));
}


# 47 "node-list.c"
static inline uint64_t node_list_length(node_list_t node_list)
# 47 "node-list.c"
{

# 48 "node-list.c"
  if (((node_list.list)==((void *)0)))

# 48 "node-list.c"
  {

# 49 "node-list.c"
    return 0;
  }

# 51 "node-list.c"
  return ((node_list.list)->length);
}


# 59 "node-list.c"
static inline boolean_t node_list_is_empty(node_list_t node_list)
# 59 "node-list.c"
{

# 60 "node-list.c"
  return (node_list_length(node_list)==0);
}


# 68 "node-list.c"
static inline parse_node_t* node_list_get(node_list_t node_list, uint64_t index)
# 69 "node-list.c"
{

# 70 "node-list.c"
  if (((node_list.list)==((void *)0)))

# 70 "node-list.c"
  {

# 71 "node-list.c"
    fatal_error(ERROR_ACCESS_OUT_OF_BOUNDS);
  }

# 73 "node-list.c"
  return value_array_get_ptr((node_list.list), index, typeof(parse_node_t*));
}


# 3 "header-file-extractor.c"
static inline char* remove_type_suffix(char* typename)
# 3 "header-file-extractor.c"
{

# 4 "header-file-extractor.c"
  if (string_ends_with(typename, "_t"))

# 4 "header-file-extractor.c"
  {

# 5 "header-file-extractor.c"
    return string_substring(typename, 0, (strlen(typename)-2));
  }
  else

# 6 "header-file-extractor.c"
  {

# 7 "header-file-extractor.c"
    return typename;
  }
}


# 9 "srcgen.c"
static inline char* remove_type_suffix_1(char* typename)
# 9 "srcgen.c"
{

# 10 "srcgen.c"
  if (string_ends_with(typename, "_t"))

# 10 "srcgen.c"
  {

# 11 "srcgen.c"
    return string_substring(typename, 0, (strlen(typename)-2));
  }
  else

# 12 "srcgen.c"
  {

# 13 "srcgen.c"
    return typename;
  }
}


# 111 "pratt-parser.c"
static inline operator_node_t* malloc_operator_node(void)
# 111 "pratt-parser.c"
{
  operator_node_t* result = malloc_struct(operator_node_t);

# 113 "pratt-parser.c"
  ((result->tag)=PARSE_NODE_OPERATOR);

# 114 "pratt-parser.c"
  return result;
}


# 117 "pratt-parser.c"
static inline identifier_node_t* malloc_identifier_node(void)
# 117 "pratt-parser.c"
{
  identifier_node_t* result = malloc_struct(identifier_node_t);

# 119 "pratt-parser.c"
  ((result->tag)=PARSE_NODE_IDENTIFIER);

# 120 "pratt-parser.c"
  return result;
}


# 123 "pratt-parser.c"
static inline call_node_t* malloc_call_node(void)
# 123 "pratt-parser.c"
{
  call_node_t* result = malloc_struct(call_node_t);

# 125 "pratt-parser.c"
  ((result->tag)=PARSE_NODE_CALL);

# 126 "pratt-parser.c"
  return result;
}


# 129 "pratt-parser.c"
static inline conditional_node_t* malloc_conditional_node(void)
# 129 "pratt-parser.c"
{
  conditional_node_t* result = malloc_struct(conditional_node_t);

# 131 "pratt-parser.c"
  ((result->tag)=PARSE_NODE_CONDITIONAL);

# 132 "pratt-parser.c"
  return result;
}


# 141 "pratt-parser.c"
static inline identifier_node_t* to_identifier_node(parse_node_t* ptr)
# 141 "pratt-parser.c"
{

# 142 "pratt-parser.c"
  if (((ptr==((void *)0))||((ptr->tag)!=PARSE_NODE_IDENTIFIER)))

# 142 "pratt-parser.c"
  {

# 143 "pratt-parser.c"
    fatal_error(ERROR_ILLEGAL_STATE);
  }

# 145 "pratt-parser.c"
  return (/*CAST*/(identifier_node_t*) ptr);
}


# 154 "pratt-parser.c"
static inline operator_node_t* to_operator_node(parse_node_t* ptr)
# 154 "pratt-parser.c"
{

# 155 "pratt-parser.c"
  if (((ptr==((void *)0))||((ptr->tag)!=PARSE_NODE_OPERATOR)))

# 155 "pratt-parser.c"
  {

# 156 "pratt-parser.c"
    fatal_error(ERROR_ILLEGAL_STATE);
  }

# 158 "pratt-parser.c"
  return (/*CAST*/(operator_node_t*) ptr);
}


# 167 "pratt-parser.c"
static inline call_node_t* to_call_node(parse_node_t* ptr)
# 167 "pratt-parser.c"
{

# 168 "pratt-parser.c"
  if (((ptr==((void *)0))||((ptr->tag)!=PARSE_NODE_CALL)))

# 168 "pratt-parser.c"
  {

# 169 "pratt-parser.c"
    fatal_error(ERROR_ILLEGAL_STATE);
  }

# 171 "pratt-parser.c"
  return (/*CAST*/(call_node_t*) ptr);
}


# 180 "pratt-parser.c"
static inline conditional_node_t* to_conditional_node(parse_node_t* ptr)
# 180 "pratt-parser.c"
{

# 181 "pratt-parser.c"
  if (((ptr==((void *)0))||((ptr->tag)!=PARSE_NODE_CONDITIONAL)))

# 181 "pratt-parser.c"
  {

# 182 "pratt-parser.c"
    fatal_error(ERROR_ILLEGAL_STATE);
  }

# 184 "pratt-parser.c"
  return (/*CAST*/(conditional_node_t*) ptr);
}


# 188 "pratt-parser.c"
static inline pratt_parser_instruction_t make_parser_instruction(token_t* token, pratt_parser_operation_t operation, precedence_t precedence)
# 189 "pratt-parser.c"
{

# 190 "pratt-parser.c"
  return ((pratt_parser_instruction_t) {.token = token, .operation = operation, .precedence = precedence});
}


# 65 "type-parser.c"
static inline type_node_t* malloc_type_node(void)
# 65 "type-parser.c"
{
  type_node_t* result = malloc_struct(type_node_t);

# 67 "type-parser.c"
  ((result->tag)=PARSE_NODE_TYPE);

# 68 "type-parser.c"
  return result;
}


# 77 "type-parser.c"
static inline type_node_t* to_type_node(parse_node_t* ptr)
# 77 "type-parser.c"
{

# 78 "type-parser.c"
  if (((ptr==((void *)0))||((ptr->tag)!=PARSE_NODE_TYPE)))

# 78 "type-parser.c"
  {

# 79 "type-parser.c"
    fatal_error(ERROR_ILLEGAL_STATE);
  }

# 81 "type-parser.c"
  return (/*CAST*/(type_node_t*) ptr);
}


# 72 "user-type-parser.c"
static inline boolean_t is_struct_node(parse_node_t* ptr)
# 72 "user-type-parser.c"
{

# 73 "user-type-parser.c"
  return ((ptr!=((void *)0))&&((ptr->tag)==PARSE_NODE_STRUCT));
}


# 82 "user-type-parser.c"
static inline boolean_t is_enum_node(parse_node_t* ptr)
# 82 "user-type-parser.c"
{

# 83 "user-type-parser.c"
  return ((ptr!=((void *)0))&&((ptr->tag)==PARSE_NODE_ENUM));
}


# 92 "user-type-parser.c"
static inline struct_node_t* to_struct_node(parse_node_t* ptr)
# 92 "user-type-parser.c"
{

# 94 "user-type-parser.c"
  if (((ptr==((void *)0))||(!(((ptr->tag)==PARSE_NODE_STRUCT)||((ptr->tag)==PARSE_NODE_UNION)))))

# 95 "user-type-parser.c"
  {

# 96 "user-type-parser.c"
    fatal_error(ERROR_ILLEGAL_STATE);
  }

# 98 "user-type-parser.c"
  return (/*CAST*/(struct_node_t*) ptr);
}


# 107 "user-type-parser.c"
static inline union_node_t* to_union_node(parse_node_t* ptr)
# 107 "user-type-parser.c"
{

# 108 "user-type-parser.c"
  if (((ptr==((void *)0))||((ptr->tag)!=PARSE_NODE_UNION)))

# 108 "user-type-parser.c"
  {

# 109 "user-type-parser.c"
    fatal_error(ERROR_ILLEGAL_STATE);
  }

# 111 "user-type-parser.c"
  return (/*CAST*/(union_node_t*) ptr);
}


# 120 "user-type-parser.c"
static inline field_node_t* to_field_node(parse_node_t* ptr)
# 120 "user-type-parser.c"
{

# 121 "user-type-parser.c"
  if (((ptr==((void *)0))||((ptr->tag)!=PARSE_NODE_FIELD)))

# 121 "user-type-parser.c"
  {

# 122 "user-type-parser.c"
    fatal_error(ERROR_ILLEGAL_STATE);
  }

# 124 "user-type-parser.c"
  return (/*CAST*/(field_node_t*) ptr);
}


# 133 "user-type-parser.c"
static inline enum_node_t* to_enum_node(parse_node_t* ptr)
# 133 "user-type-parser.c"
{

# 134 "user-type-parser.c"
  if (((ptr==((void *)0))||((ptr->tag)!=PARSE_NODE_ENUM)))

# 134 "user-type-parser.c"
  {

# 135 "user-type-parser.c"
    fatal_error(ERROR_ILLEGAL_STATE);
  }

# 137 "user-type-parser.c"
  return (/*CAST*/(enum_node_t*) ptr);
}


# 146 "user-type-parser.c"
static inline enum_element_t* to_enum_element_node(parse_node_t* ptr)
# 146 "user-type-parser.c"
{

# 147 "user-type-parser.c"
  if (((ptr==((void *)0))||((ptr->tag)!=PARSE_NODE_ENUM_ELEMENT)))

# 147 "user-type-parser.c"
  {

# 148 "user-type-parser.c"
    fatal_error(ERROR_ILLEGAL_STATE);
  }

# 150 "user-type-parser.c"
  return (/*CAST*/(enum_element_t*) ptr);
}


# 153 "user-type-parser.c"
static inline enum_node_t* malloc_enum_node(void)
# 153 "user-type-parser.c"
{
  enum_node_t* result = malloc_struct(enum_node_t);

# 155 "user-type-parser.c"
  ((result->tag)=PARSE_NODE_ENUM);

# 156 "user-type-parser.c"
  return result;
}


# 159 "user-type-parser.c"
static inline enum_element_t* malloc_enum_element(void)
# 159 "user-type-parser.c"
{
  enum_element_t* result = malloc_struct(enum_element_t);

# 161 "user-type-parser.c"
  ((result->tag)=PARSE_NODE_ENUM_ELEMENT);

# 162 "user-type-parser.c"
  return result;
}


# 165 "user-type-parser.c"
static inline struct_node_t* malloc_struct_node(void)
# 165 "user-type-parser.c"
{
  struct_node_t* result = malloc_struct(struct_node_t);

# 167 "user-type-parser.c"
  ((result->tag)=PARSE_NODE_STRUCT);

# 168 "user-type-parser.c"
  return result;
}


# 171 "user-type-parser.c"
static inline union_node_t* malloc_union_node(void)
# 171 "user-type-parser.c"
{
  union_node_t* result = malloc_struct(union_node_t);

# 173 "user-type-parser.c"
  ((result->tag)=PARSE_NODE_UNION);

# 174 "user-type-parser.c"
  return result;
}


# 177 "user-type-parser.c"
static inline field_node_t* malloc_field_node(void)
# 177 "user-type-parser.c"
{
  field_node_t* result = malloc_struct(field_node_t);

# 179 "user-type-parser.c"
  ((result->tag)=PARSE_NODE_FIELD);

# 180 "user-type-parser.c"
  return result;
}


# 25 "variable-definition-parser.c"
static inline variable_definition_node_t* to_variable_definition_node(parse_node_t* ptr)
# 25 "variable-definition-parser.c"
{

# 26 "variable-definition-parser.c"
  if (((ptr==((void *)0))||((ptr->tag)!=PARSE_NODE_VARIABLE_DEFINITION)))

# 26 "variable-definition-parser.c"
  {

# 27 "variable-definition-parser.c"
    fatal_error(ERROR_ILLEGAL_STATE);
  }

# 29 "variable-definition-parser.c"
  return (/*CAST*/(variable_definition_node_t*) ptr);
}


# 33 "variable-definition-parser.c"
static inline variable_definition_node_t* malloc_variable_definition_node(void)
# 33 "variable-definition-parser.c"
{
  variable_definition_node_t* result = malloc_struct(variable_definition_node_t);

# 36 "variable-definition-parser.c"
  ((result->tag)=PARSE_NODE_VARIABLE_DEFINITION);

# 37 "variable-definition-parser.c"
  return result;
}


# 37 "literal-parser.c"
static inline literal_node_t* malloc_literal_node(void)
# 37 "literal-parser.c"
{
  literal_node_t* result = malloc_struct(literal_node_t);

# 39 "literal-parser.c"
  ((result->tag)=PARSE_NODE_LITERAL);

# 40 "literal-parser.c"
  return result;
}


# 43 "literal-parser.c"
static inline compound_literal_node_t* malloc_compound_literal_node(void)
# 43 "literal-parser.c"
{
  compound_literal_node_t* result = malloc_struct(compound_literal_node_t);

# 45 "literal-parser.c"
  ((result->tag)=PARSE_NODE_COMPOUND_LITERAL);

# 46 "literal-parser.c"
  return result;
}


# 50 "literal-parser.c"
static inline designated_initializer_node_t* malloc_designated_initializer_node(void)
# 50 "literal-parser.c"
{
  designated_initializer_node_t* result = malloc_struct(designated_initializer_node_t);

# 53 "literal-parser.c"
  ((result->tag)=PARSE_NODE_DESIGNATED_INITIALIZER);

# 54 "literal-parser.c"
  return result;
}


# 25 "balanced-construct-parser.c"
static inline balanced_construct_node_t* to_balanced_construct_node(parse_node_t* ptr)
# 25 "balanced-construct-parser.c"
{

# 26 "balanced-construct-parser.c"
  if (((ptr==((void *)0))||((ptr->tag)!=PARSE_NODE_BALANCED_CONSTRUCT)))

# 26 "balanced-construct-parser.c"
  {

# 27 "balanced-construct-parser.c"
    fatal_error(ERROR_ILLEGAL_STATE);
  }

# 29 "balanced-construct-parser.c"
  return (/*CAST*/(balanced_construct_node_t*) ptr);
}


# 35 "balanced-construct-parser.c"
static inline balanced_construct_node_t* malloc_balanced_construct_node()
# 35 "balanced-construct-parser.c"
{
  balanced_construct_node_t* result = malloc_struct(balanced_construct_node_t);

# 37 "balanced-construct-parser.c"
  ((result->tag)=PARSE_NODE_BALANCED_CONSTRUCT);

# 38 "balanced-construct-parser.c"
  return result;
}


// ========== functions ==========


# 80 "lib/leb128.c"
unsigned encode_sleb_128(int64_t Value, uint8_t* p)
# 80 "lib/leb128.c"
{
  uint8_t* orig_p = p;
  int More;

# 83 "lib/leb128.c"
  do
# 83 "lib/leb128.c"
  {
    uint8_t Byte = (Value&0x7f);

# 86 "lib/leb128.c"
    (Value>>=7);

# 87 "lib/leb128.c"
    (More=(!(((Value==0)&&((Byte&0x40)==0))||((Value==(-1))&&((Byte&0x40)!=0)))));

# 89 "lib/leb128.c"
    if (More)

# 90 "lib/leb128.c"
    (Byte|=0x80);

# 91 "lib/leb128.c"
    ((*(p++))=Byte);
  }
  while (More);

# 94 "lib/leb128.c"
  return (/*CAST*/(unsigned) (p-orig_p));
}


# 104 "lib/leb128.c"
unsigned encode_uleb_128(uint64_t Value, uint8_t* p)
# 104 "lib/leb128.c"
{
  uint8_t* orig_p = p;

# 106 "lib/leb128.c"
  do
# 106 "lib/leb128.c"
  {
    uint8_t Byte = (Value&0x7f);

# 108 "lib/leb128.c"
    (Value>>=7);

# 109 "lib/leb128.c"
    if ((Value!=0))

# 110 "lib/leb128.c"
    (Byte|=0x80);

# 111 "lib/leb128.c"
    ((*(p++))=Byte);
  }
  while ((Value!=0));

# 114 "lib/leb128.c"
  return (/*CAST*/(unsigned) (p-orig_p));
}


# 122 "lib/leb128.c"
unsigned_decode_result decode_uleb_128(const uint8_t* p, const uint8_t* end)
# 122 "lib/leb128.c"
{
  const uint8_t* orig_p = p;
  uint64_t Value = 0;
  unsigned Shift = 0;

# 126 "lib/leb128.c"
  do
# 126 "lib/leb128.c"
  {

# 127 "lib/leb128.c"
    if ((p==end))

# 127 "lib/leb128.c"
    {
      unsigned_decode_result result = ((unsigned_decode_result) {0, ERROR_INSUFFICIENT_INPUT});

# 130 "lib/leb128.c"
      return result;
    }
    uint64_t Slice = ((*p)&0x7f);

# 133 "lib/leb128.c"
    if ((((Shift>=64)&&(Slice!=0))||(((Slice<<Shift)>>Shift)!=Slice)))

# 133 "lib/leb128.c"
    {
      unsigned_decode_result result = ((unsigned_decode_result) {0, ERROR_TOO_BIG});

# 136 "lib/leb128.c"
      return result;
    }

# 138 "lib/leb128.c"
    (Value+=(Slice<<Shift));

# 139 "lib/leb128.c"
    (Shift+=7);
  }
  while (((*(p++))>=128));
  unsigned_decode_result result = ((unsigned_decode_result) {Value, cast(unsigned, p - orig_p)});

# 143 "lib/leb128.c"
  return result;
}


# 151 "lib/leb128.c"
signed_decode_result decode_sleb_128(const uint8_t* p, const uint8_t* end)
# 151 "lib/leb128.c"
{
  const uint8_t* orig_p = p;
  int64_t Value = 0;
  unsigned Shift = 0;
  uint8_t Byte;

# 156 "lib/leb128.c"
  do
# 156 "lib/leb128.c"
  {

# 157 "lib/leb128.c"
    if ((p==end))

# 157 "lib/leb128.c"
    {
      signed_decode_result result = ((signed_decode_result) {0, ERROR_INSUFFICIENT_INPUT});

# 160 "lib/leb128.c"
      return result;
    }

# 162 "lib/leb128.c"
    (Byte=(*p));
    uint64_t Slice = (Byte&0x7f);

# 166 "lib/leb128.c"
    if ((((Shift>=64)&&(Slice!=((Value<0) ? 0x7f : 0x00)))||(((Shift==63)&&(Slice!=0))&&(Slice!=0x7f))))

# 167 "lib/leb128.c"
    {
      signed_decode_result result = ((signed_decode_result) {0, ERROR_TOO_BIG});

# 170 "lib/leb128.c"
      return result;
    }

# 172 "lib/leb128.c"
    (Value|=(Slice<<Shift));

# 173 "lib/leb128.c"
    (Shift+=7);

# 174 "lib/leb128.c"
    (++p);
  }
  while ((Byte>=128));

# 177 "lib/leb128.c"
  if (((Shift<64)&&(Byte&0x40)))

# 178 "lib/leb128.c"
  (Value|=((-1ULL)<<Shift));
  signed_decode_result result = ((signed_decode_result) {Value, (p - orig_p)});

# 181 "lib/leb128.c"
  return result;
}


# 119 "lib/fatal-error.c"
_Noreturn void fatal_error_impl(char* file, int line, int error_code)
# 119 "lib/fatal-error.c"
{

# 120 "lib/fatal-error.c"
  print_fatal_error_banner();

# 121 "lib/fatal-error.c"
  print_backtrace();

# 122 "lib/fatal-error.c"
  fprintf(stderr, "%s:%d: FATAL ERROR %d", file, line, error_code);

# 123 "lib/fatal-error.c"
  print_error_code_name(error_code);

# 124 "lib/fatal-error.c"
  fprintf(stderr, "\nCommand line: %s\n\n", get_command_line());
  char* sleep_str = getenv("ARMYKNIFE_FATAL_ERROR_SLEEP_SECONDS");

# 126 "lib/fatal-error.c"
  if ((sleep_str!=NULL))

# 126 "lib/fatal-error.c"
  {
    value_result_t sleep_time = string_parse_uint64(sleep_str);

# 128 "lib/fatal-error.c"
    if (is_ok(sleep_time))

# 128 "lib/fatal-error.c"
    {

# 129 "lib/fatal-error.c"
      fprintf(stderr, "Sleeping for %lu seconds so you can attach a debugger.\n", (sleep_time.u64));

# 132 "lib/fatal-error.c"
      fprintf(stderr, "  gdb -tui %s %d\n", get_program_path(), getpid());

# 133 "lib/fatal-error.c"
      sleep((sleep_time.u64));
    }
  }
  else

# 135 "lib/fatal-error.c"
  {

# 136 "lib/fatal-error.c"
    fprintf(stderr, "(ARMYKNIFE_FATAL_ERROR_SLEEP_SECONDS is not set)\n");
  }

# 138 "lib/fatal-error.c"
  fprintf(stderr, "Necessaria Morte Mori...\n");

# 139 "lib/fatal-error.c"
  exit((-(error_code+100)));
}


# 163 "lib/fatal-error.c"
const char* fatal_error_code_to_string(int error_code)
# 163 "lib/fatal-error.c"
{

# 164 "lib/fatal-error.c"
  switch (error_code)

# 164 "lib/fatal-error.c"
  {

# 165 "lib/fatal-error.c"
    case ERROR_UKNOWN:

# 166 "lib/fatal-error.c"
    return "ERROR_UKNOWN";

# 167 "lib/fatal-error.c"
    case ERROR_MEMORY_ALLOCATION:

# 168 "lib/fatal-error.c"
    return "ERROR_MEMORY_ALLOCATION";

# 169 "lib/fatal-error.c"
    case ERROR_MEMORY_FREE_NULL:

# 170 "lib/fatal-error.c"
    return "ERROR_MEMORY_FREE_NULL";

# 171 "lib/fatal-error.c"
    case ERROR_REFERENCE_NOT_EXPECTED_TYPE:

# 172 "lib/fatal-error.c"
    return "ERROR_REFERENCE_NOT_EXPECTED_TYPE";

# 173 "lib/fatal-error.c"
    case ERROR_ILLEGAL_INITIAL_CAPACITY:

# 174 "lib/fatal-error.c"
    return "ERROR_ILLEGAL_INITIAL_CAPACITY";

# 175 "lib/fatal-error.c"
    case ERROR_DYNAMICALLY_SIZED_TYPE_ILLEGAL_IN_CONTAINER:

# 176 "lib/fatal-error.c"
    return "ERROR_DYNAMICALLY_SIZED_TYPE_ILLEGAL_IN_CONTAINER";

# 177 "lib/fatal-error.c"
    case ERROR_ACCESS_OUT_OF_BOUNDS:

# 178 "lib/fatal-error.c"
    return "ERROR_ACCESS_OUT_OF_BOUNDS";

# 179 "lib/fatal-error.c"
    case ERROR_NOT_REACHED:

# 180 "lib/fatal-error.c"
    return "ERROR_NOT_REACHED";

# 181 "lib/fatal-error.c"
    case ERROR_ILLEGAL_ZERO_HASHCODE_VALUE:

# 182 "lib/fatal-error.c"
    return "ERROR_ILLEGAL_ZERO_HASHCODE_VALUE";

# 183 "lib/fatal-error.c"
    case ERROR_UNIMPLEMENTED:

# 184 "lib/fatal-error.c"
    return "ERROR_UNIMPLEMENTED";

# 185 "lib/fatal-error.c"
    case ERROR_ILLEGAL_NULL_ARGUMENT:

# 186 "lib/fatal-error.c"
    return "ERROR_ILLEGAL_NULL_ARGUMENT";

# 187 "lib/fatal-error.c"
    case ERROR_ILLEGAL_ARGUMENT:

# 188 "lib/fatal-error.c"
    return "ERROR_ILLEGAL_ARGUMENT";

# 189 "lib/fatal-error.c"
    case ERROR_MEMORY_START_PADDING_ERROR:

# 190 "lib/fatal-error.c"
    return "ERROR_MEMORY_START_PADDING_ERROR";

# 191 "lib/fatal-error.c"
    case ERROR_MEMORY_END_PADDING_ERROR:

# 192 "lib/fatal-error.c"
    return "ERROR_MEMORY_END_PADDING_ERROR";

# 194 "lib/fatal-error.c"
    default:

# 195 "lib/fatal-error.c"
    return "error";
  }
}


# 91 "lib/fatal-error.c"
void configure_fatal_errors(fatal_error_config_t config)
# 91 "lib/fatal-error.c"
{

# 92 "lib/fatal-error.c"
  (fatal_error_config=config);

# 93 "lib/fatal-error.c"
  if ((config.catch_sigsegv))

# 93 "lib/fatal-error.c"
  {

# 94 "lib/fatal-error.c"
    signal(SIGSEGV, segmentation_fault_handler);
  }
}


# 87 "lib/fatal-error.c"
void segmentation_fault_handler(int signal_number)
# 87 "lib/fatal-error.c"
{

# 88 "lib/fatal-error.c"
  fatal_error(ERROR_SIGSEGV);
}


# 142 "lib/fatal-error.c"
void print_fatal_error_banner()
# 142 "lib/fatal-error.c"
{

# 145 "lib/fatal-error.c"
  fprintf(stderr, "\n========== FATAL_ERROR ==========\n");
}


# 148 "lib/fatal-error.c"
void print_backtrace()
# 148 "lib/fatal-error.c"
{

# 150 "lib/fatal-error.c"
  do
# 150 "lib/fatal-error.c"
  {
    void* array[10];
    int size = backtrace(array, 10);
    char** strings = backtrace_symbols(array, size);

# 156 "lib/fatal-error.c"
    for (
      int i = 0;
      (i<size);
      (i++))

# 156 "lib/fatal-error.c"
    {

# 157 "lib/fatal-error.c"
      printf("#%d %s\n", i, (strings[i]));
    }
  }
  while (0);
}


# 199 "lib/fatal-error.c"
void print_error_code_name(int error_code)
# 199 "lib/fatal-error.c"
{

# 200 "lib/fatal-error.c"
  fprintf(stderr, " ");

# 201 "lib/fatal-error.c"
  fprintf(stderr, "*** ");

# 202 "lib/fatal-error.c"
  fprintf(stderr, "%s", fatal_error_code_to_string(error_code));

# 203 "lib/fatal-error.c"
  fprintf(stderr, " ***\n");
}


# 102 "lib/fatal-error.c"
char* get_command_line()
# 102 "lib/fatal-error.c"
{
  buffer_t* buffer = buffer_append_file_contents(make_buffer(1), "/proc/self/cmdline");

# 105 "lib/fatal-error.c"
  buffer_replace_matching_byte(buffer, 0, ' ');

# 106 "lib/fatal-error.c"
  return buffer_to_c_string(buffer);
}


# 109 "lib/fatal-error.c"
char* get_program_path()
# 109 "lib/fatal-error.c"
{
  char buf[4096];
  int n = readlink("/proc/self/exe", buf, (sizeof(buf)));

# 112 "lib/fatal-error.c"
  if ((n>0))

# 112 "lib/fatal-error.c"
  {

# 113 "lib/fatal-error.c"
    return string_duplicate(buf);
  }
  else

# 114 "lib/fatal-error.c"
  {

# 115 "lib/fatal-error.c"
    return "<program-path-unknown>";
  }
}


# 182 "lib/value.c"
int cmp_string_values(value_t value1, value_t value2)
# 182 "lib/value.c"
{

# 183 "lib/value.c"
  return strcmp((value1.str), (value2.str));
}


# 191 "lib/value.c"
uint64_t hash_string_value(value_t value1)
# 191 "lib/value.c"
{

# 191 "lib/value.c"
  return string_hash((value1.str));
}


# 83 "lib/gc-allocate.c"
uint8_t* checked_malloc(char* file, int line, uint64_t amount)
# 83 "lib/gc-allocate.c"
{

# 85 "lib/gc-allocate.c"
  if (((amount==0)||(amount>ARMYKNIFE_MEMORY_ALLOCATION_MAXIMUM_AMOUNT)))

# 85 "lib/gc-allocate.c"
  {

# 86 "lib/gc-allocate.c"
    fatal_error(ERROR_BAD_ALLOCATION_SIZE);
  }
  uint8_t* result = GC_malloc(amount);

# 91 "lib/gc-allocate.c"
  if ((result==((void *)0)))

# 91 "lib/gc-allocate.c"
  {

# 92 "lib/gc-allocate.c"
    fatal_error_impl(file, line, ERROR_MEMORY_ALLOCATION);
  }

# 95 "lib/gc-allocate.c"
  return result;
}


# 104 "lib/gc-allocate.c"
uint8_t* checked_malloc_copy_of(char* file, int line, uint8_t* source, uint64_t amount)
# 105 "lib/gc-allocate.c"
{
  uint8_t* result = checked_malloc(file, line, amount);

# 107 "lib/gc-allocate.c"
  memcpy(result, source, amount);

# 108 "lib/gc-allocate.c"
  return result;
}


# 121 "lib/gc-allocate.c"
void checked_free(char* file, int line, void* pointer)
# 121 "lib/gc-allocate.c"
{

# 122 "lib/gc-allocate.c"
  return;
}


# 22 "lib/uint64.c"
int uint64_highest_bit_set(uint64_t n)
# 22 "lib/uint64.c"
{

# 23 "lib/uint64.c"
  if ((n>=(1ULL<<32)))

# 23 "lib/uint64.c"
  {

# 24 "lib/uint64.c"
    return (uint64_highest_bit_set((n>>32))+32);
  }
  else

# 25 "lib/uint64.c"
  if ((n>=(1ULL<<16)))

# 25 "lib/uint64.c"
  {

# 26 "lib/uint64.c"
    return (uint64_highest_bit_set((n>>16))+16);
  }
  else

# 27 "lib/uint64.c"
  if ((n>=(1ULL<<8)))

# 27 "lib/uint64.c"
  {

# 28 "lib/uint64.c"
    return (uint64_highest_bit_set((n>>8))+8);
  }
  else

# 29 "lib/uint64.c"
  if ((n>=(1ULL<<4)))

# 29 "lib/uint64.c"
  {

# 30 "lib/uint64.c"
    return (uint64_highest_bit_set((n>>4))+4);
  }
  else

# 31 "lib/uint64.c"
  if ((n>=(1ULL<<2)))

# 31 "lib/uint64.c"
  {

# 32 "lib/uint64.c"
    return (uint64_highest_bit_set((n>>2))+2);
  }
  else

# 33 "lib/uint64.c"
  if ((n>=(1ULL<<1)))

# 33 "lib/uint64.c"
  {

# 34 "lib/uint64.c"
    return (uint64_highest_bit_set((n>>1))+1);
  }
  else

# 35 "lib/uint64.c"
  {

# 36 "lib/uint64.c"
    return 0;
  }
}


# 52 "lib/string-util.c"
int string_is_null_or_empty(const char* str)
# 52 "lib/string-util.c"
{

# 53 "lib/string-util.c"
  return ((str==((void *)0))||(strlen(str)==0));
}


# 61 "lib/string-util.c"
int string_equal(const char* str1, const char* str2)
# 61 "lib/string-util.c"
{

# 62 "lib/string-util.c"
  if (string_is_null_or_empty(str1))

# 62 "lib/string-util.c"
  {

# 63 "lib/string-util.c"
    return string_is_null_or_empty(str2);
  }

# 65 "lib/string-util.c"
  return (strcmp(str1, str2)==0);
}


# 73 "lib/string-util.c"
int string_starts_with(const char* str1, const char* str2)
# 73 "lib/string-util.c"
{

# 74 "lib/string-util.c"
  return (strncmp(str1, str2, strlen(str2))==0);
}


# 82 "lib/string-util.c"
int string_ends_with(const char* str1, const char* str2)
# 82 "lib/string-util.c"
{
  size_t len1 = strlen(str1);
  size_t len2 = strlen(str2);

# 86 "lib/string-util.c"
  if ((len2>len1))

# 86 "lib/string-util.c"
  {

# 87 "lib/string-util.c"
    return 0;
  }

# 90 "lib/string-util.c"
  return (strcmp((str1+(len1-len2)), str2)==0);
}


# 98 "lib/string-util.c"
boolean_t string_contains_char(const char* str, char ch)
# 98 "lib/string-util.c"
{

# 99 "lib/string-util.c"
  return (string_index_of_char(str, ch)>=0);
}


# 112 "lib/string-util.c"
int string_index_of_char(const char* str, char ch)
# 112 "lib/string-util.c"
{

# 113 "lib/string-util.c"
  if (string_is_null_or_empty(str))

# 113 "lib/string-util.c"
  {

# 114 "lib/string-util.c"
    return (-1);
  }
  int str_length = strlen(str);

# 117 "lib/string-util.c"
  for (
    int i = 0;
    (i<str_length);
    (i++))

# 117 "lib/string-util.c"
  {

# 118 "lib/string-util.c"
    if (((str[i])==ch))

# 118 "lib/string-util.c"
    {

# 119 "lib/string-util.c"
      return i;
    }
  }

# 122 "lib/string-util.c"
  return (-1);
}


# 310 "lib/string-util.c"
char* uint64_to_string(uint64_t number)
# 310 "lib/string-util.c"
{
  char buffer[32];

# 312 "lib/string-util.c"
  sprintf(buffer, "%lu", number);

# 313 "lib/string-util.c"
  return string_duplicate(buffer);
}


# 131 "lib/string-util.c"
uint64_t string_hash(const char* str)
# 131 "lib/string-util.c"
{

# 132 "lib/string-util.c"
  return fasthash64(str, strlen(str), 0);
}


# 140 "lib/string-util.c"
char* string_substring(const char* str, int start, int end)
# 140 "lib/string-util.c"
{
  uint64_t len = strlen(str);

# 142 "lib/string-util.c"
  if ((((start>=len)||(start>=end))||(end<start)))

# 142 "lib/string-util.c"
  {

# 143 "lib/string-util.c"
    fatal_error(ERROR_ILLEGAL_ARGUMENT);
  }
  int result_size = ((end-start)+1);
  char* result = (/*CAST*/(char*) malloc_bytes(result_size));

# 147 "lib/string-util.c"
  for (
    int i = start;
    (i<end);
    (i++))

# 147 "lib/string-util.c"
  {

# 148 "lib/string-util.c"
    ((result[(i-start)])=(str[i]));
  }

# 150 "lib/string-util.c"
  ((result[(result_size-1)])='\0');

# 151 "lib/string-util.c"
  return result;
}


# 261 "lib/string-util.c"
value_result_t string_parse_uint64(const char* string)
# 261 "lib/string-util.c"
{

# 262 "lib/string-util.c"
  if (string_starts_with(string, "0x"))

# 262 "lib/string-util.c"
  {

# 263 "lib/string-util.c"
    return string_parse_uint64_hex((&(string[2])));
  }
  else

# 264 "lib/string-util.c"
  if (string_starts_with(string, "0b"))

# 264 "lib/string-util.c"
  {

# 265 "lib/string-util.c"
    return string_parse_uint64_bin((&(string[2])));
  }
  else

# 266 "lib/string-util.c"
  {

# 267 "lib/string-util.c"
    return string_parse_uint64_dec(string);
  }
}


# 154 "lib/string-util.c"
value_result_t string_parse_uint64_dec(const char* string)
# 154 "lib/string-util.c"
{
  uint64_t len = strlen(string);
  uint64_t integer = 0;

# 158 "lib/string-util.c"
  if ((len==0))

# 158 "lib/string-util.c"
  {

# 159 "lib/string-util.c"
    return ((value_result_t) {.u64 = 0, .nf_error = NF_ERROR_NOT_PARSED_AS_NUMBER});
  }

# 163 "lib/string-util.c"
  for (
    int i = 0;
    (i<len);
    (i++))

# 163 "lib/string-util.c"
  {
    char ch = (string[i]);

# 165 "lib/string-util.c"
    if (((ch<'0')||(ch>'9')))

# 165 "lib/string-util.c"
    {

# 166 "lib/string-util.c"
      return ((value_result_t) {.u64 = 0, .nf_error = NF_ERROR_NOT_PARSED_AS_NUMBER});
    }
    uint64_t digit = ((string[i])-'0');

# 171 "lib/string-util.c"
    (integer=((integer*10)+digit));
  }

# 174 "lib/string-util.c"
  return ((value_result_t) {.u64 = integer, .nf_error = NF_OK});
}


# 222 "lib/string-util.c"
value_result_t string_parse_uint64_hex(const char* string)
# 222 "lib/string-util.c"
{
  uint64_t len = strlen(string);
  uint64_t integer = 0;

# 226 "lib/string-util.c"
  if ((len==0))

# 226 "lib/string-util.c"
  {

# 227 "lib/string-util.c"
    return ((value_result_t) {.u64 = 0, .nf_error = NF_ERROR_NOT_PARSED_AS_NUMBER});
  }

# 231 "lib/string-util.c"
  for (
    int i = 0;
    (i<len);
    (i++))

# 231 "lib/string-util.c"
  {
    char ch = (string[i]);

# 233 "lib/string-util.c"
    if ((!is_hex_digit(ch)))

# 233 "lib/string-util.c"
    {

# 234 "lib/string-util.c"
      return ((value_result_t) {.u64 = 0, .nf_error = NF_ERROR_NOT_PARSED_AS_NUMBER});
    }
    uint64_t digit = hex_digit_to_value(ch);

# 239 "lib/string-util.c"
    (integer=((integer<<4)|digit));
  }

# 242 "lib/string-util.c"
  return ((value_result_t) {.u64 = integer, .nf_error = NF_OK});
}


# 182 "lib/string-util.c"
value_result_t string_parse_uint64_bin(const char* string)
# 182 "lib/string-util.c"
{
  uint64_t len = strlen(string);
  uint64_t integer = 0;

# 186 "lib/string-util.c"
  if ((len==0))

# 186 "lib/string-util.c"
  {

# 187 "lib/string-util.c"
    return ((value_result_t) {.u64 = 0, .nf_error = NF_ERROR_NOT_PARSED_AS_NUMBER});
  }

# 191 "lib/string-util.c"
  for (
    int i = 0;
    (i<len);
    (i++))

# 191 "lib/string-util.c"
  {
    char ch = (string[i]);

# 193 "lib/string-util.c"
    if (((ch<'0')||(ch>'1')))

# 193 "lib/string-util.c"
    {

# 194 "lib/string-util.c"
      return ((value_result_t) {.u64 = 0, .nf_error = NF_ERROR_NOT_PARSED_AS_NUMBER});
    }
    uint64_t digit = ((string[i])-'0');

# 199 "lib/string-util.c"
    (integer=((integer<<1)|digit));
  }

# 202 "lib/string-util.c"
  return ((value_result_t) {.u64 = integer, .nf_error = NF_OK});
}


# 277 "lib/string-util.c"
char* string_duplicate(const char* src)
# 277 "lib/string-util.c"
{

# 278 "lib/string-util.c"
  if ((src==NULL))

# 278 "lib/string-util.c"
  {

# 279 "lib/string-util.c"
    return NULL;
  }
  int len = (strlen(src)+1);
  char* result = (/*CAST*/(char*) malloc_bytes(len));

# 283 "lib/string-util.c"
  memcpy(result, src, len);

# 285 "lib/string-util.c"
  return result;
}


# 294 "lib/string-util.c"
char* string_append(const char* a, const char* b)
# 294 "lib/string-util.c"
{

# 295 "lib/string-util.c"
  if (((a==NULL)||(b==NULL)))

# 295 "lib/string-util.c"
  {

# 296 "lib/string-util.c"
    fatal_error(ERROR_ILLEGAL_NULL_ARGUMENT);
  }
  int total_length = ((strlen(a)+strlen(b))+1);
  char* result = (/*CAST*/(char*) malloc_bytes(total_length));

# 300 "lib/string-util.c"
  strcat(result, a);

# 301 "lib/string-util.c"
  strcat(result, b);

# 302 "lib/string-util.c"
  return result;
}


# 322 "lib/string-util.c"
char* string_left_pad(const char* str, int n, char ch)
# 322 "lib/string-util.c"
{

# 323 "lib/string-util.c"
  if ((n<0))

# 323 "lib/string-util.c"
  {

# 324 "lib/string-util.c"
    fatal_error(ERROR_ILLEGAL_RANGE);
  }
  int input_length = strlen(str);
  int padding_needed = (n-input_length);
  int len = 1;
  buffer_t* buffer = make_buffer(len);

# 341 "lib/string-util.c"
  for (
    int i = 0;
    (i<padding_needed);
    (i++))

# 341 "lib/string-util.c"
  {

# 342 "lib/string-util.c"
    (buffer=buffer_append_byte(buffer, ch));
  }

# 344 "lib/string-util.c"
  (buffer=buffer_append_string(buffer, str));
  char* result = buffer_to_c_string(buffer);

# 346 "lib/string-util.c"
  free_bytes(buffer);

# 347 "lib/string-util.c"
  return result;
}


# 356 "lib/string-util.c"
char* string_right_pad(const char* str, int n, char ch)
# 356 "lib/string-util.c"
{

# 357 "lib/string-util.c"
  if ((n<0))

# 357 "lib/string-util.c"
  {

# 358 "lib/string-util.c"
    fatal_error(ERROR_ILLEGAL_RANGE);
  }
  int input_length = strlen(str);
  int padding_needed = (n-input_length);
  int len = 1;
  buffer_t* buffer = make_buffer(len);

# 375 "lib/string-util.c"
  (buffer=buffer_append_string(buffer, str));

# 376 "lib/string-util.c"
  for (
    int i = 0;
    (i<padding_needed);
    (i++))

# 376 "lib/string-util.c"
  {

# 377 "lib/string-util.c"
    (buffer=buffer_append_byte(buffer, ch));
  }
  char* result = buffer_to_c_string(buffer);

# 380 "lib/string-util.c"
  free_bytes(buffer);

# 381 "lib/string-util.c"
  return result;
}


# 428 "lib/string-util.c"
__attribute__((format(printf, 1, 2))) char* string_printf(char* format, ...)
# 428 "lib/string-util.c"
{
  char buffer[STRING_PRINTF_INITIAL_BUFFER_SIZE];
  int n_bytes = 0;

# 431 "lib/string-util.c"
  do
# 431 "lib/string-util.c"
  {
    va_list args;

# 433 "lib/string-util.c"
    va_start(args, format);

# 434 "lib/string-util.c"
    (n_bytes=vsnprintf(buffer, STRING_PRINTF_INITIAL_BUFFER_SIZE, format, args));

# 436 "lib/string-util.c"
    va_end(args);
  }
  while (0);

# 439 "lib/string-util.c"
  if ((n_bytes<STRING_PRINTF_INITIAL_BUFFER_SIZE))

# 439 "lib/string-util.c"
  {
    char* result = (/*CAST*/(char*) malloc_bytes((n_bytes+1)));

# 441 "lib/string-util.c"
    strcat(result, buffer);

# 442 "lib/string-util.c"
    return result;
  }
  else

# 443 "lib/string-util.c"
  {
    char* result = (/*CAST*/(char*) malloc_bytes((n_bytes+1)));
    va_list args;

# 446 "lib/string-util.c"
    va_start(args, format);
    int n_bytes_second = vsnprintf(result, (n_bytes+1), format, args);

# 448 "lib/string-util.c"
    va_end(args);

# 449 "lib/string-util.c"
    if ((n_bytes_second!=n_bytes))

# 449 "lib/string-util.c"
    {

# 450 "lib/string-util.c"
      fatal_error(ERROR_INTERNAL_ASSERTION_FAILURE);
    }

# 452 "lib/string-util.c"
    return result;
  }
}


# 394 "lib/string-util.c"
char* string_truncate(char* str, int limit, char* at_limit_suffix)
# 394 "lib/string-util.c"
{
  buffer_t* buffer = make_buffer(limit);

# 397 "lib/string-util.c"
  for (
    int i = 0;
    ;
    (i++))

# 397 "lib/string-util.c"
  {
    char ch = (str[i]);

# 399 "lib/string-util.c"
    if ((ch=='\0'))

# 399 "lib/string-util.c"
    {
      char* result = buffer_to_c_string(buffer);

# 401 "lib/string-util.c"
      free_bytes(buffer);

# 402 "lib/string-util.c"
      return result;
    }

# 404 "lib/string-util.c"
    (buffer=buffer_append_byte(buffer, ch));
  }

# 406 "lib/string-util.c"
  if (at_limit_suffix)

# 406 "lib/string-util.c"
  {

# 407 "lib/string-util.c"
    (buffer=buffer_append_string(buffer, at_limit_suffix));
  }
  char* result = buffer_to_c_string(buffer);

# 410 "lib/string-util.c"
  free_bytes(buffer);

# 411 "lib/string-util.c"
  return result;
}


# 496 "lib/string-util.c"
uint64_t fasthash64(const void* buf, size_t len, uint64_t seed)
# 496 "lib/string-util.c"
{
  const uint64_t m = 0x880355f21e6d1965ULL;
  const uint64_t* pos = (/*CAST*/(const uint64_t*) buf);
  const uint64_t* end = (pos+(len/8));
  const unsigned char* pos2;
  uint64_t h = (seed^(len*m));
  uint64_t v;

# 504 "lib/string-util.c"
  while ((pos!=end))

# 504 "lib/string-util.c"
  {

# 505 "lib/string-util.c"
    (v=(*(pos++)));

# 506 "lib/string-util.c"
    (h^=mix(v));

# 507 "lib/string-util.c"
    (h*=m);
  }

# 510 "lib/string-util.c"
  (pos2=(/*CAST*/(const unsigned char*) pos));

# 511 "lib/string-util.c"
  (v=0);

# 513 "lib/string-util.c"
  switch ((len&7))

# 513 "lib/string-util.c"
  {

# 514 "lib/string-util.c"
    case 7:

# 515 "lib/string-util.c"
    (v^=((/*CAST*/(uint64_t) (pos2[6]))<<48));

# 516 "lib/string-util.c"
    case 6:

# 517 "lib/string-util.c"
    (v^=((/*CAST*/(uint64_t) (pos2[5]))<<40));

# 518 "lib/string-util.c"
    case 5:

# 519 "lib/string-util.c"
    (v^=((/*CAST*/(uint64_t) (pos2[4]))<<32));

# 520 "lib/string-util.c"
    case 4:

# 521 "lib/string-util.c"
    (v^=((/*CAST*/(uint64_t) (pos2[3]))<<24));

# 522 "lib/string-util.c"
    case 3:

# 523 "lib/string-util.c"
    (v^=((/*CAST*/(uint64_t) (pos2[2]))<<16));

# 524 "lib/string-util.c"
    case 2:

# 525 "lib/string-util.c"
    (v^=((/*CAST*/(uint64_t) (pos2[1]))<<8));

# 526 "lib/string-util.c"
    case 1:

# 527 "lib/string-util.c"
    (v^=(/*CAST*/(uint64_t) (pos2[0])));

# 528 "lib/string-util.c"
    (h^=mix(v));

# 529 "lib/string-util.c"
    (h*=m);
  }

# 532 "lib/string-util.c"
  return mix(h);
}


# 262 "lib/logger.c"
void logger_init(void)
# 262 "lib/logger.c"
{
  char* level_string = getenv("ARMYKNIFE_LIB_LOG_LEVEL");

# 264 "lib/logger.c"
  if ((level_string!=NULL))

# 264 "lib/logger.c"
  {
    value_result_t parsed = string_parse_uint64(level_string);

# 266 "lib/logger.c"
    if (is_ok(parsed))

# 266 "lib/logger.c"
    {

# 267 "lib/logger.c"
      ((global_logger_state.level)=(parsed.u64));
    }
    else

# 268 "lib/logger.c"
    {
      value_result_t parsed = parse_log_level_enum(level_string);

# 270 "lib/logger.c"
      if (is_ok(parsed))

# 270 "lib/logger.c"
      {

# 271 "lib/logger.c"
        ((global_logger_state.level)=(parsed.u64));
      }
      else

# 272 "lib/logger.c"
      {

# 273 "lib/logger.c"
        log_warn("%s could not be converted to a log level.", level_string);
      }
    }
  }

# 278 "lib/logger.c"
  fprintf(stderr, "Log level is set to %s (%d)\n", logger_level_to_string((global_logger_state.level)), (global_logger_state.level));
  char* output_file_name = getenv("ARMYKNIFE_LIB_LOG_FILE");

# 289 "lib/logger.c"
  if ((output_file_name!=NULL))

# 289 "lib/logger.c"
  {

# 290 "lib/logger.c"
    ((global_logger_state.output)=fopen(output_file_name, "w"));

# 291 "lib/logger.c"
    if ((!(global_logger_state.output)))

# 291 "lib/logger.c"
    {

# 292 "lib/logger.c"
      fatal_error(ERROR_OPEN_LOG_FILE);
    }

# 299 "lib/logger.c"
    ((global_logger_state.logger_output_filename)=output_file_name);
  }
  else

# 300 "lib/logger.c"
  {

# 301 "lib/logger.c"
    ((global_logger_state.output)=stderr);

# 302 "lib/logger.c"
    ((global_logger_state.initialized)=true);
  }
}


# 335 "lib/logger.c"
__attribute__((format(printf, 5, 6))) void logger_impl(char* file, int line_number, const char* function, int level, char* format, ...)
# 336 "lib/logger.c"
{
  FILE* output = (global_logger_state.output);

# 342 "lib/logger.c"
  if ((output==NULL))

# 342 "lib/logger.c"
  {

# 343 "lib/logger.c"
    (output=stderr);
  }

# 346 "lib/logger.c"
  if ((level>=(global_logger_state.level)))

# 346 "lib/logger.c"
  {

# 347 "lib/logger.c"
    fprintf(output, "%s ", logger_level_to_string(level));
    va_list args;

# 349 "lib/logger.c"
    fprintf(output, "%s:%d %s | ", file, line_number, function);

# 351 "lib/logger.c"
    va_start(args, format);

# 352 "lib/logger.c"
    vfprintf(output, format, args);

# 353 "lib/logger.c"
    va_end(args);

# 355 "lib/logger.c"
    fprintf(output, "\n");
  }
}


# 228 "lib/logger.c"
value_result_t parse_log_level_enum(char* str)
# 228 "lib/logger.c"
{

# 229 "lib/logger.c"
  if (((strcmp("FATAL", str)==0)||(strcmp("fatal", str)==0)))

# 229 "lib/logger.c"
  {

# 230 "lib/logger.c"
    return ((value_result_t) {.u64 = LOGGER_FATAL});
  }
  else

# 231 "lib/logger.c"
  if (((strcmp("WARN", str)==0)||(strcmp("warn", str)==0)))

# 231 "lib/logger.c"
  {

# 232 "lib/logger.c"
    return ((value_result_t) {.u64 = LOGGER_WARN});
  }
  else

# 233 "lib/logger.c"
  if (((strcmp("INFO", str)==0)||(strcmp("info", str)==0)))

# 233 "lib/logger.c"
  {

# 234 "lib/logger.c"
    return ((value_result_t) {.u64 = LOGGER_INFO});
  }
  else

# 235 "lib/logger.c"
  if (((strcmp("DEBUG", str)==0)||(strcmp("debug", str)==0)))

# 235 "lib/logger.c"
  {

# 236 "lib/logger.c"
    return ((value_result_t) {.u64 = LOGGER_DEBUG});
  }
  else

# 237 "lib/logger.c"
  if (((strcmp("TRACE", str)==0)||(strcmp("trace", str)==0)))

# 237 "lib/logger.c"
  {

# 238 "lib/logger.c"
    return ((value_result_t) {.u64 = LOGGER_TRACE});
  }
  else

# 239 "lib/logger.c"
  if (((strcmp("OFF", str)==0)||(strcmp("off", str)==0)))

# 239 "lib/logger.c"
  {

# 240 "lib/logger.c"
    return ((value_result_t) {.u64 = LOGGER_OFF});
  }
  else

# 241 "lib/logger.c"
  {

# 242 "lib/logger.c"
    return ((value_result_t) {.nf_error = NF_ERROR_NOT_PARSED_AS_EXPECTED_ENUM});
  }
}


# 308 "lib/logger.c"
char* logger_level_to_string(int level)
# 308 "lib/logger.c"
{

# 309 "lib/logger.c"
  switch (level)

# 309 "lib/logger.c"
  {

# 310 "lib/logger.c"
    case LOGGER_OFF:

# 311 "lib/logger.c"
    return "LOGGER_OFF";

# 312 "lib/logger.c"
    case LOGGER_TRACE:

# 313 "lib/logger.c"
    return "TRACE";

# 314 "lib/logger.c"
    case LOGGER_DEBUG:

# 315 "lib/logger.c"
    return "DEBUG";

# 316 "lib/logger.c"
    case LOGGER_INFO:

# 317 "lib/logger.c"
    return "INFO";

# 318 "lib/logger.c"
    case LOGGER_WARN:

# 319 "lib/logger.c"
    return "WARN";

# 320 "lib/logger.c"
    case LOGGER_FATAL:

# 321 "lib/logger.c"
    return "FATAL";

# 322 "lib/logger.c"
    default:

# 323 "lib/logger.c"
    return "LEVEL_UNKNOWN";
  }
}


# 34 "lib/utf8-decoder.c"
utf8_decode_result_t utf8_decode(const uint8_t* array)
# 34 "lib/utf8-decoder.c"
{
  uint8_t firstByte = (array[0]);

# 36 "lib/utf8-decoder.c"
  if (((firstByte&0x80)==0))

# 36 "lib/utf8-decoder.c"
  {

# 37 "lib/utf8-decoder.c"
    return ((utf8_decode_result_t) {.code_point = firstByte, .num_bytes = 1});
  }
  else

# 39 "lib/utf8-decoder.c"
  if (((firstByte&0xE0)==0xC0))

# 39 "lib/utf8-decoder.c"
  {

# 40 "lib/utf8-decoder.c"
    return ((utf8_decode_result_t) {.code_point = ((firstByte & 0x1F) << 6) | (array[1] & 0x3F),
         .num_bytes = 2});
  }
  else

# 44 "lib/utf8-decoder.c"
  if (((firstByte&0xF0)==0xE0))

# 44 "lib/utf8-decoder.c"
  {

# 45 "lib/utf8-decoder.c"
    return ((utf8_decode_result_t) {.code_point = ((firstByte & 0x0F) << 12)
                                           | ((array[1] & 0x3F) << 6)
                                           | (array[2] & 0x3F),
                             .num_bytes = 3});
  }
  else

# 50 "lib/utf8-decoder.c"
  if (((firstByte&0xF8)==0xF0))

# 50 "lib/utf8-decoder.c"
  {

# 51 "lib/utf8-decoder.c"
    return ((utf8_decode_result_t) {.code_point = ((firstByte & 0x07) << 18) | ((array[1] & 0x3F) << 12)
                       | ((array[2] & 0x3F) << 6) | (array[3] & 0x3F),
         .num_bytes = 4});
  }
  else

# 56 "lib/utf8-decoder.c"
  {

# 57 "lib/utf8-decoder.c"
    return ((utf8_decode_result_t) {.error = true});
  }
}


# 46 "lib/buffer.c"
buffer_t* make_buffer(uint64_t initial_capacity)
# 46 "lib/buffer.c"
{
  buffer_t* result = malloc_struct(buffer_t);

# 48 "lib/buffer.c"
  if ((initial_capacity<16))

# 48 "lib/buffer.c"
  {

# 49 "lib/buffer.c"
    (initial_capacity=16);
  }

# 51 "lib/buffer.c"
  if ((initial_capacity>0))

# 51 "lib/buffer.c"
  {

# 52 "lib/buffer.c"
    ((result->capacity)=initial_capacity);

# 53 "lib/buffer.c"
    ((result->elements)=malloc_bytes(initial_capacity));
  }

# 55 "lib/buffer.c"
  return result;
}


# 63 "lib/buffer.c"
uint64_t buffer_length(buffer_t* array)
# 63 "lib/buffer.c"
{

# 63 "lib/buffer.c"
  return (array->length);
}


# 70 "lib/buffer.c"
void buffer_clear(buffer_t* buffer)
# 70 "lib/buffer.c"
{

# 71 "lib/buffer.c"
  for (
    int i = 0;
    (i<(buffer->capacity));
    (i++))

# 71 "lib/buffer.c"
  {

# 72 "lib/buffer.c"
    (((buffer->elements)[i])=0);
  }

# 74 "lib/buffer.c"
  ((buffer->length)=0);
}


# 82 "lib/buffer.c"
uint8_t buffer_get(buffer_t* buffer, uint64_t position)
# 82 "lib/buffer.c"
{

# 83 "lib/buffer.c"
  if ((position<(buffer->length)))

# 83 "lib/buffer.c"
  {

# 84 "lib/buffer.c"
    return ((buffer->elements)[position]);
  }
  else

# 85 "lib/buffer.c"
  {

# 86 "lib/buffer.c"
    fatal_error(ERROR_ACCESS_OUT_OF_BOUNDS);

# 89 "lib/buffer.c"
    return 0;
  }
}


# 100 "lib/buffer.c"
char* buffer_c_substring(buffer_t* buffer, uint64_t start, uint64_t end)
# 100 "lib/buffer.c"
{

# 101 "lib/buffer.c"
  if ((buffer==((void *)0)))

# 101 "lib/buffer.c"
  {

# 102 "lib/buffer.c"
    fatal_error(ERROR_ILLEGAL_NULL_ARGUMENT);
  }

# 105 "lib/buffer.c"
  if ((start>end))

# 105 "lib/buffer.c"
  {

# 106 "lib/buffer.c"
    fatal_error(ERROR_ILLEGAL_RANGE);
  }
  uint64_t copy_length = (end-start);
  char* result = (/*CAST*/(char*) malloc_bytes((copy_length+1)));

# 111 "lib/buffer.c"
  if ((copy_length>0))

# 111 "lib/buffer.c"
  {

# 112 "lib/buffer.c"
    memcpy(result, (&((buffer->elements)[start])), copy_length);
  }

# 114 "lib/buffer.c"
  ((result[copy_length])='\0');

# 115 "lib/buffer.c"
  return result;
}


# 124 "lib/buffer.c"
char* buffer_to_c_string(buffer_t* buffer)
# 124 "lib/buffer.c"
{

# 125 "lib/buffer.c"
  return buffer_c_substring(buffer, 0, (buffer->length));
}


# 133 "lib/buffer.c"
buffer_t* buffer_append_byte(buffer_t* buffer, uint8_t element)
# 133 "lib/buffer.c"
{

# 134 "lib/buffer.c"
  if (((buffer->length)<(buffer->capacity)))

# 134 "lib/buffer.c"
  {

# 135 "lib/buffer.c"
    (((buffer->elements)[(buffer->length)])=element);

# 136 "lib/buffer.c"
    ((buffer->length)++);

# 137 "lib/buffer.c"
    return buffer;
  }

# 139 "lib/buffer.c"
  (buffer=buffer_increase_capacity(buffer, ((buffer->capacity)*2)));

# 140 "lib/buffer.c"
  return buffer_append_byte(buffer, element);
}


# 148 "lib/buffer.c"
buffer_t* buffer_append_bytes(buffer_t* buffer, uint8_t* bytes, uint64_t n_bytes)
# 149 "lib/buffer.c"
{

# 151 "lib/buffer.c"
  for (
    int i = 0;
    (i<n_bytes);
    (i++))

# 151 "lib/buffer.c"
  {

# 152 "lib/buffer.c"
    (buffer=buffer_append_byte(buffer, (bytes[i])));
  }

# 154 "lib/buffer.c"
  return buffer;
}


# 163 "lib/buffer.c"
buffer_t* buffer_append_string(buffer_t* buffer, const char* str)
# 163 "lib/buffer.c"
{

# 164 "lib/buffer.c"
  return buffer_append_bytes(buffer, (/*CAST*/(uint8_t*) str), strlen(str));
}


# 175 "lib/buffer.c"
extern buffer_t* buffer_increase_capacity(buffer_t* buffer, uint64_t capacity)
# 175 "lib/buffer.c"
{

# 176 "lib/buffer.c"
  if (((buffer->capacity)<capacity))

# 176 "lib/buffer.c"
  {
    uint8_t* new_elements = malloc_bytes(capacity);

# 178 "lib/buffer.c"
    memcpy(new_elements, (buffer->elements), (buffer->length));

# 179 "lib/buffer.c"
    free_bytes((buffer->elements));

# 180 "lib/buffer.c"
    ((buffer->elements)=new_elements);

# 181 "lib/buffer.c"
    ((buffer->capacity)=capacity);
  }

# 183 "lib/buffer.c"
  return buffer;
}


# 197 "lib/buffer.c"
__attribute__((format(printf, 2, 3))) buffer_t* buffer_printf(buffer_t* buffer, char* format, ...)
# 197 "lib/buffer.c"
{
  char cbuffer[BUFFER_PRINTF_INITIAL_BUFFER_SIZE];
  int n_bytes = 0;

# 200 "lib/buffer.c"
  do
# 200 "lib/buffer.c"
  {
    va_list args;

# 202 "lib/buffer.c"
    va_start(args, format);

# 203 "lib/buffer.c"
    (n_bytes=vsnprintf(cbuffer, BUFFER_PRINTF_INITIAL_BUFFER_SIZE, format, args));

# 205 "lib/buffer.c"
    va_end(args);
  }
  while (0);

# 208 "lib/buffer.c"
  if ((n_bytes<BUFFER_PRINTF_INITIAL_BUFFER_SIZE))

# 208 "lib/buffer.c"
  {

# 209 "lib/buffer.c"
    return buffer_append_string(buffer, cbuffer);
  }
  else

# 210 "lib/buffer.c"
  {
    char* result = (/*CAST*/(char*) malloc_bytes((n_bytes+1)));
    va_list args;

# 216 "lib/buffer.c"
    va_start(args, format);
    int n_bytes_second = vsnprintf(result, (n_bytes+1), format, args);

# 218 "lib/buffer.c"
    va_end(args);

# 219 "lib/buffer.c"
    if ((n_bytes_second!=n_bytes))

# 219 "lib/buffer.c"
    {

# 220 "lib/buffer.c"
      fatal_error(ERROR_INTERNAL_ASSERTION_FAILURE);
    }

# 222 "lib/buffer.c"
    (buffer=buffer_append_string(buffer, result));

# 223 "lib/buffer.c"
    free_bytes(result);

# 224 "lib/buffer.c"
    return buffer;
  }
}


# 235 "lib/buffer.c"
extern buffer_t* buffer_append_repeated_byte(buffer_t* buffer, uint8_t byte, int count)
# 236 "lib/buffer.c"
{

# 237 "lib/buffer.c"
  for (
    int i = 0;
    (i<count);
    (i++))

# 237 "lib/buffer.c"
  {

# 238 "lib/buffer.c"
    (buffer=buffer_append_byte(buffer, byte));
  }

# 240 "lib/buffer.c"
  return buffer;
}


# 249 "lib/buffer.c"
utf8_decode_result_t buffer_utf8_decode(buffer_t* buffer, uint64_t position)
# 249 "lib/buffer.c"
{

# 250 "lib/buffer.c"
  if ((position>=(buffer->length)))

# 250 "lib/buffer.c"
  {

# 251 "lib/buffer.c"
    return ((utf8_decode_result_t) {.error = true});
  }
  utf8_decode_result_t result = utf8_decode((&((buffer->elements)[position])));

# 254 "lib/buffer.c"
  if ((result.error))

# 254 "lib/buffer.c"
  {

# 255 "lib/buffer.c"
    return result;
  }

# 257 "lib/buffer.c"
  if (((position+(result.num_bytes))>(buffer->length)))

# 257 "lib/buffer.c"
  {

# 258 "lib/buffer.c"
    return ((utf8_decode_result_t) {.error = true});
  }

# 260 "lib/buffer.c"
  return result;
}


# 275 "lib/buffer.c"
extern buffer_t* buffer_append_code_point(buffer_t* buffer, uint32_t code_point)
# 276 "lib/buffer.c"
{

# 277 "lib/buffer.c"
  if ((code_point<0x80))

# 277 "lib/buffer.c"
  {

# 279 "lib/buffer.c"
    (buffer=buffer_append_byte(buffer, code_point));

# 280 "lib/buffer.c"
    return buffer;
  }
  else

# 281 "lib/buffer.c"
  if ((code_point<0x800))

# 281 "lib/buffer.c"
  {

# 283 "lib/buffer.c"
    (buffer=buffer_append_byte(buffer, (0xc0|(code_point>>6))));

# 284 "lib/buffer.c"
    (buffer=buffer_append_byte(buffer, (0x80|(code_point&0x3f))));

# 285 "lib/buffer.c"
    return buffer;
  }
  else

# 286 "lib/buffer.c"
  if ((code_point<0x10000))

# 286 "lib/buffer.c"
  {

# 288 "lib/buffer.c"
    (buffer=buffer_append_byte(buffer, (0xe0|(code_point>>12))));

# 289 "lib/buffer.c"
    (buffer=buffer_append_byte(buffer, (0x80|((code_point>>6)&0x3f))));

# 290 "lib/buffer.c"
    (buffer=buffer_append_byte(buffer, (0x80|(code_point&0x3f))));

# 291 "lib/buffer.c"
    return buffer;
  }
  else

# 292 "lib/buffer.c"
  if ((code_point<=0x10FFFF))

# 292 "lib/buffer.c"
  {

# 294 "lib/buffer.c"
    (buffer=buffer_append_byte(buffer, (0xf0|(code_point>>18))));

# 295 "lib/buffer.c"
    (buffer=buffer_append_byte(buffer, (0x80|((code_point>>12)&0x3f))));

# 296 "lib/buffer.c"
    (buffer=buffer_append_byte(buffer, (0x80|((code_point>>6)&0x3f))));

# 297 "lib/buffer.c"
    (buffer=buffer_append_byte(buffer, (0x80|(code_point&0x3f))));

# 298 "lib/buffer.c"
    return buffer;
  }
  else

# 299 "lib/buffer.c"
  {

# 301 "lib/buffer.c"
    fatal_error(ERROR_ILLEGAL_UTF_8_CODE_POINT);

# 302 "lib/buffer.c"
    return 0;
  }
}


# 310 "lib/buffer.c"
boolean_t buffer_match_string_at(buffer_t* buffer, uint64_t start_position, char* str)
# 311 "lib/buffer.c"
{

# 312 "lib/buffer.c"
  for (
    uint64_t pos = start_position;
    true;
    (pos++))

# 312 "lib/buffer.c"
  {
    uint8_t byte_str = ((/*CAST*/(uint8_t*) str)[(pos-start_position)]);

# 314 "lib/buffer.c"
    if ((byte_str==0))

# 314 "lib/buffer.c"
    {

# 315 "lib/buffer.c"
      return true;
    }

# 317 "lib/buffer.c"
    if ((pos>=buffer_length(buffer)))

# 317 "lib/buffer.c"
    {

# 318 "lib/buffer.c"
      return false;
    }
    uint8_t byte_buf = buffer_get(buffer, pos);

# 321 "lib/buffer.c"
    if ((byte_str!=byte_buf))

# 321 "lib/buffer.c"
    {

# 322 "lib/buffer.c"
      return false;
    }
  }

# 326 "lib/buffer.c"
  return false;
}


# 336 "lib/buffer.c"
buffer_t* buffer_from_string(char* string)
# 336 "lib/buffer.c"
{
  buffer_t* result = make_buffer(strlen(string));

# 338 "lib/buffer.c"
  (result=buffer_append_string(result, string));

# 339 "lib/buffer.c"
  return result;
}


# 349 "lib/buffer.c"
buffer_t* buffer_adjust_region(buffer_t* buffer, uint64_t start, uint64_t end, uint64_t new_width)
# 350 "lib/buffer.c"
{

# 352 "lib/buffer.c"
  if ((start>end))

# 352 "lib/buffer.c"
  {

# 353 "lib/buffer.c"
    fatal_error(ERROR_ILLEGAL_RANGE);
  }
  uint64_t original_width = (end-start);

# 356 "lib/buffer.c"
  if ((original_width>new_width))

# 356 "lib/buffer.c"
  {
    uint64_t difference = (original_width-new_width);
    uint64_t tail_length = ((buffer->length)-end);

# 361 "lib/buffer.c"
    memmove((&((buffer->elements)[(end-difference)])), (&((buffer->elements)[end])), tail_length);

# 363 "lib/buffer.c"
    ((buffer->length)-=difference);
  }
  else

# 364 "lib/buffer.c"
  if ((original_width<new_width))

# 364 "lib/buffer.c"
  {
    uint64_t difference = (new_width-original_width);
    uint64_t tail_length = ((buffer->length)-end);

# 369 "lib/buffer.c"
    (buffer=buffer_increase_capacity(buffer, ((buffer->length)+difference)));

# 370 "lib/buffer.c"
    memmove((&((buffer->elements)[(end+difference)])), (&((buffer->elements)[end])), tail_length);

# 372 "lib/buffer.c"
    ((buffer->length)+=difference);
  }

# 374 "lib/buffer.c"
  return buffer;
}


# 383 "lib/buffer.c"
buffer_t* buffer_replace_all(buffer_t* buffer, char* original_text, char* replacement_text)
# 384 "lib/buffer.c"
{
  int len_original = strlen(original_text);
  int len_replacement = strlen(replacement_text);

# 387 "lib/buffer.c"
  if ((len_original<(buffer->length)))

# 387 "lib/buffer.c"
  {
    uint64_t pos = 0;

# 389 "lib/buffer.c"
    while ((pos<=((buffer->length)-len_original)))

# 389 "lib/buffer.c"
    {

# 390 "lib/buffer.c"
      if (buffer_match_string_at(buffer, pos, original_text))

# 390 "lib/buffer.c"
      {

# 391 "lib/buffer.c"
        (buffer=buffer_adjust_region(buffer, pos, (pos+len_original), len_replacement));

# 393 "lib/buffer.c"
        memmove((&((buffer->elements)[pos])), replacement_text, len_replacement);

# 394 "lib/buffer.c"
        (pos+=len_replacement);
      }
      else

# 395 "lib/buffer.c"
      {

# 396 "lib/buffer.c"
        (pos++);
      }
    }
  }

# 400 "lib/buffer.c"
  return buffer;
}


# 403 "lib/buffer.c"
line_and_column_t buffer_position_to_line_and_column(buffer_t* buffer, uint64_t position)
# 404 "lib/buffer.c"
{
  uint64_t line = 1;
  uint64_t column = 1;

# 410 "lib/buffer.c"
  for (
    uint64_t pos = 0;
    (pos<position);
    (pos++))

# 410 "lib/buffer.c"
  {
    uint8_t ch = buffer_get(buffer, pos);

# 412 "lib/buffer.c"
    if ((ch=='\n'))

# 412 "lib/buffer.c"
    {

# 413 "lib/buffer.c"
      (line++);

# 414 "lib/buffer.c"
      (column=1);
    }
    else

# 415 "lib/buffer.c"
    {

# 416 "lib/buffer.c"
      (column++);
    }
  }

# 419 "lib/buffer.c"
  return ((line_and_column_t) {
                                                 .line = line,
                                                 .column = column,
                                             });
}


# 430 "lib/buffer.c"
boolean_t buffer_region_contains(buffer_t* buffer, uint64_t start, uint64_t end, char* text)
# 431 "lib/buffer.c"
{

# 432 "lib/buffer.c"
  for (
    int i = start;
    (i<end);
    (i++))

# 432 "lib/buffer.c"
  {

# 433 "lib/buffer.c"
    if (buffer_match_string_at(buffer, i, text))

# 433 "lib/buffer.c"
    {

# 434 "lib/buffer.c"
      return true;
    }
  }

# 437 "lib/buffer.c"
  return false;
}


# 448 "lib/buffer.c"
buffer_t* buffer_replace_matching_byte(buffer_t* buffer, uint8_t original, uint8_t replacement)
# 449 "lib/buffer.c"
{

# 450 "lib/buffer.c"
  for (
    int i = 0;
    (i<(buffer->length));
    (i++))

# 450 "lib/buffer.c"
  {

# 451 "lib/buffer.c"
    if ((((buffer->elements)[i])==original))

# 451 "lib/buffer.c"
    {

# 452 "lib/buffer.c"
      (((buffer->elements)[i])=replacement);
    }
  }

# 455 "lib/buffer.c"
  return buffer;
}


# 465 "lib/buffer.c"
uint64_t buffer_beginning_of_line(buffer_t* buffer, uint64_t start)
# 465 "lib/buffer.c"
{
  uint64_t position = start;

# 467 "lib/buffer.c"
  while ((position>0))

# 467 "lib/buffer.c"
  {

# 468 "lib/buffer.c"
    (position--);

# 469 "lib/buffer.c"
    if ((buffer_get(buffer, position)=='\n'))

# 469 "lib/buffer.c"
    {

# 470 "lib/buffer.c"
      return (position+1);
    }
  }

# 473 "lib/buffer.c"
  return position;
}


# 484 "lib/buffer.c"
uint64_t buffer_end_of_line(buffer_t* buffer, uint64_t start)
# 484 "lib/buffer.c"
{
  uint64_t position = start;

# 486 "lib/buffer.c"
  while (((position<(buffer->length))&&(buffer_get(buffer, position)!='\n')))

# 486 "lib/buffer.c"
  {

# 487 "lib/buffer.c"
    (position++);
  }

# 489 "lib/buffer.c"
  return position;
}


# 499 "lib/buffer.c"
extern buffer_t* buffer_append_buffer(buffer_t* buffer, buffer_t* src_buffer)
# 499 "lib/buffer.c"
{

# 500 "lib/buffer.c"
  return buffer_append_sub_buffer(buffer, 0, (src_buffer->length), src_buffer);
}


# 511 "lib/buffer.c"
extern buffer_t* buffer_append_sub_buffer(buffer_t* buffer, uint64_t start_position, uint64_t end_position, buffer_t* src_buffer)
# 514 "lib/buffer.c"
{

# 515 "lib/buffer.c"
  if ((buffer==src_buffer))

# 515 "lib/buffer.c"
  {

# 516 "lib/buffer.c"
    fatal_error(ERROR_ILLEGAL_STATE);
  }

# 518 "lib/buffer.c"
  for (
    uint64_t position = start_position;
    (position<end_position);
    (position++))

# 519 "lib/buffer.c"
  {

# 520 "lib/buffer.c"
    (buffer=buffer_append_byte(buffer, buffer_get(src_buffer, position)));
  }

# 522 "lib/buffer.c"
  return buffer;
}


# 534 "lib/buffer.c"
buffer_t* buffer_to_uppercase(buffer_t* buffer)
# 534 "lib/buffer.c"
{

# 535 "lib/buffer.c"
  for (
    uint64_t i = 0;
    (i<(buffer->length));
    (i++))

# 535 "lib/buffer.c"
  {

# 536 "lib/buffer.c"
    (((buffer->elements)[i])=toupper(((buffer->elements)[i])));
  }

# 538 "lib/buffer.c"
  return buffer;
}


# 550 "lib/buffer.c"
buffer_t* buffer_to_lowercase(buffer_t* buffer)
# 550 "lib/buffer.c"
{

# 551 "lib/buffer.c"
  for (
    uint64_t i = 0;
    (i<(buffer->length));
    (i++))

# 551 "lib/buffer.c"
  {

# 552 "lib/buffer.c"
    (((buffer->elements)[i])=tolower(((buffer->elements)[i])));
  }

# 554 "lib/buffer.c"
  return buffer;
}


# 17 "lib/byte-stream.c"
byte_stream_source_t* buffer_to_byte_source(buffer_t* buffer)
# 17 "lib/byte-stream.c"
{
  byte_stream_source_t* result = malloc_struct(byte_stream_source_t);

# 19 "lib/byte-stream.c"
  ((result->read_byte)=(&buffer_stream_source_read));
  buffer_byte_stream_source_data_t* data = malloc_struct(buffer_byte_stream_source_data_t);

# 21 "lib/byte-stream.c"
  ((data->buffer)=buffer);

# 22 "lib/byte-stream.c"
  ((data->position)=0);

# 23 "lib/byte-stream.c"
  ((result->data)=(/*CAST*/(void*) data));

# 24 "lib/byte-stream.c"
  return result;
}


# 32 "lib/byte-stream.c"
uint8_t buffer_stream_source_read(byte_stream_source_t* source, boolean_t* has_byte)
# 32 "lib/byte-stream.c"
{
  buffer_byte_stream_source_data_t* data = (/*CAST*/(buffer_byte_stream_source_data_t*) (source->data));
  uint8_t result = 0;

# 35 "lib/byte-stream.c"
  if (((data->position)<buffer_length((data->buffer))))

# 35 "lib/byte-stream.c"
  {

# 36 "lib/byte-stream.c"
    (result=buffer_get((data->buffer), ((data->position)++)));

# 37 "lib/byte-stream.c"
    ((*has_byte)=true);
  }
  else

# 38 "lib/byte-stream.c"
  {

# 39 "lib/byte-stream.c"
    ((*has_byte)=false);
  }

# 41 "lib/byte-stream.c"
  return result;
}


# 46 "lib/byte-stream.c"
byte_stream_source_t* cstring_to_byte_source(char* string)
# 46 "lib/byte-stream.c"
{
  byte_stream_source_t* result = malloc_struct(byte_stream_source_t);

# 48 "lib/byte-stream.c"
  ((result->read_byte)=(&cstring_stream_source_read));
  cstring_byte_stream_source_data_t* data = malloc_struct(cstring_byte_stream_source_data_t);

# 50 "lib/byte-stream.c"
  ((data->string)=string);

# 51 "lib/byte-stream.c"
  ((data->length)=strlen(string));

# 52 "lib/byte-stream.c"
  ((data->position)=0);

# 53 "lib/byte-stream.c"
  ((result->data)=(/*CAST*/(void*) data));

# 54 "lib/byte-stream.c"
  return result;
}


# 63 "lib/byte-stream.c"
uint8_t cstring_stream_source_read(byte_stream_source_t* source, boolean_t* has_byte)
# 63 "lib/byte-stream.c"
{
  cstring_byte_stream_source_data_t* data = (/*CAST*/(cstring_byte_stream_source_data_t*) (source->data));
  uint8_t result = 0;

# 66 "lib/byte-stream.c"
  if (((data->position)<(data->length)))

# 66 "lib/byte-stream.c"
  {

# 67 "lib/byte-stream.c"
    (result=((data->string)[((data->position)++)]));

# 68 "lib/byte-stream.c"
    ((*has_byte)=true);
  }
  else

# 69 "lib/byte-stream.c"
  {

# 70 "lib/byte-stream.c"
    ((*has_byte)=false);
  }

# 72 "lib/byte-stream.c"
  return result;
}


# 77 "lib/byte-stream.c"
byte_stream_target_t* buffer_to_byte_target(buffer_t* buffer)
# 77 "lib/byte-stream.c"
{
  byte_stream_target_t* result = malloc_struct(byte_stream_target_t);

# 79 "lib/byte-stream.c"
  ((result->write_byte)=(&buffer_stream_target_write));

# 80 "lib/byte-stream.c"
  ((result->data)=(/*CAST*/(void*) buffer));

# 81 "lib/byte-stream.c"
  return result;
}


# 84 "lib/byte-stream.c"
byte_stream_target_t* buffer_stream_target_write(byte_stream_target_t* target, uint8_t byte)
# 84 "lib/byte-stream.c"
{
  buffer_t* buffer = (/*CAST*/(buffer_t*) (target->data));

# 86 "lib/byte-stream.c"
  buffer_append_byte(buffer, byte);

# 87 "lib/byte-stream.c"
  return target;
}


# 66 "lib/value-array.c"
value_array_t* make_value_array(uint64_t initial_capacity)
# 66 "lib/value-array.c"
{

# 67 "lib/value-array.c"
  if ((initial_capacity==0))

# 67 "lib/value-array.c"
  {

# 68 "lib/value-array.c"
    (initial_capacity=1);
  }
  value_array_t* result = malloc_struct(value_array_t);

# 72 "lib/value-array.c"
  ((result->capacity)=initial_capacity);

# 73 "lib/value-array.c"
  ((result->elements)=(/*CAST*/(value_t*) malloc_bytes(((sizeof(value_t))*initial_capacity))));

# 76 "lib/value-array.c"
  return result;
}


# 104 "lib/value-array.c"
value_t value_array_get(value_array_t* array, uint32_t index)
# 104 "lib/value-array.c"
{

# 105 "lib/value-array.c"
  if ((index<(array->length)))

# 105 "lib/value-array.c"
  {

# 106 "lib/value-array.c"
    return ((array->elements)[index]);
  }

# 108 "lib/value-array.c"
  fatal_error(ERROR_ACCESS_OUT_OF_BOUNDS);

# 110 "lib/value-array.c"
  return ((value_t) {0});
}


# 119 "lib/value-array.c"
void value_array_replace(value_array_t* array, uint32_t index, value_t element)
# 120 "lib/value-array.c"
{

# 121 "lib/value-array.c"
  if ((index<(array->length)))

# 121 "lib/value-array.c"
  {

# 122 "lib/value-array.c"
    (((array->elements)[index])=element);

# 123 "lib/value-array.c"
    return;
  }

# 125 "lib/value-array.c"
  fatal_error(ERROR_ACCESS_OUT_OF_BOUNDS);
}


# 136 "lib/value-array.c"
void value_array_add(value_array_t* array, value_t element)
# 136 "lib/value-array.c"
{

# 137 "lib/value-array.c"
  value_array_ensure_capacity(array, ((array->length)+1));

# 138 "lib/value-array.c"
  (((array->elements)[((array->length)++)])=element);
}


# 147 "lib/value-array.c"
void value_array_push(value_array_t* array, value_t element)
# 147 "lib/value-array.c"
{

# 148 "lib/value-array.c"
  value_array_add(array, element);
}


# 162 "lib/value-array.c"
value_t value_array_pop(value_array_t* array)
# 162 "lib/value-array.c"
{

# 163 "lib/value-array.c"
  if (((array->length)==0))

# 163 "lib/value-array.c"
  {

# 164 "lib/value-array.c"
    fatal_error(ERROR_ACCESS_OUT_OF_BOUNDS);
  }
  uint32_t last_index = ((array->length)-1);
  value_t result = value_array_get(array, last_index);

# 168 "lib/value-array.c"
  (((array->elements)[last_index])=u64_to_value(0));

# 169 "lib/value-array.c"
  ((array->length)--);

# 170 "lib/value-array.c"
  return result;
}


# 190 "lib/value-array.c"
void value_array_insert_at(value_array_t* array, uint32_t position, value_t element)
# 191 "lib/value-array.c"
{

# 192 "lib/value-array.c"
  if ((position==(array->length)))

# 192 "lib/value-array.c"
  {

# 193 "lib/value-array.c"
    value_array_add(array, element);

# 194 "lib/value-array.c"
    return;
  }

# 197 "lib/value-array.c"
  if ((position>(array->length)))

# 197 "lib/value-array.c"
  {

# 198 "lib/value-array.c"
    fatal_error(ERROR_ACCESS_OUT_OF_BOUNDS);

# 199 "lib/value-array.c"
    return;
  }

# 202 "lib/value-array.c"
  value_array_ensure_capacity(array, ((array->length)+1));

# 207 "lib/value-array.c"
  for (
    int64_t i = ((array->length)-1);
    (i>=position);
    (i--))

# 207 "lib/value-array.c"
  {

# 208 "lib/value-array.c"
    (((array->elements)[(i+1)])=((array->elements)[i]));
  }

# 210 "lib/value-array.c"
  ((array->length)++);

# 211 "lib/value-array.c"
  (((array->elements)[position])=element);
}


# 223 "lib/value-array.c"
value_t value_array_delete_at(value_array_t* array, uint32_t position)
# 223 "lib/value-array.c"
{
  value_t result = value_array_get(array, position);

# 225 "lib/value-array.c"
  for (
    int i = position;
    (i<((array->length)-1));
    (i++))

# 225 "lib/value-array.c"
  {

# 226 "lib/value-array.c"
    (((array->elements)[i])=((array->elements)[(i+1)]));
  }

# 228 "lib/value-array.c"
  ((array->length)--);

# 229 "lib/value-array.c"
  return result;
}


# 79 "lib/value-array.c"
void value_array_ensure_capacity(value_array_t* array, uint32_t required_capacity)
# 80 "lib/value-array.c"
{

# 81 "lib/value-array.c"
  if (((array->capacity)<required_capacity))

# 81 "lib/value-array.c"
  {
    uint32_t new_capacity = ((array->capacity)*2);

# 83 "lib/value-array.c"
    if ((new_capacity<required_capacity))

# 83 "lib/value-array.c"
    {

# 84 "lib/value-array.c"
      (new_capacity=required_capacity);
    }
    value_t* new_elements = (/*CAST*/(value_t*) malloc_bytes(((sizeof(value_t))*new_capacity)));

# 88 "lib/value-array.c"
    for (
      int i = 0;
      (i<(array->length));
      (i++))

# 88 "lib/value-array.c"
    {

# 89 "lib/value-array.c"
      ((new_elements[i])=((array->elements)[i]));
    }

# 91 "lib/value-array.c"
    ((array->capacity)=new_capacity);

# 92 "lib/value-array.c"
    free_bytes((array->elements));

# 93 "lib/value-array.c"
    ((array->elements)=new_elements);

# 94 "lib/value-array.c"
    return;
  }
}


# 97 "lib/value-alist.c"
value_result_t value_alist_find(value_alist_t* list, value_comparison_fn cmp_fn, value_t key)
# 98 "lib/value-alist.c"
{

# 99 "lib/value-alist.c"
  while (list)

# 99 "lib/value-alist.c"
  {

# 100 "lib/value-alist.c"
    if ((cmp_fn(key, (list->key))==0))

# 100 "lib/value-alist.c"
    {

# 101 "lib/value-alist.c"
      return ((value_result_t) {.val = list->value});
    }

# 103 "lib/value-alist.c"
    (list=(list->next));
  }

# 105 "lib/value-alist.c"
  return ((value_result_t) {.nf_error = NF_ERROR_NOT_FOUND});
}


# 57 "lib/value-alist.c"
value_alist_t* value_alist_insert(value_alist_t* list, value_comparison_fn cmp_fn, value_t key, value_t value)
# 59 "lib/value-alist.c"
{
  value_alist_t* result = malloc_struct(value_alist_t);

# 61 "lib/value-alist.c"
  ((result->next)=value_alist_delete(list, cmp_fn, key));

# 62 "lib/value-alist.c"
  ((result->key)=key);

# 63 "lib/value-alist.c"
  ((result->value)=value);

# 64 "lib/value-alist.c"
  return result;
}


# 74 "lib/value-alist.c"
value_alist_t* value_alist_delete(value_alist_t* list, value_comparison_fn cmp_fn, value_t key)
# 75 "lib/value-alist.c"
{

# 78 "lib/value-alist.c"
  if ((list==NULL))

# 78 "lib/value-alist.c"
  {

# 79 "lib/value-alist.c"
    return list;
  }

# 81 "lib/value-alist.c"
  if (((*cmp_fn)(key, (list->key))==0))

# 81 "lib/value-alist.c"
  {
    value_alist_t* result = (list->next);

# 83 "lib/value-alist.c"
    free_bytes(list);

# 84 "lib/value-alist.c"
    return result;
  }

# 86 "lib/value-alist.c"
  ((list->next)=value_alist_delete((list->next), cmp_fn, key));

# 87 "lib/value-alist.c"
  return list;
}


# 116 "lib/value-alist.c"
__attribute__((warn_unused_result)) extern uint64_t value_alist_length(value_alist_t* list)
# 116 "lib/value-alist.c"
{
  uint64_t result = 0;

# 118 "lib/value-alist.c"
  while (list)

# 118 "lib/value-alist.c"
  {

# 119 "lib/value-alist.c"
    (result++);

# 120 "lib/value-alist.c"
    (list=(list->next));
  }

# 122 "lib/value-alist.c"
  return result;
}


# 102 "lib/value-hashtable.c"
value_hashtable_t* make_value_hashtable(uint64_t n_buckets)
# 102 "lib/value-hashtable.c"
{

# 103 "lib/value-hashtable.c"
  if ((n_buckets<2))

# 103 "lib/value-hashtable.c"
  {

# 104 "lib/value-hashtable.c"
    (n_buckets=2);
  }
  value_hashtable_t* result = malloc_struct(value_hashtable_t);

# 107 "lib/value-hashtable.c"
  ((result->n_buckets)=n_buckets);

# 108 "lib/value-hashtable.c"
  ((result->buckets)=(/*CAST*/(value_alist_t**) malloc_bytes(((sizeof(typeof(value_alist_t*)))*n_buckets))));

# 111 "lib/value-hashtable.c"
  return result;
}


# 119 "lib/value-hashtable.c"
value_hashtable_t* value_ht_insert(value_hashtable_t* ht, value_hash_fn hash_fn, value_comparison_fn cmp_fn, value_t key, value_t value)
# 121 "lib/value-hashtable.c"
{
  uint64_t hashcode = hash_fn(key);
  int bucket = (hashcode%(ht->n_buckets));
  value_alist_t* list = ((ht->buckets)[bucket]);
  uint64_t len = value_alist_length(list);

# 126 "lib/value-hashtable.c"
  (list=value_alist_insert(list, cmp_fn, key, value));

# 127 "lib/value-hashtable.c"
  (((ht->buckets)[bucket])=list);
  uint64_t len_after = value_alist_length(list);

# 129 "lib/value-hashtable.c"
  if ((len_after>len))

# 129 "lib/value-hashtable.c"
  {

# 130 "lib/value-hashtable.c"
    ((ht->n_entries)++);

# 135 "lib/value-hashtable.c"
    if (((ht->n_entries)>=((ht->n_buckets)*ARMYKNIFE_HT_LOAD_FACTOR)))

# 135 "lib/value-hashtable.c"
    {

# 136 "lib/value-hashtable.c"
      value_hashtable_upsize_internal(ht, hash_fn, cmp_fn);
    }
  }

# 139 "lib/value-hashtable.c"
  return ht;
}


# 148 "lib/value-hashtable.c"
value_hashtable_t* value_ht_delete(value_hashtable_t* ht, value_hash_fn hash_fn, value_comparison_fn cmp_fn, value_t key)
# 149 "lib/value-hashtable.c"
{
  uint64_t hashcode = hash_fn(key);
  int bucket = (hashcode%(ht->n_buckets));
  value_alist_t* list = ((ht->buckets)[bucket]);
  uint64_t len = value_alist_length(list);

# 154 "lib/value-hashtable.c"
  (list=value_alist_delete(list, cmp_fn, key));

# 155 "lib/value-hashtable.c"
  (((ht->buckets)[bucket])=list);
  uint64_t len_after = value_alist_length(list);

# 157 "lib/value-hashtable.c"
  if ((len_after<len))

# 157 "lib/value-hashtable.c"
  {

# 158 "lib/value-hashtable.c"
    ((ht->n_entries)--);
  }

# 160 "lib/value-hashtable.c"
  return ht;
}


# 168 "lib/value-hashtable.c"
value_result_t value_ht_find(value_hashtable_t* ht, value_hash_fn hash_fn, value_comparison_fn cmp_fn, value_t key)
# 169 "lib/value-hashtable.c"
{
  uint64_t hashcode = hash_fn(key);
  int bucket = (hashcode%(ht->n_buckets));
  value_alist_t* list = ((ht->buckets)[bucket]);

# 173 "lib/value-hashtable.c"
  return value_alist_find(list, cmp_fn, key);
}


# 190 "lib/value-hashtable.c"
void value_hashtable_upsize_internal(value_hashtable_t* ht, value_hash_fn hash_fn, value_comparison_fn cmp_fn)
# 192 "lib/value-hashtable.c"
{
  uint64_t new_num_buckets = ((ht->n_buckets)*AK_HT_UPSCALE_MULTIPLIER);
  value_hashtable_t* new_ht = make_value_hashtable(new_num_buckets);

# 196 "lib/value-hashtable.c"
  value_ht_foreach(ht, key, value, 
# 196 "lib/value-hashtable.c"
  {
    value_hashtable_t* should_be_result = value_ht_insert(new_ht, hash_fn, cmp_fn, key, value);

# 203 "lib/value-hashtable.c"
    if ((new_ht!=should_be_result))

# 203 "lib/value-hashtable.c"
    {

# 204 "lib/value-hashtable.c"
      fatal_error(ERROR_ILLEGAL_STATE);
    }
  }
);
  value_alist_t** old_buckets = (ht->buckets);

# 209 "lib/value-hashtable.c"
  ((ht->buckets)=(new_ht->buckets));

# 210 "lib/value-hashtable.c"
  ((ht->n_buckets)=(new_ht->n_buckets));

# 211 "lib/value-hashtable.c"
  ((ht->n_entries)=(new_ht->n_entries));

# 212 "lib/value-hashtable.c"
  free_bytes(old_buckets);

# 213 "lib/value-hashtable.c"
  free_bytes(new_ht);
}


# 91 "lib/value-tree.c"
value_result_t value_tree_find(value_tree_t* t, value_comparison_fn cmp_fn, value_t key)
# 92 "lib/value-tree.c"
{

# 93 "lib/value-tree.c"
  if ((t==NULL))

# 93 "lib/value-tree.c"
  {

# 94 "lib/value-tree.c"
    return ((value_result_t) {.nf_error = NF_ERROR_NOT_FOUND});
  }
  int cmp_result = cmp_fn(key, (t->key));

# 98 "lib/value-tree.c"
  if ((cmp_result<0))

# 98 "lib/value-tree.c"
  {

# 99 "lib/value-tree.c"
    return value_tree_find((t->left), cmp_fn, key);
  }
  else

# 100 "lib/value-tree.c"
  if ((cmp_result>0))

# 100 "lib/value-tree.c"
  {

# 101 "lib/value-tree.c"
    return value_tree_find((t->right), cmp_fn, key);
  }
  else

# 102 "lib/value-tree.c"
  {

# 103 "lib/value-tree.c"
    return ((value_result_t) {
                                                .val = t->value,
                                            });
  }
}


# 158 "lib/value-tree.c"
value_tree_t* value_tree_insert(value_tree_t* t, value_comparison_fn cmp_fn, value_t key, value_t value)
# 159 "lib/value-tree.c"
{

# 160 "lib/value-tree.c"
  if ((t==NULL))

# 160 "lib/value-tree.c"
  {

# 162 "lib/value-tree.c"
    return make_value_tree_leaf(key, value);
  }
  int cmp_result = cmp_fn(key, (t->key));

# 165 "lib/value-tree.c"
  if ((cmp_result<0))

# 165 "lib/value-tree.c"
  {

# 166 "lib/value-tree.c"
    ((t->left)=value_tree_insert((t->left), cmp_fn, key, value));
  }
  else

# 167 "lib/value-tree.c"
  if ((cmp_result>0))

# 167 "lib/value-tree.c"
  {

# 168 "lib/value-tree.c"
    ((t->right)=value_tree_insert((t->right), cmp_fn, key, value));
  }
  else

# 169 "lib/value-tree.c"
  {

# 173 "lib/value-tree.c"
    ((t->value)=value);

# 174 "lib/value-tree.c"
    return t;
  }

# 177 "lib/value-tree.c"
  (t=value_tree_skew(t));

# 178 "lib/value-tree.c"
  (t=value_tree_split(t));

# 180 "lib/value-tree.c"
  return t;
}


# 227 "lib/value-tree.c"
value_tree_t* value_tree_delete(value_tree_t* t, value_comparison_fn cmp_fn, value_t key)
# 228 "lib/value-tree.c"
{

# 230 "lib/value-tree.c"
  if ((t==NULL))

# 230 "lib/value-tree.c"
  {

# 231 "lib/value-tree.c"
    return t;
  }
  int cmp_result = cmp_fn(key, (t->key));

# 235 "lib/value-tree.c"
  if ((cmp_result<0))

# 235 "lib/value-tree.c"
  {

# 236 "lib/value-tree.c"
    ((t->left)=value_tree_delete((t->left), cmp_fn, key));
  }
  else

# 237 "lib/value-tree.c"
  if ((cmp_result>0))

# 237 "lib/value-tree.c"
  {

# 238 "lib/value-tree.c"
    ((t->right)=value_tree_delete((t->right), cmp_fn, key));
  }
  else

# 239 "lib/value-tree.c"
  {

# 240 "lib/value-tree.c"
    if (value_tree_is_leaf(t))

# 240 "lib/value-tree.c"
    {

# 245 "lib/value-tree.c"
      return NULL;
    }
    else

# 246 "lib/value-tree.c"
    if (((t->left)==NULL))

# 246 "lib/value-tree.c"
    {
      value_tree_t* L = value_tree_successor(t);

# 251 "lib/value-tree.c"
      ((t->key)=(L->key));

# 252 "lib/value-tree.c"
      ((t->value)=(L->value));

# 253 "lib/value-tree.c"
      ((t->right)=value_tree_delete((t->right), cmp_fn, (L->key)));
    }
    else

# 254 "lib/value-tree.c"
    {
      value_tree_t* L = value_tree_predecessor(t);

# 259 "lib/value-tree.c"
      ((t->key)=(L->key));

# 260 "lib/value-tree.c"
      ((t->value)=(L->value));

# 261 "lib/value-tree.c"
      ((t->left)=value_tree_delete((t->left), cmp_fn, (L->key)));
    }
  }

# 268 "lib/value-tree.c"
  (t=value_tree_decrease_level(t));

# 269 "lib/value-tree.c"
  (t=value_tree_skew(t));

# 270 "lib/value-tree.c"
  ((t->right)=value_tree_skew((t->right)));

# 271 "lib/value-tree.c"
  if (((t->right)!=NULL))

# 271 "lib/value-tree.c"
  {

# 272 "lib/value-tree.c"
    (((t->right)->right)=value_tree_skew(((t->right)->right)));
  }

# 274 "lib/value-tree.c"
  (t=value_tree_split(t));

# 275 "lib/value-tree.c"
  ((t->right)=value_tree_split((t->right)));

# 276 "lib/value-tree.c"
  return t;
}


# 109 "lib/value-tree.c"
value_tree_t* value_tree_skew(value_tree_t* t)
# 109 "lib/value-tree.c"
{

# 110 "lib/value-tree.c"
  if ((t==NULL))

# 110 "lib/value-tree.c"
  {

# 111 "lib/value-tree.c"
    return NULL;
  }

# 113 "lib/value-tree.c"
  if (((t->left)==NULL))

# 113 "lib/value-tree.c"
  {

# 114 "lib/value-tree.c"
    return t;
  }

# 116 "lib/value-tree.c"
  if ((((t->left)->level)==(t->level)))

# 116 "lib/value-tree.c"
  {
    value_tree_t* L = (t->left);

# 118 "lib/value-tree.c"
    ((t->left)=(L->right));

# 119 "lib/value-tree.c"
    ((L->right)=t);

# 120 "lib/value-tree.c"
    return L;
  }

# 122 "lib/value-tree.c"
  return t;
}


# 125 "lib/value-tree.c"
value_tree_t* value_tree_split(value_tree_t* t)
# 125 "lib/value-tree.c"
{

# 126 "lib/value-tree.c"
  if ((t==NULL))

# 126 "lib/value-tree.c"
  {

# 127 "lib/value-tree.c"
    return NULL;
  }

# 129 "lib/value-tree.c"
  if ((((t->right)==NULL)||(((t->right)->right)==NULL)))

# 129 "lib/value-tree.c"
  {

# 130 "lib/value-tree.c"
    return t;
  }

# 132 "lib/value-tree.c"
  if (((t->level)==(((t->right)->right)->level)))

# 132 "lib/value-tree.c"
  {
    value_tree_t* R = (t->right);

# 136 "lib/value-tree.c"
    ((t->right)=(R->left));

# 137 "lib/value-tree.c"
    ((R->left)=t);

# 138 "lib/value-tree.c"
    ((R->level)++);

# 139 "lib/value-tree.c"
    return R;
  }

# 141 "lib/value-tree.c"
  return t;
}


# 144 "lib/value-tree.c"
value_tree_t* make_value_tree_leaf(value_t key, value_t value)
# 144 "lib/value-tree.c"
{
  value_tree_t* result = malloc_struct(value_tree_t);

# 146 "lib/value-tree.c"
  ((result->level)=1);

# 147 "lib/value-tree.c"
  ((result->key)=key);

# 148 "lib/value-tree.c"
  ((result->value)=value);

# 149 "lib/value-tree.c"
  return result;
}


# 187 "lib/value-tree.c"
value_tree_t* value_tree_decrease_level(value_tree_t* t)
# 187 "lib/value-tree.c"
{

# 188 "lib/value-tree.c"
  if (((t->left)&&(t->right)))

# 188 "lib/value-tree.c"
  {
    uint32_t should_be = (value_tree_min_level(((t->left)->level), ((t->right)->level))+1);

# 191 "lib/value-tree.c"
    if ((should_be<(t->level)))

# 191 "lib/value-tree.c"
    {

# 192 "lib/value-tree.c"
      ((t->level)=should_be);

# 193 "lib/value-tree.c"
      if ((should_be<((t->right)->level)))

# 193 "lib/value-tree.c"
      {

# 194 "lib/value-tree.c"
        (((t->right)->level)=should_be);
      }
    }
  }

# 198 "lib/value-tree.c"
  return t;
}


# 201 "lib/value-tree.c"
value_tree_t* value_tree_predecessor(value_tree_t* t)
# 201 "lib/value-tree.c"
{

# 202 "lib/value-tree.c"
  (t=(t->left));

# 203 "lib/value-tree.c"
  while (((t->right)!=NULL))

# 203 "lib/value-tree.c"
  {

# 204 "lib/value-tree.c"
    (t=(t->right));
  }

# 206 "lib/value-tree.c"
  return t;
}


# 209 "lib/value-tree.c"
value_tree_t* value_tree_successor(value_tree_t* t)
# 209 "lib/value-tree.c"
{

# 210 "lib/value-tree.c"
  (t=(t->right));

# 211 "lib/value-tree.c"
  while (((t->left)!=NULL))

# 211 "lib/value-tree.c"
  {

# 212 "lib/value-tree.c"
    (t=(t->left));
  }

# 214 "lib/value-tree.c"
  return t;
}


# 206 "lib/flag.c"
void flag_program_name(char* name)
# 206 "lib/flag.c"
{

# 207 "lib/flag.c"
  (current_program=malloc_struct(program_descriptor_t));

# 208 "lib/flag.c"
  ((current_program->name)=name);

# 209 "lib/flag.c"
  (current_command=NULL);

# 210 "lib/flag.c"
  (current_flag=NULL);
}


# 233 "lib/flag.c"
void flag_description(char* description)
# 233 "lib/flag.c"
{

# 234 "lib/flag.c"
  if ((current_flag!=NULL))

# 234 "lib/flag.c"
  {

# 235 "lib/flag.c"
    ((current_flag->description)=description);
  }
  else

# 236 "lib/flag.c"
  if ((current_command!=NULL))

# 236 "lib/flag.c"
  {

# 237 "lib/flag.c"
    ((current_command->description)=description);
  }
  else

# 238 "lib/flag.c"
  if ((current_program!=NULL))

# 238 "lib/flag.c"
  {

# 239 "lib/flag.c"
    ((current_program->description)=description);
  }
  else

# 240 "lib/flag.c"
  {

# 241 "lib/flag.c"
    log_fatal("A current flag, program or command must be executed first");

# 242 "lib/flag.c"
    fatal_error(ERROR_ILLEGAL_STATE);
  }
}


# 259 "lib/flag.c"
void flag_file_args(value_array_t** write_back_file_args_ptr)
# 259 "lib/flag.c"
{

# 260 "lib/flag.c"
  if ((current_command!=NULL))

# 260 "lib/flag.c"
  {

# 261 "lib/flag.c"
    ((current_command->write_back_file_args_ptr)=write_back_file_args_ptr);
  }
  else

# 262 "lib/flag.c"
  if ((current_program!=NULL))

# 262 "lib/flag.c"
  {

# 263 "lib/flag.c"
    ((current_program->write_back_file_args_ptr)=write_back_file_args_ptr);
  }
  else

# 264 "lib/flag.c"
  {

# 265 "lib/flag.c"
    log_fatal("A current program or command must be executed first");

# 266 "lib/flag.c"
    fatal_error(ERROR_ILLEGAL_STATE);
  }
}


# 219 "lib/flag.c"
void flag_command(char* name, char** write_back_ptr)
# 219 "lib/flag.c"
{

# 220 "lib/flag.c"
  (current_command=malloc_struct(command_descriptor_t));

# 221 "lib/flag.c"
  ((current_command->name)=name);

# 222 "lib/flag.c"
  ((current_command->write_back_ptr)=write_back_ptr);

# 223 "lib/flag.c"
  (current_flag=NULL);

# 224 "lib/flag.c"
  ((current_program->commands)=string_tree_insert((current_program->commands), name, ptr_to_value(current_command)));
}


# 291 "lib/flag.c"
void flag_boolean(char* name, boolean_t* write_back_ptr)
# 291 "lib/flag.c"
{

# 292 "lib/flag.c"
  add_flag(name, write_back_ptr, flag_type_boolean);
}


# 295 "lib/flag.c"
void flag_string(char* name, char** write_back_ptr)
# 295 "lib/flag.c"
{

# 296 "lib/flag.c"
  add_flag(name, write_back_ptr, flag_type_string);
}


# 299 "lib/flag.c"
void flag_uint64(char* name, uint64_t* write_back_ptr)
# 299 "lib/flag.c"
{

# 300 "lib/flag.c"
  add_flag(name, write_back_ptr, flag_type_uint64);
}


# 303 "lib/flag.c"
void flag_int64(char* name, int64_t* write_back_ptr)
# 303 "lib/flag.c"
{

# 304 "lib/flag.c"
  add_flag(name, write_back_ptr, flag_type_int64);
}


# 307 "lib/flag.c"
void flag_double(char* name, double* write_back_ptr)
# 307 "lib/flag.c"
{

# 308 "lib/flag.c"
  add_flag(name, write_back_ptr, flag_type_double);
}


# 328 "lib/flag.c"
void flag_enum(char* name, int* write_back_ptr)
# 328 "lib/flag.c"
{

# 329 "lib/flag.c"
  add_flag(name, write_back_ptr, flag_type_enum);

# 330 "lib/flag.c"
  ((current_flag->enum_size)=((sizeof(int))*8));
}


# 333 "lib/flag.c"
void flag_enum_64(char* name, uint64_t* write_back_ptr)
# 333 "lib/flag.c"
{

# 334 "lib/flag.c"
  add_flag(name, write_back_ptr, flag_type_enum);

# 335 "lib/flag.c"
  ((current_flag->enum_size)=64);
}


# 338 "lib/flag.c"
void flag_enum_value(char* name, uint64_t value)
# 338 "lib/flag.c"
{

# 339 "lib/flag.c"
  if (((!current_flag)||((current_flag->flag_type)!=flag_type_enum)))

# 339 "lib/flag.c"
  {

# 340 "lib/flag.c"
    log_fatal("The current flag is not an enum type");

# 341 "lib/flag.c"
    fatal_error(ERROR_ILLEGAL_STATE);
  }

# 344 "lib/flag.c"
  ((current_flag->enum_values)=string_tree_insert((current_flag->enum_values), name, u64_to_value(value)));
}


# 348 "lib/flag.c"
void flag_alias(char* alias)
# 348 "lib/flag.c"
{

# 349 "lib/flag.c"
  if ((current_flag!=NULL))

# 349 "lib/flag.c"
  {

# 351 "lib/flag.c"
    if ((current_command!=NULL))

# 351 "lib/flag.c"
    {

# 352 "lib/flag.c"
      ((current_command->flags)=string_tree_insert((current_command->flags), alias, ptr_to_value(current_flag)));
    }
    else

# 354 "lib/flag.c"
    if ((current_program!=NULL))

# 354 "lib/flag.c"
    {

# 355 "lib/flag.c"
      ((current_program->flags)=string_tree_insert((current_program->flags), alias, ptr_to_value(current_flag)));
    }
    else

# 357 "lib/flag.c"
    {

# 358 "lib/flag.c"
      log_fatal("A current program or command must exist first");

# 359 "lib/flag.c"
      fatal_error(ERROR_ILLEGAL_STATE);
    }
  }
  else

# 361 "lib/flag.c"
  {

# 362 "lib/flag.c"
    log_fatal("A current flag must present to use flag_alias");

# 363 "lib/flag.c"
    fatal_error(ERROR_ILLEGAL_STATE);
  }
}


# 379 "lib/flag.c"
char* flag_parse_command_line(int argc, char** argv)
# 379 "lib/flag.c"
{

# 380 "lib/flag.c"
  if ((current_program==NULL))

# 380 "lib/flag.c"
  {

# 381 "lib/flag.c"
    log_fatal("flag_parse_command_line can't be called unless flag_program_name() is " "first called.");

# 384 "lib/flag.c"
    fatal_error(ERROR_ILLEGAL_STATE);
  }
  int start = 1;
  command_descriptor_t* command = NULL;

# 389 "lib/flag.c"
  if ((current_program->commands))

# 389 "lib/flag.c"
  {

# 390 "lib/flag.c"
    if ((argc<=1))

# 390 "lib/flag.c"
    {

# 391 "lib/flag.c"
      return "This program requires a command but not enough arguments were " "given";
    }
    char* name = (argv[1]);

# 395 "lib/flag.c"
    (command=flag_find_command_descriptor(name));

# 396 "lib/flag.c"
    if ((command==NULL))

# 396 "lib/flag.c"
    {

# 397 "lib/flag.c"
      return string_printf("The first command line argument is not a known command: %s", name);
    }
    else

# 399 "lib/flag.c"
    {

# 400 "lib/flag.c"
      ((*(command->write_back_ptr))=(command->name));
    }

# 402 "lib/flag.c"
    (start=2);
  }
  value_array_t* files = make_value_array(argc);
  boolean_t parse_flags = true;

# 408 "lib/flag.c"
  for (
    int i = start;
    (i<argc);
    (i++))

# 408 "lib/flag.c"
  {
    char* arg = (argv[i]);

# 410 "lib/flag.c"
    if (parse_flags)

# 410 "lib/flag.c"
    {

# 411 "lib/flag.c"
      if (string_equal(arg, "--"))

# 411 "lib/flag.c"
      {

# 412 "lib/flag.c"
        (parse_flags=false);

# 413 "lib/flag.c"
        continue;
      }

# 416 "lib/flag.c"
      if (string_starts_with(arg, "-"))

# 416 "lib/flag.c"
      {
        flag_key_value_t key_value = flag_split_argument(arg);

# 418 "lib/flag.c"
        if (((key_value.key)==NULL))

# 418 "lib/flag.c"
        {

# 419 "lib/flag.c"
          return string_printf("This argument is not a well formed flag: %s", arg);
        }
        flag_descriptor_t* flag = flag_find_flag_descriptor(command, (key_value.key));

# 424 "lib/flag.c"
        if ((flag==NULL))

# 424 "lib/flag.c"
        {

# 425 "lib/flag.c"
          return string_printf("The argument looks like a flag but was not found: '%s'\n\n" "(You may want to use ' -- ' to seperate flags from non flag " "arguments (aka file arguments).)", arg);
        }

# 436 "lib/flag.c"
        if (((key_value.value)==NULL))

# 436 "lib/flag.c"
        {

# 438 "lib/flag.c"
          (i++);

# 439 "lib/flag.c"
          ((key_value.value)=(argv[i]));
        }
        char* error = parse_and_write_value(flag, key_value);

# 442 "lib/flag.c"
        if (error)

# 442 "lib/flag.c"
        {

# 443 "lib/flag.c"
          return error;
        }

# 445 "lib/flag.c"
        continue;
      }
    }

# 450 "lib/flag.c"
    value_array_add(files, str_to_value(arg));
  }

# 454 "lib/flag.c"
  if (((command!=NULL)&&((command->write_back_file_args_ptr)!=NULL)))

# 454 "lib/flag.c"
  {

# 455 "lib/flag.c"
    ((*(command->write_back_file_args_ptr))=files);
  }

# 457 "lib/flag.c"
  if (((current_program->write_back_file_args_ptr)!=NULL))

# 457 "lib/flag.c"
  {

# 458 "lib/flag.c"
    ((*(current_program->write_back_file_args_ptr))=files);
  }

# 460 "lib/flag.c"
  return NULL;
}


# 638 "lib/flag.c"
void flag_print_help(FILE* out, char* message)
# 638 "lib/flag.c"
{

# 639 "lib/flag.c"
  fprintf(out, "\nMessage: %s\n", message);

# 641 "lib/flag.c"
  if ((current_program==NULL))

# 641 "lib/flag.c"
  {

# 642 "lib/flag.c"
    fprintf(out, "Command line parsing was not configured so help can not be " "provided.");

# 645 "lib/flag.c"
    return;
  }

# 648 "lib/flag.c"
  if (((current_program->commands)!=NULL))

# 648 "lib/flag.c"
  {

# 649 "lib/flag.c"
    fprintf(out, "\nUsage: %s <command> <flags> <files>\n", (current_program->name));

# 651 "lib/flag.c"
    fprintf(out, "\nDescription: %s\n\n", (current_program->description));

# 653 "lib/flag.c"
    flag_print_flags(out, "Global flags:", (current_program->flags));

# 655 "lib/flag.c"
    fprintf(out, "\nCommands:\n");

# 657 "lib/flag.c"
    string_tree_foreach((current_program->commands), key, value, 
# 657 "lib/flag.c"
    {

# 658 "lib/flag.c"
      fprintf(out, "\n    %s\t%s\n", key, ((/*CAST*/(command_descriptor_t*) (value.ptr))->description));

# 659 "lib/flag.c"
      flag_print_flags(out, "      Flags:", ((/*CAST*/(command_descriptor_t*) (value.ptr))->flags));
    }
);
  }
  else

# 662 "lib/flag.c"
  {

# 663 "lib/flag.c"
    fprintf(out, "\nUsage: %s <flags> <files>\n", (current_program->name));

# 664 "lib/flag.c"
    fprintf(out, "\nDescription: %s\n\n", (current_program->description));

# 665 "lib/flag.c"
    flag_print_flags(out, "Flags:", (current_program->flags));
  }
}


# 465 "lib/flag.c"
command_descriptor_t* flag_find_command_descriptor(char* name)
# 465 "lib/flag.c"
{

# 466 "lib/flag.c"
  if (((current_program->commands)==NULL))

# 466 "lib/flag.c"
  {

# 467 "lib/flag.c"
    log_fatal("flag_get_command() shouldn't not be called when we don't have any " "defined commands.");

# 470 "lib/flag.c"
    fatal_error(ERROR_ILLEGAL_STATE);
  }
  value_result_t command_value = string_tree_find((current_program->commands), name);

# 474 "lib/flag.c"
  if (is_ok(command_value))

# 474 "lib/flag.c"
  {

# 475 "lib/flag.c"
    return (/*CAST*/(command_descriptor_t*) (command_value.ptr));
  }
  else

# 476 "lib/flag.c"
  {

# 477 "lib/flag.c"
    return NULL;
  }
}


# 485 "lib/flag.c"
flag_descriptor_t* flag_find_flag_descriptor(command_descriptor_t* command, char* name)
# 487 "lib/flag.c"
{

# 488 "lib/flag.c"
  if ((command!=NULL))

# 488 "lib/flag.c"
  {
    value_result_t command_flag_value = string_tree_find((command->flags), name);

# 490 "lib/flag.c"
    if (is_ok(command_flag_value))

# 490 "lib/flag.c"
    {

# 491 "lib/flag.c"
      return (/*CAST*/(flag_descriptor_t*) (command_flag_value.ptr));
    }
  }
  value_result_t program_flag_value = string_tree_find((current_program->flags), name);

# 497 "lib/flag.c"
  if (is_ok(program_flag_value))

# 497 "lib/flag.c"
  {

# 498 "lib/flag.c"
    return (/*CAST*/(flag_descriptor_t*) (program_flag_value.ptr));
  }

# 501 "lib/flag.c"
  return NULL;
}


# 521 "lib/flag.c"
flag_key_value_t flag_split_argument(char* arg)
# 521 "lib/flag.c"
{
  int equal_sign_index = string_index_of_char(arg, '=');

# 523 "lib/flag.c"
  if ((equal_sign_index>=0))

# 523 "lib/flag.c"
  {
    char* key = string_substring(arg, 0, equal_sign_index);
    char* value = string_substring(arg, (equal_sign_index+1), strlen(arg));

# 530 "lib/flag.c"
    return ((flag_key_value_t) {.key = key, .value = value});
  }

# 532 "lib/flag.c"
  return ((flag_key_value_t) {.key = arg, .value = NULL});
}


# 538 "lib/flag.c"
char* parse_and_write_value(flag_descriptor_t* flag, flag_key_value_t key_value)
# 539 "lib/flag.c"
{

# 540 "lib/flag.c"
  switch ((flag->flag_type))

# 540 "lib/flag.c"
  {

# 541 "lib/flag.c"
    case flag_type_boolean:

# 542 "lib/flag.c"
    return parse_and_write_boolean(flag, key_value);

# 544 "lib/flag.c"
    case flag_type_string:

# 545 "lib/flag.c"
    ((*(/*CAST*/(char**) (flag->write_back_ptr)))=(key_value.value));

# 546 "lib/flag.c"
    return NULL;

# 548 "lib/flag.c"
    case flag_type_uint64:

# 549 "lib/flag.c"
    return parse_and_write_uint64(flag, key_value);

# 551 "lib/flag.c"
    case flag_type_enum:

# 552 "lib/flag.c"
    return parse_and_write_enum(flag, key_value);

# 554 "lib/flag.c"
    default:

# 555 "lib/flag.c"
    fatal_error(ERROR_ILLEGAL_STATE);

# 556 "lib/flag.c"
    break;
  }

# 558 "lib/flag.c"
  return "<ILLEGAL-STATE-NOT-REACHED>";
}


# 561 "lib/flag.c"
char* parse_and_write_boolean(flag_descriptor_t* flag, flag_key_value_t key_value)
# 562 "lib/flag.c"
{
  char* val = (key_value.value);

# 564 "lib/flag.c"
  if (((string_equal("true", val)||string_equal("t", val))||string_equal("1", val)))

# 565 "lib/flag.c"
  {

# 566 "lib/flag.c"
    ((*(/*CAST*/(boolean_t*) (flag->write_back_ptr)))=true);
  }
  else

# 567 "lib/flag.c"
  if (((string_equal("false", val)||string_equal("f", val))||string_equal("0", val)))

# 568 "lib/flag.c"
  {

# 569 "lib/flag.c"
    ((*(/*CAST*/(boolean_t*) (flag->write_back_ptr)))=false);
  }
  else

# 570 "lib/flag.c"
  {

# 571 "lib/flag.c"
    return string_printf("boolean flag %s does not accept value %s", (key_value.key), (key_value.value));
  }

# 574 "lib/flag.c"
  return NULL;
}


# 577 "lib/flag.c"
char* parse_and_write_uint64(flag_descriptor_t* flag, flag_key_value_t key_value)
# 578 "lib/flag.c"
{
  value_result_t val_result = string_parse_uint64((key_value.value));

# 580 "lib/flag.c"
  if (is_ok(val_result))

# 580 "lib/flag.c"
  {

# 581 "lib/flag.c"
    ((*(/*CAST*/(uint64_t*) (flag->write_back_ptr)))=(val_result.u64));
  }
  else

# 582 "lib/flag.c"
  {

# 583 "lib/flag.c"
    return string_printf("uint64_t flag %s does not accept value %s", (key_value.key), (key_value.value));
  }

# 586 "lib/flag.c"
  return NULL;
}


# 589 "lib/flag.c"
char* parse_and_write_enum(flag_descriptor_t* flag, flag_key_value_t key_value)
# 590 "lib/flag.c"
{
  value_result_t val_result = string_tree_find((flag->enum_values), (key_value.value));

# 593 "lib/flag.c"
  if (is_ok(val_result))

# 593 "lib/flag.c"
  {

# 594 "lib/flag.c"
    switch ((flag->enum_size))

# 594 "lib/flag.c"
    {

# 595 "lib/flag.c"
      case 64:

# 597 "lib/flag.c"
      ((*(/*CAST*/(uint64_t*) (flag->write_back_ptr)))=(val_result.u64));

# 598 "lib/flag.c"
      return NULL;

# 599 "lib/flag.c"
      case 32:

# 601 "lib/flag.c"
      ((*(/*CAST*/(uint32_t*) (flag->write_back_ptr)))=(val_result.u64));

# 602 "lib/flag.c"
      return NULL;

# 604 "lib/flag.c"
      default:

# 605 "lib/flag.c"
      fatal_error(ERROR_ILLEGAL_STATE);

# 606 "lib/flag.c"
      break;
    }
  }

# 609 "lib/flag.c"
  return string_printf("Flag %s does not accept the argument value %s", (key_value.key), (key_value.value));
}


# 272 "lib/flag.c"
void add_flag(char* name, void* write_back_ptr, flag_type_t flag_type)
# 272 "lib/flag.c"
{

# 273 "lib/flag.c"
  (current_flag=malloc_struct(flag_descriptor_t));

# 274 "lib/flag.c"
  ((current_flag->flag_type)=flag_type);

# 275 "lib/flag.c"
  ((current_flag->name)=name);

# 276 "lib/flag.c"
  ((current_flag->write_back_ptr)=write_back_ptr);

# 279 "lib/flag.c"
  if ((current_command!=NULL))

# 279 "lib/flag.c"
  {

# 280 "lib/flag.c"
    ((current_command->flags)=string_tree_insert((current_command->flags), name, ptr_to_value(current_flag)));
  }
  else

# 282 "lib/flag.c"
  if ((current_program!=NULL))

# 282 "lib/flag.c"
  {

# 283 "lib/flag.c"
    ((current_program->flags)=string_tree_insert((current_program->flags), name, ptr_to_value(current_flag)));
  }
  else

# 285 "lib/flag.c"
  {

# 286 "lib/flag.c"
    log_fatal("A current program or command must be executed first");

# 287 "lib/flag.c"
    fatal_error(ERROR_ILLEGAL_STATE);
  }
}


# 623 "lib/flag.c"
void flag_print_flags(FILE* out, char* header, string_tree_t* flags)
# 623 "lib/flag.c"
{

# 624 "lib/flag.c"
  fprintf(out, "%s\n", header);

# 626 "lib/flag.c"
  string_tree_foreach(flags, key, value, 
# 626 "lib/flag.c"
  {

# 627 "lib/flag.c"
    fprintf(out, "      %s\t%s\n", key, ((/*CAST*/(flag_descriptor_t*) (value.ptr))->description));
  }
);
}


# 69 "lib/io.c"
__attribute__((warn_unused_result)) buffer_t* buffer_append_file_contents(buffer_t* bytes, char* file_name)
# 69 "lib/io.c"
{
  uint64_t capacity = (bytes->capacity);

# 74 "lib/io.c"
  {
    struct stat st;

# 76 "lib/io.c"
    if ((stat(file_name, (&st))<0))

# 76 "lib/io.c"
    {

# 77 "lib/io.c"
      log_fatal("file does not exist: %s", file_name);

# 78 "lib/io.c"
      fatal_error(ERROR_ILLEGAL_STATE);
    }

# 80 "lib/io.c"
    (capacity=(st.st_size));
  }

# 83 "lib/io.c"
  (bytes=buffer_increase_capacity(bytes, capacity));
  FILE* file = fopen(file_name, "r");

# 86 "lib/io.c"
  (bytes=buffer_append_all(bytes, file));

# 87 "lib/io.c"
  fclose(file);

# 89 "lib/io.c"
  return bytes;
}


# 99 "lib/io.c"
__attribute__((warn_unused_result)) extern buffer_t* buffer_append_all(buffer_t* bytes, FILE* input)
# 99 "lib/io.c"
{
  uint8_t buffer[1024];

# 101 "lib/io.c"
  while (1)

# 101 "lib/io.c"
  {
    uint64_t n_read = fread(buffer, 1, (sizeof(buffer)), input);

# 103 "lib/io.c"
    if ((n_read==0))

# 103 "lib/io.c"
    {

# 104 "lib/io.c"
      break;
    }

# 106 "lib/io.c"
    (bytes=buffer_append_bytes(bytes, buffer, n_read));
  }

# 108 "lib/io.c"
  return bytes;
}


# 116 "lib/io.c"
void buffer_write_file(buffer_t* bytes, char* file_name)
# 116 "lib/io.c"
{
  FILE* file = fopen(file_name, "w");

# 118 "lib/io.c"
  if ((file==NULL))

# 118 "lib/io.c"
  {

# 119 "lib/io.c"
    log_fatal("Failed to open file for writing: %s", file_name);

# 120 "lib/io.c"
    log_fatal("strerror(errno) = %s", strerror(errno));

# 121 "lib/io.c"
    fatal_error(ERROR_ILLEGAL_STATE);
  }
  size_t bytes_written = fwrite((bytes->elements), 1, (bytes->length), file);

# 124 "lib/io.c"
  if ((bytes_written!=(bytes->length)))

# 124 "lib/io.c"
  {

# 125 "lib/io.c"
    log_fatal("Failed to write %d bytes to %s", (bytes->length), file_name);

# 126 "lib/io.c"
    log_fatal("strerror(errno) = %s", strerror(errno));

# 127 "lib/io.c"
    fatal_error(ERROR_ILLEGAL_STATE);
  }

# 129 "lib/io.c"
  if ((fclose(file)!=0))

# 129 "lib/io.c"
  {

# 130 "lib/io.c"
    log_fatal("Failed to close file: %s", file_name);

# 131 "lib/io.c"
    log_fatal("strerror(errno) = %s", strerror(errno));

# 132 "lib/io.c"
    fatal_error(ERROR_ILLEGAL_STATE);
  }
}


# 196 "lib/io.c"
buffer_t* buffer_read_until(buffer_t* buffer, FILE* input, char end_of_line)
# 196 "lib/io.c"
{

# 197 "lib/io.c"
  while ((!feof(input)))

# 197 "lib/io.c"
  {
    int ch = fgetc(input);

# 199 "lib/io.c"
    if ((ch<0))

# 199 "lib/io.c"
    {

# 200 "lib/io.c"
      return buffer;
    }

# 202 "lib/io.c"
    if ((ch==end_of_line))

# 202 "lib/io.c"
    {

# 203 "lib/io.c"
      return buffer;
    }

# 205 "lib/io.c"
    (buffer=buffer_append_byte(buffer, ch));
  }

# 207 "lib/io.c"
  return buffer;
}


# 217 "lib/io.c"
extern buffer_t* buffer_read_ready_bytes(buffer_t* buffer, FILE* input, uint64_t max_bytes)
# 218 "lib/io.c"
{
  int file_number = fileno(input);

# 220 "lib/io.c"
  return buffer_read_ready_bytes_file_number(buffer, file_number, max_bytes);
}


# 230 "lib/io.c"
extern buffer_t* buffer_read_ready_bytes_file_number(buffer_t* buffer, int file_number, uint64_t max_bytes)
# 232 "lib/io.c"
{

# 233 "lib/io.c"
  fcntl(file_number, F_SETFL, (fcntl(file_number, F_GETFL)|O_NONBLOCK));
  uint64_t bytes_remaining = (max_bytes-buffer_length(buffer));
  char read_buffer[1024];

# 239 "lib/io.c"
  while ((bytes_remaining>0))

# 239 "lib/io.c"
  {
    int bytes_read = read(file_number, read_buffer, (sizeof(read_buffer)));

# 241 "lib/io.c"
    if ((bytes_read>0))

# 241 "lib/io.c"
    {

# 242 "lib/io.c"
      for (
        int i = 0;
        (i<bytes_read);
        (i++))

# 242 "lib/io.c"
      {

# 243 "lib/io.c"
        (buffer=buffer_append_byte(buffer, (/*CAST*/(uint8_t) (read_buffer[i]))));

# 244 "lib/io.c"
        (bytes_remaining--);
      }
    }
    else

# 246 "lib/io.c"
    if ((bytes_read==0))

# 246 "lib/io.c"
    {

# 248 "lib/io.c"
      break;
    }
    else

# 249 "lib/io.c"
    {

# 251 "lib/io.c"
      if (((errno!=EAGAIN)&&(errno!=EWOULDBLOCK)))

# 251 "lib/io.c"
      {

# 253 "lib/io.c"
        log_fatal("Error reading from file descriptor %d: %s", file_number, strerror(errno));

# 255 "lib/io.c"
        fatal_error(ERROR_ILLEGAL_STATE);
      }

# 258 "lib/io.c"
      break;
    }
  }

# 262 "lib/io.c"
  return buffer;
}


# 274 "lib/io.c"
int file_peek_byte(FILE* input)
# 274 "lib/io.c"
{

# 275 "lib/io.c"
  if (feof(input))

# 275 "lib/io.c"
  {

# 276 "lib/io.c"
    return (-1);
  }
  int result = fgetc(input);

# 282 "lib/io.c"
  if ((result>=0))

# 282 "lib/io.c"
  {

# 283 "lib/io.c"
    ungetc(result, input);
  }

# 285 "lib/io.c"
  return result;
}


# 294 "lib/io.c"
boolean_t file_eof(FILE* input)
# 294 "lib/io.c"
{

# 295 "lib/io.c"
  return (feof(input)||(file_peek_byte(input)<0));
}


# 305 "lib/io.c"
void file_copy_stream(FILE* input, FILE* output, boolean_t until_eof, uint64_t size)
# 306 "lib/io.c"
{

# 307 "lib/io.c"
  if (until_eof)

# 307 "lib/io.c"
  {

# 308 "lib/io.c"
    (size=ULLONG_MAX);
  }
  uint8_t buffer[FILE_COPY_STREAM_BUFFER_SIZE];

# 312 "lib/io.c"
  while ((size>0))

# 312 "lib/io.c"
  {
    int minimum = ((size<FILE_COPY_STREAM_BUFFER_SIZE) ? size : FILE_COPY_STREAM_BUFFER_SIZE);
    uint64_t n_read = fread(buffer, 1, minimum, input);

# 317 "lib/io.c"
    if ((n_read==0))

# 317 "lib/io.c"
    {

# 318 "lib/io.c"
      break;
    }

# 320 "lib/io.c"
    fwrite(buffer, 1, n_read, output);

# 321 "lib/io.c"
    (size-=n_read);
  }
}


# 335 "lib/io.c"
void file_skip_bytes(FILE* input, uint64_t n_bytes)
# 335 "lib/io.c"
{

# 342 "lib/io.c"
  while (1)

# 342 "lib/io.c"
  {

# 343 "lib/io.c"
    if (((n_bytes==0)||feof(input)))

# 343 "lib/io.c"
    {

# 344 "lib/io.c"
      return;
    }
    int ch = fgetc(input);

# 347 "lib/io.c"
    if ((ch<0))

# 347 "lib/io.c"
    {

# 349 "lib/io.c"
      return;
    }

# 351 "lib/io.c"
    (n_bytes--);
  }
}


# 141 "lib/io.c"
void make_file_read_only(char* file_name)
# 141 "lib/io.c"
{

# 143 "lib/io.c"
  if ((chmod(file_name, ((S_IRUSR|S_IRGRP)|S_IROTH))!=0))

# 143 "lib/io.c"
  {

# 144 "lib/io.c"
    log_fatal("Failed to set file permissions: %s", file_name);

# 145 "lib/io.c"
    log_fatal("strerror(errno) = %s", strerror(errno));

# 146 "lib/io.c"
    fatal_error(ERROR_ILLEGAL_STATE);
  }
}


# 156 "lib/io.c"
void make_writable_if_exists(const char* file_name)
# 156 "lib/io.c"
{

# 158 "lib/io.c"
  if ((access(file_name, F_OK)!=0))

# 158 "lib/io.c"
  {

# 160 "lib/io.c"
    return;
  }
  struct stat file_stat;

# 165 "lib/io.c"
  if ((stat(file_name, (&file_stat))!=0))

# 165 "lib/io.c"
  {

# 166 "lib/io.c"
    log_fatal("Error getting file status for %s: %s\n", file_name, strerror(errno));

# 167 "lib/io.c"
    fatal_error(ERROR_ILLEGAL_STATE);
  }
  mode_t new_mode = ((file_stat.st_mode)|S_IWUSR);

# 173 "lib/io.c"
  if ((chmod(file_name, new_mode)!=0))

# 173 "lib/io.c"
  {

# 174 "lib/io.c"
    log_fatal("Error setting permissions for %s: %s\n", file_name, strerror(errno));

# 175 "lib/io.c"
    fatal_error(ERROR_ILLEGAL_STATE);
  }
}


# 362 "lib/terminal.c"
__attribute__((warn_unused_result)) extern buffer_t* term_clear_screen(buffer_t* buffer)
# 362 "lib/terminal.c"
{

# 363 "lib/terminal.c"
  return buffer_printf(buffer, TERM_ESCAPE_STRING("2J"));
}


# 225 "lib/terminal.c"
__attribute__((warn_unused_result)) extern buffer_t* term_set_foreground_color(buffer_t* buffer, uint32_t color)
# 225 "lib/terminal.c"
{
  uint8_t blue = (color&0xff);
  uint8_t green = ((color>>8)&0xff);
  uint8_t red = ((color>>16)&0xff);

# 231 "lib/terminal.c"
  return buffer_printf(buffer, TERM_ESCAPE_STRING_START_AND_END("38;2;%d;%d;%d"), red, green, blue);
}


# 246 "lib/terminal.c"
__attribute__((warn_unused_result)) extern buffer_t* term_set_background_color(buffer_t* buffer, uint32_t color)
# 246 "lib/terminal.c"
{
  uint8_t blue = (color&0xff);
  uint8_t green = ((color>>8)&0xff);
  uint8_t red = ((color>>16)&0xff);

# 252 "lib/terminal.c"
  return buffer_printf(buffer, TERM_ESCAPE_STRING_START_AND_END("48;2;%d;%d;%d"), red, green, blue);
}


# 269 "lib/terminal.c"
__attribute__((warn_unused_result)) extern buffer_t* term_move_cursor_absolute(buffer_t* buffer, int x, int y)
# 269 "lib/terminal.c"
{

# 271 "lib/terminal.c"
  return buffer_printf(buffer, TERM_ESCAPE_STRING("%d;%dH"), (y+1), (x+1));
}


# 284 "lib/terminal.c"
__attribute__((warn_unused_result)) extern buffer_t* term_move_cursor_relative(buffer_t* buffer, int x, int y)
# 284 "lib/terminal.c"
{

# 286 "lib/terminal.c"
  if ((x>0))

# 286 "lib/terminal.c"
  {

# 287 "lib/terminal.c"
    (buffer=buffer_printf(buffer, TERM_ESCAPE_STRING("%dC"), x));
  }
  else

# 288 "lib/terminal.c"
  if ((x<0))

# 288 "lib/terminal.c"
  {

# 289 "lib/terminal.c"
    (buffer=buffer_printf(buffer, TERM_ESCAPE_STRING("%dD"), (-x)));
  }

# 291 "lib/terminal.c"
  if ((y>0))

# 291 "lib/terminal.c"
  {

# 292 "lib/terminal.c"
    (buffer=buffer_printf(buffer, TERM_ESCAPE_STRING("%dB"), y));
  }
  else

# 293 "lib/terminal.c"
  {

# 294 "lib/terminal.c"
    (buffer=buffer_printf(buffer, TERM_ESCAPE_STRING("%dA"), (-y)));
  }

# 296 "lib/terminal.c"
  return buffer;
}


# 306 "lib/terminal.c"
__attribute__((warn_unused_result)) extern buffer_t* term_bold(buffer_t* buffer)
# 306 "lib/terminal.c"
{

# 307 "lib/terminal.c"
  return buffer_printf(buffer, TERM_ESCAPE_STRING("1m"));
}


# 317 "lib/terminal.c"
__attribute__((warn_unused_result)) extern buffer_t* term_dim(buffer_t* buffer)
# 317 "lib/terminal.c"
{

# 318 "lib/terminal.c"
  return buffer_printf(buffer, TERM_ESCAPE_STRING("2m"));
}


# 328 "lib/terminal.c"
__attribute__((warn_unused_result)) extern buffer_t* term_italic(buffer_t* buffer)
# 328 "lib/terminal.c"
{

# 329 "lib/terminal.c"
  return buffer_printf(buffer, TERM_ESCAPE_STRING("3m"));
}


# 339 "lib/terminal.c"
__attribute__((warn_unused_result)) extern buffer_t* term_underline(buffer_t* buffer)
# 339 "lib/terminal.c"
{

# 340 "lib/terminal.c"
  return buffer_printf(buffer, TERM_ESCAPE_STRING("4m"));
}


# 351 "lib/terminal.c"
__attribute__((warn_unused_result)) extern buffer_t* term_reset_formatting(buffer_t* buffer)
# 351 "lib/terminal.c"
{

# 352 "lib/terminal.c"
  return buffer_printf(buffer, TERM_ESCAPE_STRING("0m"));
}


# 372 "lib/terminal.c"
__attribute__((warn_unused_result)) extern buffer_t* term_draw_box(buffer_t* buffer, uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, box_drawing_t* box)
# 373 "lib/terminal.c"
{

# 375 "lib/terminal.c"
  (buffer=term_move_cursor_absolute(buffer, x0, y0));

# 376 "lib/terminal.c"
  (buffer=buffer_append_code_point(buffer, (box->upper_left_corner)));

# 377 "lib/terminal.c"
  for (
    uint64_t x = (x0+1);
    (x<x1);
    (x++))

# 377 "lib/terminal.c"
  {

# 378 "lib/terminal.c"
    (buffer=buffer_append_code_point(buffer, (box->top_edge)));
  }

# 380 "lib/terminal.c"
  (buffer=buffer_append_code_point(buffer, (box->upper_right_corner)));

# 383 "lib/terminal.c"
  (buffer=term_move_cursor_absolute(buffer, x0, y1));

# 384 "lib/terminal.c"
  (buffer=buffer_append_code_point(buffer, (box->lower_left_corner)));

# 385 "lib/terminal.c"
  for (
    uint64_t x = (x0+1);
    (x<x1);
    (x++))

# 385 "lib/terminal.c"
  {

# 386 "lib/terminal.c"
    (buffer=buffer_append_code_point(buffer, (box->bottom_edge)));
  }

# 388 "lib/terminal.c"
  (buffer=buffer_append_code_point(buffer, (box->lower_right_corner)));

# 391 "lib/terminal.c"
  for (
    int y = (y0+1);
    (y<y1);
    (y++))

# 391 "lib/terminal.c"
  {

# 392 "lib/terminal.c"
    (buffer=term_move_cursor_absolute(buffer, x0, y));

# 393 "lib/terminal.c"
    (buffer=buffer_append_code_point(buffer, (box->left_edge)));

# 395 "lib/terminal.c"
    for (
      int x = (x0+1);
      (x<x1);
      (x++))

# 395 "lib/terminal.c"
    {

# 396 "lib/terminal.c"
      (buffer=buffer_append_code_point(buffer, ' '));
    }

# 400 "lib/terminal.c"
    (buffer=buffer_append_code_point(buffer, (box->right_edge)));
  }

# 403 "lib/terminal.c"
  return buffer;
}


# 412 "lib/terminal.c"
extern struct termios term_echo_off()
# 412 "lib/terminal.c"
{
  struct termios oldt;
  struct termios newt;

# 415 "lib/terminal.c"
  tcgetattr(STDIN_FILENO, (&oldt));

# 416 "lib/terminal.c"
  (newt=oldt);

# 419 "lib/terminal.c"
  tcgetattr(STDIN_FILENO, (&oldt));

# 420 "lib/terminal.c"
  (newt=oldt);

# 423 "lib/terminal.c"
  ((newt.c_lflag)&=(~(ICANON|ECHO)));

# 424 "lib/terminal.c"
  tcsetattr(STDIN_FILENO, TCSANOW, (&newt));

# 426 "lib/terminal.c"
  return oldt;
}


# 434 "lib/terminal.c"
extern void term_echo_restore(struct termios oldt)
# 434 "lib/terminal.c"
{

# 436 "lib/terminal.c"
  tcsetattr(STDIN_FILENO, TCSANOW, (&oldt));
}


# 33 "lib/tokenizer.c"
value_array_t* string_tokenize(const char* str, const char* delimiters)
# 33 "lib/tokenizer.c"
{

# 34 "lib/tokenizer.c"
  return tokenize_memory_range((/*CAST*/(uint8_t*) str), strlen(str), delimiters);
}


# 47 "lib/tokenizer.c"
value_array_t* buffer_tokenize(buffer_t* buffer, const char* delimiters)
# 47 "lib/tokenizer.c"
{

# 48 "lib/tokenizer.c"
  return tokenize_memory_range((&((buffer->elements)[0])), (buffer->length), delimiters);
}


# 60 "lib/tokenizer.c"
value_array_t* tokenize_memory_range(uint8_t* str, uint64_t length, const char* delimiters)
# 61 "lib/tokenizer.c"
{
  value_array_t* result = make_value_array(1);
  char token_data[1024];
  int cpos = 0;

# 65 "lib/tokenizer.c"
  for (
    int i = 0;
    (i<length);
    (i++))

# 65 "lib/tokenizer.c"
  {
    uint8_t ch = (str[i]);

# 67 "lib/tokenizer.c"
    if (((ch==0)||string_contains_char(delimiters, ch)))

# 67 "lib/tokenizer.c"
    {

# 68 "lib/tokenizer.c"
      ((token_data[(cpos++)])='\0');

# 69 "lib/tokenizer.c"
      if ((strlen(token_data)>0))

# 69 "lib/tokenizer.c"
      {

# 70 "lib/tokenizer.c"
        add_duplicate(result, token_data);
      }

# 72 "lib/tokenizer.c"
      (cpos=0);
    }
    else

# 73 "lib/tokenizer.c"
    {

# 74 "lib/tokenizer.c"
      ((token_data[(cpos++)])=ch);
    }
  }

# 77 "lib/tokenizer.c"
  ((token_data[(cpos++)])='\0');

# 78 "lib/tokenizer.c"
  if ((strlen(token_data)>0))

# 78 "lib/tokenizer.c"
  {

# 79 "lib/tokenizer.c"
    add_duplicate(result, token_data);
  }

# 82 "lib/tokenizer.c"
  return result;
}


# 86 "lib/tokenizer.c"
void add_duplicate(value_array_t* token_array, const char* data)
# 86 "lib/tokenizer.c"
{

# 87 "lib/tokenizer.c"
  value_array_add(token_array, str_to_value(string_duplicate(data)));
}


# 40 "lib/random.c"
random_state_t random_state_for_test(void)
# 40 "lib/random.c"
{

# 41 "lib/random.c"
  return ((random_state_t) {.a = 0x1E1D43C2CA44B1F5, .b = 0x4FDD267452CEDBAC});
}


# 91 "lib/random.c"
uint64_t random_next_uint64_below(random_state_t* state, uint64_t maximum)
# 91 "lib/random.c"
{

# 92 "lib/random.c"
  if ((maximum==0))

# 92 "lib/random.c"
  {

# 93 "lib/random.c"
    fatal_error(ERROR_ILLEGAL_ARGUMENT);
  }

# 97 "lib/random.c"
  return (random_next(state)%maximum);
  int mask = ((1ULL<<(uint64_highest_bit_set(maximum)+1))-1);

# 102 "lib/random.c"
  while (1)

# 102 "lib/random.c"
  {
    uint64_t n = random_next(state);

# 104 "lib/random.c"
    (n&=mask);

# 105 "lib/random.c"
    if ((n<maximum))

# 105 "lib/random.c"
    {

# 106 "lib/random.c"
      return n;
    }
  }
}


# 53 "lib/random.c"
random_state_t* random_state(void)
# 53 "lib/random.c"
{

# 54 "lib/random.c"
  if (((shared_random_state.a)==0))

# 54 "lib/random.c"
  {

# 55 "lib/random.c"
    ((shared_random_state.a)=(0x1E1D43C2CA44B1F5^(/*CAST*/(uint64_t) time(NULL))));

# 56 "lib/random.c"
    ((shared_random_state.b)=(0x4FDD267452CEDBAC^(/*CAST*/(uint64_t) time(NULL))));
  }

# 59 "lib/random.c"
  return (&shared_random_state);
}


# 73 "lib/random.c"
uint64_t random_next(random_state_t* state)
# 73 "lib/random.c"
{
  uint64_t s0 = (state->a);
  uint64_t s1 = (state->b);
  uint64_t result = (rotl((s0*5), 7)*9);

# 77 "lib/random.c"
  (s1^=s0);

# 78 "lib/random.c"
  ((state->a)=((rotl(s0, 24)^s1)^(s1<<16)));

# 79 "lib/random.c"
  ((state->b)=rotl(s1, 37));

# 81 "lib/random.c"
  return result;
}


# 38 "lib/cdl-printer.c"
cdl_printer_t* make_cdl_printer(buffer_t* buffer)
# 38 "lib/cdl-printer.c"
{
  cdl_printer_t* result = malloc_struct(cdl_printer_t);

# 40 "lib/cdl-printer.c"
  ((result->buffer)=buffer);

# 41 "lib/cdl-printer.c"
  return result;
}


# 44 "lib/cdl-printer.c"
void cdl_indent(cdl_printer_t* printer)
# 44 "lib/cdl-printer.c"
{

# 45 "lib/cdl-printer.c"
  buffer_append_repeated_byte((printer->buffer), ' ', (4*(printer->indention_level)));
}


# 49 "lib/cdl-printer.c"
boolean_t is_safe_string(char* string)
# 49 "lib/cdl-printer.c"
{
  buffer_t* buffer = buffer_from_string(string);

# 51 "lib/cdl-printer.c"
  for (
    int pos = 0;
    (pos<buffer_length(buffer));
    )

# 51 "lib/cdl-printer.c"
  {
    utf8_decode_result_t decode_result = buffer_utf8_decode(buffer, pos);

# 53 "lib/cdl-printer.c"
    if ((decode_result.error))

# 53 "lib/cdl-printer.c"
    {

# 54 "lib/cdl-printer.c"
      fatal_error(ERROR_ILLEGAL_UTF_8_CODE_POINT);
    }
    uint32_t code_point = (decode_result.code_point);

# 57 "lib/cdl-printer.c"
    if ((code_point<=32))

# 57 "lib/cdl-printer.c"
    {

# 58 "lib/cdl-printer.c"
      return false;
    }

# 60 "lib/cdl-printer.c"
    switch (code_point)

# 60 "lib/cdl-printer.c"
    {

# 61 "lib/cdl-printer.c"
      case '"':

# 62 "lib/cdl-printer.c"
      case '#':

# 63 "lib/cdl-printer.c"
      case '(':

# 64 "lib/cdl-printer.c"
      case ')':

# 65 "lib/cdl-printer.c"
      case ',':

# 66 "lib/cdl-printer.c"
      case ':':

# 67 "lib/cdl-printer.c"
      case '=':

# 68 "lib/cdl-printer.c"
      case '[':

# 69 "lib/cdl-printer.c"
      case '\'':

# 70 "lib/cdl-printer.c"
      case ']':

# 71 "lib/cdl-printer.c"
      case '`':

# 72 "lib/cdl-printer.c"
      case '{':

# 73 "lib/cdl-printer.c"
      case '}':

# 74 "lib/cdl-printer.c"
      return false;
    }

# 76 "lib/cdl-printer.c"
    (pos+=(decode_result.num_bytes));
  }

# 78 "lib/cdl-printer.c"
  return true;
}


# 81 "lib/cdl-printer.c"
void cdl_output_token(cdl_printer_t* printer, char* string)
# 81 "lib/cdl-printer.c"
{

# 82 "lib/cdl-printer.c"
  cdl_indent(printer);

# 83 "lib/cdl-printer.c"
  if (((printer->key_token)!=NULL))

# 83 "lib/cdl-printer.c"
  {

# 84 "lib/cdl-printer.c"
    buffer_printf((printer->buffer), "%s = %s\n", (printer->key_token), string);

# 85 "lib/cdl-printer.c"
    ((printer->key_token)=NULL);
  }
  else

# 86 "lib/cdl-printer.c"
  {

# 87 "lib/cdl-printer.c"
    buffer_printf((printer->buffer), "%s\n", string);
  }
}


# 91 "lib/cdl-printer.c"
void cdl_boolean(cdl_printer_t* printer, boolean_t boolean)
# 91 "lib/cdl-printer.c"
{

# 92 "lib/cdl-printer.c"
  cdl_output_token(printer, (boolean ? "true" : "false"));
}


# 95 "lib/cdl-printer.c"
void cdl_string(cdl_printer_t* printer, char* string)
# 95 "lib/cdl-printer.c"
{

# 96 "lib/cdl-printer.c"
  if ((!is_safe_string(string)))

# 96 "lib/cdl-printer.c"
  {

# 97 "lib/cdl-printer.c"
    cdl_output_token(printer, string_printf("\"%s\"", string));
  }
  else

# 98 "lib/cdl-printer.c"
  {

# 99 "lib/cdl-printer.c"
    cdl_output_token(printer, string);
  }
}


# 103 "lib/cdl-printer.c"
void cdl_int64(cdl_printer_t* printer, int64_t number)
# 103 "lib/cdl-printer.c"
{

# 104 "lib/cdl-printer.c"
  cdl_output_token(printer, string_printf("%ld", number));
}


# 107 "lib/cdl-printer.c"
void cdl_uint64(cdl_printer_t* printer, uint64_t number)
# 107 "lib/cdl-printer.c"
{

# 108 "lib/cdl-printer.c"
  cdl_output_token(printer, uint64_to_string(number));
}


# 111 "lib/cdl-printer.c"
void cdl_double(cdl_printer_t* printer, double number)
# 111 "lib/cdl-printer.c"
{

# 112 "lib/cdl-printer.c"
  cdl_output_token(printer, string_printf("%lf", number));
}


# 115 "lib/cdl-printer.c"
void cdl_start_array(cdl_printer_t* printer)
# 115 "lib/cdl-printer.c"
{

# 116 "lib/cdl-printer.c"
  cdl_output_token(printer, "[");

# 117 "lib/cdl-printer.c"
  ((printer->indention_level)+=1);
}


# 120 "lib/cdl-printer.c"
void cdl_end_array(cdl_printer_t* printer)
# 120 "lib/cdl-printer.c"
{

# 121 "lib/cdl-printer.c"
  ((printer->indention_level)-=1);

# 122 "lib/cdl-printer.c"
  cdl_output_token(printer, "]");
}


# 125 "lib/cdl-printer.c"
void cdl_start_table(cdl_printer_t* printer)
# 125 "lib/cdl-printer.c"
{

# 126 "lib/cdl-printer.c"
  cdl_output_token(printer, "{");

# 127 "lib/cdl-printer.c"
  ((printer->indention_level)+=1);
}


# 130 "lib/cdl-printer.c"
void cdl_key(cdl_printer_t* printer, char* key)
# 130 "lib/cdl-printer.c"
{

# 130 "lib/cdl-printer.c"
  ((printer->key_token)=key);
}


# 132 "lib/cdl-printer.c"
void cdl_end_table(cdl_printer_t* printer)
# 132 "lib/cdl-printer.c"
{

# 133 "lib/cdl-printer.c"
  ((printer->indention_level)-=1);

# 134 "lib/cdl-printer.c"
  cdl_output_token(printer, "}");
}


# 71 "lib/sub-process.c"
sub_process_t* make_sub_process(value_array_t* argv)
# 71 "lib/sub-process.c"
{
  sub_process_t* result = malloc_struct(sub_process_t);

# 73 "lib/sub-process.c"
  ((result->argv)=argv);

# 74 "lib/sub-process.c"
  ((result->exit_code)=(-1));

# 75 "lib/sub-process.c"
  return result;
}


# 84 "lib/sub-process.c"
boolean_t sub_process_launch(sub_process_t* sub_process)
# 84 "lib/sub-process.c"
{
  uint64_t length = ((sub_process->argv)->length);

# 86 "lib/sub-process.c"
  if ((length<1))

# 86 "lib/sub-process.c"
  {

# 87 "lib/sub-process.c"
    log_fatal("Expected at least the program path in argv");

# 88 "lib/sub-process.c"
    fatal_error(ERROR_ILLEGAL_STATE);
  }
  char** argv = (/*CAST*/(typeof(char**)) malloc_bytes(((length+1)*(sizeof(typeof(char*))))));

# 94 "lib/sub-process.c"
  for (
    int i = 0;
    (i<length);
    (i++))

# 94 "lib/sub-process.c"
  {

# 95 "lib/sub-process.c"
    ((argv[i])=value_array_get_ptr((sub_process->argv), i, typeof(char*)));
  }

# 99 "lib/sub-process.c"
  ((argv[length])=NULL);
  int stdin_pipe[2] = {0};
  int stdout_pipe[2] = {0};
  int stderr_pipe[2] = {0};

# 107 "lib/sub-process.c"
  if ((((pipe(stdin_pipe)==(-1))||(pipe(stdout_pipe)==(-1)))||(pipe(stderr_pipe)==(-1))))

# 108 "lib/sub-process.c"
  {

# 109 "lib/sub-process.c"
    log_fatal("Failed to create pipe for stdin, stdout or stderr");

# 110 "lib/sub-process.c"
    fatal_error(ERROR_ILLEGAL_STATE);

# 111 "lib/sub-process.c"
    return false;
  }
  pid_t pid = fork();

# 116 "lib/sub-process.c"
  if ((pid==(-1)))

# 116 "lib/sub-process.c"
  {

# 117 "lib/sub-process.c"
    log_fatal("fork() failed.");

# 118 "lib/sub-process.c"
    fatal_error(ERROR_ILLEGAL_STATE);

# 119 "lib/sub-process.c"
    return false;
  }

# 122 "lib/sub-process.c"
  if ((pid==0))

# 122 "lib/sub-process.c"
  {

# 128 "lib/sub-process.c"
    dup2((stdin_pipe[0]), STDIN_FILENO);

# 129 "lib/sub-process.c"
    dup2((stdout_pipe[1]), STDOUT_FILENO);

# 130 "lib/sub-process.c"
    dup2((stderr_pipe[1]), STDERR_FILENO);

# 131 "lib/sub-process.c"
    close((stdin_pipe[0]));

# 132 "lib/sub-process.c"
    close((stdin_pipe[1]));

# 133 "lib/sub-process.c"
    close((stdout_pipe[0]));

# 134 "lib/sub-process.c"
    close((stdout_pipe[1]));

# 135 "lib/sub-process.c"
    close((stderr_pipe[0]));

# 136 "lib/sub-process.c"
    close((stderr_pipe[1]));
    int exec_exit_status = execvp((argv[0]), argv);

# 141 "lib/sub-process.c"
    log_fatal("execvp returned non zero exit status %d", exec_exit_status);

# 142 "lib/sub-process.c"
    fatal_error(ERROR_ILLEGAL_STATE);

# 143 "lib/sub-process.c"
    return false;
  }
  else

# 144 "lib/sub-process.c"
  {

# 151 "lib/sub-process.c"
    close((stdin_pipe[0]));

# 152 "lib/sub-process.c"
    close((stdout_pipe[1]));

# 153 "lib/sub-process.c"
    close((stderr_pipe[1]));

# 156 "lib/sub-process.c"
    ((sub_process->pid)=pid);

# 157 "lib/sub-process.c"
    ((sub_process->stdin)=(stdin_pipe[1]));

# 158 "lib/sub-process.c"
    ((sub_process->stdout)=(stdout_pipe[0]));

# 159 "lib/sub-process.c"
    ((sub_process->stderr)=(stderr_pipe[0]));

# 161 "lib/sub-process.c"
    free_bytes(argv);

# 163 "lib/sub-process.c"
    return true;
  }
}


# 167 "lib/sub-process.c"
uint64_t sub_process_write(sub_process_t* sub_process, buffer_t* data, uint64_t start_position)
# 168 "lib/sub-process.c"
{
  int stdin_fd = (sub_process->stdin);
  int flags = fcntl(stdin_fd, F_GETFL, 0);

# 173 "lib/sub-process.c"
  fcntl(stdin_fd, F_SETFL, (flags|O_NONBLOCK));
  ssize_t bytes_written = write(stdin_fd, (&((data->elements)[start_position])), ((data->length)-start_position));

# 178 "lib/sub-process.c"
  if ((bytes_written==(-1)))

# 178 "lib/sub-process.c"
  {

# 179 "lib/sub-process.c"
    if (((errno==EAGAIN)||(errno==EWOULDBLOCK)))

# 179 "lib/sub-process.c"
    {

# 180 "lib/sub-process.c"
      return 0;
    }
    else

# 181 "lib/sub-process.c"
    {

# 183 "lib/sub-process.c"
      log_fatal("Error writing to subprocess stdin: %s", strerror(errno));

# 184 "lib/sub-process.c"
      fatal_error(ERROR_ILLEGAL_STATE);
    }
  }

# 188 "lib/sub-process.c"
  return bytes_written;
}


# 191 "lib/sub-process.c"
void sub_process_close_stdin(sub_process_t* sub_process)
# 191 "lib/sub-process.c"
{

# 192 "lib/sub-process.c"
  if (((sub_process->stdin)!=(-1)))

# 192 "lib/sub-process.c"
  {

# 193 "lib/sub-process.c"
    if ((close((sub_process->stdin))==(-1)))

# 193 "lib/sub-process.c"
    {

# 194 "lib/sub-process.c"
      log_fatal("Error closing subprocess stdin: %s", strerror(errno));

# 195 "lib/sub-process.c"
      fatal_error(ERROR_ILLEGAL_STATE);
    }

# 197 "lib/sub-process.c"
    ((sub_process->stdin)=(-1));
  }
}


# 201 "lib/sub-process.c"
void sub_process_read(sub_process_t* sub_process, buffer_t* stdout, buffer_t* stderr)
# 202 "lib/sub-process.c"
{

# 203 "lib/sub-process.c"
  if ((stdout!=NULL))

# 203 "lib/sub-process.c"
  {

# 204 "lib/sub-process.c"
    buffer_read_ready_bytes_file_number(stdout, (sub_process->stdout), 0xffffffff);
  }

# 207 "lib/sub-process.c"
  if ((stderr!=NULL))

# 207 "lib/sub-process.c"
  {

# 208 "lib/sub-process.c"
    buffer_read_ready_bytes_file_number(stderr, (sub_process->stderr), 0xffffffff);
  }
}


# 262 "lib/sub-process.c"
void sub_process_wait(sub_process_t* sub_process)
# 262 "lib/sub-process.c"
{

# 263 "lib/sub-process.c"
  if (((sub_process->exit_status)!=EXIT_STATUS_UNKNOWN))

# 263 "lib/sub-process.c"
  {
    int status = 0;
    pid_t result = waitpid((sub_process->pid), (&status), 0);

# 266 "lib/sub-process.c"
    sub_process_record_exit_status(sub_process, result, status);
  }
}


# 280 "lib/sub-process.c"
void sub_process_launch_and_wait(sub_process_t* sub_process, buffer_t* child_stdin, buffer_t* child_stdout, buffer_t* child_stderr)
# 282 "lib/sub-process.c"
{

# 283 "lib/sub-process.c"
  sub_process_launch(sub_process);
  uint64_t written = 0;

# 285 "lib/sub-process.c"
  do
# 285 "lib/sub-process.c"
  {

# 286 "lib/sub-process.c"
    if (((child_stdin!=NULL)&&(written<(child_stdin->length))))

# 286 "lib/sub-process.c"
    {

# 287 "lib/sub-process.c"
      (written+=sub_process_write(sub_process, child_stdin, written));

# 288 "lib/sub-process.c"
      if ((written>=(child_stdin->length)))

# 288 "lib/sub-process.c"
      {

# 289 "lib/sub-process.c"
        sub_process_close_stdin(sub_process);
      }
    }

# 292 "lib/sub-process.c"
    sub_process_read(sub_process, child_stdout, child_stderr);

# 293 "lib/sub-process.c"
    usleep(5);
  }
  while (is_sub_process_running(sub_process));

# 295 "lib/sub-process.c"
  sub_process_read(sub_process, child_stdout, child_stderr);

# 296 "lib/sub-process.c"
  sub_process_wait(sub_process);
}


# 215 "lib/sub-process.c"
void sub_process_record_exit_status(sub_process_t* sub_process, pid_t pid, int status)
# 216 "lib/sub-process.c"
{

# 217 "lib/sub-process.c"
  if ((pid==(-1)))

# 217 "lib/sub-process.c"
  {

# 218 "lib/sub-process.c"
    ((sub_process->exit_status)=EXIT_STATUS_ABNORMAL);

# 219 "lib/sub-process.c"
    return;
  }

# 223 "lib/sub-process.c"
  if (WIFEXITED(status))

# 223 "lib/sub-process.c"
  {

# 224 "lib/sub-process.c"
    ((sub_process->exit_status)=EXIT_STATUS_NORMAL_EXIT);

# 225 "lib/sub-process.c"
    ((sub_process->exit_code)=WEXITSTATUS(status));
  }
  else

# 226 "lib/sub-process.c"
  if (WIFSIGNALED(status))

# 226 "lib/sub-process.c"
  {

# 227 "lib/sub-process.c"
    ((sub_process->exit_status)=EXIT_STATUS_SIGNAL);

# 228 "lib/sub-process.c"
    ((sub_process->exit_signal)=WTERMSIG(status));
  }
  else

# 229 "lib/sub-process.c"
  {

# 230 "lib/sub-process.c"
    ((sub_process->exit_status)=EXIT_STATUS_ABNORMAL);
  }
}


# 241 "lib/sub-process.c"
boolean_t is_sub_process_running(sub_process_t* sub_process)
# 241 "lib/sub-process.c"
{

# 242 "lib/sub-process.c"
  if (((sub_process->exit_status)!=EXIT_STATUS_UNKNOWN))

# 242 "lib/sub-process.c"
  {

# 243 "lib/sub-process.c"
    return false;
  }
  int status = 0;
  pid_t result = waitpid((sub_process->pid), (&status), WNOHANG);

# 248 "lib/sub-process.c"
  if ((result==0))

# 248 "lib/sub-process.c"
  {

# 249 "lib/sub-process.c"
    return true;
  }

# 251 "lib/sub-process.c"
  sub_process_record_exit_status(sub_process, result, status);

# 252 "lib/sub-process.c"
  return false;
}


# 28 "lib/splitjoin.c"
buffer_t* join_array_of_strings(value_array_t* array_of_strings, char* separator)
# 29 "lib/splitjoin.c"
{
  buffer_t* result = make_buffer(1);

# 31 "lib/splitjoin.c"
  for (
    int i = 0;
    (i<(array_of_strings->length));
    (i++))

# 31 "lib/splitjoin.c"
  {

# 32 "lib/splitjoin.c"
    if ((i>0))

# 32 "lib/splitjoin.c"
    {

# 33 "lib/splitjoin.c"
      buffer_append_string(result, separator);
    }

# 35 "lib/splitjoin.c"
    buffer_append_string(result, (value_array_get(array_of_strings, i).str));
  }

# 37 "lib/splitjoin.c"
  return result;
}


# 17 "lib/oarchive.c"
void oarchive_append_header_and_file_contents(FILE* out, char* filename)
# 17 "lib/oarchive.c"
{
  buffer_t* contents = make_buffer(1);

# 19 "lib/oarchive.c"
  (contents=buffer_append_file_contents(contents, filename));

# 20 "lib/oarchive.c"
  fprintf(out, "filename=%s", filename);

# 21 "lib/oarchive.c"
  fputc(0, out);

# 22 "lib/oarchive.c"
  fprintf(out, "size=%d", (contents->length));

# 23 "lib/oarchive.c"
  fputc(0, out);

# 24 "lib/oarchive.c"
  fputc(0, out);

# 26 "lib/oarchive.c"
  for (
    uint64_t i = 0;
    (i<(contents->length));
    (i++))

# 26 "lib/oarchive.c"
  {

# 27 "lib/oarchive.c"
    fputc(buffer_get(contents, i), out);
  }
}


# 37 "lib/oarchive.c"
string_tree_t* oarchive_read_header(FILE* in)
# 37 "lib/oarchive.c"
{
  string_tree_t* metadata = NULL;

# 39 "lib/oarchive.c"
  while ((!feof(in)))

# 39 "lib/oarchive.c"
  {

# 40 "lib/oarchive.c"
    if ((file_peek_byte(in)=='\0'))

# 40 "lib/oarchive.c"
    {

# 41 "lib/oarchive.c"
      fgetc(in);

# 42 "lib/oarchive.c"
      break;
    }
    buffer_t* key = make_buffer(8);

# 47 "lib/oarchive.c"
    (key=buffer_read_until(key, in, '='));
    buffer_t* value = make_buffer(8);

# 49 "lib/oarchive.c"
    (value=buffer_read_until(value, in, '\0'));

# 50 "lib/oarchive.c"
    if ((((key->length)==0)&&((value->length)==0)))

# 50 "lib/oarchive.c"
    {

# 51 "lib/oarchive.c"
      return metadata;
    }

# 53 "lib/oarchive.c"
    (metadata=string_tree_insert(metadata, buffer_to_c_string(key), str_to_value(buffer_to_c_string(value))));
  }

# 56 "lib/oarchive.c"
  return metadata;
}


# 86 "lib/oarchive.c"
void oarchive_stream_members(FILE* in, oarchive_stream_headers_callback_t callback, void* callback_data)
# 88 "lib/oarchive.c"
{

# 89 "lib/oarchive.c"
  while ((!file_eof(in)))

# 89 "lib/oarchive.c"
  {
    string_tree_t* metadata = oarchive_read_header(in);
    int64_t size = 0;
    value_result_t size_value = string_tree_find(metadata, "size");

# 94 "lib/oarchive.c"
    if ((!is_ok(size_value)))

# 94 "lib/oarchive.c"
    {

# 95 "lib/oarchive.c"
      log_warn("Encounterd a header without an explicit size.");
    }
    else

# 96 "lib/oarchive.c"
    {
      value_result_t data_size = string_parse_uint64_dec((size_value.str));

# 98 "lib/oarchive.c"
      if ((!is_ok(data_size)))

# 98 "lib/oarchive.c"
      {

# 99 "lib/oarchive.c"
        log_fatal("Encounterd a header with an unparseable size %s", (size_value.str));

# 101 "lib/oarchive.c"
        fatal_error(ERROR_FATAL);
      }
      else

# 102 "lib/oarchive.c"
      {

# 103 "lib/oarchive.c"
        (size=(data_size.u64));
      }
    }
    boolean_t skip_data = callback(in, metadata, size, callback_data);

# 111 "lib/oarchive.c"
    if ((skip_data&&(size>0)))

# 111 "lib/oarchive.c"
    {

# 112 "lib/oarchive.c"
      log_none("Skipping %lu\n", size);

# 114 "lib/oarchive.c"
      file_skip_bytes(in, size);
    }
  }
}


# 92 "lib/test.c"
__attribute__((format(printf, 3, 4))) void test_fail_and_exit(char* file_name, int line_number, char* format, ...)
# 92 "lib/test.c"
{
  va_list args;

# 94 "lib/test.c"
  fprintf(stdout, "%s:%d: ", file_name, line_number);

# 95 "lib/test.c"
  va_start(args, format);

# 96 "lib/test.c"
  vfprintf(stdout, format, args);

# 97 "lib/test.c"
  fprintf(stdout, "\n");

# 98 "lib/test.c"
  va_end(args);

# 99 "lib/test.c"
  exit(1);
}


# 86 "keywords.c"
void initialize_keyword_maps(void)
# 86 "keywords.c"
{
  int num_keywords = ((sizeof(c_keywords_array))/(sizeof((c_keywords_array[0]))));

# 93 "keywords.c"
  (c_keywords_ht=make_string_hashtable((2*num_keywords)));

# 94 "keywords.c"
  (cpp_keywords_ht=make_string_hashtable((2*num_keywords)));

# 95 "keywords.c"
  for (
    int i = 0;
    (i<num_keywords);
    (i++))

# 95 "keywords.c"
  {

# 96 "keywords.c"
    (c_keywords_ht=string_ht_insert(c_keywords_ht, (c_keywords_array[i]), u64_to_value(1)));

# 98 "keywords.c"
    (oc_keywords_ht=string_ht_insert(c_keywords_ht, (c_keywords_array[i]), u64_to_value(1)));

# 100 "keywords.c"
    (cpp_keywords_ht=string_ht_insert(cpp_keywords_ht, (c_keywords_array[i]), u64_to_value(1)));
  }
  int num_types = ((sizeof(c_builtin_types_array))/(sizeof((c_builtin_types_array[0]))));

# 106 "keywords.c"
  (c_builtin_types_ht=make_string_hashtable((2*num_types)));

# 107 "keywords.c"
  (cpp_builtin_types_ht=make_string_hashtable((2*num_types)));

# 109 "keywords.c"
  for (
    int i = 0;
    (i<num_types);
    (i++))

# 109 "keywords.c"
  {

# 110 "keywords.c"
    (c_builtin_types_ht=string_ht_insert(c_builtin_types_ht, (c_builtin_types_array[i]), u64_to_value(1)));

# 112 "keywords.c"
    (oc_builtin_types_ht=string_ht_insert(c_builtin_types_ht, (c_builtin_types_array[i]), u64_to_value(1)));

# 114 "keywords.c"
    (cpp_builtin_types_ht=string_ht_insert(cpp_builtin_types_ht, (c_builtin_types_array[i]), u64_to_value(1)));
  }

# 119 "keywords.c"
  (cpp_builtin_types_ht=string_ht_insert(cpp_builtin_types_ht, "char8_t", u64_to_value(1)));
}


# 135 "keywords.c"
boolean_t is_reserved_word(input_mode_t mode, char* str)
# 135 "keywords.c"
{

# 136 "keywords.c"
  maybe_initialize_keyword_maps();

# 137 "keywords.c"
  switch (mode)

# 137 "keywords.c"
  {

# 138 "keywords.c"
    case INPUT_MODE_OMNI_C:

# 139 "keywords.c"
    return is_ok(string_ht_find(oc_keywords_ht, str));

# 140 "keywords.c"
    case INPUT_MODE_STANDARD_C:

# 141 "keywords.c"
    return is_ok(string_ht_find(c_keywords_ht, str));

# 142 "keywords.c"
    case INPUT_MODE_C_PLUS_PLUS:

# 143 "keywords.c"
    return is_ok(string_ht_find(cpp_keywords_ht, str));

# 144 "keywords.c"
    default:

# 145 "keywords.c"
    break;
  }

# 147 "keywords.c"
  fatal_error(ERROR_ILLEGAL_STATE);
}


# 156 "keywords.c"
boolean_t is_builtin_type_name(input_mode_t mode, char* str)
# 156 "keywords.c"
{

# 157 "keywords.c"
  maybe_initialize_keyword_maps();

# 158 "keywords.c"
  switch (mode)

# 158 "keywords.c"
  {

# 159 "keywords.c"
    case INPUT_MODE_OMNI_C:

# 160 "keywords.c"
    return is_ok(string_ht_find(oc_builtin_types_ht, str));

# 161 "keywords.c"
    case INPUT_MODE_STANDARD_C:

# 162 "keywords.c"
    return is_ok(string_ht_find(c_builtin_types_ht, str));

# 163 "keywords.c"
    case INPUT_MODE_C_PLUS_PLUS:

# 164 "keywords.c"
    return is_ok(string_ht_find(cpp_builtin_types_ht, str));

# 165 "keywords.c"
    default:

# 166 "keywords.c"
    break;
  }

# 168 "keywords.c"
  fatal_error(ERROR_ILLEGAL_STATE);
}


# 23 "file-reader.c"
value_array_t* read_files(value_array_t* files)
# 23 "file-reader.c"
{

# 24 "file-reader.c"
  fprintf(stderr, "Parsing %d files...\n", (files->length));
  value_array_t* result = make_value_array((files->length));

# 26 "file-reader.c"
  for (
    int i = 0;
    (i<(files->length));
    (i++))

# 26 "file-reader.c"
  {
    char* file_name = (value_array_get(files, i).str);

# 28 "file-reader.c"
    if (string_ends_with(file_name, ".oar"))

# 28 "file-reader.c"
    {

# 29 "file-reader.c"
      fprintf(stderr, "Reading archive %s\n", file_name);

# 30 "file-reader.c"
      add_all_oarchive_members(result, file_name);
    }
    else

# 31 "file-reader.c"
    {

# 32 "file-reader.c"
      fprintf(stderr, "Reading %s\n", file_name);

# 33 "file-reader.c"
      value_array_add(result, ptr_to_value(read_file(file_name)));
    }
  }

# 36 "file-reader.c"
  return result;
}


# 45 "file-reader.c"
file_t* read_file(char* file_name)
# 45 "file-reader.c"
{
  file_t* result = malloc_struct(file_t);
  buffer_t* buffer = make_buffer((1024*8));

# 49 "file-reader.c"
  (buffer=buffer_append_file_contents(buffer, file_name));

# 52 "file-reader.c"
  ((result->tag)=STD_C_SOURCE_FILE);

# 53 "file-reader.c"
  ((result->file_name)=file_name);

# 54 "file-reader.c"
  ((result->data)=buffer);

# 55 "file-reader.c"
  return result;
}


# 58 "file-reader.c"
void add_all_oarchive_members(value_array_t* result, char* archive_file_name)
# 58 "file-reader.c"
{
  FILE* in = fopen(archive_file_name, "r");

# 60 "file-reader.c"
  oarchive_stream_members(in, (&add_orachive_file), result);

# 61 "file-reader.c"
  fclose(in);
}


# 64 "file-reader.c"
boolean_t add_orachive_file(FILE* input, string_tree_t* metadata, int64_t size, void* callback_data)
# 65 "file-reader.c"
{
  value_result_t filename_value = string_tree_find(metadata, "filename");

# 67 "file-reader.c"
  if (is_ok(filename_value))

# 67 "file-reader.c"
  {
    char* file_name = (filename_value.str);

# 69 "file-reader.c"
    log_info("Extracting %s from archive", file_name);
    file_t* read_file = malloc_struct(file_t);
    buffer_t* buffer = make_buffer(size);

# 74 "file-reader.c"
    while (((size--)>0))

# 74 "file-reader.c"
    {
      int b = fgetc(input);

# 76 "file-reader.c"
      buffer_append_byte(buffer, b);
    }

# 80 "file-reader.c"
    ((read_file->tag)=STD_C_SOURCE_FILE);

# 81 "file-reader.c"
    ((read_file->file_name)=file_name);

# 82 "file-reader.c"
    ((read_file->data)=buffer);

# 83 "file-reader.c"
    value_array_add((/*CAST*/(value_array_t*) callback_data), ptr_to_value(read_file));
  }
  else

# 85 "file-reader.c"
  {

# 86 "file-reader.c"
    log_fatal("There is no filename for an omni-archive memember.");

# 87 "file-reader.c"
    log_fatal("Perhaps the .oar file isn't properly formed or created by omni-c " "archive?");

# 90 "file-reader.c"
    fatal_error(ERROR_ILLEGAL_INPUT);
  }

# 95 "file-reader.c"
  return false;
}


# 73 "compiler-errors.c"
buffer_t* buffer_append_human_readable_error(buffer_t* buffer, compiler_error_t* error)
# 74 "compiler-errors.c"
{

# 75 "compiler-errors.c"
  if (((error->tokenizer_error_code)!=TOKENIZER_ERROR_UNKNOWN))

# 75 "compiler-errors.c"
  {

# 76 "compiler-errors.c"
    (buffer=buffer_append_human_readable_tokenizer_error(buffer, error));
  }

# 78 "compiler-errors.c"
  if (((error->parse_error_code)!=PARSE_ERROR_UNKNOWN))

# 78 "compiler-errors.c"
  {

# 79 "compiler-errors.c"
    (buffer=buffer_append_human_readable_parser_error(buffer, error));
  }

# 81 "compiler-errors.c"
  return buffer;
}


# 94 "compiler-errors.c"
src_code_snippets_t get_source_code_snippet(buffer_t* buffer, uint64_t location, int before_lines, int after_lines)
# 95 "compiler-errors.c"
{
  src_code_snippets_t result = {0};
  uint64_t current_begin = buffer_beginning_of_line(buffer, location);
  uint64_t current_end = buffer_end_of_line(buffer, location);

# 101 "compiler-errors.c"
  ((result.current_line)=buffer_c_substring(buffer, current_begin, current_end));
  uint64_t prefix_begin = current_begin;

# 104 "compiler-errors.c"
  for (
    int i = 0;
    ((i<before_lines)&&(prefix_begin>0));
    (i++))

# 104 "compiler-errors.c"
  {

# 105 "compiler-errors.c"
    (prefix_begin=buffer_beginning_of_line(buffer, (prefix_begin-1)));
  }

# 107 "compiler-errors.c"
  ((result.previous_lines)=buffer_c_substring(buffer, prefix_begin, current_begin));
  uint64_t suffix_end = current_end;

# 111 "compiler-errors.c"
  for (
    int i = 0;
    ((i<after_lines)&&(suffix_end<(buffer->length)));
    (i++))

# 111 "compiler-errors.c"
  {

# 112 "compiler-errors.c"
    (suffix_end=buffer_end_of_line(buffer, (suffix_end+1)));
  }

# 114 "compiler-errors.c"
  ((result.next_lines)=buffer_c_substring(buffer, current_end, suffix_end));

# 116 "compiler-errors.c"
  return result;
}


# 129 "compiler-errors.c"
char* do_common_replacements(char* template, compiler_error_t* error)
# 129 "compiler-errors.c"
{
  buffer_t* buffer = make_buffer(256);
  char* file_name = (error->file_name);
  src_code_snippets_t snippet = {0};

# 135 "compiler-errors.c"
  if ((error->error_token))

# 135 "compiler-errors.c"
  {
    line_and_column_t position_info = buffer_position_to_line_and_column(((error->error_token)->buffer), ((error->error_token)->start));

# 140 "compiler-errors.c"
    (buffer=buffer_printf(buffer, "%s:%d.%d: ", file_name, ((position_info.line)&0xffffffff), ((position_info.column)&0xffffffff)));

# 144 "compiler-errors.c"
    (snippet=get_source_code_snippet(((error->error_token)->buffer), ((error->error_token)->start), 5, 3));
  }
  else

# 146 "compiler-errors.c"
  {

# 147 "compiler-errors.c"
    (buffer=buffer_printf(buffer, "%s:%d.%d: ", file_name, 0, 0));
  }

# 150 "compiler-errors.c"
  (buffer=buffer_append_string(buffer, template));

# 153 "compiler-errors.c"
  (buffer=buffer_replace_all(buffer, "{formatted_snippet}", formatted_snippet));

# 159 "compiler-errors.c"
  (buffer=buffer_replace_all(buffer, "{error-highlight-on}", "\033[1m"));

# 161 "compiler-errors.c"
  (buffer=buffer_replace_all(buffer, "{error-highlight-off}", "\033[0m"));

# 163 "compiler-errors.c"
  (buffer=buffer_replace_all(buffer, "{error-prefix-lines}", (snippet.previous_lines)));

# 165 "compiler-errors.c"
  (buffer=buffer_replace_all(buffer, "{error-current-line}", (snippet.current_line)));

# 167 "compiler-errors.c"
  (buffer=buffer_replace_all(buffer, "{error-suffix-lines}", (snippet.next_lines)));

# 170 "compiler-errors.c"
  return buffer_to_c_string(buffer);
}


# 228 "compiler-errors.c"
buffer_t* buffer_append_human_readable_tokenizer_error(buffer_t* buffer, compiler_error_t* error)
# 229 "compiler-errors.c"
{

# 234 "compiler-errors.c"
  (buffer=buffer_printf(buffer, "\nlexer error code = %d\n", (error->tokenizer_error_code)));

# 236 "compiler-errors.c"
  return buffer;
}


# 239 "compiler-errors.c"
buffer_t* buffer_append_human_readable_parser_error(buffer_t* buffer, compiler_error_t* error)
# 240 "compiler-errors.c"
{

# 241 "compiler-errors.c"
  (buffer=buffer_printf(buffer, "\nparser error code = %d\n", (error->parse_error_code)));
  char* template = ((void *)0);

# 244 "compiler-errors.c"
  switch ((error->parse_error_code))

# 244 "compiler-errors.c"
  {

# 245 "compiler-errors.c"
    case PARSE_ERROR_EXPECTED_FIELD_WIDTH_OR_SEMICOLON:

# 246 "compiler-errors.c"
    (template=error_field_width_or_semicolon);

# 247 "compiler-errors.c"
    break;

# 249 "compiler-errors.c"
    case PARSE_ERROR_CLOSE_BRACKET_EXPECTED:

# 250 "compiler-errors.c"
    (template=error_open_brace_expected);

# 251 "compiler-errors.c"
    break;

# 253 "compiler-errors.c"
    case PARSE_ERROR_UNRECOGNIZED_TOP_LEVEL_DECLARATION:

# 254 "compiler-errors.c"
    (template=error_unrecognized_top_level_declaration);

# 255 "compiler-errors.c"
    break;

# 257 "compiler-errors.c"
    case PARSE_ERROR_SEMICOLON_EXPECTED:

# 258 "compiler-errors.c"
    (template=error_open_semicolon_expected);

# 259 "compiler-errors.c"
    break;

# 261 "compiler-errors.c"
    case PARSE_ERROR_CONFLICTING_STORAGE_CLASS_SPECIFIER:

# 262 "compiler-errors.c"
    (template=error_conflicting_storage_class_specifier);

# 263 "compiler-errors.c"
    break;

# 265 "compiler-errors.c"
    case PARSE_ERROR_CONFLICTING_FUNCTION_SPECIFIER:

# 266 "compiler-errors.c"
    (template=error_conflicting_function_specifier);

# 267 "compiler-errors.c"
    break;

# 269 "compiler-errors.c"
    case PARSE_ERROR_EXPECTED_OPEN_PAREN_AFTER_UNDERSCORE_ATTRIBUTE:

# 270 "compiler-errors.c"
    (template=error_expected_open_paren_for_underscore_attribute);

# 271 "compiler-errors.c"
    break;

# 273 "compiler-errors.c"
    case PARSE_ERROR_EXPECTED_MATCHING_CLOSE_PAREN_AFTER_UNDERSCORE_ATTRIBUTE:

# 274 "compiler-errors.c"
    (template=error_expected_matching_close_paren_after_underscore_attribute);

# 275 "compiler-errors.c"
    break;

# 277 "compiler-errors.c"
    default:

# 278 "compiler-errors.c"
    (template=parse_error_unknown);

# 279 "compiler-errors.c"
    break;
  }
  char* template_string = do_common_replacements(template, error);

# 283 "compiler-errors.c"
  return buffer_append_string(buffer, template_string);
}


# 101 "lexer.c"
char* token_to_string(token_t* token)
# 101 "lexer.c"
{

# 102 "lexer.c"
  return buffer_c_substring((token->buffer), (token->start), (token->end));
}


# 111 "lexer.c"
token_t* make_derived_token(token_t* source_token)
# 111 "lexer.c"
{
  token_t* result = (/*CAST*/(token_t*) malloc_copy_of((/*CAST*/(uint8_t*) source_token), (sizeof(token_t))));
  buffer_t* buffer = make_buffer(((source_token->end)-(source_token->start)));

# 115 "lexer.c"
  (buffer=buffer_append_sub_buffer(buffer, (source_token->start), (source_token->end), (result->buffer)));

# 117 "lexer.c"
  ((result->buffer)=buffer);

# 118 "lexer.c"
  ((result->start)=0);

# 119 "lexer.c"
  ((result->end)=(buffer->length));

# 120 "lexer.c"
  return result;
}


# 132 "lexer.c"
__attribute__((warn_unused_result)) buffer_t* append_token_debug_string(buffer_t* buffer, token_t token)
# 132 "lexer.c"
{
  char* str = token_to_string((&token));

# 134 "lexer.c"
  (buffer=buffer_printf(buffer, "type: %s start: %d end: %d line=%d column=%d str: %s", token_type_to_string((token.type)), (token.start), (token.end), (token.line_number), (token.column_number), str));

# 138 "lexer.c"
  free_bytes(str);

# 139 "lexer.c"
  return buffer;
}


# 149 "lexer.c"
buffer_t* buffer_append_token_string(buffer_t* buffer, token_t* token)
# 149 "lexer.c"
{
  char* str = token_to_string(token);

# 153 "lexer.c"
  (buffer=buffer_printf(buffer, "%s", str));

# 154 "lexer.c"
  free_bytes(str);

# 155 "lexer.c"
  return buffer;
}


# 181 "lexer.c"
token_or_error_t tokenize_whitespace(buffer_t* buffer, uint64_t start_position)
# 182 "lexer.c"
{
  uint64_t pos = start_position;

# 184 "lexer.c"
  while ((pos<buffer_length(buffer)))

# 184 "lexer.c"
  {
    utf8_decode_result_t decode_result = buffer_utf8_decode(buffer, pos);

# 186 "lexer.c"
    if ((decode_result.error))

# 186 "lexer.c"
    {

# 187 "lexer.c"
      return ((token_or_error_t) {.error_code = TOKENIZER_ERROR_UTF_DECODE_ERROR,
                               .error_position = pos});
    }
    uint32_t code_point = (decode_result.code_point);

# 192 "lexer.c"
    if ((!isspace(code_point)))

# 192 "lexer.c"
    {

# 193 "lexer.c"
      break;
    }

# 195 "lexer.c"
    (pos+=(decode_result.num_bytes));

# 201 "lexer.c"
    if ((code_point=='\n'))

# 201 "lexer.c"
    {

# 202 "lexer.c"
      break;
    }
  }

# 206 "lexer.c"
  return ((token_or_error_t) {.token = compound_literal(token_t, {.buffer = buffer,
                                           .type = TOKEN_TYPE_WHITESPACE,
                                           .start = start_position,
                                           .end = pos})});
}


# 218 "lexer.c"
boolean_t is_identifier_start(uint32_t code_point)
# 218 "lexer.c"
{

# 223 "lexer.c"
  switch (code_point)

# 223 "lexer.c"
  {

# 224 "lexer.c"
    case '$':

# 225 "lexer.c"
    return true;

# 226 "lexer.c"
    case '_':

# 227 "lexer.c"
    return true;

# 228 "lexer.c"
    default:

# 229 "lexer.c"
    return isalpha(code_point);
  }
}


# 238 "lexer.c"
token_or_error_t tokenize_identifier(buffer_t* buffer, uint64_t start_position)
# 239 "lexer.c"
{
  uint64_t pos = start_position;

# 241 "lexer.c"
  while ((pos<buffer_length(buffer)))

# 241 "lexer.c"
  {
    utf8_decode_result_t decode_result = buffer_utf8_decode(buffer, pos);

# 243 "lexer.c"
    if ((decode_result.error))

# 243 "lexer.c"
    {

# 244 "lexer.c"
      return ((token_or_error_t) {.error_code = TOKENIZER_ERROR_UTF_DECODE_ERROR,
                               .error_position = pos});
    }
    uint32_t code_point = (decode_result.code_point);

# 249 "lexer.c"
    if ((!(is_identifier_start(code_point)||isdigit(code_point))))

# 249 "lexer.c"
    {

# 250 "lexer.c"
      break;
    }

# 252 "lexer.c"
    (pos+=(decode_result.num_bytes));
  }

# 255 "lexer.c"
  return ((token_or_error_t) {.token = compound_literal(token_t, {.buffer = buffer,
                                           .type = TOKEN_TYPE_IDENTIFIER,
                                           .start = start_position,
                                           .end = pos})});
}


# 280 "lexer.c"
token_or_error_t tokenize_numeric(buffer_t* buffer, uint64_t start_position)
# 280 "lexer.c"
{
  numeric_literal_encoding_t encoding = NUMERIC_LITERAL_ENCODING_UNDECIDED;
  uint32_t previous_code_point = 0;
  token_type_t token_type = TOKEN_TYPE_INTEGER_LITERAL;
  uint64_t offset = 0;
  uint64_t pos = start_position;

# 288 "lexer.c"
  while ((pos<buffer_length(buffer)))

# 288 "lexer.c"
  {
    utf8_decode_result_t decode_result = buffer_utf8_decode(buffer, pos);

# 290 "lexer.c"
    if ((decode_result.error))

# 290 "lexer.c"
    {

# 291 "lexer.c"
      return ((token_or_error_t) {.error_code = TOKENIZER_ERROR_UTF_DECODE_ERROR,
                               .error_position = pos});
    }
    uint32_t code_point = (decode_result.code_point);

# 297 "lexer.c"
    if (((offset==0)&&(code_point!='0')))

# 297 "lexer.c"
    {

# 298 "lexer.c"
      (encoding=NUMERIC_LITERAL_ENCODING_FLOAT_OR_DECIMAL);
    }

# 301 "lexer.c"
    if (((offset==1)&&(encoding==NUMERIC_LITERAL_ENCODING_UNDECIDED)))

# 301 "lexer.c"
    {
      boolean_t changed = false;

# 303 "lexer.c"
      if ((code_point=='x'))

# 303 "lexer.c"
      {

# 304 "lexer.c"
        (encoding=NUMERIC_LITERAL_ENCODING_HEX);

# 305 "lexer.c"
        (changed=true);
      }
      else

# 306 "lexer.c"
      if ((code_point=='o'))

# 306 "lexer.c"
      {

# 307 "lexer.c"
        (encoding=NUMERIC_LITERAL_ENCODING_OCTAL);

# 308 "lexer.c"
        (changed=true);
      }
      else

# 309 "lexer.c"
      if ((code_point=='b'))

# 309 "lexer.c"
      {

# 310 "lexer.c"
        (encoding=NUMERIC_LITERAL_ENCODING_HEX);

# 311 "lexer.c"
        (changed=true);
      }

# 313 "lexer.c"
      if (changed)

# 313 "lexer.c"
      {

# 314 "lexer.c"
        (pos+=(decode_result.num_bytes));

# 315 "lexer.c"
        (previous_code_point=code_point);

# 316 "lexer.c"
        (offset+=1);

# 317 "lexer.c"
        continue;
      }
    }

# 321 "lexer.c"
    if (((code_point=='.')&&((encoding==NUMERIC_LITERAL_ENCODING_UNDECIDED)||(encoding==NUMERIC_LITERAL_ENCODING_FLOAT_OR_DECIMAL))))

# 323 "lexer.c"
    {

# 324 "lexer.c"
      (token_type=TOKEN_TYPE_FLOAT_LITERAL);

# 325 "lexer.c"
      (encoding=NUMERIC_LITERAL_ENCODING_FLOAT);

# 326 "lexer.c"
      (pos+=(decode_result.num_bytes));

# 327 "lexer.c"
      (previous_code_point=code_point);

# 328 "lexer.c"
      (offset+=1);

# 329 "lexer.c"
      continue;
    }

# 332 "lexer.c"
    if ((!can_extend_number(encoding, code_point, previous_code_point)))

# 332 "lexer.c"
    {

# 333 "lexer.c"
      break;
    }

# 336 "lexer.c"
    (pos+=(decode_result.num_bytes));

# 337 "lexer.c"
    (previous_code_point=code_point);

# 338 "lexer.c"
    (offset+=1);
  }

# 341 "lexer.c"
  return ((token_or_error_t) {.token = compound_literal(token_t, {.buffer = buffer,
                                           .type = token_type,
                                           .start = start_position,
                                           .end = pos})});
}


# 349 "lexer.c"
boolean_t can_extend_number(numeric_literal_encoding_t encoding, uint32_t code_point, uint32_t previous_code_point)
# 350 "lexer.c"
{

# 351 "lexer.c"
  switch (encoding)

# 351 "lexer.c"
  {

# 352 "lexer.c"
    case NUMERIC_LITERAL_ENCODING_UNDECIDED:

# 353 "lexer.c"
    return string_contains_char("0123456789.eEobxLlUuFf", code_point);

# 355 "lexer.c"
    case NUMERIC_LITERAL_ENCODING_FLOAT_OR_DECIMAL:

# 356 "lexer.c"
    return string_contains_char("0123456789.eELlUuFf", code_point);

# 358 "lexer.c"
    case NUMERIC_LITERAL_ENCODING_BINARY:

# 359 "lexer.c"
    return string_contains_char("01LlUu", code_point);

# 361 "lexer.c"
    case NUMERIC_LITERAL_ENCODING_OCTAL:

# 362 "lexer.c"
    return string_contains_char("01234567LlUu", code_point);

# 364 "lexer.c"
    case NUMERIC_LITERAL_ENCODING_HEX:

# 365 "lexer.c"
    return string_contains_char("0123456789abcdefABCDEFLlUu", code_point);

# 367 "lexer.c"
    case NUMERIC_LITERAL_ENCODING_DECIMAL:

# 368 "lexer.c"
    return string_contains_char("0123456789LlUu", code_point);

# 370 "lexer.c"
    case NUMERIC_LITERAL_ENCODING_FLOAT:

# 371 "lexer.c"
    return (string_contains_char("0123456789FfeE", code_point)||(((previous_code_point=='e')||(previous_code_point=='E'))&&string_contains_char("+-", code_point)));

# 375 "lexer.c"
    default:

# 376 "lexer.c"
    fatal_error(ERROR_ILLEGAL_STATE);

# 377 "lexer.c"
    return false;
  }

# 380 "lexer.c"
  return false;
}


# 449 "lexer.c"
token_or_error_t tokenize_punctuation(buffer_t* buffer, uint64_t start_position)
# 450 "lexer.c"
{
  int num_elements = ((sizeof(c_punctuation))/(sizeof((c_punctuation[0]))));

# 452 "lexer.c"
  for (
    int i = 0;
    (i<num_elements);
    (i++))

# 452 "lexer.c"
  {

# 453 "lexer.c"
    if (buffer_match_string_at(buffer, start_position, (c_punctuation[i])))

# 453 "lexer.c"
    {

# 454 "lexer.c"
      return ((token_or_error_t) {.token = compound_literal(
               token_t, {.buffer = buffer,
                         .type = TOKEN_TYPE_PUNCTUATION,
                         .start = start_position,
                         .end = start_position + strlen(c_punctuation[i])})});
    }
  }

# 463 "lexer.c"
  return ((token_or_error_t) {.error_code = TOKENIZER_ERROR_UNRECOGNIZED_PUNCTUATION,
                         .error_position = start_position});
}


# 468 "lexer.c"
boolean_t is_comment_start(buffer_t* buffer, uint64_t position)
# 468 "lexer.c"
{

# 469 "lexer.c"
  return (buffer_match_string_at(buffer, position, "//")||buffer_match_string_at(buffer, position, "/*"));
}


# 473 "lexer.c"
token_or_error_t tokenize_comment(buffer_t* buffer, uint64_t start_position)
# 473 "lexer.c"
{

# 474 "lexer.c"
  if (buffer_match_string_at(buffer, start_position, "//"))

# 474 "lexer.c"
  {

# 475 "lexer.c"
    for (
      uint64_t position = (start_position+2);
      (position<(buffer->length));
      (position++))

# 476 "lexer.c"
    {

# 477 "lexer.c"
      if (buffer_match_string_at(buffer, position, "\n"))

# 477 "lexer.c"
      {

# 478 "lexer.c"
        return ((token_or_error_t) {.token = compound_literal(token_t, {.buffer = buffer,
                                                 .type = TOKEN_TYPE_COMMENT,
                                                 .start = start_position,
                                                 .end = position + 1})});
      }
    }
  }
  else

# 486 "lexer.c"
  {

# 487 "lexer.c"
    for (
      uint64_t position = (start_position+2);
      (position<(buffer->length));
      (position++))

# 488 "lexer.c"
    {

# 489 "lexer.c"
      if (buffer_match_string_at(buffer, position, "*/"))

# 489 "lexer.c"
      {

# 490 "lexer.c"
        return ((token_or_error_t) {.token = compound_literal(token_t, {.buffer = buffer,
                                                 .type = TOKEN_TYPE_COMMENT,
                                                 .start = start_position,
                                                 .end = position + 2})});
      }
    }
  }

# 500 "lexer.c"
  return ((token_or_error_t) {.error_code = TOKENIZER_ERROR_UNTERMINATED_COMMENT,
                           .error_position = start_position});
}


# 505 "lexer.c"
boolean_t is_string_literal_start(buffer_t* buffer, uint64_t position)
# 505 "lexer.c"
{

# 506 "lexer.c"
  return buffer_match_string_at(buffer, position, "\"");
}


# 509 "lexer.c"
token_or_error_t tokenize_quoted_literal_common(buffer_t* buffer, uint64_t start_position, char* opening_sequence, char* quoted_closing_sequence, char* closing_sequence, token_type_t token_type, tokenizer_error_t unterminated_error_code)
# 512 "lexer.c"
{

# 519 "lexer.c"
  if ((!buffer_match_string_at(buffer, start_position, opening_sequence)))

# 519 "lexer.c"
  {

# 520 "lexer.c"
    fatal_error(ERROR_ILLEGAL_STATE);
  }

# 523 "lexer.c"
  for (
    uint64_t position = (start_position+strlen(opening_sequence));
    (position<(buffer->length));
    )

# 524 "lexer.c"
  {

# 525 "lexer.c"
    if (buffer_match_string_at(buffer, position, quoted_closing_sequence))

# 525 "lexer.c"
    {

# 526 "lexer.c"
      (position+=strlen(quoted_closing_sequence));
    }
    else

# 527 "lexer.c"
    if (buffer_match_string_at(buffer, position, closing_sequence))

# 527 "lexer.c"
    {

# 528 "lexer.c"
      return ((token_or_error_t) {.token = compound_literal(
               token_t, {.buffer = buffer,
                         .type = token_type,
                         .start = start_position,
                         .end = position + strlen(closing_sequence)})});
    }
    else

# 535 "lexer.c"
    {

# 541 "lexer.c"
      (position+=1);
    }
  }

# 544 "lexer.c"
  return ((token_or_error_t) {.error_code = unterminated_error_code,
                           .error_position = start_position});
}


# 549 "lexer.c"
token_or_error_t tokenize_string_literal(buffer_t* buffer, uint64_t start_position)
# 550 "lexer.c"
{

# 551 "lexer.c"
  return tokenize_quoted_literal_common(buffer, start_position, "\"", "\\\"", "\"", TOKEN_TYPE_STRING_LITERAL, TOKENIZER_ERROR_UNTERMINATED_STRING_LITERAL);
}


# 556 "lexer.c"
boolean_t is_character_literal_start(buffer_t* buffer, uint64_t position)
# 556 "lexer.c"
{

# 557 "lexer.c"
  return buffer_match_string_at(buffer, position, "'");
}


# 560 "lexer.c"
token_or_error_t tokenize_character_literal(buffer_t* buffer, uint64_t start_position)
# 561 "lexer.c"
{

# 566 "lexer.c"
  return tokenize_quoted_literal_common(buffer, start_position, "'", "\\'", "'", TOKEN_TYPE_CHARACTER_LITERAL, TOKENIZER_ERROR_UNTERMINATED_CHARACTER_LITERL);
}


# 598 "lexer.c"
tokenizer_result_t tokenize(buffer_t* buffer)
# 598 "lexer.c"
{
  tokenizer_result_t result = {0};
  value_array_t* result_tokens = make_value_array(1024);
  uint64_t line_number = 1;
  uint64_t column_number = 1;
  uint32_t pos = 0;

# 609 "lexer.c"
  while ((pos<buffer_length(buffer)))

# 609 "lexer.c"
  {
    utf8_decode_result_t decode_result = buffer_utf8_decode(buffer, pos);

# 611 "lexer.c"
    if ((decode_result.error))

# 611 "lexer.c"
    {

# 612 "lexer.c"
      return ((tokenizer_result_t) {.tokenizer_error_code = TOKENIZER_ERROR_UTF_DECODE_ERROR});
    }
    uint32_t code_point = (decode_result.code_point);
    token_t* token = ((void *)0);

# 620 "lexer.c"
    if (isspace(code_point))

# 620 "lexer.c"
    {

# 621 "lexer.c"
      read_token(tokenize_whitespace);
    }
    else

# 622 "lexer.c"
    if (is_identifier_start(code_point))

# 622 "lexer.c"
    {

# 623 "lexer.c"
      read_token(tokenize_identifier);
    }
    else

# 624 "lexer.c"
    if (isdigit(code_point))

# 624 "lexer.c"
    {

# 625 "lexer.c"
      read_token(tokenize_numeric);
    }
    else

# 626 "lexer.c"
    if (is_comment_start(buffer, pos))

# 626 "lexer.c"
    {

# 627 "lexer.c"
      read_token(tokenize_comment);
    }
    else

# 628 "lexer.c"
    if (is_string_literal_start(buffer, pos))

# 628 "lexer.c"
    {

# 629 "lexer.c"
      read_token(tokenize_string_literal);
    }
    else

# 630 "lexer.c"
    if (is_character_literal_start(buffer, pos))

# 630 "lexer.c"
    {

# 631 "lexer.c"
      read_token(tokenize_character_literal);
    }
    else

# 632 "lexer.c"
    {

# 633 "lexer.c"
      read_token(tokenize_punctuation);
    }

# 638 "lexer.c"
    if ((token!=((void *)0)))

# 638 "lexer.c"
    {

# 639 "lexer.c"
      ((token->line_number)=line_number);

# 640 "lexer.c"
      ((token->column_number)=column_number);

# 641 "lexer.c"
      for (
        int i = (token->start);
        (i<(token->end));
        (i++))

# 641 "lexer.c"
      {
        uint8_t ch = buffer_get((token->buffer), i);

# 643 "lexer.c"
        if ((ch=='\n'))

# 643 "lexer.c"
        {

# 644 "lexer.c"
          (line_number++);

# 645 "lexer.c"
          (column_number=1);
        }
        else

# 646 "lexer.c"
        {

# 647 "lexer.c"
          (column_number++);
        }
      }
    }
  }

# 653 "lexer.c"
  ((result.tokens)=result_tokens);

# 654 "lexer.c"
  return result;
}


# 36 "token-transformer.c"
value_array_t* transform_tokens(value_array_t* tokens, token_transformer_options_t xform_options)
# 37 "token-transformer.c"
{
  value_array_t* result = make_value_array((tokens->length));

# 39 "token-transformer.c"
  for (
    int position = 0;
    (position<(tokens->length));
    (position++))

# 39 "token-transformer.c"
  {
    token_t* token = token_at(tokens, position);

# 41 "token-transformer.c"
    if (((token->is_cpp_token)&&(!(xform_options.keep_c_preprocessor_lines))))

# 41 "token-transformer.c"
    {

# 42 "token-transformer.c"
      continue;
    }

# 44 "token-transformer.c"
    if ((((token->type)==TOKEN_TYPE_WHITESPACE)&&(!(xform_options.keep_whitespace))))

# 45 "token-transformer.c"
    {

# 46 "token-transformer.c"
      continue;
    }

# 48 "token-transformer.c"
    if (((token->type)==TOKEN_TYPE_COMMENT))

# 48 "token-transformer.c"
    {

# 49 "token-transformer.c"
      if (token_starts_with(token, "/**"))

# 49 "token-transformer.c"
      {

# 50 "token-transformer.c"
        if ((!(xform_options.keep_javadoc_comments)))

# 50 "token-transformer.c"
        {

# 51 "token-transformer.c"
          continue;
        }
      }
      else

# 53 "token-transformer.c"
      if ((!(xform_options.keep_comments)))

# 53 "token-transformer.c"
      {

# 54 "token-transformer.c"
        continue;
      }
    }

# 58 "token-transformer.c"
    value_array_add(result, ptr_to_value(token));
  }

# 61 "token-transformer.c"
  return result;
}


# 6 "pstate.c"
boolean_t pstate_is_eof(pstate_t* pstate)
# 6 "pstate.c"
{

# 7 "pstate.c"
  return ((pstate->position)<((pstate->tokens)->length));
}


# 16 "pstate.c"
pstatus_t pstate_error(pstate_t* pstate, uint64_t saved_position, parse_error_code_t parse_error_code)
# 17 "pstate.c"
{

# 18 "pstate.c"
  ((pstate->result_token)=((void *)0));

# 19 "pstate.c"
  ((pstate->result_node)=((void *)0));

# 20 "pstate.c"
  (((pstate->error).parse_error_code)=parse_error_code);

# 21 "pstate.c"
  (((pstate->error).error_position)=(pstate->position));

# 22 "pstate.c"
  (((pstate->error).error_token)=token_at((pstate->tokens), (pstate->position)));

# 24 "pstate.c"
  ((pstate->position)=saved_position);

# 25 "pstate.c"
  return false;
}


# 41 "pstate.c"
pstate_t* pstate_ignore_error(pstate_t* pstate)
# 41 "pstate.c"
{

# 42 "pstate.c"
  ((pstate->error)=((compiler_error_t) {0}));

# 43 "pstate.c"
  return pstate;
}


# 53 "pstate.c"
pstatus_t pstate_propagate_error(pstate_t* pstate, uint64_t saved_position)
# 53 "pstate.c"
{

# 54 "pstate.c"
  ((pstate->position)=saved_position);

# 55 "pstate.c"
  if ((!((pstate->error).parse_error_code)))

# 55 "pstate.c"
  {

# 56 "pstate.c"
    fatal_error(ERROR_ILLEGAL_STATE);
  }

# 58 "pstate.c"
  return false;
}


# 67 "pstate.c"
pstatus_t pstate_set_result_token(pstate_t* pstate, token_t* token)
# 67 "pstate.c"
{

# 68 "pstate.c"
  ((pstate->error)=((compiler_error_t) {0}));

# 69 "pstate.c"
  ((pstate->result_node)=((void *)0));

# 70 "pstate.c"
  ((pstate->result_token)=token);

# 71 "pstate.c"
  return true;
}


# 80 "pstate.c"
pstatus_t pstate_set_result_node(pstate_t* pstate, parse_node_t* node)
# 80 "pstate.c"
{

# 81 "pstate.c"
  ((pstate->error)=((compiler_error_t) {0}));

# 82 "pstate.c"
  ((pstate->result_node)=node);

# 83 "pstate.c"
  ((pstate->result_token)=((void *)0));

# 84 "pstate.c"
  return true;
}


# 94 "pstate.c"
token_t* pstate_get_result_token(pstate_t* pstate)
# 94 "pstate.c"
{

# 95 "pstate.c"
  if ((((pstate->error).parse_error_code)!=PARSE_ERROR_UNKNOWN))

# 95 "pstate.c"
  {

# 96 "pstate.c"
    log_debug("error code is not zero");

# 97 "pstate.c"
    ((pstate->error)=((compiler_error_t) {0}));
  }
  token_t* token = (pstate->result_token);

# 101 "pstate.c"
  ((pstate->result_token)=((void *)0));

# 102 "pstate.c"
  return token;
}


# 111 "pstate.c"
parse_node_t* pstate_get_result_node(pstate_t* pstate)
# 111 "pstate.c"
{

# 112 "pstate.c"
  if ((((pstate->error).parse_error_code)!=PARSE_ERROR_UNKNOWN))

# 112 "pstate.c"
  {

# 113 "pstate.c"
    log_debug("error code is not zero");

# 114 "pstate.c"
    ((pstate->error)=((compiler_error_t) {0}));
  }
  parse_node_t* result = (pstate->result_node);

# 118 "pstate.c"
  if ((result==((void *)0)))

# 118 "pstate.c"
  {

# 119 "pstate.c"
    fatal_error(ERROR_ILLEGAL_STATE);
  }

# 121 "pstate.c"
  ((pstate->result_node)=((void *)0));

# 122 "pstate.c"
  return result;
}


# 131 "pstate.c"
parse_node_t* pstate_get_optional_result_node(pstate_t* pstate)
# 131 "pstate.c"
{

# 132 "pstate.c"
  ((pstate->error)=((compiler_error_t) {0}));
  parse_node_t* result = (pstate->result_node);

# 134 "pstate.c"
  ((pstate->result_node)=((void *)0));

# 135 "pstate.c"
  return result;
}


# 144 "pstate.c"
token_t* pstate_peek(pstate_t* pstate, int offset)
# 144 "pstate.c"
{

# 145 "pstate.c"
  return token_at((pstate->tokens), ((pstate->position)+offset));
}


# 155 "pstate.c"
token_t* pstate_advance(pstate_t* pstate)
# 155 "pstate.c"
{

# 156 "pstate.c"
  if (((pstate->error).parse_error_code))

# 156 "pstate.c"
  {

# 157 "pstate.c"
    log_debug("error code is not zero");

# 158 "pstate.c"
    ((pstate->error)=((compiler_error_t) {0}));
  }
  token_t* token = pstate_peek(pstate, 0);

# 162 "pstate.c"
  ((pstate->position)++);

# 163 "pstate.c"
  return token;
}


# 171 "pstate.c"
boolean_t pstate_match_token_string(pstate_t* pstate, char* token_string)
# 171 "pstate.c"
{
  token_t* token = pstate_peek(pstate, 0);

# 173 "pstate.c"
  return token_matches(token, token_string);
}


# 188 "pstate.c"
pstatus_t pstate_expect_token_string(pstate_t* pstate, char* token_string)
# 188 "pstate.c"
{
  token_t* token = pstate_peek(pstate, 0);

# 190 "pstate.c"
  if (token_matches(token, token_string))

# 190 "pstate.c"
  {

# 191 "pstate.c"
    ((pstate->result_node)=((void *)0));

# 192 "pstate.c"
    ((pstate->result_token)=token);

# 193 "pstate.c"
    ((pstate->position)+=1);

# 194 "pstate.c"
    return true;
  }

# 196 "pstate.c"
  (((pstate->error).parse_error_code)=PARSE_ERROR_EXPECTED_TOKEN);

# 197 "pstate.c"
  return false;
}


# 209 "pstate.c"
pstatus_t pstate_expect_token_type(pstate_t* pstate, token_type_t token_type)
# 209 "pstate.c"
{
  token_t* token = pstate_peek(pstate, 0);

# 211 "pstate.c"
  if ((token_type==(token->type)))

# 211 "pstate.c"
  {

# 212 "pstate.c"
    ((pstate->result_node)=((void *)0));

# 213 "pstate.c"
    ((pstate->result_token)=token);

# 214 "pstate.c"
    ((pstate->position)+=1);

# 215 "pstate.c"
    return true;
  }

# 217 "pstate.c"
  (((pstate->error).parse_error_code)=PARSE_ERROR_EXPECTED_TOKEN_TYPE);

# 218 "pstate.c"
  return false;
}


# 308 "declaration-parser.c"
pstatus_t parse_declarations(pstate_t* pstate)
# 308 "declaration-parser.c"
{
  uint64_t saved_position = (pstate->position);
  declarations_node_t* result = malloc_declarations();

# 311 "declaration-parser.c"
  while (((pstate->position)<((pstate->tokens)->length)))

# 311 "declaration-parser.c"
  {

# 312 "declaration-parser.c"
    if ((!parse_declaration(pstate)))

# 312 "declaration-parser.c"
    {

# 313 "declaration-parser.c"
      return pstate_propagate_error(pstate, saved_position);
    }

# 315 "declaration-parser.c"
    node_list_add_node((&(result->declarations)), pstate_get_result_node(pstate));
  }

# 317 "declaration-parser.c"
  return pstate_set_result_node(pstate, to_node(result));
}


# 323 "declaration-parser.c"
pstatus_t parse_declaration(pstate_t* pstate)
# 323 "declaration-parser.c"
{
  uint64_t saved_position = (pstate->position);

# 325 "declaration-parser.c"
  if (((((((parse_function_node(pstate)||parse_improved_typedef_node(pstate_ignore_error(pstate)))||parse_typedef_node(pstate_ignore_error(pstate)))||parse_enum_node_declaration(pstate_ignore_error(pstate)))||parse_variable_definition_node(pstate_ignore_error(pstate)))||parse_structure_node_declaration(pstate_ignore_error(pstate)))||parse_union_node_declaration(pstate_ignore_error(pstate))))

# 331 "declaration-parser.c"
  {

# 332 "declaration-parser.c"
    return true;
  }

# 334 "declaration-parser.c"
  return pstate_error(pstate, saved_position, PARSE_ERROR_UNRECOGNIZED_TOP_LEVEL_DECLARATION);
}


# 343 "declaration-parser.c"
pstatus_t parse_enum_node_declaration(pstate_t* pstate)
# 343 "declaration-parser.c"
{
  uint64_t saved_position = (pstate->position);

# 347 "declaration-parser.c"
  if ((!parse_enum_node(pstate)))

# 347 "declaration-parser.c"
  {

# 348 "declaration-parser.c"
    return pstate_propagate_error(pstate, saved_position);
  }
  parse_node_t* result = pstate_get_result_node(pstate);

# 351 "declaration-parser.c"
  if ((!pstate_expect_token_string(pstate, ";")))

# 351 "declaration-parser.c"
  {

# 352 "declaration-parser.c"
    return pstate_propagate_error(pstate, saved_position);
  }

# 355 "declaration-parser.c"
  return pstate_set_result_node(pstate, result);
}


# 363 "declaration-parser.c"
pstatus_t parse_structure_node_declaration(pstate_t* pstate)
# 363 "declaration-parser.c"
{
  uint64_t saved_position = (pstate->position);

# 367 "declaration-parser.c"
  if ((!parse_structure_node(pstate)))

# 367 "declaration-parser.c"
  {

# 368 "declaration-parser.c"
    return pstate_propagate_error(pstate, saved_position);
  }
  parse_node_t* result = pstate_get_result_node(pstate);

# 371 "declaration-parser.c"
  if ((!pstate_expect_token_string(pstate, ";")))

# 371 "declaration-parser.c"
  {

# 372 "declaration-parser.c"
    return pstate_propagate_error(pstate, saved_position);
  }

# 375 "declaration-parser.c"
  return pstate_set_result_node(pstate, result);
}


# 383 "declaration-parser.c"
pstatus_t parse_union_node_declaration(pstate_t* pstate)
# 383 "declaration-parser.c"
{
  uint64_t saved_position = (pstate->position);

# 387 "declaration-parser.c"
  if ((!parse_union_node(pstate)))

# 387 "declaration-parser.c"
  {

# 388 "declaration-parser.c"
    return pstate_propagate_error(pstate, saved_position);
  }
  parse_node_t* result = pstate_get_result_node(pstate);

# 391 "declaration-parser.c"
  if ((!pstate_expect_token_string(pstate, ";")))

# 391 "declaration-parser.c"
  {

# 392 "declaration-parser.c"
    return pstate_propagate_error(pstate, saved_position);
  }

# 395 "declaration-parser.c"
  return pstate_set_result_node(pstate, result);
}


# 403 "declaration-parser.c"
pstatus_t parse_attribute_node(pstate_t* pstate)
# 403 "declaration-parser.c"
{
  uint64_t saved_position = (pstate->position);

# 405 "declaration-parser.c"
  if (((!pstate_expect_token_string(pstate, "__attribute__"))||(!parse_balanced_construct(pstate))))

# 406 "declaration-parser.c"
  {

# 407 "declaration-parser.c"
    return pstate_propagate_error(pstate, saved_position);
  }
  attribute_node_t* result = malloc_attribute_node();

# 410 "declaration-parser.c"
  ((result->inner_start_token)=token_at((pstate->tokens), saved_position));

# 411 "declaration-parser.c"
  ((result->inner_end_token)=pstate_peek(pstate, (-1)));

# 412 "declaration-parser.c"
  return pstate_set_result_node(pstate, to_node(result));
}


# 421 "declaration-parser.c"
pstatus_t parse_function_node(pstate_t* pstate)
# 421 "declaration-parser.c"
{
  uint64_t saved_position = (pstate->position);
  token_t* storage_class_specifier = ((void *)0);
  token_list_t function_specifiers = {0};
  node_list_t attributes = {0};

# 428 "declaration-parser.c"
  while (1)

# 428 "declaration-parser.c"
  {

# 429 "declaration-parser.c"
    if ((((pstate_expect_token_string(pstate, "static")||pstate_expect_token_string(pstate_ignore_error(pstate), "extern"))||pstate_expect_token_string(pstate_ignore_error(pstate), "auto"))||pstate_expect_token_string(pstate_ignore_error(pstate), "register")))

# 433 "declaration-parser.c"
    {

# 434 "declaration-parser.c"
      if ((storage_class_specifier==((void *)0)))

# 434 "declaration-parser.c"
      {

# 435 "declaration-parser.c"
        (storage_class_specifier=pstate_get_result_token(pstate));
      }
      else

# 436 "declaration-parser.c"
      {

# 437 "declaration-parser.c"
        return pstate_error(pstate, saved_position, PARSE_ERROR_CONFLICTING_STORAGE_CLASS_SPECIFIER);
      }
    }
    else

# 440 "declaration-parser.c"
    if ((pstate_expect_token_string(pstate_ignore_error(pstate), "inline")||pstate_expect_token_string(pstate_ignore_error(pstate), "_Noreturn")))

# 442 "declaration-parser.c"
    {

# 443 "declaration-parser.c"
      token_list_add((&function_specifiers), pstate_get_result_token(pstate));
    }
    else

# 444 "declaration-parser.c"
    if (parse_attribute_node(pstate_ignore_error(pstate)))

# 444 "declaration-parser.c"
    {

# 445 "declaration-parser.c"
      node_list_add_node((&attributes), pstate_get_result_node(pstate));
    }
    else

# 446 "declaration-parser.c"
    {

# 447 "declaration-parser.c"
      pstate_ignore_error(pstate);

# 448 "declaration-parser.c"
      break;
    }
  }

# 452 "declaration-parser.c"
  if ((!parse_type_node(pstate)))

# 452 "declaration-parser.c"
  {

# 453 "declaration-parser.c"
    return pstate_propagate_error(pstate, saved_position);
  }
  type_node_t* return_type = to_type_node(pstate_get_result_node(pstate));

# 457 "declaration-parser.c"
  if ((!pstate_expect_token_type(pstate, TOKEN_TYPE_IDENTIFIER)))

# 457 "declaration-parser.c"
  {

# 458 "declaration-parser.c"
    return pstate_propagate_error(pstate, saved_position);
  }
  token_t* fn_name = pstate_get_result_token(pstate);

# 462 "declaration-parser.c"
  if ((!pstate_expect_token_string(pstate, "(")))

# 462 "declaration-parser.c"
  {

# 463 "declaration-parser.c"
    return pstate_propagate_error(pstate, saved_position);
  }
  function_node_t* fn_node = malloc_function_node();

# 467 "declaration-parser.c"
  ((fn_node->attributes)=attributes);

# 468 "declaration-parser.c"
  ((fn_node->storage_class_specifier)=storage_class_specifier);

# 469 "declaration-parser.c"
  ((fn_node->function_specifiers)=function_specifiers);

# 470 "declaration-parser.c"
  ((fn_node->return_type)=return_type);

# 471 "declaration-parser.c"
  ((fn_node->function_name)=fn_name);

# 473 "declaration-parser.c"
  while (parse_function_argument_node(pstate))

# 473 "declaration-parser.c"
  {

# 474 "declaration-parser.c"
    node_list_add_node((&(fn_node->function_args)), pstate_get_result_node(pstate));
  }

# 476 "declaration-parser.c"
  pstate_ignore_error(pstate);

# 478 "declaration-parser.c"
  if ((!pstate_expect_token_string(pstate, ")")))

# 478 "declaration-parser.c"
  {

# 479 "declaration-parser.c"
    return pstate_propagate_error(pstate, saved_position);
  }

# 482 "declaration-parser.c"
  if (parse_function_body_node(pstate))

# 482 "declaration-parser.c"
  {

# 483 "declaration-parser.c"
    ((fn_node->body)=pstate_get_result_node(pstate));
  }
  else

# 484 "declaration-parser.c"
  {

# 485 "declaration-parser.c"
    pstate_ignore_error(pstate);

# 486 "declaration-parser.c"
    if ((!pstate_expect_token_string(pstate, ";")))

# 486 "declaration-parser.c"
    {

# 487 "declaration-parser.c"
      return pstate_propagate_error(pstate, saved_position);
    }
  }

# 491 "declaration-parser.c"
  return pstate_set_result_node(pstate, to_node(fn_node));
}


# 499 "declaration-parser.c"
pstatus_t parse_function_argument_node(pstate_t* pstate)
# 499 "declaration-parser.c"
{
  uint64_t saved_position = (pstate->position);
  function_argument_node_t* result = malloc_function_argument_node();

# 503 "declaration-parser.c"
  if (pstate_match_token_string(pstate, "..."))

# 503 "declaration-parser.c"
  {

# 504 "declaration-parser.c"
    pstate_advance(pstate);

# 505 "declaration-parser.c"
    ((result->is_var_args)=true);
  }
  else

# 506 "declaration-parser.c"
  {

# 507 "declaration-parser.c"
    if ((!parse_type_node(pstate)))

# 507 "declaration-parser.c"
    {

# 508 "declaration-parser.c"
      return pstate_propagate_error(pstate, saved_position);
    }

# 510 "declaration-parser.c"
    ((result->arg_type)=to_type_node(pstate_get_result_node(pstate)));

# 511 "declaration-parser.c"
    if (pstate_expect_token_type(pstate, TOKEN_TYPE_IDENTIFIER))

# 511 "declaration-parser.c"
    {

# 512 "declaration-parser.c"
      ((result->arg_name)=pstate_get_result_token(pstate));
    }
    else

# 513 "declaration-parser.c"
    {

# 514 "declaration-parser.c"
      pstate_ignore_error(pstate);
    }
  }

# 517 "declaration-parser.c"
  if (pstate_match_token_string(pstate, ","))

# 517 "declaration-parser.c"
  {

# 518 "declaration-parser.c"
    pstate_advance(pstate);
  }

# 520 "declaration-parser.c"
  return pstate_set_result_node(pstate, to_node(result));
}


# 530 "declaration-parser.c"
pstatus_t parse_function_body_node(pstate_t* pstate)
# 530 "declaration-parser.c"
{

# 531 "declaration-parser.c"
  if ((pstate->use_statement_parser))

# 531 "declaration-parser.c"
  {

# 532 "declaration-parser.c"
    log_info("USING STATEMENT PARSER");

# 533 "declaration-parser.c"
    return parse_block(pstate);
  }
  else

# 534 "declaration-parser.c"
  {

# 535 "declaration-parser.c"
    return parse_balanced_construct(pstate);
  }
}


# 547 "declaration-parser.c"
pstatus_t parse_typedef_node(pstate_t* pstate)
# 547 "declaration-parser.c"
{
  uint64_t saved_position = (pstate->position);

# 549 "declaration-parser.c"
  if ((!pstate_expect_token_string(pstate, "typedef")))

# 549 "declaration-parser.c"
  {

# 550 "declaration-parser.c"
    return pstate_propagate_error(pstate, saved_position);
  }

# 552 "declaration-parser.c"
  if ((!parse_type_node(pstate)))

# 552 "declaration-parser.c"
  {

# 553 "declaration-parser.c"
    return pstate_propagate_error(pstate, saved_position);
  }
  type_node_t* type_node = to_type_node(pstate_get_result_node(pstate));

# 556 "declaration-parser.c"
  if ((!pstate_expect_token_type(pstate, TOKEN_TYPE_IDENTIFIER)))

# 556 "declaration-parser.c"
  {

# 557 "declaration-parser.c"
    return pstate_propagate_error(pstate, saved_position);
  }
  token_t* name = pstate_get_result_token(pstate);

# 560 "declaration-parser.c"
  if ((!pstate_expect_token_string(pstate, ";")))

# 560 "declaration-parser.c"
  {

# 561 "declaration-parser.c"
    return pstate_propagate_error(pstate, saved_position);
  }
  typedef_node_t* result = malloc_typedef_node();

# 565 "declaration-parser.c"
  ((result->type_node)=type_node);

# 566 "declaration-parser.c"
  ((result->name)=name);

# 567 "declaration-parser.c"
  return pstate_set_result_node(pstate, to_node(result));
}


# 575 "declaration-parser.c"
pstatus_t parse_improved_typedef_node(pstate_t* pstate)
# 575 "declaration-parser.c"
{
  uint64_t saved_position = (pstate->position);

# 577 "declaration-parser.c"
  if ((!pstate_expect_token_string(pstate, "typedef")))

# 577 "declaration-parser.c"
  {

# 578 "declaration-parser.c"
    return pstate_propagate_error(pstate, saved_position);
  }

# 580 "declaration-parser.c"
  if ((!pstate_expect_token_type(pstate, TOKEN_TYPE_IDENTIFIER)))

# 580 "declaration-parser.c"
  {

# 581 "declaration-parser.c"
    return pstate_propagate_error(pstate, saved_position);
  }
  token_t* name = pstate_get_result_token(pstate);

# 585 "declaration-parser.c"
  if ((!pstate_expect_token_string(pstate, "=")))

# 585 "declaration-parser.c"
  {

# 586 "declaration-parser.c"
    return pstate_propagate_error(pstate, saved_position);
  }

# 589 "declaration-parser.c"
  if ((!parse_type_node(pstate)))

# 589 "declaration-parser.c"
  {

# 590 "declaration-parser.c"
    return pstate_propagate_error(pstate, saved_position);
  }
  type_node_t* type_node = to_type_node(pstate_get_result_node(pstate));

# 594 "declaration-parser.c"
  if ((!pstate_expect_token_string(pstate, ";")))

# 594 "declaration-parser.c"
  {

# 595 "declaration-parser.c"
    return pstate_propagate_error(pstate, saved_position);
  }
  typedef_node_t* result = malloc_typedef_node();

# 599 "declaration-parser.c"
  ((result->type_node)=type_node);

# 600 "declaration-parser.c"
  ((result->name)=name);

# 601 "declaration-parser.c"
  return pstate_set_result_node(pstate, to_node(result));
}


# 17 "debug-printer.c"
void buffer_append_dbg_parse_node(cdl_printer_t* printer, parse_node_t* node)
# 17 "debug-printer.c"
{

# 18 "debug-printer.c"
  switch ((node->tag))

# 18 "debug-printer.c"
  {

# 19 "debug-printer.c"
    case PARSE_NODE_DECLARATIONS:

# 20 "debug-printer.c"
    buffer_append_dbg_declarations(printer, to_declarations_node(node));

# 21 "debug-printer.c"
    break;

# 23 "debug-printer.c"
    case PARSE_NODE_ENUM:

# 24 "debug-printer.c"
    buffer_append_dbg_enum(printer, to_enum_node(node));

# 25 "debug-printer.c"
    break;

# 27 "debug-printer.c"
    case PARSE_NODE_ENUM_ELEMENT:

# 28 "debug-printer.c"
    buffer_append_dbg_enum_element(printer, to_enum_element_node(node));

# 29 "debug-printer.c"
    break;

# 31 "debug-printer.c"
    case PARSE_NODE_STRUCT:

# 32 "debug-printer.c"
    buffer_append_dbg_struct_node(printer, to_struct_node(node));

# 33 "debug-printer.c"
    break;

# 35 "debug-printer.c"
    case PARSE_NODE_UNION:

# 36 "debug-printer.c"
    buffer_append_dbg_union_node(printer, to_union_node(node));

# 37 "debug-printer.c"
    break;

# 39 "debug-printer.c"
    case PARSE_NODE_FIELD:

# 40 "debug-printer.c"
    buffer_append_dbg_field_node(printer, to_field_node(node));

# 41 "debug-printer.c"
    break;

# 43 "debug-printer.c"
    case PARSE_NODE_TYPE:

# 44 "debug-printer.c"
    buffer_append_dbg_type_node(printer, to_type_node(node));

# 45 "debug-printer.c"
    break;

# 46 "debug-printer.c"
    ;

# 48 "debug-printer.c"
    case PARSE_NODE_LITERAL:

# 49 "debug-printer.c"
    buffer_append_dbg_literal_node(printer, to_literal_node(node));

# 50 "debug-printer.c"
    break;

# 52 "debug-printer.c"
    case PARSE_NODE_FUNCTION:

# 53 "debug-printer.c"
    buffer_append_dbg_function_node(printer, to_function_node(node));

# 54 "debug-printer.c"
    break;

# 56 "debug-printer.c"
    case PARSE_NODE_FUNCTION_ARGUMENT:

# 57 "debug-printer.c"
    buffer_append_dbg_function_argument_node(printer, to_function_argument_node(node));

# 59 "debug-printer.c"
    break;

# 61 "debug-printer.c"
    case PARSE_NODE_BALANCED_CONSTRUCT:

# 62 "debug-printer.c"
    buffer_append_dbg_balanced_construct_node(printer, to_balanced_construct_node(node));

# 64 "debug-printer.c"
    break;

# 66 "debug-printer.c"
    case PARSE_NODE_TYPEDEF:

# 67 "debug-printer.c"
    buffer_append_dbg_typedef_node(printer, to_typedef_node(node));

# 68 "debug-printer.c"
    break;

# 70 "debug-printer.c"
    case PARSE_NODE_VARIABLE_DEFINITION:

# 71 "debug-printer.c"
    buffer_append_dbg_variable_definition_node(printer, to_variable_definition_node(node));

# 73 "debug-printer.c"
    break;

# 75 "debug-printer.c"
    case PARSE_NODE_ATTRIBUTE:

# 76 "debug-printer.c"
    buffer_append_dbg_attribute_node(printer, to_attribute_node(node));

# 77 "debug-printer.c"
    break;

# 79 "debug-printer.c"
    case PARSE_NODE_IDENTIFIER:

# 80 "debug-printer.c"
    buffer_append_dbg_identifier_node(printer, to_identifier_node(node));

# 81 "debug-printer.c"
    break;

# 83 "debug-printer.c"
    case PARSE_NODE_OPERATOR:

# 84 "debug-printer.c"
    buffer_append_dbg_operator_node(printer, to_operator_node(node));

# 85 "debug-printer.c"
    break;

# 87 "debug-printer.c"
    case PARSE_NODE_BREAK_STATEMENT:

# 88 "debug-printer.c"
    buffer_append_dbg_break_statement_node(printer, to_break_statement_node(node));

# 90 "debug-printer.c"
    break;

# 92 "debug-printer.c"
    case PARSE_NODE_CONTINUE_STATEMENT:

# 93 "debug-printer.c"
    buffer_append_dbg_continue_statement_node(printer, to_continue_statement_node(node));

# 95 "debug-printer.c"
    break;

# 97 "debug-printer.c"
    case PARSE_NODE_LABEL_STATEMENT:

# 98 "debug-printer.c"
    buffer_append_dbg_label_statement_node(printer, to_label_statement_node(node));

# 100 "debug-printer.c"
    break;

# 102 "debug-printer.c"
    case PARSE_NODE_CASE_LABEL:

# 103 "debug-printer.c"
    buffer_append_dbg_case_label_node(printer, to_case_label_node(node));

# 104 "debug-printer.c"
    break;

# 106 "debug-printer.c"
    case PARSE_NODE_DEFAULT_LABEL:

# 107 "debug-printer.c"
    buffer_append_dbg_default_label_node(printer, to_default_label_node(node));

# 108 "debug-printer.c"
    break;

# 110 "debug-printer.c"
    case PARSE_NODE_CALL:

# 111 "debug-printer.c"
    buffer_append_dbg_call_node(printer, to_call_node(node));

# 112 "debug-printer.c"
    break;

# 114 "debug-printer.c"
    case PARSE_NODE_BLOCK:

# 115 "debug-printer.c"
    buffer_append_dbg_block_node(printer, to_block_node(node));

# 116 "debug-printer.c"
    break;

# 118 "debug-printer.c"
    case PARSE_NODE_WHILE_STATEMENT:

# 119 "debug-printer.c"
    buffer_append_dbg_while_node(printer, to_while_statement_node(node));

# 120 "debug-printer.c"
    break;

# 122 "debug-printer.c"
    case PARSE_NODE_FOR_STATEMENT:

# 123 "debug-printer.c"
    buffer_append_dbg_for_node(printer, to_for_statement_node(node));

# 124 "debug-printer.c"
    break;

# 126 "debug-printer.c"
    case PARSE_NODE_DO_STATEMENT:

# 127 "debug-printer.c"
    buffer_append_dbg_do_node(printer, to_do_statement_node(node));

# 128 "debug-printer.c"
    break;

# 130 "debug-printer.c"
    case PARSE_NODE_IF_STATEMENT:

# 131 "debug-printer.c"
    buffer_append_dbg_if_node(printer, to_if_statement_node(node));

# 132 "debug-printer.c"
    break;

# 134 "debug-printer.c"
    case PARSE_NODE_EMPTY_STATEMENT:

# 135 "debug-printer.c"
    buffer_append_dbg_empty_statement_node(printer, to_empty_statement_node(node));

# 137 "debug-printer.c"
    break;

# 139 "debug-printer.c"
    case PARSE_NODE_RETURN_STATEMENT:

# 140 "debug-printer.c"
    buffer_append_dbg_return_statement_node(printer, to_return_statement_node(node));

# 142 "debug-printer.c"
    break;

# 144 "debug-printer.c"
    case PARSE_NODE_EXPRESSION_STATEMENT:

# 145 "debug-printer.c"
    buffer_append_dbg_expression_statement_node(printer, to_expression_statement_node(node));

# 147 "debug-printer.c"
    break;

# 149 "debug-printer.c"
    case PARSE_NODE_SWITCH_STATEMENT:

# 150 "debug-printer.c"
    buffer_append_dbg_switch_node(printer, to_switch_statement_node(node));

# 151 "debug-printer.c"
    break;

# 153 "debug-printer.c"
    case PARSE_NODE_CONDITIONAL:

# 154 "debug-printer.c"
    buffer_append_dbg_conditional_node(printer, to_conditional_node(node));

# 155 "debug-printer.c"
    break;

# 157 "debug-printer.c"
    default:

# 158 "debug-printer.c"
    log_fatal("No debug printer for %s", parse_node_type_to_string((node->tag)));

# 159 "debug-printer.c"
    fatal_error(ERROR_ILLEGAL_STATE);

# 160 "debug-printer.c"
    break;
  }
}


# 169 "debug-printer.c"
void buffer_append_dbg_node_list(cdl_printer_t* printer, node_list_t list)
# 169 "debug-printer.c"
{

# 170 "debug-printer.c"
  cdl_start_array(printer);
  uint64_t length = node_list_length(list);

# 172 "debug-printer.c"
  for (
    uint64_t i = 0;
    (i<length);
    (i++))

# 172 "debug-printer.c"
  {

# 173 "debug-printer.c"
    buffer_append_dbg_parse_node(printer, node_list_get(list, i));
  }

# 175 "debug-printer.c"
  cdl_end_array(printer);
}


# 184 "debug-printer.c"
void buffer_append_dbg_tokens(cdl_printer_t* printer, value_array_t* tokens, char* field_name)
# 185 "debug-printer.c"
{

# 186 "debug-printer.c"
  cdl_key(printer, field_name);

# 187 "debug-printer.c"
  cdl_start_array(printer);
  uint64_t length = (tokens->length);

# 189 "debug-printer.c"
  for (
    uint64_t i = 0;
    (i<length);
    (i++))

# 189 "debug-printer.c"
  {
    token_t* token = value_array_get_ptr(tokens, i, typeof(token_t*));

# 191 "debug-printer.c"
    cdl_string(printer, token_to_string(token));
  }

# 193 "debug-printer.c"
  cdl_end_array(printer);
}


# 197 "debug-printer.c"
void buffer_append_dbg_declarations(cdl_printer_t* printer, declarations_node_t* node)
# 198 "debug-printer.c"
{

# 199 "debug-printer.c"
  buffer_append_dbg_node_list(printer, (node->declarations));
}


# 202 "debug-printer.c"
void buffer_append_dbg_enum(cdl_printer_t* printer, enum_node_t* node)
# 202 "debug-printer.c"
{

# 203 "debug-printer.c"
  cdl_start_table(printer);

# 204 "debug-printer.c"
  cdl_key(printer, "tag");

# 205 "debug-printer.c"
  cdl_string(printer, "PARSE_NODE_ENUM");

# 206 "debug-printer.c"
  if (((node->name)!=((void *)0)))

# 206 "debug-printer.c"
  {

# 207 "debug-printer.c"
    cdl_key(printer, "name");

# 208 "debug-printer.c"
    cdl_string(printer, token_to_string((node->name)));
  }

# 210 "debug-printer.c"
  cdl_key(printer, "elements");

# 211 "debug-printer.c"
  buffer_append_dbg_node_list(printer, (node->elements));

# 212 "debug-printer.c"
  cdl_key(printer, "partial_definition");

# 213 "debug-printer.c"
  cdl_boolean(printer, (node->partial_definition));

# 214 "debug-printer.c"
  cdl_end_table(printer);
}


# 217 "debug-printer.c"
void buffer_append_dbg_struct_node(cdl_printer_t* printer, struct_node_t* node)
# 218 "debug-printer.c"
{

# 219 "debug-printer.c"
  cdl_start_table(printer);

# 220 "debug-printer.c"
  cdl_key(printer, "tag");

# 221 "debug-printer.c"
  cdl_string(printer, "PARSE_NODE_STRUCT");

# 222 "debug-printer.c"
  if (((node->name)!=((void *)0)))

# 222 "debug-printer.c"
  {

# 223 "debug-printer.c"
    cdl_key(printer, "name");

# 224 "debug-printer.c"
    cdl_string(printer, token_to_string((node->name)));
  }

# 226 "debug-printer.c"
  cdl_key(printer, "partial_definition");

# 227 "debug-printer.c"
  cdl_boolean(printer, (node->partial_definition));

# 228 "debug-printer.c"
  cdl_key(printer, "fields");

# 229 "debug-printer.c"
  buffer_append_dbg_node_list(printer, (node->fields));

# 230 "debug-printer.c"
  cdl_end_table(printer);
}


# 233 "debug-printer.c"
void buffer_append_dbg_union_node(cdl_printer_t* printer, union_node_t* node)
# 233 "debug-printer.c"
{

# 234 "debug-printer.c"
  cdl_start_table(printer);

# 235 "debug-printer.c"
  cdl_key(printer, "tag");

# 236 "debug-printer.c"
  cdl_string(printer, "PARSE_NODE_UNION");

# 237 "debug-printer.c"
  if (((node->name)!=((void *)0)))

# 237 "debug-printer.c"
  {

# 238 "debug-printer.c"
    cdl_key(printer, "name");

# 239 "debug-printer.c"
    cdl_string(printer, token_to_string((node->name)));
  }

# 241 "debug-printer.c"
  cdl_key(printer, "partial_definition");

# 242 "debug-printer.c"
  cdl_boolean(printer, (node->partial_definition));

# 243 "debug-printer.c"
  cdl_key(printer, "fields");

# 244 "debug-printer.c"
  buffer_append_dbg_node_list(printer, (node->fields));

# 245 "debug-printer.c"
  cdl_end_table(printer);
}


# 248 "debug-printer.c"
void buffer_append_dbg_enum_element(cdl_printer_t* printer, enum_element_t* node)
# 249 "debug-printer.c"
{

# 250 "debug-printer.c"
  cdl_start_table(printer);

# 251 "debug-printer.c"
  cdl_key(printer, "tag");

# 252 "debug-printer.c"
  cdl_string(printer, "PARSE_NODE_ENUM_ELEMENT");

# 253 "debug-printer.c"
  if (((node->name)!=((void *)0)))

# 253 "debug-printer.c"
  {

# 254 "debug-printer.c"
    cdl_key(printer, "name");

# 255 "debug-printer.c"
    cdl_string(printer, token_to_string((node->name)));
  }

# 257 "debug-printer.c"
  if (((node->value_expr)!=((void *)0)))

# 257 "debug-printer.c"
  {

# 258 "debug-printer.c"
    cdl_key(printer, "value_expr");

# 259 "debug-printer.c"
    buffer_append_dbg_parse_node(printer, (node->value_expr));
  }

# 261 "debug-printer.c"
  cdl_end_table(printer);
}


# 264 "debug-printer.c"
void buffer_append_dbg_field_node(cdl_printer_t* printer, field_node_t* node)
# 264 "debug-printer.c"
{

# 265 "debug-printer.c"
  cdl_start_table(printer);

# 266 "debug-printer.c"
  cdl_key(printer, "tag");

# 267 "debug-printer.c"
  cdl_string(printer, "PARSE_NODE_FIELD");

# 268 "debug-printer.c"
  if (((node->name)!=((void *)0)))

# 268 "debug-printer.c"
  {

# 269 "debug-printer.c"
    cdl_key(printer, "name");

# 270 "debug-printer.c"
    cdl_string(printer, token_to_string((node->name)));
  }

# 272 "debug-printer.c"
  if (((node->type)!=((void *)0)))

# 272 "debug-printer.c"
  {

# 273 "debug-printer.c"
    cdl_key(printer, "type");

# 274 "debug-printer.c"
    buffer_append_dbg_type_node(printer, (node->type));
  }

# 276 "debug-printer.c"
  if (((node->suffixes)!=((void *)0)))

# 276 "debug-printer.c"
  {

# 277 "debug-printer.c"
    cdl_key(printer, "suffixes");

# 278 "debug-printer.c"
    cdl_start_array(printer);

# 279 "debug-printer.c"
    for (
      uint64_t i = 0;
      (i<((node->suffixes)->length));
      (i++))

# 279 "debug-printer.c"
    {
      parse_node_t* suffix = value_array_get_ptr((node->suffixes), i, typeof(parse_node_t*));

# 282 "debug-printer.c"
      buffer_append_dbg_parse_node(printer, suffix);
    }

# 284 "debug-printer.c"
    cdl_end_array(printer);
  }

# 287 "debug-printer.c"
  cdl_end_table(printer);
}


# 290 "debug-printer.c"
void buffer_append_dbg_type_node(cdl_printer_t* printer, type_node_t* node)
# 290 "debug-printer.c"
{

# 291 "debug-printer.c"
  cdl_start_table(printer);

# 292 "debug-printer.c"
  cdl_key(printer, "tag");

# 293 "debug-printer.c"
  cdl_string(printer, "PARSE_NODE_TYPE");

# 294 "debug-printer.c"
  cdl_key(printer, "type_node_kind");

# 295 "debug-printer.c"
  cdl_string(printer, type_node_kind_to_string((node->type_node_kind)));

# 299 "debug-printer.c"
  if (((node->qualifiers)>0))

# 299 "debug-printer.c"
  {

# 300 "debug-printer.c"
    cdl_key(printer, "qualifiers");

# 301 "debug-printer.c"
    cdl_start_array(printer);

# 302 "debug-printer.c"
    if ((((node->qualifiers)&TYPE_QUALIFIER_CONST)==TYPE_QUALIFIER_CONST))

# 302 "debug-printer.c"
    {

# 303 "debug-printer.c"
      cdl_string(printer, "const");
    }

# 305 "debug-printer.c"
    if ((((node->qualifiers)&TYPE_QUALIFIER_VOLATILE)==TYPE_QUALIFIER_VOLATILE))

# 306 "debug-printer.c"
    {

# 307 "debug-printer.c"
      cdl_string(printer, "volatile");
    }

# 309 "debug-printer.c"
    if ((((node->qualifiers)&TYPE_QUALIFIER_RESTRICT)==TYPE_QUALIFIER_RESTRICT))

# 310 "debug-printer.c"
    {

# 311 "debug-printer.c"
      cdl_string(printer, "restrict");
    }

# 313 "debug-printer.c"
    cdl_end_array(printer);
  }

# 316 "debug-printer.c"
  if (((node->type_name)!=((void *)0)))

# 316 "debug-printer.c"
  {

# 317 "debug-printer.c"
    cdl_key(printer, "type_name");

# 318 "debug-printer.c"
    cdl_string(printer, token_to_string((node->type_name)));
  }

# 320 "debug-printer.c"
  if (((node->user_type)!=((void *)0)))

# 320 "debug-printer.c"
  {

# 321 "debug-printer.c"
    cdl_key(printer, "user_type");

# 322 "debug-printer.c"
    buffer_append_dbg_parse_node(printer, (node->user_type));
  }

# 324 "debug-printer.c"
  cdl_key(printer, "type_args");

# 325 "debug-printer.c"
  buffer_append_dbg_node_list(printer, (node->type_args));

# 326 "debug-printer.c"
  cdl_end_table(printer);
}


# 329 "debug-printer.c"
void buffer_append_dbg_literal_node(cdl_printer_t* printer, literal_node_t* node)
# 330 "debug-printer.c"
{

# 331 "debug-printer.c"
  cdl_start_table(printer);

# 332 "debug-printer.c"
  cdl_key(printer, "tag");

# 333 "debug-printer.c"
  cdl_string(printer, "PARSE_NODE_LITERAL");

# 334 "debug-printer.c"
  if (((node->token)!=((void *)0)))

# 334 "debug-printer.c"
  {

# 335 "debug-printer.c"
    cdl_key(printer, "token");

# 336 "debug-printer.c"
    cdl_string(printer, token_to_string((node->token)));
  }

# 338 "debug-printer.c"
  if (((node->tokens)!=((void *)0)))

# 338 "debug-printer.c"
  {

# 339 "debug-printer.c"
    buffer_append_dbg_tokens(printer, (node->tokens), "tokens");
  }

# 341 "debug-printer.c"
  if (((node->initializer_node)!=((void *)0)))

# 341 "debug-printer.c"
  {

# 342 "debug-printer.c"
    cdl_key(printer, "initializer_node");

# 343 "debug-printer.c"
    buffer_append_dbg_parse_node(printer, (node->initializer_node));
  }

# 345 "debug-printer.c"
  if (((node->initializer_type)!=((void *)0)))

# 345 "debug-printer.c"
  {

# 346 "debug-printer.c"
    cdl_key(printer, "initializer_type");

# 347 "debug-printer.c"
    buffer_append_dbg_parse_node(printer, (node->initializer_type));
  }

# 349 "debug-printer.c"
  cdl_end_table(printer);
}


# 352 "debug-printer.c"
void buffer_append_dbg_function_node(cdl_printer_t* printer, function_node_t* node)
# 353 "debug-printer.c"
{

# 354 "debug-printer.c"
  cdl_start_table(printer);

# 355 "debug-printer.c"
  cdl_key(printer, "tag");

# 356 "debug-printer.c"
  cdl_string(printer, "PARSE_NODE_FUNCTION");

# 357 "debug-printer.c"
  cdl_key(printer, "attributes");

# 358 "debug-printer.c"
  buffer_append_dbg_node_list(printer, (node->attributes));

# 360 "debug-printer.c"
  if (((node->storage_class_specifier)!=((void *)0)))

# 360 "debug-printer.c"
  {

# 361 "debug-printer.c"
    cdl_key(printer, "storage_class_specifier");

# 362 "debug-printer.c"
    cdl_string(printer, token_to_string((node->storage_class_specifier)));
  }

# 367 "debug-printer.c"
  if (((node->return_type)!=((void *)0)))

# 367 "debug-printer.c"
  {

# 368 "debug-printer.c"
    cdl_key(printer, "return_type");

# 369 "debug-printer.c"
    buffer_append_dbg_type_node(printer, (node->return_type));
  }

# 372 "debug-printer.c"
  if (((node->function_name)!=((void *)0)))

# 372 "debug-printer.c"
  {

# 373 "debug-printer.c"
    cdl_key(printer, "function_name");

# 374 "debug-printer.c"
    cdl_string(printer, token_to_string((node->function_name)));
  }

# 377 "debug-printer.c"
  cdl_key(printer, "function_args");

# 378 "debug-printer.c"
  buffer_append_dbg_node_list(printer, (node->function_args));

# 379 "debug-printer.c"
  if (((node->body)!=((void *)0)))

# 379 "debug-printer.c"
  {

# 380 "debug-printer.c"
    cdl_key(printer, "body");

# 381 "debug-printer.c"
    buffer_append_dbg_parse_node(printer, (node->body));
  }

# 383 "debug-printer.c"
  cdl_end_table(printer);
}


# 386 "debug-printer.c"
void buffer_append_dbg_function_argument_node(cdl_printer_t* printer, function_argument_node_t* node)
# 387 "debug-printer.c"
{

# 388 "debug-printer.c"
  cdl_start_table(printer);

# 389 "debug-printer.c"
  cdl_key(printer, "tag");

# 390 "debug-printer.c"
  cdl_string(printer, "PARSE_NODE_FUNCTION_ARGUEMENT");

# 392 "debug-printer.c"
  if (((node->arg_type)!=((void *)0)))

# 392 "debug-printer.c"
  {

# 393 "debug-printer.c"
    cdl_key(printer, "arg_type");

# 394 "debug-printer.c"
    buffer_append_dbg_type_node(printer, (node->arg_type));
  }

# 396 "debug-printer.c"
  if (((node->arg_name)!=((void *)0)))

# 396 "debug-printer.c"
  {

# 397 "debug-printer.c"
    cdl_key(printer, "arg_name");

# 398 "debug-printer.c"
    cdl_string(printer, token_to_string((node->arg_name)));
  }

# 400 "debug-printer.c"
  cdl_end_table(printer);
}


# 403 "debug-printer.c"
void buffer_append_dbg_balanced_construct_node(cdl_printer_t* printer, balanced_construct_node_t* node)
# 404 "debug-printer.c"
{

# 405 "debug-printer.c"
  cdl_start_table(printer);

# 406 "debug-printer.c"
  cdl_key(printer, "tag");

# 407 "debug-printer.c"
  cdl_string(printer, "PARSE_NODE_BALANCED_CONSTRUCT");

# 408 "debug-printer.c"
  if (((node->start_token)!=((void *)0)))

# 408 "debug-printer.c"
  {

# 409 "debug-printer.c"
    cdl_key(printer, "start_token");

# 410 "debug-printer.c"
    cdl_string(printer, token_to_string((node->start_token)));
  }

# 412 "debug-printer.c"
  if (((node->end_token)!=((void *)0)))

# 412 "debug-printer.c"
  {

# 413 "debug-printer.c"
    cdl_key(printer, "end_token");

# 414 "debug-printer.c"
    cdl_string(printer, token_to_string((node->end_token)));
  }

# 416 "debug-printer.c"
  cdl_end_table(printer);
}


# 419 "debug-printer.c"
void buffer_append_dbg_typedef_node(cdl_printer_t* printer, typedef_node_t* node)
# 420 "debug-printer.c"
{

# 421 "debug-printer.c"
  cdl_start_table(printer);

# 422 "debug-printer.c"
  cdl_key(printer, "tag");

# 423 "debug-printer.c"
  cdl_string(printer, "PARSE_NODE_TYPEDEF");

# 424 "debug-printer.c"
  if (((node->name)!=((void *)0)))

# 424 "debug-printer.c"
  {

# 425 "debug-printer.c"
    cdl_key(printer, "name");

# 426 "debug-printer.c"
    cdl_string(printer, token_to_string((node->name)));
  }

# 428 "debug-printer.c"
  if (((node->type_node)!=((void *)0)))

# 428 "debug-printer.c"
  {

# 429 "debug-printer.c"
    cdl_key(printer, "type_node");

# 430 "debug-printer.c"
    buffer_append_dbg_type_node(printer, (node->type_node));
  }

# 432 "debug-printer.c"
  cdl_end_table(printer);
}


# 435 "debug-printer.c"
void buffer_append_dbg_variable_definition_node(cdl_printer_t* printer, variable_definition_node_t* node)
# 436 "debug-printer.c"
{

# 437 "debug-printer.c"
  cdl_start_table(printer);

# 438 "debug-printer.c"
  cdl_key(printer, "tag");

# 439 "debug-printer.c"
  cdl_string(printer, "PARSE_NODE_VARIABLE_DEFINITION");

# 440 "debug-printer.c"
  if (((node->name)!=((void *)0)))

# 440 "debug-printer.c"
  {

# 441 "debug-printer.c"
    cdl_key(printer, "name");

# 442 "debug-printer.c"
    cdl_string(printer, token_to_string((node->name)));
  }

# 444 "debug-printer.c"
  if (((node->type)!=((void *)0)))

# 444 "debug-printer.c"
  {

# 445 "debug-printer.c"
    cdl_key(printer, "type");

# 446 "debug-printer.c"
    buffer_append_dbg_type_node(printer, (node->type));
  }

# 448 "debug-printer.c"
  if (((node->value)!=((void *)0)))

# 448 "debug-printer.c"
  {

# 449 "debug-printer.c"
    cdl_key(printer, "value");

# 450 "debug-printer.c"
    buffer_append_dbg_parse_node(printer, (node->value));
  }

# 452 "debug-printer.c"
  if (((node->storage_class_specifier)!=((void *)0)))

# 452 "debug-printer.c"
  {

# 453 "debug-printer.c"
    cdl_key(printer, "storage_class_specifier");

# 454 "debug-printer.c"
    cdl_string(printer, token_to_string((node->storage_class_specifier)));
  }

# 456 "debug-printer.c"
  if (((node->suffixes)!=((void *)0)))

# 456 "debug-printer.c"
  {

# 457 "debug-printer.c"
    cdl_key(printer, "suffixes");

# 458 "debug-printer.c"
    cdl_start_array(printer);

# 459 "debug-printer.c"
    for (
      uint64_t i = 0;
      (i<((node->suffixes)->length));
      (i++))

# 459 "debug-printer.c"
    {
      parse_node_t* suffix = value_array_get_ptr((node->suffixes), i, typeof(parse_node_t*));

# 462 "debug-printer.c"
      buffer_append_dbg_parse_node(printer, suffix);
    }

# 464 "debug-printer.c"
    cdl_end_array(printer);
  }

# 466 "debug-printer.c"
  cdl_end_table(printer);
}


# 469 "debug-printer.c"
void buffer_append_dbg_attribute_node(cdl_printer_t* printer, attribute_node_t* node)
# 470 "debug-printer.c"
{

# 471 "debug-printer.c"
  cdl_start_table(printer);

# 472 "debug-printer.c"
  cdl_key(printer, "tag");

# 473 "debug-printer.c"
  cdl_string(printer, "PARSE_NODE_ATTRIBUTE");

# 474 "debug-printer.c"
  if (((node->inner_start_token)!=((void *)0)))

# 474 "debug-printer.c"
  {

# 475 "debug-printer.c"
    cdl_key(printer, "inner_start_token");

# 476 "debug-printer.c"
    cdl_string(printer, token_to_string((node->inner_start_token)));
  }

# 478 "debug-printer.c"
  if (((node->inner_end_token)!=((void *)0)))

# 478 "debug-printer.c"
  {

# 479 "debug-printer.c"
    cdl_key(printer, "inner_end_token");

# 480 "debug-printer.c"
    cdl_string(printer, token_to_string((node->inner_end_token)));
  }

# 482 "debug-printer.c"
  cdl_end_table(printer);
}


# 492 "debug-printer.c"
void buffer_append_dbg_empty_statement_node(cdl_printer_t* printer, empty_statement_node_t* node)
# 493 "debug-printer.c"
{

# 494 "debug-printer.c"
  cdl_start_table(printer);

# 495 "debug-printer.c"
  cdl_key(printer, "tag");

# 496 "debug-printer.c"
  cdl_string(printer, "PARSE_NODE_EMPTY_STATEMENT");

# 497 "debug-printer.c"
  cdl_end_table(printer);
}


# 503 "debug-printer.c"
void buffer_append_dbg_block_node(cdl_printer_t* printer, block_node_t* node)
# 503 "debug-printer.c"
{

# 504 "debug-printer.c"
  cdl_start_table(printer);

# 505 "debug-printer.c"
  cdl_key(printer, "tag");

# 506 "debug-printer.c"
  cdl_string(printer, "PARSE_NODE_BLOCK");

# 507 "debug-printer.c"
  cdl_key(printer, "statements");

# 508 "debug-printer.c"
  buffer_append_dbg_node_list(printer, (node->statements));

# 509 "debug-printer.c"
  cdl_end_table(printer);
}


# 512 "debug-printer.c"
void buffer_append_dbg_if_node(cdl_printer_t* printer, if_statement_node_t* node)
# 513 "debug-printer.c"
{

# 514 "debug-printer.c"
  cdl_start_table(printer);

# 515 "debug-printer.c"
  cdl_key(printer, "tag");

# 516 "debug-printer.c"
  cdl_string(printer, "PARSE_NODE_IF_STATEMENT");

# 517 "debug-printer.c"
  if (((node->if_condition)!=((void *)0)))

# 517 "debug-printer.c"
  {

# 518 "debug-printer.c"
    cdl_key(printer, "if_condition");

# 519 "debug-printer.c"
    buffer_append_dbg_parse_node(printer, (node->if_condition));
  }

# 521 "debug-printer.c"
  if (((node->if_true)!=((void *)0)))

# 521 "debug-printer.c"
  {

# 522 "debug-printer.c"
    cdl_key(printer, "if_true");

# 523 "debug-printer.c"
    buffer_append_dbg_parse_node(printer, (node->if_true));
  }

# 525 "debug-printer.c"
  if (((node->if_else)!=((void *)0)))

# 525 "debug-printer.c"
  {

# 526 "debug-printer.c"
    cdl_key(printer, "if_else");

# 527 "debug-printer.c"
    buffer_append_dbg_parse_node(printer, (node->if_else));
  }

# 529 "debug-printer.c"
  cdl_end_table(printer);
}


# 533 "debug-printer.c"
void buffer_append_dbg_while_node(cdl_printer_t* printer, while_statement_node_t* node)
# 534 "debug-printer.c"
{

# 535 "debug-printer.c"
  cdl_start_table(printer);

# 536 "debug-printer.c"
  cdl_key(printer, "tag");

# 537 "debug-printer.c"
  cdl_string(printer, "PARSE_NODE_WHILE_STATEMENT");

# 538 "debug-printer.c"
  if (((node->condition)!=((void *)0)))

# 538 "debug-printer.c"
  {

# 539 "debug-printer.c"
    cdl_key(printer, "condition");

# 540 "debug-printer.c"
    buffer_append_dbg_parse_node(printer, (node->condition));
  }

# 542 "debug-printer.c"
  if (((node->body)!=((void *)0)))

# 542 "debug-printer.c"
  {

# 543 "debug-printer.c"
    cdl_key(printer, "body");

# 544 "debug-printer.c"
    buffer_append_dbg_parse_node(printer, (node->body));
  }

# 546 "debug-printer.c"
  cdl_end_table(printer);
}


# 549 "debug-printer.c"
void buffer_append_dbg_for_node(cdl_printer_t* printer, for_statement_node_t* node)
# 550 "debug-printer.c"
{

# 551 "debug-printer.c"
  cdl_start_table(printer);

# 552 "debug-printer.c"
  cdl_key(printer, "tag");

# 553 "debug-printer.c"
  cdl_string(printer, "PARSE_NODE_FOR_STATEMENT");

# 554 "debug-printer.c"
  if (((node->for_init)!=((void *)0)))

# 554 "debug-printer.c"
  {

# 555 "debug-printer.c"
    cdl_key(printer, "for_init");

# 556 "debug-printer.c"
    buffer_append_dbg_parse_node(printer, (node->for_init));
  }

# 558 "debug-printer.c"
  if (((node->for_test)!=((void *)0)))

# 558 "debug-printer.c"
  {

# 559 "debug-printer.c"
    cdl_key(printer, "for_test");

# 560 "debug-printer.c"
    buffer_append_dbg_parse_node(printer, (node->for_test));
  }

# 562 "debug-printer.c"
  if (((node->for_increment)!=((void *)0)))

# 562 "debug-printer.c"
  {

# 563 "debug-printer.c"
    cdl_key(printer, "for_increment");

# 564 "debug-printer.c"
    buffer_append_dbg_parse_node(printer, (node->for_increment));
  }

# 566 "debug-printer.c"
  if (((node->for_body)!=((void *)0)))

# 566 "debug-printer.c"
  {

# 567 "debug-printer.c"
    cdl_key(printer, "for_body");

# 568 "debug-printer.c"
    buffer_append_dbg_parse_node(printer, (node->for_body));
  }

# 570 "debug-printer.c"
  cdl_end_table(printer);
}


# 573 "debug-printer.c"
void buffer_append_dbg_do_node(cdl_printer_t* printer, do_statement_node_t* node)
# 574 "debug-printer.c"
{

# 575 "debug-printer.c"
  cdl_start_table(printer);

# 576 "debug-printer.c"
  cdl_key(printer, "tag");

# 577 "debug-printer.c"
  cdl_string(printer, "PARSE_NODE_DO_STATEMENT");

# 578 "debug-printer.c"
  if (((node->body)!=((void *)0)))

# 578 "debug-printer.c"
  {

# 579 "debug-printer.c"
    cdl_key(printer, "body");

# 580 "debug-printer.c"
    buffer_append_dbg_parse_node(printer, (node->body));
  }

# 582 "debug-printer.c"
  if (((node->condition)!=((void *)0)))

# 582 "debug-printer.c"
  {

# 583 "debug-printer.c"
    cdl_key(printer, "condition");

# 584 "debug-printer.c"
    buffer_append_dbg_parse_node(printer, (node->condition));
  }

# 586 "debug-printer.c"
  cdl_end_table(printer);
}


# 589 "debug-printer.c"
void buffer_append_dbg_break_statement_node(cdl_printer_t* printer, break_statement_node_t* node)
# 590 "debug-printer.c"
{

# 591 "debug-printer.c"
  cdl_start_table(printer);

# 592 "debug-printer.c"
  cdl_key(printer, "tag");

# 593 "debug-printer.c"
  cdl_string(printer, "PARSE_NODE_BREAK_STATEMENT");

# 594 "debug-printer.c"
  if (((node->break_keyword_token)!=((void *)0)))

# 594 "debug-printer.c"
  {

# 595 "debug-printer.c"
    cdl_key(printer, "break_keyword_token");

# 596 "debug-printer.c"
    cdl_string(printer, token_to_string((node->break_keyword_token)));
  }

# 598 "debug-printer.c"
  cdl_end_table(printer);
}


# 601 "debug-printer.c"
void buffer_append_dbg_continue_statement_node(cdl_printer_t* printer, continue_statement_node_t* node)
# 602 "debug-printer.c"
{

# 603 "debug-printer.c"
  cdl_start_table(printer);

# 604 "debug-printer.c"
  cdl_key(printer, "tag");

# 605 "debug-printer.c"
  cdl_string(printer, "PARSE_NODE_CONTINUE_STATEMENT");

# 606 "debug-printer.c"
  if (((node->continue_keyword_token)!=((void *)0)))

# 606 "debug-printer.c"
  {

# 607 "debug-printer.c"
    cdl_key(printer, "continue_keyword_token");

# 608 "debug-printer.c"
    cdl_string(printer, token_to_string((node->continue_keyword_token)));
  }

# 610 "debug-printer.c"
  cdl_end_table(printer);
}


# 613 "debug-printer.c"
void buffer_append_dbg_label_statement_node(cdl_printer_t* printer, label_statement_node_t* node)
# 614 "debug-printer.c"
{

# 615 "debug-printer.c"
  cdl_start_table(printer);

# 616 "debug-printer.c"
  cdl_key(printer, "tag");

# 617 "debug-printer.c"
  cdl_string(printer, "PARSE_NODE_LABEL_STATEMENT");

# 618 "debug-printer.c"
  if (((node->label)!=((void *)0)))

# 618 "debug-printer.c"
  {

# 619 "debug-printer.c"
    cdl_key(printer, "label");

# 620 "debug-printer.c"
    cdl_string(printer, token_to_string((node->label)));
  }

# 622 "debug-printer.c"
  cdl_end_table(printer);
}


# 625 "debug-printer.c"
void buffer_append_dbg_case_label_node(cdl_printer_t* printer, case_label_node_t* node)
# 626 "debug-printer.c"
{

# 627 "debug-printer.c"
  cdl_start_table(printer);

# 628 "debug-printer.c"
  cdl_key(printer, "tag");

# 629 "debug-printer.c"
  cdl_string(printer, "PARSE_NODE_CASE_LABEL");

# 630 "debug-printer.c"
  if (((node->expression)!=((void *)0)))

# 630 "debug-printer.c"
  {

# 631 "debug-printer.c"
    cdl_key(printer, "expression");

# 632 "debug-printer.c"
    buffer_append_dbg_parse_node(printer, (node->expression));
  }

# 634 "debug-printer.c"
  cdl_end_table(printer);
}


# 637 "debug-printer.c"
void buffer_append_dbg_default_label_node(cdl_printer_t* printer, default_label_node_t* node)
# 638 "debug-printer.c"
{

# 639 "debug-printer.c"
  cdl_start_table(printer);

# 640 "debug-printer.c"
  cdl_key(printer, "tag");

# 641 "debug-printer.c"
  cdl_string(printer, "PARSE_NODE_DEFAULT_LABEL");

# 642 "debug-printer.c"
  cdl_end_table(printer);
}


# 645 "debug-printer.c"
void buffer_append_dbg_return_statement_node(cdl_printer_t* printer, return_statement_node_t* node)
# 646 "debug-printer.c"
{

# 647 "debug-printer.c"
  cdl_start_table(printer);

# 648 "debug-printer.c"
  cdl_key(printer, "tag");

# 649 "debug-printer.c"
  cdl_string(printer, "PARSE_NODE_RETURN_STATEMENT");

# 650 "debug-printer.c"
  if (((node->expression)!=((void *)0)))

# 650 "debug-printer.c"
  {

# 651 "debug-printer.c"
    cdl_key(printer, "expression");

# 652 "debug-printer.c"
    buffer_append_dbg_parse_node(printer, (node->expression));
  }

# 654 "debug-printer.c"
  cdl_end_table(printer);
}


# 657 "debug-printer.c"
void buffer_append_dbg_expression_statement_node(cdl_printer_t* printer, expression_statement_node_t* node)
# 658 "debug-printer.c"
{

# 659 "debug-printer.c"
  cdl_start_table(printer);

# 660 "debug-printer.c"
  cdl_key(printer, "tag");

# 661 "debug-printer.c"
  cdl_string(printer, "PARSE_NODE_EXPRESSION_STATEMENT");

# 662 "debug-printer.c"
  if (((node->expression)!=((void *)0)))

# 662 "debug-printer.c"
  {

# 663 "debug-printer.c"
    cdl_key(printer, "expression");

# 664 "debug-printer.c"
    buffer_append_dbg_parse_node(printer, (node->expression));
  }

# 666 "debug-printer.c"
  cdl_end_table(printer);
}


# 671 "debug-printer.c"
void buffer_append_dbg_identifier_node(cdl_printer_t* printer, identifier_node_t* node)
# 672 "debug-printer.c"
{

# 673 "debug-printer.c"
  cdl_start_table(printer);

# 674 "debug-printer.c"
  cdl_key(printer, "tag");

# 675 "debug-printer.c"
  cdl_string(printer, "PARSE_NODE_IDENTIFIER");

# 676 "debug-printer.c"
  cdl_key(printer, "token");

# 677 "debug-printer.c"
  cdl_string(printer, token_to_string((node->token)));

# 678 "debug-printer.c"
  cdl_end_table(printer);
}


# 681 "debug-printer.c"
void buffer_append_dbg_operator_node(cdl_printer_t* printer, operator_node_t* node)
# 682 "debug-printer.c"
{

# 683 "debug-printer.c"
  cdl_start_table(printer);

# 684 "debug-printer.c"
  cdl_key(printer, "tag");

# 685 "debug-printer.c"
  cdl_string(printer, "PARSE_NODE_OPERATOR");

# 686 "debug-printer.c"
  cdl_key(printer, "operator");

# 687 "debug-printer.c"
  cdl_string(printer, token_to_string((node->operator)));

# 688 "debug-printer.c"
  if (((node->left)!=((void *)0)))

# 688 "debug-printer.c"
  {

# 689 "debug-printer.c"
    cdl_key(printer, "left");

# 690 "debug-printer.c"
    buffer_append_dbg_parse_node(printer, (node->left));
  }

# 692 "debug-printer.c"
  if (((node->right)!=((void *)0)))

# 692 "debug-printer.c"
  {

# 693 "debug-printer.c"
    cdl_key(printer, "right");

# 694 "debug-printer.c"
    buffer_append_dbg_parse_node(printer, (node->right));
  }

# 696 "debug-printer.c"
  cdl_end_table(printer);
}


# 699 "debug-printer.c"
void buffer_append_dbg_call_node(cdl_printer_t* printer, call_node_t* node)
# 699 "debug-printer.c"
{

# 700 "debug-printer.c"
  cdl_start_table(printer);

# 701 "debug-printer.c"
  cdl_key(printer, "tag");

# 702 "debug-printer.c"
  cdl_string(printer, "PARSE_NODE_CALL");

# 703 "debug-printer.c"
  cdl_key(printer, "function");

# 704 "debug-printer.c"
  buffer_append_dbg_parse_node(printer, (node->function));

# 705 "debug-printer.c"
  cdl_key(printer, "args");

# 706 "debug-printer.c"
  buffer_append_dbg_node_list(printer, (node->args));

# 707 "debug-printer.c"
  cdl_end_table(printer);
}


# 710 "debug-printer.c"
void buffer_append_dbg_conditional_node(cdl_printer_t* printer, conditional_node_t* node)
# 711 "debug-printer.c"
{

# 712 "debug-printer.c"
  cdl_start_table(printer);

# 713 "debug-printer.c"
  cdl_key(printer, "tag");

# 714 "debug-printer.c"
  cdl_string(printer, "PARSE_NODE_CONDITIONAL");

# 715 "debug-printer.c"
  if (((node->condition)!=((void *)0)))

# 715 "debug-printer.c"
  {

# 716 "debug-printer.c"
    cdl_key(printer, "condition");

# 717 "debug-printer.c"
    buffer_append_dbg_parse_node(printer, (node->condition));
  }

# 719 "debug-printer.c"
  if (((node->expr_if_true)!=((void *)0)))

# 719 "debug-printer.c"
  {

# 720 "debug-printer.c"
    cdl_key(printer, "expr_if_true");

# 721 "debug-printer.c"
    buffer_append_dbg_parse_node(printer, (node->expr_if_true));
  }

# 723 "debug-printer.c"
  if (((node->expr_if_false)!=((void *)0)))

# 723 "debug-printer.c"
  {

# 724 "debug-printer.c"
    cdl_key(printer, "expr_if_false");

# 725 "debug-printer.c"
    buffer_append_dbg_parse_node(printer, (node->expr_if_false));
  }

# 727 "debug-printer.c"
  cdl_end_table(printer);
}


# 730 "debug-printer.c"
void buffer_append_dbg_switch_node(cdl_printer_t* printer, switch_statement_node_t* node)
# 731 "debug-printer.c"
{

# 732 "debug-printer.c"
  cdl_start_table(printer);

# 733 "debug-printer.c"
  cdl_key(printer, "tag");

# 734 "debug-printer.c"
  cdl_string(printer, "PARSE_NODE_SWITCH_STATEMENT");

# 735 "debug-printer.c"
  if (((node->expression)!=((void *)0)))

# 735 "debug-printer.c"
  {

# 736 "debug-printer.c"
    cdl_key(printer, "expression");

# 737 "debug-printer.c"
    buffer_append_dbg_parse_node(printer, (node->expression));
  }

# 739 "debug-printer.c"
  if (((node->block)!=((void *)0)))

# 739 "debug-printer.c"
  {

# 740 "debug-printer.c"
    cdl_key(printer, "block");

# 741 "debug-printer.c"
    buffer_append_dbg_parse_node(printer, (node->block));
  }

# 743 "debug-printer.c"
  cdl_end_table(printer);
}


# 756 "debug-printer.c"
void debug_append_tokens(buffer_t* buffer, value_array_t* tokens)
# 756 "debug-printer.c"
{

# 757 "debug-printer.c"
  for (
    int i = 0;
    (i<(tokens->length));
    (i++))

# 757 "debug-printer.c"
  {
    token_t* token = token_at(tokens, i);

# 759 "debug-printer.c"
    buffer_append_sub_buffer(buffer, (token->start), (token->end), (token->buffer));
  }
}


# 25 "c-file-printer.c"
printer_t* append_parse_node(printer_t* printer, parse_node_t* node)
# 25 "c-file-printer.c"
{

# 26 "c-file-printer.c"
  switch ((node->tag))

# 26 "c-file-printer.c"
  {

# 28 "c-file-printer.c"
    case PARSE_NODE_ENUM:

# 29 "c-file-printer.c"
    return append_enum_node(printer, to_enum_node(node));

# 31 "c-file-printer.c"
    case PARSE_NODE_STRUCT:

# 32 "c-file-printer.c"
    case PARSE_NODE_UNION:

# 33 "c-file-printer.c"
    return append_struct_node(printer, to_struct_node(node));

# 35 "c-file-printer.c"
    case PARSE_NODE_TYPE:

# 36 "c-file-printer.c"
    return append_type_node(printer, to_type_node(node));

# 38 "c-file-printer.c"
    case PARSE_NODE_LITERAL:

# 39 "c-file-printer.c"
    return append_literal_node(printer, to_literal_node(node));

# 41 "c-file-printer.c"
    case PARSE_NODE_IDENTIFIER:

# 42 "c-file-printer.c"
    return append_identifier_node(printer, to_identifier_node(node));

# 49 "c-file-printer.c"
    case PARSE_NODE_TYPEDEF:

# 50 "c-file-printer.c"
    return append_typedef_node(printer, to_typedef_node(node));

# 52 "c-file-printer.c"
    case PARSE_NODE_BLOCK:

# 53 "c-file-printer.c"
    return append_block_node(printer, to_block_node(node));

# 55 "c-file-printer.c"
    case PARSE_NODE_BALANCED_CONSTRUCT:

# 56 "c-file-printer.c"
    return append_balanced_construct_node(printer, to_balanced_construct_node(node));

# 62 "c-file-printer.c"
    case PARSE_NODE_VARIABLE_DEFINITION:

# 63 "c-file-printer.c"
    return append_variable_definition_node(printer, to_variable_definition_node(node), true);

# 66 "c-file-printer.c"
    case PARSE_NODE_WHILE_STATEMENT:

# 67 "c-file-printer.c"
    return append_while_statement_node(printer, to_while_statement_node(node));

# 69 "c-file-printer.c"
    case PARSE_NODE_FOR_STATEMENT:

# 70 "c-file-printer.c"
    return append_for_statement_node(printer, to_for_statement_node(node));

# 72 "c-file-printer.c"
    case PARSE_NODE_DO_STATEMENT:

# 73 "c-file-printer.c"
    return append_do_statement_node(printer, to_do_statement_node(node));

# 75 "c-file-printer.c"
    case PARSE_NODE_EMPTY_STATEMENT:

# 76 "c-file-printer.c"
    return append_empty_statement_node(printer, to_empty_statement_node(node));

# 78 "c-file-printer.c"
    case PARSE_NODE_IF_STATEMENT:

# 79 "c-file-printer.c"
    return append_if_statement_node(printer, to_if_statement_node(node));

# 81 "c-file-printer.c"
    case PARSE_NODE_RETURN_STATEMENT:

# 82 "c-file-printer.c"
    return append_return_statement_node(printer, to_return_statement_node(node));

# 85 "c-file-printer.c"
    case PARSE_NODE_BREAK_STATEMENT:

# 86 "c-file-printer.c"
    return append_break_statement_node(printer, to_break_statement_node(node));

# 88 "c-file-printer.c"
    case PARSE_NODE_CONTINUE_STATEMENT:

# 89 "c-file-printer.c"
    return append_continue_statement_node(printer, to_continue_statement_node(node));

# 92 "c-file-printer.c"
    case PARSE_NODE_LABEL_STATEMENT:

# 93 "c-file-printer.c"
    return append_label_statement_node(printer, to_label_statement_node(node));

# 95 "c-file-printer.c"
    case PARSE_NODE_CASE_LABEL:

# 96 "c-file-printer.c"
    return append_case_label_node(printer, to_case_label_node(node));

# 98 "c-file-printer.c"
    case PARSE_NODE_DEFAULT_LABEL:

# 99 "c-file-printer.c"
    return append_default_label_node(printer, to_default_label_node(node));

# 101 "c-file-printer.c"
    case PARSE_NODE_EXPRESSION_STATEMENT:

# 102 "c-file-printer.c"
    return append_expression_statement_node(printer, to_expression_statement_node(node));

# 105 "c-file-printer.c"
    case PARSE_NODE_SWITCH_STATEMENT:

# 106 "c-file-printer.c"
    return append_switch_statement_node(printer, to_switch_statement_node(node));

# 109 "c-file-printer.c"
    case PARSE_NODE_OPERATOR:

# 110 "c-file-printer.c"
    return append_operator_node(printer, to_operator_node(node));

# 112 "c-file-printer.c"
    case PARSE_NODE_CONDITIONAL:

# 113 "c-file-printer.c"
    return append_conditional_node(printer, to_conditional_node(node));

# 115 "c-file-printer.c"
    case PARSE_NODE_CALL:

# 116 "c-file-printer.c"
    return append_call_node(printer, to_call_node(node));

# 118 "c-file-printer.c"
    default:

# 119 "c-file-printer.c"
    break;
  }

# 121 "c-file-printer.c"
  log_fatal("No C file appender for %s", parse_node_type_to_string((node->tag)));

# 122 "c-file-printer.c"
  fatal_error(ERROR_ILLEGAL_STATE);
}


# 132 "c-file-printer.c"
printer_t* append_c_function_node_prefix(printer_t* printer, function_node_t* node)
# 133 "c-file-printer.c"
{

# 135 "c-file-printer.c"
  for (
    int i = 0;
    (i<node_list_length((node->attributes)));
    (i++))

# 135 "c-file-printer.c"
  {

# 136 "c-file-printer.c"
    append_c_attribute_node(printer, to_attribute_node(node_list_get((node->attributes), i)));

# 138 "c-file-printer.c"
    append_string(printer, " ");
  }

# 141 "c-file-printer.c"
  if (((node->storage_class_specifier)!=((void *)0)))

# 141 "c-file-printer.c"
  {

# 142 "c-file-printer.c"
    append_token(printer, (node->storage_class_specifier));

# 143 "c-file-printer.c"
    append_string(printer, " ");
  }

# 146 "c-file-printer.c"
  for (
    int i = 0;
    (i<token_list_length((node->function_specifiers)));
    (i++))

# 146 "c-file-printer.c"
  {

# 147 "c-file-printer.c"
    append_token(printer, token_list_get((node->function_specifiers), i));

# 148 "c-file-printer.c"
    append_string(printer, " ");
  }

# 151 "c-file-printer.c"
  append_type_node(printer, (node->return_type));

# 152 "c-file-printer.c"
  printer_space(printer);

# 153 "c-file-printer.c"
  append_token(printer, (node->function_name));

# 154 "c-file-printer.c"
  append_string(printer, "(");

# 156 "c-file-printer.c"
  for (
    int i = 0;
    (i<node_list_length((node->function_args)));
    (i++))

# 156 "c-file-printer.c"
  {

# 157 "c-file-printer.c"
    if ((i>0))

# 157 "c-file-printer.c"
    {

# 158 "c-file-printer.c"
      append_string(printer, ", ");
    }
    function_argument_node_t* arg_node = to_function_argument_node(node_list_get((node->function_args), i));

# 162 "c-file-printer.c"
    append_c_function_argument_node(printer, arg_node);
  }

# 164 "c-file-printer.c"
  append_string(printer, ")");

# 165 "c-file-printer.c"
  return printer;
}


# 168 "c-file-printer.c"
printer_t* append_c_function_node_prototype(printer_t* printer, function_node_t* node)
# 169 "c-file-printer.c"
{

# 170 "c-file-printer.c"
  append_c_function_node_prefix(printer, node);

# 171 "c-file-printer.c"
  append_string(printer, ";\n");

# 172 "c-file-printer.c"
  return printer;
}


# 175 "c-file-printer.c"
printer_t* append_balanced_construct_node(printer_t* printer, balanced_construct_node_t* node)
# 176 "c-file-printer.c"
{
  uint64_t start = ((node->start_token)->start);
  uint64_t end = ((node->end_token)->end);

# 179 "c-file-printer.c"
  buffer_append_sub_buffer((printer->buffer), start, end, ((node->start_token)->buffer));

# 181 "c-file-printer.c"
  return printer;
}


# 184 "c-file-printer.c"
printer_t* append_c_function_node_and_body(printer_t* printer, function_node_t* node)
# 185 "c-file-printer.c"
{

# 186 "c-file-printer.c"
  if ((printer->output_line_directives))

# 186 "c-file-printer.c"
  {

# 187 "c-file-printer.c"
    append_line_directive(printer, (node->function_name));
  }

# 189 "c-file-printer.c"
  append_c_function_node_prefix(printer, node);

# 190 "c-file-printer.c"
  append_parse_node(printer, (node->body));

# 191 "c-file-printer.c"
  printer_newline(printer);

# 192 "c-file-printer.c"
  return printer;
}


# 195 "c-file-printer.c"
printer_t* append_c_function_argument_node(printer_t* printer, function_argument_node_t* node)
# 196 "c-file-printer.c"
{

# 197 "c-file-printer.c"
  if ((node->is_var_args))

# 197 "c-file-printer.c"
  {

# 198 "c-file-printer.c"
    append_string(printer, "...");
  }
  else

# 199 "c-file-printer.c"
  {

# 200 "c-file-printer.c"
    append_type_node(printer, (node->arg_type));

# 201 "c-file-printer.c"
    if (((node->arg_name)!=((void *)0)))

# 201 "c-file-printer.c"
    {

# 202 "c-file-printer.c"
      printer_space(printer);

# 203 "c-file-printer.c"
      append_token(printer, (node->arg_name));
    }
  }

# 206 "c-file-printer.c"
  return printer;
}


# 214 "c-file-printer.c"
printer_t* append_type_node(printer_t* printer, type_node_t* node)
# 214 "c-file-printer.c"
{

# 216 "c-file-printer.c"
  if ((((node->qualifiers)&TYPE_QUALIFIER_CONST)==TYPE_QUALIFIER_CONST))

# 216 "c-file-printer.c"
  {

# 217 "c-file-printer.c"
    append_string(printer, "const ");
  }

# 219 "c-file-printer.c"
  if ((((node->qualifiers)&TYPE_QUALIFIER_VOLATILE)==TYPE_QUALIFIER_VOLATILE))

# 219 "c-file-printer.c"
  {

# 220 "c-file-printer.c"
    append_string(printer, "volatile ");
  }

# 222 "c-file-printer.c"
  if ((((node->qualifiers)&TYPE_QUALIFIER_RESTRICT)==TYPE_QUALIFIER_RESTRICT))

# 222 "c-file-printer.c"
  {

# 223 "c-file-printer.c"
    append_string(printer, "restrict ");
  }

# 226 "c-file-printer.c"
  switch ((node->type_node_kind))

# 226 "c-file-printer.c"
  {

# 227 "c-file-printer.c"
    case TYPE_NODE_KIND_POINTER:

# 228 "c-file-printer.c"
    append_type_node(printer, to_type_node(node_list_get((node->type_args), 0)));

# 229 "c-file-printer.c"
    append_string(printer, "*");

# 230 "c-file-printer.c"
    break;

# 232 "c-file-printer.c"
    case TYPE_NODE_KIND_PRIMITIVE_TYPENAME:

# 233 "c-file-printer.c"
    case TYPE_NODE_KIND_TYPENAME:

# 234 "c-file-printer.c"
    if (((node->type_name)!=((void *)0)))

# 234 "c-file-printer.c"
    {

# 235 "c-file-printer.c"
      append_token(printer, (node->type_name));
    }

# 237 "c-file-printer.c"
    break;

# 239 "c-file-printer.c"
    case TYPE_NODE_KIND_TYPE_EXPRESSION:

# 240 "c-file-printer.c"
    if (token_matches((node->type_name), "fn_t"))

# 240 "c-file-printer.c"
    {

# 241 "c-file-printer.c"
      append_fn_type_node(printer, node);
    }
    else

# 242 "c-file-printer.c"
    {

# 247 "c-file-printer.c"
      append_parse_node(printer, (node->user_type));
    }

# 249 "c-file-printer.c"
    break;

# 251 "c-file-printer.c"
    case TYPE_NODE_KIND_ARRAY:

# 252 "c-file-printer.c"
    append_string(printer, "typeof(");

# 253 "c-file-printer.c"
    append_type_node(printer, to_type_node(node_list_get((node->type_args), 0)));

# 254 "c-file-printer.c"
    append_string(printer, "[])");

# 255 "c-file-printer.c"
    break;

# 257 "c-file-printer.c"
    case TYPE_NODE_KIND_TYPEOF:

# 258 "c-file-printer.c"
    append_string(printer, "typeof(");

# 259 "c-file-printer.c"
    append_type_node(printer, to_type_node((node->user_type)));

# 260 "c-file-printer.c"
    append_string(printer, ")");

# 261 "c-file-printer.c"
    break;

# 263 "c-file-printer.c"
    default:

# 264 "c-file-printer.c"
    log_fatal("type_node_kind is not expected %s", type_node_kind_to_string((node->type_node_kind)));

# 266 "c-file-printer.c"
    fatal_error(ERROR_ILLEGAL_STATE);

# 267 "c-file-printer.c"
    break;
  }

# 270 "c-file-printer.c"
  return printer;
}


# 280 "c-file-printer.c"
printer_t* append_fn_type_node(printer_t* printer, type_node_t* node)
# 280 "c-file-printer.c"
{

# 281 "c-file-printer.c"
  append_token(printer, (node->type_name));

# 282 "c-file-printer.c"
  append_string(printer, "(");

# 283 "c-file-printer.c"
  for (
    int i = 0;
    (i<node_list_length((node->type_args)));
    (i++))

# 283 "c-file-printer.c"
  {

# 284 "c-file-printer.c"
    if ((i>0))

# 284 "c-file-printer.c"
    {

# 285 "c-file-printer.c"
      append_string(printer, ", ");
    }

# 287 "c-file-printer.c"
    append_parse_node(printer, node_list_get((node->type_args), i));
  }

# 289 "c-file-printer.c"
  append_string(printer, ")");

# 290 "c-file-printer.c"
  return printer;
}


# 293 "c-file-printer.c"
printer_t* append_c_attribute_node(printer_t* printer, attribute_node_t* node)
# 293 "c-file-printer.c"
{

# 306 "c-file-printer.c"
  append_c_raw_token_span(printer, (node->inner_start_token), (node->inner_end_token));

# 308 "c-file-printer.c"
  return printer;
}


# 317 "c-file-printer.c"
printer_t* append_c_raw_token_span(printer_t* printer, token_t* start_token, token_t* end_token)
# 318 "c-file-printer.c"
{

# 319 "c-file-printer.c"
  if (((start_token->buffer)!=(end_token->buffer)))

# 319 "c-file-printer.c"
  {

# 320 "c-file-printer.c"
    fatal_error(ERROR_ILLEGAL_STATE);
  }

# 322 "c-file-printer.c"
  buffer_append_sub_buffer((printer->buffer), (start_token->start), (end_token->end), (start_token->buffer));

# 324 "c-file-printer.c"
  return printer;
}


# 327 "c-file-printer.c"
printer_t* append_enum_node(printer_t* printer, enum_node_t* node)
# 327 "c-file-printer.c"
{

# 329 "c-file-printer.c"
  append_string(printer, "enum ");

# 330 "c-file-printer.c"
  if (((node->name)!=((void *)0)))

# 330 "c-file-printer.c"
  {

# 331 "c-file-printer.c"
    append_token(printer, (node->name));

# 332 "c-file-printer.c"
    printer_newline(printer);
  }

# 334 "c-file-printer.c"
  if ((node->partial_definition))

# 334 "c-file-printer.c"
  {

# 335 "c-file-printer.c"
    return printer;
  }

# 338 "c-file-printer.c"
  append_string(printer, "{\n");

# 339 "c-file-printer.c"
  printer_increase_indent(printer);

# 341 "c-file-printer.c"
  for (
    int i = 0;
    (i<node_list_length((node->elements)));
    (i++))

# 341 "c-file-printer.c"
  {

# 342 "c-file-printer.c"
    printer_indent(printer);

# 343 "c-file-printer.c"
    append_enum_element(printer, to_enum_element_node(node_list_get((node->elements), i)));

# 345 "c-file-printer.c"
    append_string(printer, ",\n");
  }

# 347 "c-file-printer.c"
  append_string(printer, "}");

# 348 "c-file-printer.c"
  printer_decrease_indent(printer);

# 350 "c-file-printer.c"
  return printer;
}


# 353 "c-file-printer.c"
printer_t* append_enum_element(printer_t* printer, enum_element_t* node)
# 353 "c-file-printer.c"
{

# 354 "c-file-printer.c"
  append_token(printer, (node->name));

# 355 "c-file-printer.c"
  if (((node->value_expr)!=((void *)0)))

# 355 "c-file-printer.c"
  {

# 356 "c-file-printer.c"
    append_string(printer, " = ");

# 357 "c-file-printer.c"
    append_parse_node(printer, (node->value_expr));
  }

# 359 "c-file-printer.c"
  return printer;
}


# 368 "c-file-printer.c"
printer_t* append_enum_to_string(printer_t* printer, enum_node_t* node, char* to_string_fn_prefix, char* type_string)
# 369 "c-file-printer.c"
{

# 370 "c-file-printer.c"
  append_string(printer, "char* ");

# 371 "c-file-printer.c"
  append_string(printer, to_string_fn_prefix);

# 372 "c-file-printer.c"
  append_string(printer, "_to_string(");

# 373 "c-file-printer.c"
  append_string(printer, type_string);

# 374 "c-file-printer.c"
  append_string(printer, " value) {\n");

# 375 "c-file-printer.c"
  printer_increase_indent(printer);

# 376 "c-file-printer.c"
  printer_indent(printer);

# 377 "c-file-printer.c"
  append_string(printer, "switch (value) {\n");

# 378 "c-file-printer.c"
  printer_indent(printer);

# 380 "c-file-printer.c"
  for (
    int i = 0;
    (i<node_list_length((node->elements)));
    (i++))

# 380 "c-file-printer.c"
  {
    enum_element_t* element = to_enum_element_node(node_list_get((node->elements), i));

# 383 "c-file-printer.c"
    printer_indent(printer);

# 384 "c-file-printer.c"
    append_string(printer, "case ");

# 385 "c-file-printer.c"
    append_token(printer, (element->name));

# 386 "c-file-printer.c"
    append_string(printer, ":\n");

# 387 "c-file-printer.c"
    printer_increase_indent(printer);

# 388 "c-file-printer.c"
    printer_indent(printer);

# 389 "c-file-printer.c"
    append_string(printer, "return \"");

# 390 "c-file-printer.c"
    append_token(printer, (element->name));

# 391 "c-file-printer.c"
    append_string(printer, "\";\n");

# 392 "c-file-printer.c"
    printer_decrease_indent(printer);
  }

# 395 "c-file-printer.c"
  printer_indent(printer);

# 396 "c-file-printer.c"
  append_string(printer, "default:\n");

# 398 "c-file-printer.c"
  printer_increase_indent(printer);

# 399 "c-file-printer.c"
  printer_indent(printer);

# 400 "c-file-printer.c"
  append_string(printer, "return \"<<unknown-");

# 401 "c-file-printer.c"
  append_string(printer, to_string_fn_prefix);

# 402 "c-file-printer.c"
  append_string(printer, ">>\";\n");

# 403 "c-file-printer.c"
  printer_decrease_indent(printer);

# 404 "c-file-printer.c"
  printer_indent(printer);

# 405 "c-file-printer.c"
  append_string(printer, "}\n");

# 406 "c-file-printer.c"
  printer_decrease_indent(printer);

# 407 "c-file-printer.c"
  append_string(printer, "}\n\n");

# 409 "c-file-printer.c"
  return printer;
}


# 418 "c-file-printer.c"
printer_t* append_string_to_enum(printer_t* printer, enum_node_t* node, char* to_string_fn_prefix, char* type_string)
# 419 "c-file-printer.c"
{

# 420 "c-file-printer.c"
  append_string(printer, type_string);

# 421 "c-file-printer.c"
  append_string(printer, " string_to_");

# 422 "c-file-printer.c"
  append_string(printer, to_string_fn_prefix);

# 423 "c-file-printer.c"
  append_string(printer, "(char* value) {\n");

# 424 "c-file-printer.c"
  printer_increase_indent(printer);

# 426 "c-file-printer.c"
  for (
    int i = 0;
    (i<node_list_length((node->elements)));
    (i++))

# 426 "c-file-printer.c"
  {
    enum_element_t* element = to_enum_element_node(node_list_get((node->elements), i));

# 429 "c-file-printer.c"
    printer_indent(printer);

# 430 "c-file-printer.c"
    append_string(printer, "if (strcmp(value, \"");

# 431 "c-file-printer.c"
    append_token(printer, (element->name));

# 432 "c-file-printer.c"
    append_string(printer, "\") == 0) {\n");

# 433 "c-file-printer.c"
    printer_increase_indent(printer);

# 434 "c-file-printer.c"
    printer_indent(printer);

# 435 "c-file-printer.c"
    append_string(printer, "return ");

# 436 "c-file-printer.c"
    append_token(printer, (element->name));

# 437 "c-file-printer.c"
    append_string(printer, ";\n");

# 438 "c-file-printer.c"
    printer_decrease_indent(printer);

# 439 "c-file-printer.c"
    printer_indent(printer);

# 440 "c-file-printer.c"
    append_string(printer, "}\n");
  }

# 442 "c-file-printer.c"
  printer_indent(printer);

# 443 "c-file-printer.c"
  append_string(printer, "return 0;\n");

# 444 "c-file-printer.c"
  printer_decrease_indent(printer);

# 445 "c-file-printer.c"
  append_string(printer, "}\n\n");

# 447 "c-file-printer.c"
  return printer;
}


# 450 "c-file-printer.c"
printer_t* append_field_node(printer_t* printer, field_node_t* node)
# 450 "c-file-printer.c"
{

# 451 "c-file-printer.c"
  append_type_node(printer, (node->type));

# 452 "c-file-printer.c"
  append_string(printer, " ");

# 453 "c-file-printer.c"
  if (((node->name)!=((void *)0)))

# 453 "c-file-printer.c"
  {

# 454 "c-file-printer.c"
    append_token(printer, (node->name));
  }

# 456 "c-file-printer.c"
  if ((node->suffixes))

# 456 "c-file-printer.c"
  {

# 457 "c-file-printer.c"
    for (
      int i = 0;
      (i<((node->suffixes)->length));
      (i++))

# 457 "c-file-printer.c"
    {

# 458 "c-file-printer.c"
      append_parse_node(printer, value_array_get_ptr((node->suffixes), i, typeof(parse_node_t*)));
    }
  }

# 464 "c-file-printer.c"
  return printer;
}


# 470 "c-file-printer.c"
printer_t* append_struct_node(printer_t* printer, struct_node_t* node)
# 470 "c-file-printer.c"
{

# 471 "c-file-printer.c"
  append_string(printer, (((node->tag)==PARSE_NODE_UNION) ? "union " : "struct "));

# 472 "c-file-printer.c"
  if (((node->name)!=((void *)0)))

# 472 "c-file-printer.c"
  {

# 473 "c-file-printer.c"
    append_token(printer, (node->name));
  }

# 476 "c-file-printer.c"
  if ((!(node->partial_definition)))

# 476 "c-file-printer.c"
  {

# 477 "c-file-printer.c"
    append_string(printer, " {\n");

# 478 "c-file-printer.c"
    printer_increase_indent(printer);

# 479 "c-file-printer.c"
    for (
      int i = 0;
      (i<node_list_length((node->fields)));
      (i++))

# 479 "c-file-printer.c"
    {

# 480 "c-file-printer.c"
      printer_indent(printer);

# 481 "c-file-printer.c"
      append_field_node(printer, to_field_node(node_list_get((node->fields), i)));

# 482 "c-file-printer.c"
      append_string(printer, ";\n");
    }

# 484 "c-file-printer.c"
    printer_decrease_indent(printer);

# 485 "c-file-printer.c"
    append_string(printer, "}");
  }

# 488 "c-file-printer.c"
  return printer;
}


# 494 "c-file-printer.c"
printer_t* append_typedef_node(printer_t* printer, typedef_node_t* node)
# 494 "c-file-printer.c"
{

# 495 "c-file-printer.c"
  append_string(printer, "typedef ");

# 496 "c-file-printer.c"
  append_type_node(printer, (node->type_node));

# 497 "c-file-printer.c"
  append_string(printer, " ");

# 498 "c-file-printer.c"
  append_token(printer, (node->name));

# 499 "c-file-printer.c"
  append_string(printer, ";\n");

# 500 "c-file-printer.c"
  return printer;
}


# 506 "c-file-printer.c"
printer_t* append_cpp_include_node(printer_t* printer, cpp_include_node_t* node)
# 507 "c-file-printer.c"
{

# 508 "c-file-printer.c"
  append_string(printer, (node->text));

# 509 "c-file-printer.c"
  return printer;
}


# 515 "c-file-printer.c"
printer_t* append_cpp_define_node(printer_t* printer, cpp_define_node_t* node)
# 515 "c-file-printer.c"
{

# 516 "c-file-printer.c"
  append_string(printer, (node->text));

# 517 "c-file-printer.c"
  return printer;
}


# 523 "c-file-printer.c"
printer_t* append_variable_definition_node(printer_t* printer, variable_definition_node_t* node, boolean_t is_library)
# 525 "c-file-printer.c"
{

# 526 "c-file-printer.c"
  printer_indent(printer);
  boolean_t is_header_file = (!is_library);

# 528 "c-file-printer.c"
  if (((node->storage_class_specifier)!=((void *)0)))

# 528 "c-file-printer.c"
  {

# 529 "c-file-printer.c"
    append_token(printer, (node->storage_class_specifier));

# 530 "c-file-printer.c"
    append_string(printer, " ");
  }
  else

# 531 "c-file-printer.c"
  if (is_header_file)

# 531 "c-file-printer.c"
  {

# 532 "c-file-printer.c"
    append_string(printer, "extern ");
  }

# 535 "c-file-printer.c"
  append_type_node(printer, (node->type));

# 536 "c-file-printer.c"
  append_string(printer, " ");

# 537 "c-file-printer.c"
  append_token(printer, (node->name));

# 538 "c-file-printer.c"
  if ((node->suffixes))

# 538 "c-file-printer.c"
  {

# 539 "c-file-printer.c"
    for (
      int i = 0;
      (i<((node->suffixes)->length));
      (i++))

# 539 "c-file-printer.c"
    {

# 540 "c-file-printer.c"
      append_parse_node(printer, value_array_get_ptr((node->suffixes), i, typeof(parse_node_t*)));
    }
  }

# 544 "c-file-printer.c"
  if ((is_library&&((node->value)!=((void *)0))))

# 544 "c-file-printer.c"
  {

# 545 "c-file-printer.c"
    append_string(printer, " = ");

# 546 "c-file-printer.c"
    append_parse_node(printer, (node->value));
  }

# 548 "c-file-printer.c"
  append_string(printer, ";\n");

# 549 "c-file-printer.c"
  return printer;
}


# 555 "c-file-printer.c"
printer_t* append_literal_node(printer_t* printer, literal_node_t* node)
# 555 "c-file-printer.c"
{

# 556 "c-file-printer.c"
  if (((node->token)!=((void *)0)))

# 556 "c-file-printer.c"
  {

# 557 "c-file-printer.c"
    append_token(printer, (node->token));
  }
  else

# 558 "c-file-printer.c"
  if (((node->initializer_node)!=((void *)0)))

# 558 "c-file-printer.c"
  {

# 559 "c-file-printer.c"
    if (((node->initializer_type)!=((void *)0)))

# 559 "c-file-printer.c"
    {

# 560 "c-file-printer.c"
      append_string(printer, "((");

# 561 "c-file-printer.c"
      append_parse_node(printer, (node->initializer_type));

# 562 "c-file-printer.c"
      append_string(printer, ") ");
    }

# 564 "c-file-printer.c"
    append_balanced_construct_node(printer, to_balanced_construct_node((node->initializer_node)));

# 566 "c-file-printer.c"
    if (((node->initializer_type)!=((void *)0)))

# 566 "c-file-printer.c"
    {

# 567 "c-file-printer.c"
      append_string(printer, ")");
    }
  }
  else

# 569 "c-file-printer.c"
  if ((((node->tokens)!=((void *)0))&&(((node->tokens)->length)>0)))

# 569 "c-file-printer.c"
  {

# 570 "c-file-printer.c"
    for (
      uint64_t i = 0;
      (i<((node->tokens)->length));
      (i++))

# 570 "c-file-printer.c"
    {

# 571 "c-file-printer.c"
      if ((i>0))

# 571 "c-file-printer.c"
      {

# 572 "c-file-printer.c"
        append_string(printer, " ");
      }
      token_t* token = value_array_get_ptr((node->tokens), i, typeof(token_t*));

# 575 "c-file-printer.c"
      append_token(printer, token);
    }
  }
  else

# 577 "c-file-printer.c"
  {

# 578 "c-file-printer.c"
    append_string(printer, "FIXME");
  }

# 580 "c-file-printer.c"
  return printer;
}


# 586 "c-file-printer.c"
printer_t* append_identifier_node(printer_t* printer, identifier_node_t* node)
# 586 "c-file-printer.c"
{

# 587 "c-file-printer.c"
  if (((node->token)==((void *)0)))

# 587 "c-file-printer.c"
  {

# 588 "c-file-printer.c"
    fatal_error(ERROR_ILLEGAL_STATE);
  }

# 590 "c-file-printer.c"
  if (token_matches((node->token), "nullptr"))

# 590 "c-file-printer.c"
  {

# 591 "c-file-printer.c"
    append_string(printer, "((void *)0)");
  }
  else

# 592 "c-file-printer.c"
  {

# 593 "c-file-printer.c"
    append_token(printer, (node->token));
  }

# 595 "c-file-printer.c"
  return printer;
}


# 605 "c-file-printer.c"
printer_t* append_empty_statement_node(printer_t* printer, empty_statement_node_t* node)
# 606 "c-file-printer.c"
{

# 607 "c-file-printer.c"
  if ((printer->output_line_directives))

# 607 "c-file-printer.c"
  {

# 608 "c-file-printer.c"
    append_line_directive(printer, (node->semi_colon_token));
  }

# 610 "c-file-printer.c"
  printer_indent(printer);

# 611 "c-file-printer.c"
  append_string(printer, ";\n");

# 612 "c-file-printer.c"
  return printer;
}


# 618 "c-file-printer.c"
printer_t* append_break_statement_node(printer_t* printer, break_statement_node_t* node)
# 619 "c-file-printer.c"
{

# 620 "c-file-printer.c"
  if ((printer->output_line_directives))

# 620 "c-file-printer.c"
  {

# 621 "c-file-printer.c"
    append_line_directive(printer, (node->break_keyword_token));
  }

# 623 "c-file-printer.c"
  printer_indent(printer);

# 624 "c-file-printer.c"
  append_string(printer, "break;\n");

# 625 "c-file-printer.c"
  return printer;
}


# 631 "c-file-printer.c"
printer_t* append_continue_statement_node(printer_t* printer, continue_statement_node_t* node)
# 632 "c-file-printer.c"
{

# 633 "c-file-printer.c"
  if ((printer->output_line_directives))

# 633 "c-file-printer.c"
  {

# 634 "c-file-printer.c"
    append_line_directive(printer, (node->continue_keyword_token));
  }

# 636 "c-file-printer.c"
  printer_indent(printer);

# 637 "c-file-printer.c"
  append_string(printer, "continue;\n");

# 638 "c-file-printer.c"
  return printer;
}


# 644 "c-file-printer.c"
printer_t* append_label_statement_node(printer_t* printer, label_statement_node_t* node)
# 645 "c-file-printer.c"
{

# 646 "c-file-printer.c"
  if ((printer->output_line_directives))

# 646 "c-file-printer.c"
  {

# 647 "c-file-printer.c"
    append_line_directive(printer, (node->label));
  }

# 649 "c-file-printer.c"
  printer_indent(printer);

# 650 "c-file-printer.c"
  append_token(printer, (node->label));

# 651 "c-file-printer.c"
  append_string(printer, ":\n");

# 652 "c-file-printer.c"
  return printer;
}


# 658 "c-file-printer.c"
printer_t* append_case_label_node(printer_t* printer, case_label_node_t* node)
# 658 "c-file-printer.c"
{

# 659 "c-file-printer.c"
  if ((printer->output_line_directives))

# 659 "c-file-printer.c"
  {

# 660 "c-file-printer.c"
    append_line_directive(printer, (node->first_token));
  }

# 662 "c-file-printer.c"
  printer_indent(printer);

# 663 "c-file-printer.c"
  append_string(printer, "case ");

# 664 "c-file-printer.c"
  append_parse_node(printer, (node->expression));

# 665 "c-file-printer.c"
  append_string(printer, ":\n");

# 666 "c-file-printer.c"
  return printer;
}


# 672 "c-file-printer.c"
printer_t* append_default_label_node(printer_t* printer, default_label_node_t* node)
# 673 "c-file-printer.c"
{

# 674 "c-file-printer.c"
  if ((printer->output_line_directives))

# 674 "c-file-printer.c"
  {

# 675 "c-file-printer.c"
    append_line_directive(printer, (node->default_token));
  }

# 677 "c-file-printer.c"
  printer_indent(printer);

# 678 "c-file-printer.c"
  append_string(printer, "default:\n");

# 679 "c-file-printer.c"
  return printer;
}


# 685 "c-file-printer.c"
printer_t* append_expression_statement_node(printer_t* printer, expression_statement_node_t* node)
# 686 "c-file-printer.c"
{

# 687 "c-file-printer.c"
  if ((printer->output_line_directives))

# 687 "c-file-printer.c"
  {

# 688 "c-file-printer.c"
    append_line_directive(printer, (node->first_token));
  }

# 690 "c-file-printer.c"
  printer_indent(printer);

# 691 "c-file-printer.c"
  append_parse_node(printer, (node->expression));

# 692 "c-file-printer.c"
  append_string(printer, ";\n");

# 693 "c-file-printer.c"
  return printer;
}


# 699 "c-file-printer.c"
printer_t* append_block_node(printer_t* printer, block_node_t* node)
# 699 "c-file-printer.c"
{

# 700 "c-file-printer.c"
  if ((printer->output_line_directives))

# 700 "c-file-printer.c"
  {

# 701 "c-file-printer.c"
    append_line_directive(printer, (node->first_token));
  }

# 703 "c-file-printer.c"
  printer_indent(printer);

# 704 "c-file-printer.c"
  append_string(printer, "{\n");

# 705 "c-file-printer.c"
  printer_increase_indent(printer);
  uint64_t length = node_list_length((node->statements));

# 707 "c-file-printer.c"
  for (
    uint64_t i = 0;
    (i<length);
    (i++))

# 707 "c-file-printer.c"
  {

# 708 "c-file-printer.c"
    append_parse_node(printer, node_list_get((node->statements), i));
  }

# 710 "c-file-printer.c"
  printer_decrease_indent(printer);

# 711 "c-file-printer.c"
  printer_indent(printer);

# 712 "c-file-printer.c"
  append_string(printer, "}\n");

# 713 "c-file-printer.c"
  return printer;
}


# 719 "c-file-printer.c"
printer_t* append_if_statement_node(printer_t* printer, if_statement_node_t* node)
# 720 "c-file-printer.c"
{

# 721 "c-file-printer.c"
  if ((printer->output_line_directives))

# 721 "c-file-printer.c"
  {

# 722 "c-file-printer.c"
    append_line_directive(printer, (node->first_token));
  }

# 724 "c-file-printer.c"
  printer_indent(printer);

# 725 "c-file-printer.c"
  append_string(printer, "if (");

# 726 "c-file-printer.c"
  append_parse_node(printer, (node->if_condition));

# 727 "c-file-printer.c"
  append_string(printer, ")\n");

# 728 "c-file-printer.c"
  append_parse_node(printer, (node->if_true));

# 729 "c-file-printer.c"
  if ((node->if_else))

# 729 "c-file-printer.c"
  {

# 730 "c-file-printer.c"
    printer_indent(printer);

# 731 "c-file-printer.c"
    append_string(printer, "else\n");

# 732 "c-file-printer.c"
    append_parse_node(printer, (node->if_else));
  }

# 734 "c-file-printer.c"
  return printer;
}


# 740 "c-file-printer.c"
printer_t* append_while_statement_node(printer_t* printer, while_statement_node_t* node)
# 741 "c-file-printer.c"
{

# 742 "c-file-printer.c"
  if ((printer->output_line_directives))

# 742 "c-file-printer.c"
  {

# 743 "c-file-printer.c"
    append_line_directive(printer, (node->first_token));
  }

# 745 "c-file-printer.c"
  printer_indent(printer);

# 746 "c-file-printer.c"
  append_string(printer, "while (");

# 747 "c-file-printer.c"
  append_parse_node(printer, (node->condition));

# 748 "c-file-printer.c"
  append_string(printer, ")\n");

# 749 "c-file-printer.c"
  append_parse_node(printer, (node->body));

# 750 "c-file-printer.c"
  return printer;
}


# 756 "c-file-printer.c"
printer_t* append_switch_statement_node(printer_t* printer, switch_statement_node_t* node)
# 757 "c-file-printer.c"
{

# 758 "c-file-printer.c"
  if ((printer->output_line_directives))

# 758 "c-file-printer.c"
  {

# 759 "c-file-printer.c"
    append_line_directive(printer, (node->first_token));
  }

# 761 "c-file-printer.c"
  printer_indent(printer);

# 762 "c-file-printer.c"
  append_string(printer, "switch (");

# 763 "c-file-printer.c"
  append_parse_node(printer, (node->expression));

# 764 "c-file-printer.c"
  append_string(printer, ")\n");

# 765 "c-file-printer.c"
  append_parse_node(printer, (node->block));

# 766 "c-file-printer.c"
  return printer;
}


# 772 "c-file-printer.c"
printer_t* append_for_statement_node(printer_t* printer, for_statement_node_t* node)
# 773 "c-file-printer.c"
{

# 774 "c-file-printer.c"
  if ((printer->output_line_directives))

# 774 "c-file-printer.c"
  {

# 775 "c-file-printer.c"
    append_line_directive(printer, (node->first_token));
  }

# 777 "c-file-printer.c"
  printer_indent(printer);

# 779 "c-file-printer.c"
  append_string(printer, "for (\n");

# 780 "c-file-printer.c"
  printer_increase_indent(printer);

# 783 "c-file-printer.c"
  if (((node->for_init)!=((void *)0)))

# 783 "c-file-printer.c"
  {

# 784 "c-file-printer.c"
    append_parse_node(printer, (node->for_init));
  }
  else

# 785 "c-file-printer.c"
  {

# 786 "c-file-printer.c"
    printer_indent(printer);

# 787 "c-file-printer.c"
    append_string(printer, ";\n");
  }

# 791 "c-file-printer.c"
  printer_indent(printer);

# 792 "c-file-printer.c"
  if (((node->for_test)!=((void *)0)))

# 792 "c-file-printer.c"
  {

# 793 "c-file-printer.c"
    append_parse_node(printer, (node->for_test));
  }

# 795 "c-file-printer.c"
  append_string(printer, ";");

# 796 "c-file-printer.c"
  printer_newline(printer);

# 799 "c-file-printer.c"
  printer_indent(printer);

# 800 "c-file-printer.c"
  if (((node->for_increment)!=((void *)0)))

# 800 "c-file-printer.c"
  {

# 801 "c-file-printer.c"
    append_parse_node(printer, (node->for_increment));
  }

# 803 "c-file-printer.c"
  append_string(printer, ")\n");

# 804 "c-file-printer.c"
  printer_decrease_indent(printer);

# 805 "c-file-printer.c"
  append_parse_node(printer, (node->for_body));

# 806 "c-file-printer.c"
  return printer;
}


# 812 "c-file-printer.c"
printer_t* append_do_statement_node(printer_t* printer, do_statement_node_t* node)
# 813 "c-file-printer.c"
{

# 814 "c-file-printer.c"
  if ((printer->output_line_directives))

# 814 "c-file-printer.c"
  {

# 815 "c-file-printer.c"
    append_line_directive(printer, (node->first_token));
  }

# 817 "c-file-printer.c"
  printer_indent(printer);

# 818 "c-file-printer.c"
  append_string(printer, "do");

# 819 "c-file-printer.c"
  append_parse_node(printer, (node->body));

# 820 "c-file-printer.c"
  printer_indent(printer);

# 821 "c-file-printer.c"
  append_string(printer, "while (");

# 822 "c-file-printer.c"
  append_parse_node(printer, (node->condition));

# 823 "c-file-printer.c"
  append_string(printer, ");\n");

# 824 "c-file-printer.c"
  return printer;
}


# 831 "c-file-printer.c"
printer_t* append_return_statement_node(printer_t* printer, return_statement_node_t* node)
# 832 "c-file-printer.c"
{

# 833 "c-file-printer.c"
  if ((printer->output_line_directives))

# 833 "c-file-printer.c"
  {

# 834 "c-file-printer.c"
    append_line_directive(printer, (node->first_token));
  }

# 836 "c-file-printer.c"
  printer_indent(printer);

# 837 "c-file-printer.c"
  append_string(printer, "return");

# 838 "c-file-printer.c"
  if (((node->expression)!=((void *)0)))

# 838 "c-file-printer.c"
  {

# 839 "c-file-printer.c"
    append_string(printer, " ");

# 840 "c-file-printer.c"
    append_parse_node(printer, (node->expression));
  }

# 842 "c-file-printer.c"
  append_string(printer, ";\n");

# 843 "c-file-printer.c"
  return printer;
}


# 849 "c-file-printer.c"
printer_t* append_operator_node(printer_t* printer, operator_node_t* node)
# 849 "c-file-printer.c"
{

# 850 "c-file-printer.c"
  if (token_matches((node->operator), "cast"))

# 850 "c-file-printer.c"
  {

# 851 "c-file-printer.c"
    append_string(printer, "(/*CAST*/(");

# 852 "c-file-printer.c"
    append_parse_node(printer, (node->left));

# 853 "c-file-printer.c"
    append_string(printer, ") ");

# 854 "c-file-printer.c"
    append_parse_node(printer, (node->right));

# 855 "c-file-printer.c"
    append_string(printer, ")");

# 856 "c-file-printer.c"
    return printer;
  }

# 859 "c-file-printer.c"
  if (token_matches((node->operator), "block_expr"))

# 859 "c-file-printer.c"
  {

# 860 "c-file-printer.c"
    return append_parse_node(printer, (node->left));
  }

# 863 "c-file-printer.c"
  if (token_matches((node->operator), "typeof"))

# 863 "c-file-printer.c"
  {

# 865 "c-file-printer.c"
    append_string(printer, "typeof(");

# 866 "c-file-printer.c"
    append_parse_node(printer, (node->left));

# 867 "c-file-printer.c"
    append_string(printer, ")");

# 868 "c-file-printer.c"
    return printer;
  }

# 871 "c-file-printer.c"
  append_string(printer, "(");

# 872 "c-file-printer.c"
  if (((node->left)!=((void *)0)))

# 872 "c-file-printer.c"
  {

# 873 "c-file-printer.c"
    append_parse_node(printer, (node->left));
  }

# 875 "c-file-printer.c"
  append_token(printer, (node->operator));

# 876 "c-file-printer.c"
  if (token_matches((node->operator), "sizeof"))

# 876 "c-file-printer.c"
  {

# 877 "c-file-printer.c"
    append_string(printer, "(");
  }

# 879 "c-file-printer.c"
  if (((node->right)!=((void *)0)))

# 879 "c-file-printer.c"
  {

# 880 "c-file-printer.c"
    append_parse_node(printer, (node->right));
  }

# 882 "c-file-printer.c"
  if (token_matches((node->operator), "sizeof"))

# 882 "c-file-printer.c"
  {

# 883 "c-file-printer.c"
    append_string(printer, ")");
  }
  else

# 884 "c-file-printer.c"
  if (token_matches((node->operator), "["))

# 884 "c-file-printer.c"
  {

# 885 "c-file-printer.c"
    append_string(printer, "]");
  }

# 887 "c-file-printer.c"
  append_string(printer, ")");

# 888 "c-file-printer.c"
  return printer;
}


# 894 "c-file-printer.c"
printer_t* append_conditional_node(printer_t* printer, conditional_node_t* node)
# 895 "c-file-printer.c"
{

# 896 "c-file-printer.c"
  append_string(printer, "(");

# 897 "c-file-printer.c"
  if (((node->condition)!=((void *)0)))

# 897 "c-file-printer.c"
  {

# 898 "c-file-printer.c"
    append_parse_node(printer, (node->condition));
  }

# 900 "c-file-printer.c"
  append_string(printer, " ? ");

# 901 "c-file-printer.c"
  if (((node->expr_if_true)!=((void *)0)))

# 901 "c-file-printer.c"
  {

# 902 "c-file-printer.c"
    append_parse_node(printer, (node->expr_if_true));
  }

# 904 "c-file-printer.c"
  append_string(printer, " : ");

# 905 "c-file-printer.c"
  if (((node->expr_if_false)!=((void *)0)))

# 905 "c-file-printer.c"
  {

# 906 "c-file-printer.c"
    append_parse_node(printer, (node->expr_if_false));
  }

# 908 "c-file-printer.c"
  append_string(printer, ")");

# 909 "c-file-printer.c"
  return printer;
}


# 915 "c-file-printer.c"
printer_t* append_call_node(printer_t* printer, call_node_t* node)
# 915 "c-file-printer.c"
{

# 916 "c-file-printer.c"
  append_parse_node(printer, (node->function));

# 917 "c-file-printer.c"
  append_string(printer, "(");

# 918 "c-file-printer.c"
  for (
    int i = 0;
    (i<node_list_length((node->args)));
    (i++))

# 918 "c-file-printer.c"
  {

# 919 "c-file-printer.c"
    if ((i>0))

# 919 "c-file-printer.c"
    {

# 920 "c-file-printer.c"
      append_string(printer, ", ");
    }

# 922 "c-file-printer.c"
    append_parse_node(printer, node_list_get((node->args), i));
  }

# 924 "c-file-printer.c"
  append_string(printer, ")");

# 925 "c-file-printer.c"
  return printer;
}


# 932 "c-file-printer.c"
buffer_t* buffer_append_enum_metadata(buffer_t* buffer, enum_node_t* node, char* fn_prefix, char* type_string)
# 933 "c-file-printer.c"
{
  char* code_template = "enum_metadata_t* ${fn_prefix}_metadata() {\n" "${element_constructions}" "    static enum_metadata_t enum_metadata_result = (enum_metadata_t) {\n" "        .name = \"${enum_name}\",\n" "        .elements = ${previous_var_address}\n" "    };\n" "    return &enum_metadata_result;\n" "}\n\n";
  char* field_template = "    static enum_element_metadata_t ${var_id} = (enum_element_metadata_t) {\n" "        .next = ${previous_var_address},\n" "        .name = \"${element_name}\",\n" "        .value = ${element_name}\n" "    };\n";
  buffer_t* element_constructions = make_buffer(128);
  buffer_t* buf = make_buffer(128);
  char* previous_var_address = "((void*)0)";

# 960 "c-file-printer.c"
  for (
    int i = 0;
    (i<node_list_length((node->elements)));
    (i++))

# 960 "c-file-printer.c"
  {
    enum_element_t* element = to_enum_element_node(node_list_get((node->elements), i));
    char* var_id = string_printf("var_%d", i);
    char* element_name = token_to_string((element->name));

# 966 "c-file-printer.c"
    buffer_clear(buf);

# 967 "c-file-printer.c"
    buffer_append_string(buf, field_template);

# 968 "c-file-printer.c"
    buffer_replace_all(buf, "${var_id}", var_id);

# 969 "c-file-printer.c"
    buffer_replace_all(buf, "${previous_var_address}", previous_var_address);

# 970 "c-file-printer.c"
    buffer_replace_all(buf, "${element_name}", element_name);

# 972 "c-file-printer.c"
    buffer_append_buffer(element_constructions, buf);

# 973 "c-file-printer.c"
    (previous_var_address=string_printf("&%s", var_id));
  }
  buffer_t* code = buffer_append_string(make_buffer(128), code_template);

# 977 "c-file-printer.c"
  buffer_replace_all(code, "${fn_prefix}", fn_prefix);

# 978 "c-file-printer.c"
  buffer_replace_all(code, "${enum_name}", type_string);

# 979 "c-file-printer.c"
  buffer_replace_all(code, "${previous_var_address}", previous_var_address);

# 980 "c-file-printer.c"
  buffer_replace_all(code, "${element_constructions}", buffer_to_c_string(element_constructions));

# 983 "c-file-printer.c"
  return buffer_append_buffer(buffer, code);
}


# 986 "c-file-printer.c"
printer_t* append_line_directive(printer_t* printer, token_t* token)
# 986 "c-file-printer.c"
{

# 987 "c-file-printer.c"
  if ((token==((void *)0)))

# 987 "c-file-printer.c"
  {

# 988 "c-file-printer.c"
    buffer_printf((printer->buffer), "\n// (no 'first token') provided)\n");

# 989 "c-file-printer.c"
    return printer;
  }

# 991 "c-file-printer.c"
  if (((printer->symbol_table)==((void *)0)))

# 991 "c-file-printer.c"
  {

# 992 "c-file-printer.c"
    log_fatal("printer->symbol_table is not set.");

# 993 "c-file-printer.c"
    fatal_error(ERROR_ILLEGAL_STATE);
  }
  file_t* file = symbol_table_token_to_file((printer->symbol_table), token);

# 996 "c-file-printer.c"
  if ((file!=((void *)0)))

# 996 "c-file-printer.c"
  {

# 997 "c-file-printer.c"
    buffer_printf((printer->buffer), "\n# %d \"%s\"\n", (token->line_number), ((file==((void *)0)) ? "fixme.c" : (file->file_name)));
  }

# 1000 "c-file-printer.c"
  return printer;
}


# 85 "symbol-table.c"
symbol_table_map_t* make_symbol_table_map(void)
# 85 "symbol-table.c"
{
  symbol_table_map_t* result = malloc_struct(symbol_table_map_t);

# 87 "symbol-table.c"
  ((result->ht)=make_string_hashtable(16));

# 88 "symbol-table.c"
  ((result->ordered_bindings)=make_value_array(16));

# 89 "symbol-table.c"
  return result;
}


# 92 "symbol-table.c"
symbol_table_t* make_symbol_table(void)
# 92 "symbol-table.c"
{
  symbol_table_t* result = malloc_struct(symbol_table_t);

# 94 "symbol-table.c"
  ((result->files)=make_value_array(16));

# 95 "symbol-table.c"
  ((result->system_includes)=make_value_array(16));

# 96 "symbol-table.c"
  ((result->user_includes)=make_value_array(16));

# 97 "symbol-table.c"
  ((result->defines)=make_value_array(16));

# 98 "symbol-table.c"
  ((result->enums)=make_symbol_table_map());

# 99 "symbol-table.c"
  ((result->typedefs)=make_symbol_table_map());

# 100 "symbol-table.c"
  ((result->structures)=make_symbol_table_map());

# 101 "symbol-table.c"
  ((result->variables)=make_symbol_table_map());

# 102 "symbol-table.c"
  ((result->functions)=make_symbol_table_map());

# 103 "symbol-table.c"
  return result;
}


# 111 "symbol-table.c"
symbol_table_binding_t* symbol_table_map_get(symbol_table_map_t* map, char* key_string)
# 112 "symbol-table.c"
{
  value_result_t result = string_ht_find((map->ht), key_string);

# 114 "symbol-table.c"
  if (is_ok(result))

# 114 "symbol-table.c"
  {

# 115 "symbol-table.c"
    return (/*CAST*/(symbol_table_binding_t*) (result.ptr));
  }

# 117 "symbol-table.c"
  return ((void *)0);
}


# 128 "symbol-table.c"
parse_node_t* symbol_table_map_get_only_definition(symbol_table_map_t* map, char* key_string)
# 129 "symbol-table.c"
{
  value_result_t result = string_ht_find((map->ht), key_string);

# 131 "symbol-table.c"
  if (is_ok(result))

# 131 "symbol-table.c"
  {
    symbol_table_binding_t* binding = (/*CAST*/(symbol_table_binding_t*) (result.ptr));

# 133 "symbol-table.c"
    if ((((binding->definition_nodes)->length)!=1))

# 133 "symbol-table.c"
    {

# 134 "symbol-table.c"
      fatal_error(ERROR_ILLEGAL_STATE);
    }

# 136 "symbol-table.c"
    return value_array_get_ptr((binding->definition_nodes), 0, typeof(parse_node_t*));
  }

# 139 "symbol-table.c"
  return ((void *)0);
}


# 142 "symbol-table.c"
void symbol_table_add_declaration_node(symbol_table_map_t* map, char* key_string, parse_node_t* node)
# 143 "symbol-table.c"
{
  value_result_t previous_binding = string_ht_find((map->ht), key_string);

# 145 "symbol-table.c"
  if (is_ok(previous_binding))

# 145 "symbol-table.c"
  {
    symbol_table_binding_t* binding = (/*CAST*/(symbol_table_binding_t*) (previous_binding.ptr));

# 148 "symbol-table.c"
    value_array_add((binding->definition_nodes), ptr_to_value(node));

# 149 "symbol-table.c"
    return;
  }
  symbol_table_binding_t* binding = malloc_struct(symbol_table_binding_t);

# 152 "symbol-table.c"
  ((binding->key_string)=key_string);

# 157 "symbol-table.c"
  ((binding->definition_nodes)=make_value_array(2));

# 158 "symbol-table.c"
  value_array_add((binding->definition_nodes), ptr_to_value(node));

# 159 "symbol-table.c"
  ((map->ht)=string_ht_insert((map->ht), (binding->key_string), ptr_to_value(binding)));

# 161 "symbol-table.c"
  value_array_add((map->ordered_bindings), ptr_to_value(binding));
}


# 170 "symbol-table.c"
void symbol_table_add_declartions(symbol_table_t* symbol_table, declarations_node_t* root)
# 171 "symbol-table.c"
{
  uint64_t length = node_list_length((root->declarations));

# 173 "symbol-table.c"
  for (
    uint64_t i = 0;
    (i<length);
    (i++))

# 173 "symbol-table.c"
  {
    parse_node_t* node = node_list_get((root->declarations), i);

# 175 "symbol-table.c"
    switch ((node->tag))

# 175 "symbol-table.c"
    {

# 177 "symbol-table.c"
      case PARSE_NODE_ENUM:

# 178 "symbol-table.c"
      symbol_table_add_declaration_node((symbol_table->enums), token_to_string((to_enum_node(node)->name)), node);

# 180 "symbol-table.c"
      break;

# 182 "symbol-table.c"
      case PARSE_NODE_FUNCTION:

# 183 "symbol-table.c"
      symbol_table_add_declaration_node((symbol_table->functions), token_to_string((to_function_node(node)->function_name)), node);

# 186 "symbol-table.c"
      break;

# 188 "symbol-table.c"
      case PARSE_NODE_VARIABLE_DEFINITION:

# 189 "symbol-table.c"
      symbol_table_add_declaration_node((symbol_table->variables), token_to_string((to_variable_definition_node(node)->name)), node);

# 192 "symbol-table.c"
      break;

# 194 "symbol-table.c"
      case PARSE_NODE_STRUCT:

# 195 "symbol-table.c"
      case PARSE_NODE_UNION:

# 196 "symbol-table.c"
      symbol_table_add_declaration_node((symbol_table->structures), token_to_string((to_struct_node(node)->name)), node);

# 199 "symbol-table.c"
      break;

# 201 "symbol-table.c"
      case PARSE_NODE_TYPEDEF:

# 202 "symbol-table.c"
      symbol_table_add_declaration_node((symbol_table->typedefs), token_to_string((to_typedef_node(node)->name)), node);

# 205 "symbol-table.c"
      break;

# 207 "symbol-table.c"
      default:

# 208 "symbol-table.c"
      fatal_error(ERROR_ILLEGAL_STATE);

# 209 "symbol-table.c"
      break;
    }
  }
}


# 214 "symbol-table.c"
buffer_t* symbol_table_stats(buffer_t* buffer, symbol_table_t* symbol_table)
# 214 "symbol-table.c"
{

# 215 "symbol-table.c"
  (buffer=buffer_printf(buffer, "Symbol Table Stats\n"));

# 216 "symbol-table.c"
  (buffer=buffer_printf(buffer, "#enums %d\n", string_ht_num_entries(((symbol_table->enums)->ht))));

# 218 "symbol-table.c"
  (buffer=buffer_printf(buffer, "#typedefs %d\n", string_ht_num_entries(((symbol_table->typedefs)->ht))));

# 220 "symbol-table.c"
  (buffer=buffer_printf(buffer, "#structures %d\n", string_ht_num_entries(((symbol_table->structures)->ht))));

# 222 "symbol-table.c"
  (buffer=buffer_printf(buffer, "#variables %d\n", string_ht_num_entries(((symbol_table->variables)->ht))));

# 224 "symbol-table.c"
  (buffer=buffer_printf(buffer, "#functions %d\n", string_ht_num_entries(((symbol_table->functions)->ht))));

# 226 "symbol-table.c"
  return buffer;
}


# 229 "symbol-table.c"
void buffer_append_dgb_binding(cdl_printer_t* printer, symbol_table_binding_t* binding)
# 230 "symbol-table.c"
{

# 231 "symbol-table.c"
  cdl_key(printer, (binding->key_string));

# 232 "symbol-table.c"
  buffer_append_dbg_parse_node(printer, value_array_get_ptr((binding->definition_nodes), 0, typeof(parse_node_t*)));
}


# 237 "symbol-table.c"
void buffer_appennd_dbg_symbol_table_map(cdl_printer_t* printer, symbol_table_map_t* symbol_table_map)
# 238 "symbol-table.c"
{

# 239 "symbol-table.c"
  cdl_start_table(printer);

# 240 "symbol-table.c"
  for (
    int i = 0;
    (i<((symbol_table_map->ordered_bindings)->length));
    (i++))

# 240 "symbol-table.c"
  {
    symbol_table_binding_t* binding = value_array_get_ptr((symbol_table_map->ordered_bindings), i, typeof(symbol_table_binding_t*));

# 243 "symbol-table.c"
    buffer_append_dgb_binding(printer, binding);
  }

# 245 "symbol-table.c"
  cdl_end_table(printer);
}


# 248 "symbol-table.c"
void buffer_append_dgb_symbol_table(cdl_printer_t* printer, symbol_table_t* symbol_table)
# 249 "symbol-table.c"
{

# 250 "symbol-table.c"
  cdl_start_table(printer);

# 252 "symbol-table.c"
  cdl_key(printer, "enumerations");

# 253 "symbol-table.c"
  buffer_appennd_dbg_symbol_table_map(printer, (symbol_table->enums));

# 255 "symbol-table.c"
  cdl_key(printer, "typedefs");

# 256 "symbol-table.c"
  buffer_appennd_dbg_symbol_table_map(printer, (symbol_table->typedefs));

# 258 "symbol-table.c"
  cdl_key(printer, "structures");

# 259 "symbol-table.c"
  buffer_appennd_dbg_symbol_table_map(printer, (symbol_table->structures));

# 261 "symbol-table.c"
  cdl_key(printer, "variables");

# 262 "symbol-table.c"
  buffer_appennd_dbg_symbol_table_map(printer, (symbol_table->variables));

# 264 "symbol-table.c"
  cdl_key(printer, "functions");

# 265 "symbol-table.c"
  buffer_appennd_dbg_symbol_table_map(printer, (symbol_table->functions));

# 267 "symbol-table.c"
  cdl_end_table(printer);
}


# 17 "source-to-source.c"
token_t* generate_struct_name_from_typedef_name(token_t* name)
# 17 "source-to-source.c"
{
  token_t* generated = make_derived_token(name);

# 19 "source-to-source.c"
  buffer_append_string((generated->buffer), "__generated_S");

# 20 "source-to-source.c"
  ((generated->end)=((generated->buffer)->length));

# 21 "source-to-source.c"
  return generated;
}


# 40 "source-to-source.c"
void split_structure_typedefs(symbol_table_t* symbol_table)
# 40 "source-to-source.c"
{

# 41 "source-to-source.c"
  for (
    int i = 0;
    (i<(((symbol_table->typedefs)->ordered_bindings)->length));
    (i++))

# 41 "source-to-source.c"
  {
    symbol_table_binding_t* binding = (/*CAST*/(symbol_table_binding_t*) (value_array_get(((symbol_table->typedefs)->ordered_bindings), i).ptr));

# 45 "source-to-source.c"
    if ((((binding->definition_nodes)->length)!=1))

# 45 "source-to-source.c"
    {

# 46 "source-to-source.c"
      fatal_error(ERROR_ILLEGAL_STATE);
    }
    typedef_node_t* node = to_typedef_node((/*CAST*/(parse_node_t*) (value_array_get((binding->definition_nodes), 0).ptr)));

# 50 "source-to-source.c"
    if (((((node->type_node)->type_node_kind)==TYPE_NODE_KIND_TYPE_EXPRESSION)&&is_struct_node(((node->type_node)->user_type))))

# 51 "source-to-source.c"
    {
      struct_node_t* struct_node = to_struct_node(((node->type_node)->user_type));

# 53 "source-to-source.c"
      if ((!(struct_node->partial_definition)))

# 53 "source-to-source.c"
      {

# 55 "source-to-source.c"
        if (((struct_node->name)==((void *)0)))

# 55 "source-to-source.c"
        {

# 56 "source-to-source.c"
          ((struct_node->name)=generate_struct_name_from_typedef_name((node->name)));
        }

# 60 "source-to-source.c"
        log_debug("Splitting %s off from %s", token_to_string((node->name)), token_to_string((struct_node->name)));
        struct_node_t* partial_definition = malloc_struct_node();

# 75 "source-to-source.c"
        ((partial_definition->partial_definition)=true);

# 76 "source-to-source.c"
        ((partial_definition->name)=(struct_node->name));

# 78 "source-to-source.c"
        (((node->type_node)->user_type)=(/*CAST*/(parse_node_t*) partial_definition));

# 79 "source-to-source.c"
        symbol_table_add_declaration_node((symbol_table->structures), token_to_string((struct_node->name)), (/*CAST*/(parse_node_t*) struct_node));
      }
    }
  }
}


# 129 "source-to-source.c"
void reorder_symbol_table_typedefs(symbol_table_t* symbol_table)
# 129 "source-to-source.c"
{
  value_array_t* bindings = ((symbol_table->typedefs)->ordered_bindings);
  value_array_t* reordered_bindings = make_value_array((bindings->length));

# 132 "source-to-source.c"
  for (
    int i = 0;
    (i<(bindings->length));
    (i++))

# 132 "source-to-source.c"
  {
    symbol_table_binding_t* binding = (/*CAST*/(symbol_table_binding_t*) (value_array_get(bindings, i).ptr));

# 135 "source-to-source.c"
    reorder_symbol_table_typedefs__process_binding((symbol_table->typedefs), binding, reordered_bindings);
  }

# 138 "source-to-source.c"
  (((symbol_table->typedefs)->ordered_bindings)=reordered_bindings);
}


# 141 "source-to-source.c"
void reorder_symbol_table_typedefs__process_binding(symbol_table_map_t* typedefs, symbol_table_binding_t* binding, value_array_t* reordered_bindings)
# 143 "source-to-source.c"
{

# 144 "source-to-source.c"
  log_debug("processing binding %s", (binding->key_string));

# 145 "source-to-source.c"
  if ((!(binding->visited)))

# 145 "source-to-source.c"
  {

# 146 "source-to-source.c"
    if ((((binding->definition_nodes)->length)!=1))

# 146 "source-to-source.c"
    {

# 147 "source-to-source.c"
      fatal_error(ERROR_ILLEGAL_STATE);
    }
    parse_node_t* node = (/*CAST*/(parse_node_t*) (value_array_get((binding->definition_nodes), 0).ptr));
    typedef_node_t* typedef_node = to_typedef_node(node);
    type_node_t* type_node = (typedef_node->type_node);

# 154 "source-to-source.c"
    while (((type_node->type_node_kind)==TYPE_NODE_KIND_POINTER))

# 154 "source-to-source.c"
    {

# 155 "source-to-source.c"
      (type_node=to_type_node(node_list_get((type_node->type_args), 0)));
    }

# 157 "source-to-source.c"
    if (((type_node->type_node_kind)==TYPE_NODE_KIND_TYPENAME))

# 157 "source-to-source.c"
    {
      char* type_name = token_to_string((type_node->type_name));
      symbol_table_binding_t* dependent_binding = symbol_table_map_get(typedefs, type_name);

# 161 "source-to-source.c"
      if ((dependent_binding!=((void *)0)))

# 161 "source-to-source.c"
      {

# 162 "source-to-source.c"
        reorder_symbol_table_typedefs__process_binding(typedefs, dependent_binding, reordered_bindings);
      }
    }
    else

# 165 "source-to-source.c"
    if (((type_node->type_node_kind)==TYPE_NODE_KIND_TYPE_EXPRESSION))

# 165 "source-to-source.c"
    {
    }
    else

# 171 "source-to-source.c"
    if (((type_node->type_node_kind)!=TYPE_NODE_KIND_PRIMITIVE_TYPENAME))

# 171 "source-to-source.c"
    {

# 172 "source-to-source.c"
      fatal_error(ERROR_ILLEGAL_STATE);
    }

# 175 "source-to-source.c"
    log_debug("adding binding %s at position %d", (binding->key_string), (reordered_bindings->length));

# 178 "source-to-source.c"
    value_array_add(reordered_bindings, ptr_to_value(binding));

# 179 "source-to-source.c"
    ((binding->visited)=true);
  }
}


# 188 "source-to-source.c"
struct_node_t* get_full_structure_definition_node(symbol_table_binding_t* binding)
# 188 "source-to-source.c"
{

# 189 "source-to-source.c"
  for (
    uint64_t i = 0;
    (i<((binding->definition_nodes)->length));
    (i++))

# 189 "source-to-source.c"
  {
    parse_node_t* node = (/*CAST*/(parse_node_t*) (value_array_get((binding->definition_nodes), i).ptr));
    struct_node_t* structure_node = to_struct_node(node);

# 193 "source-to-source.c"
    if ((!(structure_node->partial_definition)))

# 193 "source-to-source.c"
    {

# 194 "source-to-source.c"
      return structure_node;
    }
  }

# 197 "source-to-source.c"
  return ((void *)0);
}


# 201 "source-to-source.c"
symbol_table_binding_t* resolve_typename_to_structure_binding(symbol_table_t* symbol_table, type_node_t* type_node)
# 202 "source-to-source.c"
{

# 203 "source-to-source.c"
  if (((type_node->type_node_kind)==TYPE_NODE_KIND_POINTER))

# 203 "source-to-source.c"
  {

# 204 "source-to-source.c"
    log_debug("resolve_typename_to_structure_binding -- not looking through pointers " "%p", type_node);

# 208 "source-to-source.c"
    return ((void *)0);
  }

# 211 "source-to-source.c"
  if (((type_node->type_node_kind)==TYPE_NODE_KIND_TYPE_EXPRESSION))

# 211 "source-to-source.c"
  {
    parse_node_t* user_type = (type_node->user_type);

# 213 "source-to-source.c"
    if (is_struct_node(user_type))

# 213 "source-to-source.c"
    {
      struct_node_t* struct_node = to_struct_node(user_type);

# 215 "source-to-source.c"
      if (((struct_node->name)!=((void *)0)))

# 215 "source-to-source.c"
      {
        char* key_name = token_to_string((struct_node->name));
        symbol_table_binding_t* binding = symbol_table_map_get((symbol_table->structures), key_name);
        buffer_t* buffer = make_buffer(10);

# 220 "source-to-source.c"
        buffer_append_dgb_binding(make_cdl_printer(buffer), binding);
        char* dbg_binding = buffer_to_c_string(buffer);

# 222 "source-to-source.c"
        log_debug("resolve_typename_to_structure_binding -- returning binding %p %s", binding, dbg_binding);

# 225 "source-to-source.c"
        return binding;
      }
    }

# 229 "source-to-source.c"
    return ((void *)0);
  }
  char* key_string = token_to_string((type_node->type_name));

# 234 "source-to-source.c"
  log_debug("resolve_typename_to_structure_binding -- %d %s", (type_node->tag), key_string);
  symbol_table_binding_t* typedef_binding = symbol_table_map_get((symbol_table->typedefs), key_string);

# 238 "source-to-source.c"
  if ((typedef_binding!=((void *)0)))

# 238 "source-to-source.c"
  {

# 239 "source-to-source.c"
    if ((((typedef_binding->definition_nodes)->length)!=1))

# 239 "source-to-source.c"
    {

# 240 "source-to-source.c"
      fatal_error(ERROR_ILLEGAL_STATE);
    }
    typedef_node_t* typedef_node = to_typedef_node(value_array_get_ptr((typedef_binding->definition_nodes), 0, typeof(parse_node_t*)));

# 244 "source-to-source.c"
    return resolve_typename_to_structure_binding(symbol_table, (typedef_node->type_node));
  }
  else

# 246 "source-to-source.c"
  {

# 247 "source-to-source.c"
    return symbol_table_map_get((symbol_table->structures), key_string);
  }
}


# 251 "source-to-source.c"
void reorder_symbol_table_structures_process_binding(symbol_table_t* symbol_table, symbol_table_binding_t* binding, value_array_t* reordered_bindings)
# 253 "source-to-source.c"
{

# 254 "source-to-source.c"
  log_debug("processing %s", (binding->key_string));

# 255 "source-to-source.c"
  if ((!(binding->visited)))

# 255 "source-to-source.c"
  {

# 256 "source-to-source.c"
    ((binding->visited)=true);
    struct_node_t* structure_node = get_full_structure_definition_node(binding);

# 258 "source-to-source.c"
    if ((structure_node==((void *)0)))

# 258 "source-to-source.c"
    {

# 261 "source-to-source.c"
      return;
    }
    uint64_t length = node_list_length((structure_node->fields));

# 264 "source-to-source.c"
    for (
      uint64_t i = 0;
      (i<length);
      (i++))

# 264 "source-to-source.c"
    {
      field_node_t* field = to_field_node(node_list_get((structure_node->fields), i));
      type_node_t* type_node = (field->type);

# 268 "source-to-source.c"
      if ((type_node!=((void *)0)))

# 268 "source-to-source.c"
      {
        symbol_table_binding_t* field_type_binding = resolve_typename_to_structure_binding(symbol_table, type_node);

# 271 "source-to-source.c"
        if (((field_type_binding!=((void *)0))&&(!(field_type_binding->visited))))

# 271 "source-to-source.c"
        {

# 272 "source-to-source.c"
          reorder_symbol_table_structures_process_binding(symbol_table, field_type_binding, reordered_bindings);
        }
      }
    }

# 277 "source-to-source.c"
    value_array_add(reordered_bindings, ptr_to_value(binding));
  }
}


# 292 "source-to-source.c"
void reorder_symbol_table_structures(symbol_table_t* symbol_table)
# 292 "source-to-source.c"
{
  value_array_t* bindings = ((symbol_table->structures)->ordered_bindings);
  value_array_t* reordered_bindings = make_value_array((bindings->length));

# 295 "source-to-source.c"
  for (
    int i = 0;
    (i<(bindings->length));
    (i++))

# 295 "source-to-source.c"
  {
    symbol_table_binding_t* binding = (/*CAST*/(symbol_table_binding_t*) (value_array_get(bindings, i).ptr));

# 298 "source-to-source.c"
    reorder_symbol_table_structures_process_binding(symbol_table, binding, reordered_bindings);
  }

# 301 "source-to-source.c"
  (((symbol_table->structures)->ordered_bindings)=reordered_bindings);
}


# 48 "preprocessor.c"
c_preprocessor_directive_range_t mark_c_preprocessor_directive(c_preprocess_options_t options, value_array_t* tokens, uint64_t start_position)
# 50 "preprocessor.c"
{
  c_preprocessor_directive_range_t result = {0};
  uint64_t position = start_position;

# 60 "preprocessor.c"
  for (

# 60 "preprocessor.c"
    ;
    (position<(tokens->length));
    (position++))

# 60 "preprocessor.c"
  {
    token_t* token = token_at(tokens, position);

# 62 "preprocessor.c"
    ((token->is_cpp_token)=true);

# 63 "preprocessor.c"
    if ((position==start_position))

# 63 "preprocessor.c"
    {

# 64 "preprocessor.c"
      ((result.token_start_position)=start_position);

# 65 "preprocessor.c"
      ((result.buffer_start_position)=(token->start));

# 66 "preprocessor.c"
      ((result.buffer)=(token->buffer));
    }

# 68 "preprocessor.c"
    if (((result.buffer)!=(token->buffer)))

# 68 "preprocessor.c"
    {

# 69 "preprocessor.c"
      fatal_error(ERROR_ILLEGAL_STATE);
    }

# 71 "preprocessor.c"
    ((result.token_end_position)=(position+1));

# 72 "preprocessor.c"
    ((result.buffer_end_position)=(token->end));

# 73 "preprocessor.c"
    if ((((token->type)==TOKEN_TYPE_WHITESPACE)&&token_contains(token, "\n")))

# 73 "preprocessor.c"
    {

# 74 "preprocessor.c"
      break;
    }
  }

# 77 "preprocessor.c"
  return result;
}


# 80 "preprocessor.c"
uint64_t handle_c_preprocessor_directive(c_preprocess_options_t options, symbol_table_t* symbol_table, value_array_t* tokens, uint64_t start_position)
# 83 "preprocessor.c"
{
  c_preprocessor_directive_range_t range = mark_c_preprocessor_directive(options, tokens, start_position);
  token_t* directive_name = token_at(tokens, (start_position+1));

# 88 "preprocessor.c"
  if (token_matches(directive_name, "include"))

# 88 "preprocessor.c"
  {
    cpp_include_node_t* node = malloc_cpp_include_node();

# 90 "preprocessor.c"
    ((node->text)=buffer_c_substring((range.buffer), (range.buffer_start_position), (range.buffer_end_position)));

# 92 "preprocessor.c"
    if (string_contains_char((node->text), '<'))

# 92 "preprocessor.c"
    {

# 93 "preprocessor.c"
      value_array_add((symbol_table->system_includes), ptr_to_value(node));
    }
    else

# 94 "preprocessor.c"
    {

# 95 "preprocessor.c"
      value_array_add((symbol_table->user_includes), ptr_to_value(node));
    }
  }
  else

# 97 "preprocessor.c"
  if (token_matches(directive_name, "define"))

# 97 "preprocessor.c"
  {
    cpp_define_node_t* node = malloc_cpp_define_node();

# 99 "preprocessor.c"
    ((node->text)=buffer_c_substring((range.buffer), (range.buffer_start_position), (range.buffer_end_position)));

# 101 "preprocessor.c"
    value_array_add((symbol_table->defines), ptr_to_value(node));
  }

# 103 "preprocessor.c"
  return (range.token_end_position);
}


# 106 "preprocessor.c"
void handle_c_preprocessor_directives(c_preprocess_options_t options, symbol_table_t* symbol_table, value_array_t* tokens)
# 108 "preprocessor.c"
{

# 109 "preprocessor.c"
  for (
    uint64_t position = 0;
    (position<(tokens->length));
    )

# 109 "preprocessor.c"
  {
    token_t* token = token_at(tokens, position);

# 111 "preprocessor.c"
    if (token_matches(token, "#"))

# 111 "preprocessor.c"
    {

# 112 "preprocessor.c"
      (position=handle_c_preprocessor_directive(options, symbol_table, tokens, position));
    }
    else

# 114 "preprocessor.c"
    {

# 115 "preprocessor.c"
      (position++);
    }
  }
}


# 12 "symbol-table-builder.c"
void parse_and_add_top_level_definitions(symbol_table_t* symbol_table, value_array_t* file_names, boolean_t use_statement_parser)
# 14 "symbol-table-builder.c"
{
  value_array_t* files = read_files(file_names);

# 16 "symbol-table-builder.c"
  for (
    int i = 0;
    (i<(files->length));
    (i++))

# 16 "symbol-table-builder.c"
  {
    file_t* file = (/*CAST*/(file_t*) (value_array_get(files, i).ptr));

# 18 "symbol-table-builder.c"
    value_array_add((symbol_table->files), ptr_to_value(file));

# 19 "symbol-table-builder.c"
    symbol_table_parse_buffer(symbol_table, (file->data), (file->file_name), use_statement_parser);
  }
}


# 34 "symbol-table-builder.c"
void symbol_table_parse_buffer(symbol_table_t* symbol_table, buffer_t* buffer, char* file_name, boolean_t use_statement_parser)
# 36 "symbol-table-builder.c"
{
  tokenizer_result_t tokenizer_result = tokenize(buffer);

# 38 "symbol-table-builder.c"
  if ((tokenizer_result.tokenizer_error_code))

# 38 "symbol-table-builder.c"
  {

# 39 "symbol-table-builder.c"
    log_warn("Tokenizer error: \"%s\"::%d -- %d", file_name, (tokenizer_result.tokenizer_error_position), (tokenizer_result.tokenizer_error_code));

# 42 "symbol-table-builder.c"
    fatal_error(ERROR_ILLEGAL_INPUT);
  }
  value_array_t* tokens = (tokenizer_result.tokens);

# 46 "symbol-table-builder.c"
  handle_c_preprocessor_directives(((c_preprocess_options_t) {
                           .keep_system_includes = true,
                           .keep_user_includes = false,
                       }), symbol_table, tokens);

# 54 "symbol-table-builder.c"
  (tokens=transform_tokens(tokens, ((token_transformer_options_t) {
                                   .keep_whitespace = false,
                                   .keep_comments = false,
                                   .keep_javadoc_comments = false,
                                   .keep_c_preprocessor_lines = false,
                               })));
  pstate_t pstate = ((pstate_t) {.tokens = tokens, .use_statement_parser = use_statement_parser});

# 65 "symbol-table-builder.c"
  if ((!parse_declarations((&pstate))))

# 65 "symbol-table-builder.c"
  {

# 66 "symbol-table-builder.c"
    (((pstate.error).file_name)=file_name);
    buffer_t* buffer = make_buffer(1);

# 68 "symbol-table-builder.c"
    (buffer=buffer_append_human_readable_error(buffer, (&(pstate.error))));

# 69 "symbol-table-builder.c"
    log_fatal("%s", buffer_to_c_string(buffer));

# 70 "symbol-table-builder.c"
    fatal_error(ERROR_ILLEGAL_INPUT);
  }
  declarations_node_t* root = to_declarations_node(pstate_get_result_node((&pstate)));

# 75 "symbol-table-builder.c"
  symbol_table_add_declartions(symbol_table, root);
}


# 87 "symbol-table-builder.c"
file_t* symbol_table_token_to_file(symbol_table_t* symbol_table, token_t* token)
# 88 "symbol-table-builder.c"
{
  value_array_t* files = (symbol_table->files);
  buffer_t* buffer = (token->buffer);

# 91 "symbol-table-builder.c"
  for (
    int i = 0;
    (i<(files->length));
    (i++))

# 91 "symbol-table-builder.c"
  {
    file_t* file = (/*CAST*/(file_t*) (value_array_get(files, i).ptr));

# 93 "symbol-table-builder.c"
    if (((file->data)==buffer))

# 93 "symbol-table-builder.c"
    {

# 94 "symbol-table-builder.c"
      return file;
    }
  }

# 97 "symbol-table-builder.c"
  return ((void *)0);
}


# 27 "srcgen.c"
void srcgen_enum_to_string_converters(symbol_table_t* symbol_table)
# 27 "srcgen.c"
{
  buffer_t* buffer = make_buffer(1);
  printer_t* printer = make_printer(buffer, symbol_table, 2);

# 31 "srcgen.c"
  for (
    int i = 0;
    (i<(((symbol_table->typedefs)->ordered_bindings)->length));
    (i++))

# 31 "srcgen.c"
  {
    symbol_table_binding_t* binding = value_array_get_ptr(((symbol_table->typedefs)->ordered_bindings), i, typeof(symbol_table_binding_t*));
    typedef_node_t* typedef_node = to_typedef_node(value_array_get_ptr((binding->definition_nodes), 0, typeof(parse_node_t*)));

# 37 "srcgen.c"
    if (is_enum_node(((typedef_node->type_node)->user_type)))

# 37 "srcgen.c"
    {
      enum_node_t* enum_node = to_enum_node(((typedef_node->type_node)->user_type));
      char* enum_node_name = token_to_string((typedef_node->name));

# 41 "srcgen.c"
      log_debug("Generating converters for %s", enum_node_name);
      char* to_string_prefix = remove_type_suffix_1(enum_node_name);
      char* enum_node_type_string = enum_node_name;

# 46 "srcgen.c"
      append_enum_to_string(printer, enum_node, to_string_prefix, enum_node_type_string);

# 48 "srcgen.c"
      append_string_to_enum(printer, enum_node, to_string_prefix, enum_node_type_string);

# 50 "srcgen.c"
      buffer_append_enum_metadata(buffer, enum_node, to_string_prefix, enum_node_type_string);
    }
  }

# 55 "srcgen.c"
  for (
    int i = 0;
    (i<(((symbol_table->enums)->ordered_bindings)->length));
    (i++))

# 55 "srcgen.c"
  {
    symbol_table_binding_t* binding = value_array_get_ptr(((symbol_table->enums)->ordered_bindings), i, typeof(symbol_table_binding_t*));
    enum_node_t* enum_node = to_enum_node(value_array_get_ptr((binding->definition_nodes), 0, typeof(parse_node_t*)));
    char* enum_node_name = token_to_string((enum_node->name));
    char* to_string_prefix = remove_type_suffix_1(enum_node_name);
    char* enum_node_type_string = string_printf("enum %s", enum_node_name);

# 64 "srcgen.c"
    append_enum_to_string(printer, enum_node, to_string_prefix, enum_node_type_string);

# 66 "srcgen.c"
    append_string_to_enum(printer, enum_node, to_string_prefix, enum_node_type_string);

# 68 "srcgen.c"
    buffer_append_enum_metadata(buffer, enum_node, to_string_prefix, enum_node_type_string);
  }

# 72 "srcgen.c"
  if (((buffer->length)>0))

# 72 "srcgen.c"
  {

# 73 "srcgen.c"
    symbol_table_parse_buffer(symbol_table, buffer, "#<internal-code-generator>", false);
  }
}


# 200 "pratt-parser.c"
pstatus_t pratt_parse_expression(pstate_t* pstate, int precedence)
# 200 "pratt-parser.c"
{
  uint64_t saved_position = (pstate->position);
  token_t* token = pstate_peek(pstate, 0);

# 203 "pratt-parser.c"
  if ((token==((void *)0)))

# 203 "pratt-parser.c"
  {

# 204 "pratt-parser.c"
    return pstate_error(pstate, saved_position, PARSE_ERROR_EOF);
  }
  pratt_parser_instruction_t prefix_instruction = get_prefix_instruction(token);

# 207 "pratt-parser.c"
  if (((prefix_instruction.operation)==PRATT_PARSE_UNKNOWN))

# 207 "pratt-parser.c"
  {

# 208 "pratt-parser.c"
    log_debug("(RETURNING ERROR) No prefix for %s\n", token_to_string(token));

# 209 "pratt-parser.c"
    return pstate_error(pstate, saved_position, PARSE_ERROR_EXPECTED_PREFIX_OPERATOR_OR_TERMINAL);
  }

# 213 "pratt-parser.c"
  if ((!pratt_handle_instruction(pstate, prefix_instruction, ((void *)0))))

# 213 "pratt-parser.c"
  {

# 214 "pratt-parser.c"
    log_debug("(RETURNING ERROR) handle instruction\n", token_to_string(token));

# 215 "pratt-parser.c"
    return pstate_propagate_error(pstate, saved_position);
  }
  parse_node_t* left = pstate_get_result_node(pstate);

# 220 "pratt-parser.c"
  while (1)

# 220 "pratt-parser.c"
  {
    pratt_parser_instruction_t infix_instruction = get_infix_instruction(pstate_peek(pstate, 0));

# 223 "pratt-parser.c"
    if ((((infix_instruction.operation)==PRATT_PARSE_UNKNOWN)||(precedence>=(infix_instruction.precedence))))

# 224 "pratt-parser.c"
    {

# 225 "pratt-parser.c"
      return pstate_set_result_node(pstate, left);
    }

# 227 "pratt-parser.c"
    if ((!pratt_handle_instruction(pstate, infix_instruction, left)))

# 227 "pratt-parser.c"
    {

# 228 "pratt-parser.c"
      return pstate_propagate_error(pstate, saved_position);
    }

# 230 "pratt-parser.c"
    (left=pstate_get_result_node(pstate));
  }

# 234 "pratt-parser.c"
  return ((void *)0);
}


# 242 "pratt-parser.c"
pstatus_t pratt_handle_instruction(pstate_t* pstate, pratt_parser_instruction_t instruction, parse_node_t* left)
# 244 "pratt-parser.c"
{
  uint64_t saved_position = (pstate->position);
  token_t* token = pstate_peek(pstate, 0);

# 248 "pratt-parser.c"
  switch ((instruction.operation))

# 248 "pratt-parser.c"
  {

# 249 "pratt-parser.c"
    case PRATT_PARSE_BINARY_OPERATOR:

# 250 "pratt-parser.c"
    do
# 250 "pratt-parser.c"
    {

# 251 "pratt-parser.c"
      pstate_advance(pstate);
      int recursive_precedence = (instruction.precedence);

# 253 "pratt-parser.c"
      if ((precedence_to_associativity(recursive_precedence)==LEFT_TO_RIGHT))

# 253 "pratt-parser.c"
      {

# 254 "pratt-parser.c"
        (recursive_precedence++);
      }
      else

# 255 "pratt-parser.c"
      {

# 256 "pratt-parser.c"
        (recursive_precedence--);
      }

# 258 "pratt-parser.c"
      if ((!pratt_parse_expression(pstate, recursive_precedence)))

# 258 "pratt-parser.c"
      {

# 259 "pratt-parser.c"
        return pstate_propagate_error(pstate, saved_position);
      }
      operator_node_t* result = malloc_operator_node();

# 262 "pratt-parser.c"
      ((result->operator)=token);

# 263 "pratt-parser.c"
      ((result->left)=left);

# 264 "pratt-parser.c"
      ((result->right)=pstate_get_result_node(pstate));

# 265 "pratt-parser.c"
      return pstate_set_result_node(pstate, to_node(result));
    }
    while (0);

# 268 "pratt-parser.c"
    case PRATT_PARSE_IDENTIFIER:

# 269 "pratt-parser.c"
    do
# 269 "pratt-parser.c"
    {

# 270 "pratt-parser.c"
      pstate_advance(pstate);
      identifier_node_t* result = malloc_identifier_node();

# 272 "pratt-parser.c"
      ((result->token)=token);

# 273 "pratt-parser.c"
      return pstate_set_result_node(pstate, to_node(result));
    }
    while (0);

# 276 "pratt-parser.c"
    case PRATT_PARSE_LITERAL:

# 277 "pratt-parser.c"
    return parse_literal_node(pstate);

# 279 "pratt-parser.c"
    case PRATT_PARSE_PREFIX_OPERATOR:

# 280 "pratt-parser.c"
    do
# 280 "pratt-parser.c"
    {

# 281 "pratt-parser.c"
      pstate_advance(pstate);

# 282 "pratt-parser.c"
      if ((!pratt_parse_expression(pstate, (instruction.precedence))))

# 282 "pratt-parser.c"
      {

# 283 "pratt-parser.c"
        return pstate_propagate_error(pstate, saved_position);
      }
      operator_node_t* result = malloc_operator_node();

# 286 "pratt-parser.c"
      ((result->operator)=token);

# 287 "pratt-parser.c"
      if ((left!=((void *)0)))

# 287 "pratt-parser.c"
      {

# 288 "pratt-parser.c"
        fatal_error(ERROR_ILLEGAL_STATE);
      }

# 290 "pratt-parser.c"
      ((result->left)=((void *)0));

# 291 "pratt-parser.c"
      ((result->right)=pstate_get_result_node(pstate));

# 292 "pratt-parser.c"
      return pstate_set_result_node(pstate, to_node(result));
    }
    while (0);

# 295 "pratt-parser.c"
    case PRATT_PARSE_POSTFIX_OPERATOR:

# 296 "pratt-parser.c"
    do
# 296 "pratt-parser.c"
    {

# 297 "pratt-parser.c"
      pstate_advance(pstate);
      operator_node_t* result = malloc_operator_node();

# 299 "pratt-parser.c"
      ((result->operator)=token);

# 300 "pratt-parser.c"
      ((result->left)=left);

# 301 "pratt-parser.c"
      ((result->right)=((void *)0));

# 302 "pratt-parser.c"
      return pstate_set_result_node(pstate, to_node(result));
    }
    while (0);

# 305 "pratt-parser.c"
    case PRATT_PARSE_SUB_EXPRESSION:

# 306 "pratt-parser.c"
    do
# 306 "pratt-parser.c"
    {

# 307 "pratt-parser.c"
      if ((!pstate_expect_token_string(pstate, "(")))

# 307 "pratt-parser.c"
      {

# 308 "pratt-parser.c"
        fatal_error(ERROR_ILLEGAL_STATE);
      }

# 310 "pratt-parser.c"
      if ((!pratt_parse_expression(pstate, 0)))

# 310 "pratt-parser.c"
      {

# 311 "pratt-parser.c"
        return pstate_propagate_error(pstate, saved_position);
      }
      parse_node_t* inner_expression = pstate_get_result_node(pstate);

# 314 "pratt-parser.c"
      if ((!pstate_expect_token_string(pstate, ")")))

# 314 "pratt-parser.c"
      {

# 315 "pratt-parser.c"
        return pstate_propagate_error(pstate, saved_position);
      }

# 317 "pratt-parser.c"
      return pstate_set_result_node(pstate, inner_expression);
    }
    while (0);

# 319 "pratt-parser.c"
    break;

# 321 "pratt-parser.c"
    case PRATT_PARSE_INDEX_EXPRESSION:

# 322 "pratt-parser.c"
    do
# 322 "pratt-parser.c"
    {

# 323 "pratt-parser.c"
      if ((!pstate_expect_token_string(pstate, "[")))

# 323 "pratt-parser.c"
      {

# 324 "pratt-parser.c"
        fatal_error(ERROR_ILLEGAL_STATE);
      }

# 326 "pratt-parser.c"
      if ((!pratt_parse_expression(pstate, 0)))

# 326 "pratt-parser.c"
      {

# 327 "pratt-parser.c"
        return pstate_propagate_error(pstate, saved_position);
      }
      parse_node_t* inner_expression = pstate_get_result_node(pstate);

# 330 "pratt-parser.c"
      if ((!pstate_expect_token_string(pstate, "]")))

# 330 "pratt-parser.c"
      {

# 331 "pratt-parser.c"
        return pstate_propagate_error(pstate, saved_position);
      }
      operator_node_t* result = malloc_operator_node();

# 334 "pratt-parser.c"
      ((result->operator)=token);

# 335 "pratt-parser.c"
      ((result->left)=left);

# 336 "pratt-parser.c"
      ((result->right)=inner_expression);

# 337 "pratt-parser.c"
      return pstate_set_result_node(pstate, to_node(result));
    }
    while (0);

# 339 "pratt-parser.c"
    break;

# 341 "pratt-parser.c"
    case PRATT_PARSE_SIZEOF:

# 342 "pratt-parser.c"
    do
# 342 "pratt-parser.c"
    {

# 343 "pratt-parser.c"
      if ((!pstate_expect_token_string(pstate, "sizeof")))

# 343 "pratt-parser.c"
      {

# 344 "pratt-parser.c"
        fatal_error(ERROR_ILLEGAL_STATE);
      }

# 346 "pratt-parser.c"
      if ((!pstate_expect_token_string(pstate, "(")))

# 346 "pratt-parser.c"
      {

# 347 "pratt-parser.c"
        return pstate_propagate_error(pstate, saved_position);
      }

# 354 "pratt-parser.c"
      if ((!pratt_parse_expression(pstate, 0)))

# 354 "pratt-parser.c"
      {

# 355 "pratt-parser.c"
        return pstate_propagate_error(pstate, saved_position);
      }
      parse_node_t* inner_expression = pstate_get_result_node(pstate);

# 358 "pratt-parser.c"
      if ((!pstate_expect_token_string(pstate, ")")))

# 358 "pratt-parser.c"
      {

# 359 "pratt-parser.c"
        return pstate_propagate_error(pstate, saved_position);
      }
      operator_node_t* result = malloc_operator_node();

# 362 "pratt-parser.c"
      ((result->operator)=token);

# 363 "pratt-parser.c"
      ((result->right)=inner_expression);

# 364 "pratt-parser.c"
      return pstate_set_result_node(pstate, to_node(result));
    }
    while (0);

# 366 "pratt-parser.c"
    break;

# 368 "pratt-parser.c"
    case PRATT_PARSE_CAST_MACRO:

# 369 "pratt-parser.c"
    do
# 369 "pratt-parser.c"
    {

# 370 "pratt-parser.c"
      if ((!pstate_expect_token_string(pstate, "cast")))

# 370 "pratt-parser.c"
      {

# 371 "pratt-parser.c"
        fatal_error(ERROR_ILLEGAL_STATE);
      }

# 373 "pratt-parser.c"
      if ((!pstate_expect_token_string(pstate, "(")))

# 373 "pratt-parser.c"
      {

# 374 "pratt-parser.c"
        return pstate_propagate_error(pstate, saved_position);
      }

# 376 "pratt-parser.c"
      if ((!parse_type_node(pstate)))

# 376 "pratt-parser.c"
      {

# 377 "pratt-parser.c"
        return pstate_propagate_error(pstate, saved_position);
      }
      parse_node_t* type_node = pstate_get_result_node(pstate);

# 380 "pratt-parser.c"
      if (((!pstate_expect_token_string(pstate, ","))||(!pratt_parse_expression(pstate, PRECEDENCE_ASSIGNMENT))))

# 381 "pratt-parser.c"
      {

# 382 "pratt-parser.c"
        return pstate_propagate_error(pstate, saved_position);
      }
      parse_node_t* expression = pstate_get_result_node(pstate);

# 385 "pratt-parser.c"
      if ((!pstate_expect_token_string(pstate, ")")))

# 385 "pratt-parser.c"
      {

# 386 "pratt-parser.c"
        return pstate_propagate_error(pstate, saved_position);
      }
      operator_node_t* result = malloc_operator_node();

# 389 "pratt-parser.c"
      ((result->operator)=token);

# 390 "pratt-parser.c"
      ((result->left)=type_node);

# 391 "pratt-parser.c"
      ((result->right)=expression);

# 392 "pratt-parser.c"
      return pstate_set_result_node(pstate, to_node(result));
    }
    while (0);

# 394 "pratt-parser.c"
    break;

# 398 "pratt-parser.c"
    case PRATT_PARSE_BLOCK_EXPR:

# 399 "pratt-parser.c"
    do
# 399 "pratt-parser.c"
    {

# 400 "pratt-parser.c"
      if ((!pstate_expect_token_string(pstate, "block_expr")))

# 400 "pratt-parser.c"
      {

# 401 "pratt-parser.c"
        fatal_error(ERROR_ILLEGAL_STATE);
      }

# 403 "pratt-parser.c"
      if ((!pstate_expect_token_string(pstate, "(")))

# 403 "pratt-parser.c"
      {

# 404 "pratt-parser.c"
        return pstate_propagate_error(pstate, saved_position);
      }

# 406 "pratt-parser.c"
      if ((!parse_block(pstate)))

# 406 "pratt-parser.c"
      {

# 407 "pratt-parser.c"
        return pstate_propagate_error(pstate, saved_position);
      }
      parse_node_t* block_node = pstate_get_result_node(pstate);

# 410 "pratt-parser.c"
      if ((!pstate_expect_token_string(pstate, ")")))

# 410 "pratt-parser.c"
      {

# 411 "pratt-parser.c"
        return pstate_propagate_error(pstate, saved_position);
      }
      operator_node_t* result = malloc_operator_node();

# 414 "pratt-parser.c"
      ((result->operator)=token);

# 415 "pratt-parser.c"
      ((result->left)=block_node);

# 416 "pratt-parser.c"
      return pstate_set_result_node(pstate, to_node(result));
    }
    while (0);

# 418 "pratt-parser.c"
    break;

# 422 "pratt-parser.c"
    case PRATT_PARSE_TYPE_OF:

# 423 "pratt-parser.c"
    do
# 423 "pratt-parser.c"
    {

# 424 "pratt-parser.c"
      if ((!pstate_expect_token_string(pstate, "typeof")))

# 424 "pratt-parser.c"
      {

# 425 "pratt-parser.c"
        fatal_error(ERROR_ILLEGAL_STATE);
      }

# 427 "pratt-parser.c"
      if ((!pstate_expect_token_string(pstate, "(")))

# 427 "pratt-parser.c"
      {

# 428 "pratt-parser.c"
        return pstate_propagate_error(pstate, saved_position);
      }

# 430 "pratt-parser.c"
      if ((!parse_type_node(pstate)))

# 430 "pratt-parser.c"
      {

# 431 "pratt-parser.c"
        return pstate_propagate_error(pstate, saved_position);
      }
      parse_node_t* type_node = pstate_get_result_node(pstate);

# 434 "pratt-parser.c"
      if ((!pstate_expect_token_string(pstate, ")")))

# 434 "pratt-parser.c"
      {

# 435 "pratt-parser.c"
        return pstate_propagate_error(pstate, saved_position);
      }
      operator_node_t* result = malloc_operator_node();

# 438 "pratt-parser.c"
      ((result->operator)=token);

# 439 "pratt-parser.c"
      ((result->left)=type_node);

# 440 "pratt-parser.c"
      return pstate_set_result_node(pstate, to_node(result));
    }
    while (0);

# 442 "pratt-parser.c"
    break;

# 444 "pratt-parser.c"
    case PRATT_PARSE_CALL:

# 445 "pratt-parser.c"
    do
# 445 "pratt-parser.c"
    {

# 446 "pratt-parser.c"
      if ((!pstate_expect_token_string(pstate, "(")))

# 446 "pratt-parser.c"
      {

# 447 "pratt-parser.c"
        fatal_error(ERROR_ILLEGAL_STATE);
      }
      call_node_t* result = malloc_call_node();

# 450 "pratt-parser.c"
      ((result->function)=left);
      boolean_t expect_comma = false;

# 452 "pratt-parser.c"
      while ((!token_matches(pstate_peek(pstate, 0), ")")))

# 452 "pratt-parser.c"
      {

# 453 "pratt-parser.c"
        if (expect_comma)

# 453 "pratt-parser.c"
        {

# 454 "pratt-parser.c"
          if ((!pstate_expect_token_string(pstate, ",")))

# 454 "pratt-parser.c"
          {

# 455 "pratt-parser.c"
            return pstate_propagate_error(pstate, saved_position);
          }
        }
        else

# 457 "pratt-parser.c"
        {

# 458 "pratt-parser.c"
          (expect_comma=true);
        }

# 460 "pratt-parser.c"
        if ((!pratt_parse_expression(pstate, PRECEDENCE_ASSIGNMENT)))

# 460 "pratt-parser.c"
        {

# 461 "pratt-parser.c"
          return pstate_propagate_error(pstate, saved_position);
        }

# 463 "pratt-parser.c"
        node_list_add_node((&(result->args)), pstate_get_result_node(pstate));
      }

# 465 "pratt-parser.c"
      if ((!pstate_expect_token_string(pstate, ")")))

# 465 "pratt-parser.c"
      {

# 466 "pratt-parser.c"
        return pstate_propagate_error(pstate, saved_position);
      }

# 468 "pratt-parser.c"
      return pstate_set_result_node(pstate, to_node(result));
    }
    while (0);

# 470 "pratt-parser.c"
    break;

# 472 "pratt-parser.c"
    case PRATT_PARSE_CONDITIONAL:

# 473 "pratt-parser.c"
    do
# 473 "pratt-parser.c"
    {

# 474 "pratt-parser.c"
      pstate_advance(pstate);
      int recursive_precedence = (instruction.precedence);

# 476 "pratt-parser.c"
      if ((!pratt_parse_expression(pstate, recursive_precedence)))

# 476 "pratt-parser.c"
      {

# 477 "pratt-parser.c"
        return pstate_propagate_error(pstate, saved_position);
      }
      parse_node_t* expr_if_true = pstate_get_result_node(pstate);

# 480 "pratt-parser.c"
      if (((!pstate_expect_token_string(pstate, ":"))||(!pratt_parse_expression(pstate, recursive_precedence))))

# 481 "pratt-parser.c"
      {

# 482 "pratt-parser.c"
        return pstate_propagate_error(pstate, saved_position);
      }
      parse_node_t* expr_if_false = pstate_get_result_node(pstate);
      conditional_node_t* result = malloc_conditional_node();

# 486 "pratt-parser.c"
      ((result->condition)=left);

# 487 "pratt-parser.c"
      ((result->expr_if_true)=expr_if_true);

# 488 "pratt-parser.c"
      ((result->expr_if_false)=expr_if_false);

# 489 "pratt-parser.c"
      return pstate_set_result_node(pstate, to_node(result));
    }
    while (0);

# 492 "pratt-parser.c"
    default:

# 493 "pratt-parser.c"
    break;
  }

# 495 "pratt-parser.c"
  return pstate_error(pstate, saved_position, PARSE_ERROR_UNHANDLED_INSTRUCTION);
}


# 510 "pratt-parser.c"
pratt_parser_instruction_t get_prefix_instruction(token_t* token)
# 510 "pratt-parser.c"
{

# 511 "pratt-parser.c"
  switch ((token->type))

# 511 "pratt-parser.c"
  {

# 513 "pratt-parser.c"
    case TOKEN_TYPE_IDENTIFIER:

# 514 "pratt-parser.c"
    if (token_matches(token, "sizeof"))

# 514 "pratt-parser.c"
    {

# 515 "pratt-parser.c"
      return make_parser_instruction(token, PRATT_PARSE_SIZEOF, PRECEDENCE_UNARY);
    }

# 518 "pratt-parser.c"
    if (token_matches(token, "cast"))

# 518 "pratt-parser.c"
    {

# 519 "pratt-parser.c"
      return make_parser_instruction(token, PRATT_PARSE_CAST_MACRO, PRECEDENCE_UNARY);
    }

# 522 "pratt-parser.c"
    if (token_matches(token, "typeof"))

# 522 "pratt-parser.c"
    {

# 523 "pratt-parser.c"
      return make_parser_instruction(token, PRATT_PARSE_TYPE_OF, PRECEDENCE_UNARY);
    }

# 526 "pratt-parser.c"
    if (token_matches(token, "block_expr"))

# 526 "pratt-parser.c"
    {

# 527 "pratt-parser.c"
      return make_parser_instruction(token, PRATT_PARSE_BLOCK_EXPR, PRECEDENCE_UNARY);
    }

# 530 "pratt-parser.c"
    if (token_matches(token, "compound_literal"))

# 530 "pratt-parser.c"
    {

# 531 "pratt-parser.c"
      return make_parser_instruction(token, PRATT_PARSE_LITERAL, PRECEDENCE_UNARY);
    }

# 534 "pratt-parser.c"
    return make_parser_instruction(token, PRATT_PARSE_IDENTIFIER, PRECEDENCE_PRIMARY);

# 537 "pratt-parser.c"
    case TOKEN_TYPE_INTEGER_LITERAL:

# 538 "pratt-parser.c"
    case TOKEN_TYPE_FLOAT_LITERAL:

# 539 "pratt-parser.c"
    case TOKEN_TYPE_STRING_LITERAL:

# 540 "pratt-parser.c"
    case TOKEN_TYPE_CHARACTER_LITERAL:

# 541 "pratt-parser.c"
    return make_parser_instruction(token, PRATT_PARSE_LITERAL, PRECEDENCE_PRIMARY);

# 544 "pratt-parser.c"
    case TOKEN_TYPE_PUNCTUATION:

# 545 "pratt-parser.c"
    break;

# 547 "pratt-parser.c"
    default:

# 548 "pratt-parser.c"
    return make_parser_instruction(token, PRATT_PARSE_UNKNOWN, PRECEDENCE_UNKNOWN);
  }

# 552 "pratt-parser.c"
  if (token_matches(token, "("))

# 552 "pratt-parser.c"
  {

# 553 "pratt-parser.c"
    return make_parser_instruction(token, PRATT_PARSE_SUB_EXPRESSION, PRECEDENCE_PRIMARY);
  }

# 558 "pratt-parser.c"
  if (((((((((token_matches(token, "+")||token_matches(token, "-"))||token_matches(token, "~"))||token_matches(token, "!"))||token_matches(token, "!"))||token_matches(token, "++"))||token_matches(token, "--"))||token_matches(token, "*"))||token_matches(token, "&")))

# 562 "pratt-parser.c"
  {

# 563 "pratt-parser.c"
    return make_parser_instruction(token, PRATT_PARSE_PREFIX_OPERATOR, PRECEDENCE_UNARY);
  }

# 567 "pratt-parser.c"
  return ((pratt_parser_instruction_t) {0});
}


# 577 "pratt-parser.c"
pratt_parser_instruction_t get_infix_instruction(token_t* token)
# 577 "pratt-parser.c"
{

# 578 "pratt-parser.c"
  if ((token_matches(token, "+")||token_matches(token, "-")))

# 578 "pratt-parser.c"
  {

# 579 "pratt-parser.c"
    return make_parser_instruction(token, PRATT_PARSE_BINARY_OPERATOR, PRECEDENCE_ADDITIVE);
  }

# 582 "pratt-parser.c"
  if (((token_matches(token, "*")||token_matches(token, "/"))||token_matches(token, "%")))

# 583 "pratt-parser.c"
  {

# 584 "pratt-parser.c"
    return make_parser_instruction(token, PRATT_PARSE_BINARY_OPERATOR, PRECEDENCE_MULTIPICITIVE);
  }

# 587 "pratt-parser.c"
  if ((token_matches(token, "<<")||token_matches(token, ">>")))

# 587 "pratt-parser.c"
  {

# 588 "pratt-parser.c"
    return make_parser_instruction(token, PRATT_PARSE_BINARY_OPERATOR, PRECEDENCE_SHIFT);
  }

# 591 "pratt-parser.c"
  if (token_matches(token, "||"))

# 591 "pratt-parser.c"
  {

# 592 "pratt-parser.c"
    return make_parser_instruction(token, PRATT_PARSE_BINARY_OPERATOR, PRECEDENCE_LOGICAL_OR);
  }

# 595 "pratt-parser.c"
  if (token_matches(token, "&&"))

# 595 "pratt-parser.c"
  {

# 596 "pratt-parser.c"
    return make_parser_instruction(token, PRATT_PARSE_BINARY_OPERATOR, PRECEDENCE_LOGICAL_AND);
  }

# 599 "pratt-parser.c"
  if (token_matches(token, "|"))

# 599 "pratt-parser.c"
  {

# 600 "pratt-parser.c"
    return make_parser_instruction(token, PRATT_PARSE_BINARY_OPERATOR, PRECEDENCE_OR);
  }

# 603 "pratt-parser.c"
  if (token_matches(token, "^"))

# 603 "pratt-parser.c"
  {

# 604 "pratt-parser.c"
    return make_parser_instruction(token, PRATT_PARSE_BINARY_OPERATOR, PRECEDENCE_XOR);
  }

# 607 "pratt-parser.c"
  if (token_matches(token, "&"))

# 607 "pratt-parser.c"
  {

# 608 "pratt-parser.c"
    return make_parser_instruction(token, PRATT_PARSE_BINARY_OPERATOR, PRECEDENCE_AND);
  }

# 611 "pratt-parser.c"
  if ((token_matches(token, "==")||token_matches(token, "!=")))

# 611 "pratt-parser.c"
  {

# 612 "pratt-parser.c"
    return make_parser_instruction(token, PRATT_PARSE_BINARY_OPERATOR, PRECEDENCE_EQUALITY);
  }

# 615 "pratt-parser.c"
  if ((((token_matches(token, "<")||token_matches(token, "<="))||token_matches(token, ">"))||token_matches(token, ">=")))

# 616 "pratt-parser.c"
  {

# 617 "pratt-parser.c"
    return make_parser_instruction(token, PRATT_PARSE_BINARY_OPERATOR, PRECEDENCE_RELATIONAL);
  }

# 620 "pratt-parser.c"
  if ((token_matches(token, "->")||token_matches(token, ".")))

# 620 "pratt-parser.c"
  {

# 621 "pratt-parser.c"
    return make_parser_instruction(token, PRATT_PARSE_BINARY_OPERATOR, PRECEDENCE_POSTFIX);
  }

# 624 "pratt-parser.c"
  if ((token_matches(token, "++")||token_matches(token, "--")))

# 624 "pratt-parser.c"
  {

# 625 "pratt-parser.c"
    return make_parser_instruction(token, PRATT_PARSE_POSTFIX_OPERATOR, PRECEDENCE_POSTFIX);
  }

# 628 "pratt-parser.c"
  if (((((((((((token_matches(token, "=")||token_matches(token, "+="))||token_matches(token, "-="))||token_matches(token, "*="))||token_matches(token, "/="))||token_matches(token, "%="))||token_matches(token, "&="))||token_matches(token, "^="))||token_matches(token, "|="))||token_matches(token, "<<="))||token_matches(token, ">>=")))

# 633 "pratt-parser.c"
  {

# 634 "pratt-parser.c"
    return make_parser_instruction(token, PRATT_PARSE_BINARY_OPERATOR, PRECEDENCE_ASSIGNMENT);
  }

# 637 "pratt-parser.c"
  if (token_matches(token, "["))

# 637 "pratt-parser.c"
  {

# 638 "pratt-parser.c"
    return make_parser_instruction(token, PRATT_PARSE_INDEX_EXPRESSION, PRECEDENCE_POSTFIX);
  }

# 641 "pratt-parser.c"
  if (token_matches(token, "("))

# 641 "pratt-parser.c"
  {

# 642 "pratt-parser.c"
    return make_parser_instruction(token, PRATT_PARSE_CALL, PRECEDENCE_POSTFIX);
  }

# 644 "pratt-parser.c"
  if (token_matches(token, "?"))

# 644 "pratt-parser.c"
  {

# 645 "pratt-parser.c"
    return make_parser_instruction(token, PRATT_PARSE_CONDITIONAL, PRECEDENCE_CONDITIONAL);
  }

# 649 "pratt-parser.c"
  return ((pratt_parser_instruction_t) {0});
}


# 663 "pratt-parser.c"
associativity_t precedence_to_associativity(precedence_t precedence)
# 663 "pratt-parser.c"
{

# 664 "pratt-parser.c"
  switch (precedence)

# 664 "pratt-parser.c"
  {

# 665 "pratt-parser.c"
    case PRECEDENCE_PRIMARY:

# 666 "pratt-parser.c"
    return LEFT_TO_RIGHT;

# 667 "pratt-parser.c"
    case PRECEDENCE_POSTFIX:

# 668 "pratt-parser.c"
    return LEFT_TO_RIGHT;

# 669 "pratt-parser.c"
    case PRECEDENCE_UNARY:

# 670 "pratt-parser.c"
    return RIGHT_TO_LEFT;

# 671 "pratt-parser.c"
    case PRECEDENCE_MULTIPICITIVE:

# 672 "pratt-parser.c"
    return LEFT_TO_RIGHT;

# 673 "pratt-parser.c"
    case PRECEDENCE_ADDITIVE:

# 674 "pratt-parser.c"
    return LEFT_TO_RIGHT;

# 675 "pratt-parser.c"
    case PRECEDENCE_SHIFT:

# 676 "pratt-parser.c"
    return LEFT_TO_RIGHT;

# 677 "pratt-parser.c"
    case PRECEDENCE_RELATIONAL:

# 678 "pratt-parser.c"
    return LEFT_TO_RIGHT;

# 679 "pratt-parser.c"
    case PRECEDENCE_EQUALITY:

# 680 "pratt-parser.c"
    return LEFT_TO_RIGHT;

# 681 "pratt-parser.c"
    case PRECEDENCE_AND:

# 682 "pratt-parser.c"
    return LEFT_TO_RIGHT;

# 683 "pratt-parser.c"
    case PRECEDENCE_XOR:

# 684 "pratt-parser.c"
    return LEFT_TO_RIGHT;

# 685 "pratt-parser.c"
    case PRECEDENCE_OR:

# 686 "pratt-parser.c"
    return LEFT_TO_RIGHT;

# 687 "pratt-parser.c"
    case PRECEDENCE_LOGICAL_AND:

# 688 "pratt-parser.c"
    return LEFT_TO_RIGHT;

# 689 "pratt-parser.c"
    case PRECEDENCE_LOGICAL_OR:

# 690 "pratt-parser.c"
    return LEFT_TO_RIGHT;

# 691 "pratt-parser.c"
    case PRECEDENCE_CONDITIONAL:

# 692 "pratt-parser.c"
    return RIGHT_TO_LEFT;

# 693 "pratt-parser.c"
    case PRECEDENCE_ASSIGNMENT:

# 694 "pratt-parser.c"
    return RIGHT_TO_LEFT;

# 695 "pratt-parser.c"
    case PRECEDENCE_COMMA:

# 696 "pratt-parser.c"
    return LEFT_TO_RIGHT;

# 697 "pratt-parser.c"
    default:

# 698 "pratt-parser.c"
    fatal_error(ERROR_ILLEGAL_STATE);
  }

# 700 "pratt-parser.c"
  return 0;
}


# 164 "statement-parser.c"
pstatus_t parse_statement(pstate_t* pstate)
# 164 "statement-parser.c"
{
  uint64_t saved_position = (pstate->position);

# 166 "statement-parser.c"
  if ((((((((((((((((parse_block(pstate)||parse_break_statement(pstate_ignore_error(pstate)))||parse_return_statement(pstate_ignore_error(pstate)))||parse_if_statement(pstate_ignore_error(pstate)))||parse_while_statement(pstate_ignore_error(pstate)))||parse_do_statement(pstate_ignore_error(pstate)))||parse_for_statement(pstate_ignore_error(pstate)))||parse_switch_statement(pstate_ignore_error(pstate)))||parse_case_label(pstate_ignore_error(pstate)))||parse_default_label(pstate_ignore_error(pstate)))||parse_continue_statement(pstate_ignore_error(pstate)))||parse_goto_statement(pstate_ignore_error(pstate)))||parse_label_statement(pstate_ignore_error(pstate)))||parse_variable_definition_node(pstate_ignore_error(pstate)))||parse_expression_statement(pstate_ignore_error(pstate)))||parse_empty_statement(pstate_ignore_error(pstate))))

# 180 "statement-parser.c"
  {

# 181 "statement-parser.c"
    return true;
  }

# 183 "statement-parser.c"
  return pstate_error(pstate, saved_position, PARSE_ERROR_EXPECTED_STATEMENT);
}


# 189 "statement-parser.c"
pstatus_t parse_block(pstate_t* pstate)
# 189 "statement-parser.c"
{
  uint64_t saved_position = (pstate->position);

# 191 "statement-parser.c"
  if ((!pstate_expect_token_string(pstate, "{")))

# 191 "statement-parser.c"
  {

# 192 "statement-parser.c"
    return pstate_propagate_error(pstate, saved_position);
  }
  block_node_t* result = make_block_node(token_at((pstate->tokens), saved_position));

# 196 "statement-parser.c"
  while (parse_statement(pstate))

# 196 "statement-parser.c"
  {

# 197 "statement-parser.c"
    node_list_add_node((&(result->statements)), pstate_get_result_node(pstate));
  }

# 199 "statement-parser.c"
  if ((!pstate_expect_token_string(pstate, "}")))

# 199 "statement-parser.c"
  {

# 200 "statement-parser.c"
    return pstate_propagate_error(pstate, saved_position);
  }

# 202 "statement-parser.c"
  return pstate_set_result_node(pstate, to_node(result));
}


# 208 "statement-parser.c"
pstatus_t parse_return_statement(pstate_t* pstate)
# 208 "statement-parser.c"
{
  uint64_t saved_position = (pstate->position);

# 210 "statement-parser.c"
  if ((!pstate_expect_token_string(pstate, "return")))

# 210 "statement-parser.c"
  {

# 211 "statement-parser.c"
    return pstate_propagate_error(pstate, saved_position);
  }

# 213 "statement-parser.c"
  parse_expression(pstate);
  parse_node_t* expr = pstate_get_optional_result_node(pstate);

# 215 "statement-parser.c"
  if ((!pstate_expect_token_string(pstate, ";")))

# 215 "statement-parser.c"
  {

# 216 "statement-parser.c"
    return pstate_propagate_error(pstate, saved_position);
  }

# 218 "statement-parser.c"
  return pstate_set_result_node(pstate, to_node(make_return_statement(token_at((pstate->tokens), saved_position), expr)));
}


# 226 "statement-parser.c"
pstatus_t parse_if_statement(pstate_t* pstate)
# 226 "statement-parser.c"
{
  uint64_t saved_position = (pstate->position);

# 228 "statement-parser.c"
  if ((((!pstate_expect_token_string(pstate, "if"))||(!pstate_expect_token_string(pstate, "(")))||(!parse_expression(pstate))))

# 230 "statement-parser.c"
  {

# 231 "statement-parser.c"
    return pstate_propagate_error(pstate, saved_position);
  }
  parse_node_t* if_test = pstate_get_result_node(pstate);

# 234 "statement-parser.c"
  if (((!pstate_expect_token_string(pstate, ")"))||(!parse_statement(pstate))))

# 234 "statement-parser.c"
  {

# 235 "statement-parser.c"
    return pstate_propagate_error(pstate, saved_position);
  }
  parse_node_t* if_true = pstate_get_result_node(pstate);
  parse_node_t* if_false = ((void *)0);

# 239 "statement-parser.c"
  if (pstate_match_token_string(pstate, "else"))

# 239 "statement-parser.c"
  {

# 240 "statement-parser.c"
    pstate_advance(pstate);

# 241 "statement-parser.c"
    if ((!parse_statement(pstate)))

# 241 "statement-parser.c"
    {

# 242 "statement-parser.c"
      return pstate_propagate_error(pstate, saved_position);
    }

# 244 "statement-parser.c"
    (if_false=pstate_get_result_node(pstate));
  }

# 246 "statement-parser.c"
  return pstate_set_result_node(pstate, to_node(make_if_statement(token_at((pstate->tokens), saved_position), if_test, if_true, if_false)));
}


# 255 "statement-parser.c"
pstatus_t parse_while_statement(pstate_t* pstate)
# 255 "statement-parser.c"
{
  uint64_t saved_position = (pstate->position);

# 257 "statement-parser.c"
  if ((((!pstate_expect_token_string(pstate, "while"))||(!pstate_expect_token_string(pstate, "(")))||(!parse_expression(pstate))))

# 259 "statement-parser.c"
  {

# 260 "statement-parser.c"
    return pstate_propagate_error(pstate, saved_position);
  }
  parse_node_t* while_test = pstate_get_result_node(pstate);

# 263 "statement-parser.c"
  if (((!pstate_expect_token_string(pstate, ")"))||(!parse_statement(pstate))))

# 263 "statement-parser.c"
  {

# 264 "statement-parser.c"
    return pstate_propagate_error(pstate, saved_position);
  }
  parse_node_t* while_body = pstate_get_result_node(pstate);

# 267 "statement-parser.c"
  return pstate_set_result_node(pstate, to_node(make_while_statement(token_at((pstate->tokens), saved_position), while_test, while_body)));
}


# 276 "statement-parser.c"
pstatus_t parse_do_statement(pstate_t* pstate)
# 276 "statement-parser.c"
{
  uint64_t saved_position = (pstate->position);

# 278 "statement-parser.c"
  if (((!pstate_expect_token_string(pstate, "do"))||(!parse_statement(pstate))))

# 278 "statement-parser.c"
  {

# 279 "statement-parser.c"
    return pstate_propagate_error(pstate, saved_position);
  }
  parse_node_t* do_while_body = pstate_get_result_node(pstate);

# 282 "statement-parser.c"
  if (((!pstate_expect_token_string(pstate, "while"))||(!pstate_expect_token_string(pstate, "("))))

# 283 "statement-parser.c"
  {

# 284 "statement-parser.c"
    return pstate_propagate_error(pstate, saved_position);
  }

# 286 "statement-parser.c"
  if ((!parse_expression(pstate)))

# 286 "statement-parser.c"
  {

# 287 "statement-parser.c"
    return pstate_propagate_error(pstate, saved_position);
  }
  parse_node_t* do_while_condition = pstate_get_result_node(pstate);

# 290 "statement-parser.c"
  if (((!pstate_expect_token_string(pstate, ")"))||(!pstate_expect_token_string(pstate, ";"))))

# 291 "statement-parser.c"
  {

# 292 "statement-parser.c"
    return pstate_propagate_error(pstate, saved_position);
  }

# 294 "statement-parser.c"
  return pstate_set_result_node(pstate, to_node(make_do_statement(token_at((pstate->tokens), saved_position), do_while_body, do_while_condition)));
}


# 303 "statement-parser.c"
pstatus_t parse_for_statement(pstate_t* pstate)
# 303 "statement-parser.c"
{
  uint64_t saved_position = (pstate->position);

# 305 "statement-parser.c"
  if (((!pstate_expect_token_string(pstate, "for"))||(!pstate_expect_token_string(pstate, "("))))

# 306 "statement-parser.c"
  {

# 307 "statement-parser.c"
    return pstate_propagate_error(pstate, saved_position);
  }

# 309 "statement-parser.c"
  if ((!parse_statement(pstate)))

# 309 "statement-parser.c"
  {

# 310 "statement-parser.c"
    return pstate_propagate_error(pstate, saved_position);
  }
  parse_node_t* for_init = pstate_get_result_node(pstate);
  parse_node_t* for_test = ((void *)0);

# 314 "statement-parser.c"
  if (parse_expression(pstate))

# 314 "statement-parser.c"
  {

# 315 "statement-parser.c"
    (for_test=pstate_get_result_node(pstate));
  }

# 317 "statement-parser.c"
  if ((!pstate_expect_token_string(pstate, ";")))

# 317 "statement-parser.c"
  {

# 318 "statement-parser.c"
    return pstate_propagate_error(pstate, saved_position);
  }
  parse_node_t* for_increment = ((void *)0);

# 321 "statement-parser.c"
  if (parse_expression(pstate))

# 321 "statement-parser.c"
  {

# 322 "statement-parser.c"
    (for_increment=pstate_get_result_node(pstate));
  }

# 324 "statement-parser.c"
  if ((!pstate_expect_token_string(pstate, ")")))

# 324 "statement-parser.c"
  {

# 325 "statement-parser.c"
    return pstate_propagate_error(pstate, saved_position);
  }

# 327 "statement-parser.c"
  if ((!parse_statement(pstate)))

# 327 "statement-parser.c"
  {

# 328 "statement-parser.c"
    return pstate_propagate_error(pstate, saved_position);
  }
  parse_node_t* for_body = pstate_get_result_node(pstate);

# 331 "statement-parser.c"
  return pstate_set_result_node(pstate, to_node(make_for_statement(token_at((pstate->tokens), saved_position), for_init, for_test, for_increment, for_body)));
}


# 340 "statement-parser.c"
pstatus_t parse_switch_statement(pstate_t* pstate)
# 340 "statement-parser.c"
{
  uint64_t saved_position = (pstate->position);

# 342 "statement-parser.c"
  if ((((!pstate_expect_token_string(pstate, "switch"))||(!pstate_expect_token_string(pstate, "(")))||(!parse_expression(pstate))))

# 344 "statement-parser.c"
  {

# 345 "statement-parser.c"
    return pstate_propagate_error(pstate, saved_position);
  }
  parse_node_t* switch_item = pstate_get_result_node(pstate);

# 348 "statement-parser.c"
  if (((!pstate_expect_token_string(pstate, ")"))||(!parse_block(pstate))))

# 348 "statement-parser.c"
  {

# 349 "statement-parser.c"
    return pstate_propagate_error(pstate, saved_position);
  }
  parse_node_t* block = pstate_get_result_node(pstate);

# 352 "statement-parser.c"
  return pstate_set_result_node(pstate, to_node(make_switch_statement(token_at((pstate->tokens), saved_position), switch_item, block)));
}


# 361 "statement-parser.c"
pstatus_t parse_case_label(pstate_t* pstate)
# 361 "statement-parser.c"
{
  uint64_t saved_position = (pstate->position);

# 363 "statement-parser.c"
  if (((!pstate_expect_token_string(pstate, "case"))||(!parse_expression(pstate))))

# 364 "statement-parser.c"
  {

# 365 "statement-parser.c"
    return pstate_propagate_error(pstate, saved_position);
  }
  parse_node_t* case_expr = pstate_get_result_node(pstate);

# 368 "statement-parser.c"
  if ((!pstate_expect_token_string(pstate, ":")))

# 368 "statement-parser.c"
  {

# 369 "statement-parser.c"
    return pstate_propagate_error(pstate, saved_position);
  }

# 371 "statement-parser.c"
  return pstate_set_result_node(pstate, to_node(make_case_label(token_at((pstate->tokens), saved_position), case_expr)));
}


# 382 "statement-parser.c"
pstatus_t parse_expression_statement(pstate_t* pstate)
# 382 "statement-parser.c"
{
  uint64_t saved_position = (pstate->position);

# 384 "statement-parser.c"
  if ((!parse_expression(pstate)))

# 384 "statement-parser.c"
  {

# 385 "statement-parser.c"
    return pstate_propagate_error(pstate, saved_position);
  }
  parse_node_t* expr = pstate_get_result_node(pstate);

# 388 "statement-parser.c"
  if ((!pstate_expect_token_string(pstate, ";")))

# 388 "statement-parser.c"
  {

# 389 "statement-parser.c"
    return pstate_propagate_error(pstate, saved_position);
  }

# 391 "statement-parser.c"
  return pstate_set_result_node(pstate, to_node(make_expression_statement_node(token_at((pstate->tokens), saved_position), expr)));
}


# 399 "statement-parser.c"
pstatus_t parse_goto_statement(pstate_t* pstate)
# 399 "statement-parser.c"
{
  uint64_t saved_position = (pstate->position);
  token_t* label_token = pstate_peek(pstate, 1);

# 402 "statement-parser.c"
  if ((((!pstate_expect_token_string(pstate, "goto"))||(!pstate_expect_token_type(pstate, TOKEN_TYPE_IDENTIFIER)))||(!pstate_expect_token_string(pstate, ";"))))

# 404 "statement-parser.c"
  {

# 405 "statement-parser.c"
    return pstate_propagate_error(pstate, saved_position);
  }

# 407 "statement-parser.c"
  return pstate_set_result_node(pstate, to_node(make_goto_statement(token_at((pstate->tokens), saved_position), label_token)));
}


# 415 "statement-parser.c"
pstatus_t parse_break_statement(pstate_t* pstate)
# 415 "statement-parser.c"
{
  uint64_t saved_position = (pstate->position);
  token_t* keyword_token = pstate_peek(pstate, 0);

# 418 "statement-parser.c"
  if (((!pstate_expect_token_string(pstate, "break"))||(!pstate_expect_token_string(pstate, ";"))))

# 419 "statement-parser.c"
  {

# 420 "statement-parser.c"
    return pstate_propagate_error(pstate, saved_position);
  }

# 422 "statement-parser.c"
  return pstate_set_result_node(pstate, to_node(make_break_statement(keyword_token)));
}


# 429 "statement-parser.c"
pstatus_t parse_continue_statement(pstate_t* pstate)
# 429 "statement-parser.c"
{
  uint64_t saved_position = (pstate->position);
  token_t* keyword_token = pstate_peek(pstate, 0);

# 432 "statement-parser.c"
  if (((!pstate_expect_token_string(pstate, "continue"))||(!pstate_expect_token_string(pstate, ";"))))

# 433 "statement-parser.c"
  {

# 434 "statement-parser.c"
    return pstate_propagate_error(pstate, saved_position);
  }

# 436 "statement-parser.c"
  return pstate_set_result_node(pstate, to_node(make_continue_statement(keyword_token)));
}


# 443 "statement-parser.c"
pstatus_t parse_label_statement(pstate_t* pstate)
# 443 "statement-parser.c"
{
  uint64_t saved_position = (pstate->position);
  token_t* label_token = pstate_peek(pstate, 0);

# 446 "statement-parser.c"
  if (((!pstate_expect_token_type(pstate, TOKEN_TYPE_IDENTIFIER))||(!pstate_expect_token_string(pstate, ":"))))

# 447 "statement-parser.c"
  {

# 448 "statement-parser.c"
    return pstate_propagate_error(pstate, saved_position);
  }

# 450 "statement-parser.c"
  return pstate_set_result_node(pstate, to_node(make_label_statement(label_token)));
}


# 457 "statement-parser.c"
pstatus_t parse_default_label(pstate_t* pstate)
# 457 "statement-parser.c"
{
  uint64_t saved_position = (pstate->position);
  token_t* default_token = pstate_peek(pstate, 0);

# 460 "statement-parser.c"
  if (((!pstate_expect_token_string(pstate, "default"))||(!pstate_expect_token_string(pstate, ":"))))

# 461 "statement-parser.c"
  {

# 462 "statement-parser.c"
    return pstate_propagate_error(pstate, saved_position);
  }

# 464 "statement-parser.c"
  return pstate_set_result_node(pstate, to_node(make_default_label(default_token)));
}


# 471 "statement-parser.c"
pstatus_t parse_empty_statement(pstate_t* pstate)
# 471 "statement-parser.c"
{
  uint64_t saved_position = (pstate->position);
  token_t* semi_colon_token = pstate_peek(pstate, 0);

# 474 "statement-parser.c"
  if ((!pstate_expect_token_string(pstate, ";")))

# 474 "statement-parser.c"
  {

# 475 "statement-parser.c"
    return pstate_propagate_error(pstate, saved_position);
  }

# 477 "statement-parser.c"
  return pstate_set_result_node(pstate, to_node(make_empty_statement(semi_colon_token)));
}


# 490 "statement-parser.c"
break_statement_node_t* make_break_statement(token_t* break_keyword_token)
# 490 "statement-parser.c"
{
  break_statement_node_t* result = malloc_struct(break_statement_node_t);

# 492 "statement-parser.c"
  ((result->tag)=PARSE_NODE_BREAK_STATEMENT);

# 493 "statement-parser.c"
  ((result->break_keyword_token)=break_keyword_token);

# 494 "statement-parser.c"
  return result;
}


# 503 "statement-parser.c"
break_statement_node_t* to_break_statement_node(parse_node_t* ptr)
# 503 "statement-parser.c"
{

# 504 "statement-parser.c"
  if (((ptr==((void *)0))||((ptr->tag)!=PARSE_NODE_BREAK_STATEMENT)))

# 504 "statement-parser.c"
  {

# 505 "statement-parser.c"
    fatal_error(ERROR_ILLEGAL_STATE);
  }

# 507 "statement-parser.c"
  return (/*CAST*/(break_statement_node_t*) ptr);
}


# 517 "statement-parser.c"
continue_statement_node_t* make_continue_statement(token_t* keyword_token)
# 517 "statement-parser.c"
{
  continue_statement_node_t* result = malloc_struct(continue_statement_node_t);

# 519 "statement-parser.c"
  ((result->tag)=PARSE_NODE_CONTINUE_STATEMENT);

# 520 "statement-parser.c"
  ((result->continue_keyword_token)=keyword_token);

# 521 "statement-parser.c"
  return result;
}


# 530 "statement-parser.c"
continue_statement_node_t* to_continue_statement_node(parse_node_t* ptr)
# 530 "statement-parser.c"
{

# 531 "statement-parser.c"
  if (((ptr==((void *)0))||((ptr->tag)!=PARSE_NODE_CONTINUE_STATEMENT)))

# 531 "statement-parser.c"
  {

# 532 "statement-parser.c"
    fatal_error(ERROR_ILLEGAL_STATE);
  }

# 534 "statement-parser.c"
  return (/*CAST*/(continue_statement_node_t*) ptr);
}


# 544 "statement-parser.c"
label_statement_node_t* make_label_statement(token_t* label)
# 544 "statement-parser.c"
{
  label_statement_node_t* result = malloc_struct(label_statement_node_t);

# 546 "statement-parser.c"
  ((result->tag)=PARSE_NODE_LABEL_STATEMENT);

# 547 "statement-parser.c"
  ((result->label)=label);

# 548 "statement-parser.c"
  return result;
}


# 557 "statement-parser.c"
label_statement_node_t* to_label_statement_node(parse_node_t* ptr)
# 557 "statement-parser.c"
{

# 558 "statement-parser.c"
  if (((ptr==((void *)0))||((ptr->tag)!=PARSE_NODE_LABEL_STATEMENT)))

# 558 "statement-parser.c"
  {

# 559 "statement-parser.c"
    fatal_error(ERROR_ILLEGAL_STATE);
  }

# 561 "statement-parser.c"
  return (/*CAST*/(label_statement_node_t*) ptr);
}


# 571 "statement-parser.c"
goto_statement_node_t* make_goto_statement(token_t* first_token, token_t* label)
# 572 "statement-parser.c"
{
  goto_statement_node_t* result = malloc_struct(goto_statement_node_t);

# 574 "statement-parser.c"
  ((result->tag)=PARSE_NODE_GOTO_STATEMENT);

# 575 "statement-parser.c"
  ((result->first_token)=first_token);

# 576 "statement-parser.c"
  ((result->label)=label);

# 577 "statement-parser.c"
  return result;
}


# 586 "statement-parser.c"
goto_statement_node_t* to_goto_statement_node(parse_node_t* ptr)
# 586 "statement-parser.c"
{

# 587 "statement-parser.c"
  if (((ptr==((void *)0))||((ptr->tag)!=PARSE_NODE_GOTO_STATEMENT)))

# 587 "statement-parser.c"
  {

# 588 "statement-parser.c"
    fatal_error(ERROR_ILLEGAL_STATE);
  }

# 590 "statement-parser.c"
  return (/*CAST*/(goto_statement_node_t*) ptr);
}


# 600 "statement-parser.c"
empty_statement_node_t* make_empty_statement(token_t* semi_colon_token)
# 600 "statement-parser.c"
{
  empty_statement_node_t* result = malloc_struct(empty_statement_node_t);

# 602 "statement-parser.c"
  ((result->tag)=PARSE_NODE_EMPTY_STATEMENT);

# 603 "statement-parser.c"
  ((result->semi_colon_token)=semi_colon_token);

# 604 "statement-parser.c"
  return result;
}


# 613 "statement-parser.c"
empty_statement_node_t* to_empty_statement_node(parse_node_t* ptr)
# 613 "statement-parser.c"
{

# 614 "statement-parser.c"
  if (((ptr==((void *)0))||((ptr->tag)!=PARSE_NODE_EMPTY_STATEMENT)))

# 614 "statement-parser.c"
  {

# 615 "statement-parser.c"
    fatal_error(ERROR_ILLEGAL_STATE);
  }

# 617 "statement-parser.c"
  return (/*CAST*/(empty_statement_node_t*) ptr);
}


# 627 "statement-parser.c"
switch_statement_node_t* make_switch_statement(token_t* first_token, parse_node_t* expression, parse_node_t* block)
# 629 "statement-parser.c"
{
  switch_statement_node_t* result = malloc_struct(switch_statement_node_t);

# 631 "statement-parser.c"
  ((result->tag)=PARSE_NODE_SWITCH_STATEMENT);

# 632 "statement-parser.c"
  ((result->first_token)=first_token);

# 633 "statement-parser.c"
  ((result->expression)=expression);

# 634 "statement-parser.c"
  ((result->block)=block);

# 635 "statement-parser.c"
  return result;
}


# 644 "statement-parser.c"
switch_statement_node_t* to_switch_statement_node(parse_node_t* ptr)
# 644 "statement-parser.c"
{

# 645 "statement-parser.c"
  if (((ptr==((void *)0))||((ptr->tag)!=PARSE_NODE_SWITCH_STATEMENT)))

# 645 "statement-parser.c"
  {

# 646 "statement-parser.c"
    fatal_error(ERROR_ILLEGAL_STATE);
  }

# 648 "statement-parser.c"
  return (/*CAST*/(switch_statement_node_t*) ptr);
}


# 658 "statement-parser.c"
case_label_node_t* make_case_label(token_t* first_token, parse_node_t* expression)
# 659 "statement-parser.c"
{
  case_label_node_t* result = malloc_struct(case_label_node_t);

# 661 "statement-parser.c"
  ((result->tag)=PARSE_NODE_CASE_LABEL);

# 662 "statement-parser.c"
  ((result->first_token)=first_token);

# 663 "statement-parser.c"
  ((result->expression)=expression);

# 664 "statement-parser.c"
  return result;
}


# 673 "statement-parser.c"
case_label_node_t* to_case_label_node(parse_node_t* ptr)
# 673 "statement-parser.c"
{

# 674 "statement-parser.c"
  if (((ptr==((void *)0))||((ptr->tag)!=PARSE_NODE_CASE_LABEL)))

# 674 "statement-parser.c"
  {

# 675 "statement-parser.c"
    fatal_error(ERROR_ILLEGAL_STATE);
  }

# 677 "statement-parser.c"
  return (/*CAST*/(case_label_node_t*) ptr);
}


# 687 "statement-parser.c"
default_label_node_t* make_default_label(token_t* default_token)
# 687 "statement-parser.c"
{
  default_label_node_t* result = malloc_struct(default_label_node_t);

# 689 "statement-parser.c"
  ((result->tag)=PARSE_NODE_DEFAULT_LABEL);

# 690 "statement-parser.c"
  ((result->default_token)=default_token);

# 691 "statement-parser.c"
  return result;
}


# 700 "statement-parser.c"
default_label_node_t* to_default_label_node(parse_node_t* ptr)
# 700 "statement-parser.c"
{

# 701 "statement-parser.c"
  if (((ptr==((void *)0))||((ptr->tag)!=PARSE_NODE_DEFAULT_LABEL)))

# 701 "statement-parser.c"
  {

# 702 "statement-parser.c"
    fatal_error(ERROR_ILLEGAL_STATE);
  }

# 704 "statement-parser.c"
  return (/*CAST*/(default_label_node_t*) ptr);
}


# 715 "statement-parser.c"
block_node_t* make_block_node(token_t* first_token)
# 715 "statement-parser.c"
{
  block_node_t* result = malloc_struct(block_node_t);

# 717 "statement-parser.c"
  ((result->first_token)=first_token);

# 718 "statement-parser.c"
  ((result->tag)=PARSE_NODE_BLOCK);

# 719 "statement-parser.c"
  return result;
}


# 728 "statement-parser.c"
block_node_t* to_block_node(parse_node_t* ptr)
# 728 "statement-parser.c"
{

# 729 "statement-parser.c"
  if (((ptr==((void *)0))||((ptr->tag)!=PARSE_NODE_BLOCK)))

# 729 "statement-parser.c"
  {

# 730 "statement-parser.c"
    fatal_error(ERROR_ILLEGAL_STATE);
  }

# 732 "statement-parser.c"
  return (/*CAST*/(block_node_t*) ptr);
}


# 743 "statement-parser.c"
for_statement_node_t* make_for_statement(token_t* first_token, parse_node_t* for_init, parse_node_t* for_test, parse_node_t* for_increment, parse_node_t* for_body)
# 747 "statement-parser.c"
{
  for_statement_node_t* result = malloc_struct(for_statement_node_t);

# 749 "statement-parser.c"
  ((result->tag)=PARSE_NODE_FOR_STATEMENT);

# 750 "statement-parser.c"
  ((result->first_token)=first_token);

# 751 "statement-parser.c"
  ((result->for_init)=for_init);

# 752 "statement-parser.c"
  ((result->for_test)=for_test);

# 753 "statement-parser.c"
  ((result->for_increment)=for_increment);

# 754 "statement-parser.c"
  ((result->for_body)=for_body);

# 755 "statement-parser.c"
  return result;
}


# 764 "statement-parser.c"
for_statement_node_t* to_for_statement_node(parse_node_t* ptr)
# 764 "statement-parser.c"
{

# 765 "statement-parser.c"
  if (((ptr==((void *)0))||((ptr->tag)!=PARSE_NODE_FOR_STATEMENT)))

# 765 "statement-parser.c"
  {

# 766 "statement-parser.c"
    fatal_error(ERROR_ILLEGAL_STATE);
  }

# 768 "statement-parser.c"
  return (/*CAST*/(for_statement_node_t*) ptr);
}


# 778 "statement-parser.c"
if_statement_node_t* make_if_statement(token_t* first_token, parse_node_t* if_condition, parse_node_t* if_true, parse_node_t* if_else)
# 781 "statement-parser.c"
{
  if_statement_node_t* result = malloc_struct(if_statement_node_t);

# 783 "statement-parser.c"
  ((result->tag)=PARSE_NODE_IF_STATEMENT);

# 784 "statement-parser.c"
  ((result->first_token)=first_token);

# 785 "statement-parser.c"
  ((result->if_condition)=if_condition);

# 786 "statement-parser.c"
  ((result->if_true)=if_true);

# 787 "statement-parser.c"
  ((result->if_else)=if_else);

# 788 "statement-parser.c"
  return result;
}


# 797 "statement-parser.c"
if_statement_node_t* to_if_statement_node(parse_node_t* ptr)
# 797 "statement-parser.c"
{

# 798 "statement-parser.c"
  if (((ptr==((void *)0))||((ptr->tag)!=PARSE_NODE_IF_STATEMENT)))

# 798 "statement-parser.c"
  {

# 799 "statement-parser.c"
    fatal_error(ERROR_ILLEGAL_STATE);
  }

# 801 "statement-parser.c"
  return (/*CAST*/(if_statement_node_t*) ptr);
}


# 812 "statement-parser.c"
expression_statement_node_t* make_expression_statement_node(token_t* first_token, parse_node_t* expression)
# 813 "statement-parser.c"
{
  expression_statement_node_t* result = malloc_struct(expression_statement_node_t);

# 816 "statement-parser.c"
  ((result->tag)=PARSE_NODE_EXPRESSION_STATEMENT);

# 817 "statement-parser.c"
  ((result->first_token)=first_token);

# 818 "statement-parser.c"
  ((result->expression)=expression);

# 819 "statement-parser.c"
  return result;
}


# 828 "statement-parser.c"
expression_statement_node_t* to_expression_statement_node(parse_node_t* ptr)
# 828 "statement-parser.c"
{

# 829 "statement-parser.c"
  if (((ptr==((void *)0))||((ptr->tag)!=PARSE_NODE_EXPRESSION_STATEMENT)))

# 829 "statement-parser.c"
  {

# 830 "statement-parser.c"
    fatal_error(ERROR_ILLEGAL_STATE);
  }

# 832 "statement-parser.c"
  return (/*CAST*/(expression_statement_node_t*) ptr);
}


# 842 "statement-parser.c"
return_statement_node_t* make_return_statement(token_t* first_token, parse_node_t* expression)
# 843 "statement-parser.c"
{
  return_statement_node_t* result = malloc_struct(return_statement_node_t);

# 845 "statement-parser.c"
  ((result->tag)=PARSE_NODE_RETURN_STATEMENT);

# 846 "statement-parser.c"
  ((result->first_token)=first_token);

# 847 "statement-parser.c"
  ((result->expression)=expression);

# 848 "statement-parser.c"
  return result;
}


# 857 "statement-parser.c"
return_statement_node_t* to_return_statement_node(parse_node_t* ptr)
# 857 "statement-parser.c"
{

# 858 "statement-parser.c"
  if (((ptr==((void *)0))||((ptr->tag)!=PARSE_NODE_RETURN_STATEMENT)))

# 858 "statement-parser.c"
  {

# 859 "statement-parser.c"
    fatal_error(ERROR_ILLEGAL_STATE);
  }

# 861 "statement-parser.c"
  return (/*CAST*/(return_statement_node_t*) ptr);
}


# 871 "statement-parser.c"
while_statement_node_t* make_while_statement(token_t* first_token, parse_node_t* condition, parse_node_t* body)
# 873 "statement-parser.c"
{
  while_statement_node_t* result = malloc_struct(while_statement_node_t);

# 875 "statement-parser.c"
  ((result->tag)=PARSE_NODE_WHILE_STATEMENT);

# 876 "statement-parser.c"
  ((result->first_token)=first_token);

# 877 "statement-parser.c"
  ((result->condition)=condition);

# 878 "statement-parser.c"
  ((result->body)=body);

# 879 "statement-parser.c"
  return result;
}


# 888 "statement-parser.c"
while_statement_node_t* to_while_statement_node(parse_node_t* ptr)
# 888 "statement-parser.c"
{

# 889 "statement-parser.c"
  if (((ptr==((void *)0))||((ptr->tag)!=PARSE_NODE_WHILE_STATEMENT)))

# 889 "statement-parser.c"
  {

# 890 "statement-parser.c"
    fatal_error(ERROR_ILLEGAL_STATE);
  }

# 892 "statement-parser.c"
  return (/*CAST*/(while_statement_node_t*) ptr);
}


# 902 "statement-parser.c"
do_statement_node_t* make_do_statement(token_t* first_token, parse_node_t* body, parse_node_t* condition)
# 903 "statement-parser.c"
{
  do_statement_node_t* result = malloc_struct(do_statement_node_t);

# 905 "statement-parser.c"
  ((result->tag)=PARSE_NODE_DO_STATEMENT);

# 906 "statement-parser.c"
  ((result->first_token)=first_token);

# 907 "statement-parser.c"
  ((result->body)=body);

# 908 "statement-parser.c"
  ((result->condition)=condition);

# 909 "statement-parser.c"
  return result;
}


# 918 "statement-parser.c"
do_statement_node_t* to_do_statement_node(parse_node_t* ptr)
# 918 "statement-parser.c"
{

# 919 "statement-parser.c"
  if (((ptr==((void *)0))||((ptr->tag)!=PARSE_NODE_DO_STATEMENT)))

# 919 "statement-parser.c"
  {

# 920 "statement-parser.c"
    fatal_error(ERROR_ILLEGAL_STATE);
  }

# 922 "statement-parser.c"
  return (/*CAST*/(do_statement_node_t*) ptr);
}


# 94 "type-parser.c"
pstatus_t parse_type_node(pstate_t* pstate)
# 94 "type-parser.c"
{
  uint64_t saved_position = (pstate->position);

# 97 "type-parser.c"
  if ((parse_typeof_node(pstate)||parse_function_type(pstate)))

# 97 "type-parser.c"
  {

# 98 "type-parser.c"
    return true;
  }
  type_node_t* result = malloc_type_node();
  type_qualifier_t qualifiers = TYPE_QUALIFIER_NONE;

# 106 "type-parser.c"
  if (pstate_match_token_string(pstate, "const"))

# 106 "type-parser.c"
  {

# 107 "type-parser.c"
    pstate_advance(pstate);

# 108 "type-parser.c"
    (qualifiers|=TYPE_QUALIFIER_CONST);
  }
  canonical_type_result_t canonical_type_result = parse_canonical_type(pstate);
  token_t* type_name = (canonical_type_result.canonical_type);

# 115 "type-parser.c"
  if ((type_name!=((void *)0)))

# 115 "type-parser.c"
  {

# 116 "type-parser.c"
    while (((canonical_type_result.consumed_tokens)>0))

# 116 "type-parser.c"
    {

# 117 "type-parser.c"
      ((canonical_type_result.consumed_tokens)--);

# 118 "type-parser.c"
      pstate_advance(pstate);
    }

# 120 "type-parser.c"
    ((result->type_node_kind)=TYPE_NODE_KIND_PRIMITIVE_TYPENAME);

# 121 "type-parser.c"
    ((result->type_name)=type_name);
  }
  else

# 122 "type-parser.c"
  if (parse_user_type_node(pstate))

# 122 "type-parser.c"
  {

# 123 "type-parser.c"
    ((result->type_node_kind)=TYPE_NODE_KIND_TYPE_EXPRESSION);

# 124 "type-parser.c"
    ((result->user_type)=pstate_get_result_node(pstate));
  }
  else

# 125 "type-parser.c"
  if (pstate_expect_token_type(pstate, TOKEN_TYPE_IDENTIFIER))

# 125 "type-parser.c"
  {

# 126 "type-parser.c"
    ((result->type_node_kind)=TYPE_NODE_KIND_TYPENAME);

# 127 "type-parser.c"
    ((result->type_name)=pstate_get_result_token(pstate));
  }
  else

# 128 "type-parser.c"
  {

# 129 "type-parser.c"
    return pstate_propagate_error(pstate, saved_position);
  }

# 134 "type-parser.c"
  while (true)

# 134 "type-parser.c"
  {

# 135 "type-parser.c"
    if (pstate_expect_token_string(pstate, "*"))

# 135 "type-parser.c"
    {
      type_node_t* ptr_result = malloc_type_node();

# 137 "type-parser.c"
      ((ptr_result->type_node_kind)=TYPE_NODE_KIND_POINTER);

# 138 "type-parser.c"
      node_list_add_node((&(ptr_result->type_args)), to_node(result));

# 139 "type-parser.c"
      (result=ptr_result);
    }
    else

# 140 "type-parser.c"
    if (token_matches(pstate_peek(pstate, 0), "["))

# 140 "type-parser.c"
    {

# 141 "type-parser.c"
      pstate_advance(pstate);
      type_node_t* array_result = malloc_type_node();

# 143 "type-parser.c"
      ((array_result->type_node_kind)=TYPE_NODE_KIND_ARRAY);

# 144 "type-parser.c"
      node_list_add_node((&(array_result->type_args)), to_node(result));

# 145 "type-parser.c"
      if (pstate_expect_token_type(pstate, TOKEN_TYPE_INTEGER_LITERAL))

# 145 "type-parser.c"
      {
        literal_node_t* literal = malloc_literal_node();

# 147 "type-parser.c"
        ((literal->token)=pstate_get_result_token(pstate));

# 148 "type-parser.c"
        ((array_result->type_node_kind)=TYPE_NODE_KIND_SIZED_ARRAY);

# 149 "type-parser.c"
        node_list_add_node((&(array_result->type_args)), to_node(literal));
      }
      else

# 150 "type-parser.c"
      {

# 151 "type-parser.c"
        pstate_ignore_error(pstate);
      }

# 154 "type-parser.c"
      if ((!pstate_expect_token_string(pstate, "]")))

# 154 "type-parser.c"
      {

# 155 "type-parser.c"
        return pstate_propagate_error(pstate, saved_position);
      }

# 157 "type-parser.c"
      (result=array_result);
    }
    else

# 158 "type-parser.c"
    {

# 159 "type-parser.c"
      pstate_ignore_error(pstate);

# 160 "type-parser.c"
      break;
    }
  }

# 163 "type-parser.c"
  ((result->qualifiers)=qualifiers);

# 164 "type-parser.c"
  return pstate_set_result_node(pstate, to_node(result));
}


# 167 "type-parser.c"
canonical_type_result_t make_type_token_result(char* str, int consumed_tokens)
# 167 "type-parser.c"
{
  token_t* canonical_token = malloc_struct(typeof(token_t));

# 169 "type-parser.c"
  ((canonical_token->type)=TOKEN_TYPE_IDENTIFIER);

# 170 "type-parser.c"
  ((canonical_token->buffer)=buffer_from_string(str));

# 171 "type-parser.c"
  ((canonical_token->start)=0);

# 172 "type-parser.c"
  ((canonical_token->end)=strlen(str));

# 173 "type-parser.c"
  return ((canonical_type_result_t) {.canonical_type = canonical_token, .consumed_tokens = consumed_tokens});
}


# 178 "type-parser.c"
pstatus_t parse_typeof_node(pstate_t* pstate)
# 178 "type-parser.c"
{
  uint64_t saved_position = (pstate->position);

# 180 "type-parser.c"
  if ((!pstate_expect_token_string(pstate, "typeof")))

# 180 "type-parser.c"
  {

# 181 "type-parser.c"
    return pstate_propagate_error(pstate, saved_position);
  }

# 183 "type-parser.c"
  if ((!pstate_expect_token_string(pstate, "(")))

# 183 "type-parser.c"
  {

# 184 "type-parser.c"
    return pstate_propagate_error(pstate, saved_position);
  }

# 186 "type-parser.c"
  if ((!parse_type_node(pstate)))

# 186 "type-parser.c"
  {

# 187 "type-parser.c"
    return pstate_propagate_error(pstate, saved_position);
  }
  parse_node_t* type_node = pstate_get_result_node(pstate);

# 190 "type-parser.c"
  if ((!pstate_expect_token_string(pstate, ")")))

# 190 "type-parser.c"
  {

# 191 "type-parser.c"
    return pstate_propagate_error(pstate, saved_position);
  }
  type_node_t* result = malloc_type_node();

# 195 "type-parser.c"
  ((result->type_node_kind)=TYPE_NODE_KIND_TYPEOF);

# 196 "type-parser.c"
  ((result->user_type)=type_node);

# 197 "type-parser.c"
  return pstate_set_result_node(pstate, to_node(result));
}


# 207 "type-parser.c"
canonical_type_result_t parse_canonical_type(pstate_t* pstate)
# 207 "type-parser.c"
{
  token_t* a = pstate_peek(pstate, 0);
  token_t* b = pstate_peek(pstate, 1);
  token_t* c = pstate_peek(pstate, 2);

# 212 "type-parser.c"
  if (((token_matches(a, "signed")&&token_matches(b, "short"))&&token_matches(c, "int")))

# 213 "type-parser.c"
  {

# 214 "type-parser.c"
    return make_type_token_result("short", 3);
  }

# 217 "type-parser.c"
  if (((token_matches(a, "unsigned")&&token_matches(b, "short"))&&token_matches(c, "int")))

# 218 "type-parser.c"
  {

# 219 "type-parser.c"
    return make_type_token_result("unsigned short int", 3);
  }

# 222 "type-parser.c"
  if (((token_matches(a, "signed")&&token_matches(b, "long"))&&token_matches(c, "int")))

# 223 "type-parser.c"
  {

# 224 "type-parser.c"
    return make_type_token_result("long", 3);
  }

# 227 "type-parser.c"
  if (((token_matches(a, "unsigned")&&token_matches(b, "long"))&&token_matches(c, "int")))

# 228 "type-parser.c"
  {

# 229 "type-parser.c"
    return make_type_token_result("unsigned long", 3);
  }

# 232 "type-parser.c"
  if ((token_matches(a, "short")&&token_matches(b, "int")))

# 232 "type-parser.c"
  {

# 233 "type-parser.c"
    return make_type_token_result("short", 2);
  }

# 236 "type-parser.c"
  if ((token_matches(a, "signed")&&token_matches(b, "short")))

# 236 "type-parser.c"
  {

# 237 "type-parser.c"
    return make_type_token_result("short", 2);
  }

# 240 "type-parser.c"
  if ((token_matches(a, "signed")&&token_matches(b, "int")))

# 240 "type-parser.c"
  {

# 241 "type-parser.c"
    return make_type_token_result("int", 2);
  }

# 244 "type-parser.c"
  if ((token_matches(a, "long")&&token_matches(b, "int")))

# 244 "type-parser.c"
  {

# 245 "type-parser.c"
    return make_type_token_result("long", 2);
  }

# 248 "type-parser.c"
  if ((token_matches(a, "long")&&token_matches(b, "long")))

# 248 "type-parser.c"
  {

# 249 "type-parser.c"
    return make_type_token_result("long long", 2);
  }

# 252 "type-parser.c"
  if ((token_matches(a, "long")&&token_matches(b, "double")))

# 252 "type-parser.c"
  {

# 253 "type-parser.c"
    return make_type_token_result("long double", 2);
  }

# 256 "type-parser.c"
  if ((token_matches(a, "signed")&&token_matches(b, "long")))

# 256 "type-parser.c"
  {

# 257 "type-parser.c"
    return make_type_token_result("long", 2);
  }

# 260 "type-parser.c"
  if ((token_matches(a, "unsigned")&&token_matches(b, "int")))

# 260 "type-parser.c"
  {

# 261 "type-parser.c"
    return make_type_token_result("unsigned int", 2);
  }

# 264 "type-parser.c"
  if ((token_matches(a, "unsigned")&&token_matches(b, "long")))

# 264 "type-parser.c"
  {

# 265 "type-parser.c"
    return make_type_token_result("unsigned long", 2);
  }

# 268 "type-parser.c"
  if ((token_matches(a, "unsigned")&&token_matches(b, "char")))

# 268 "type-parser.c"
  {

# 269 "type-parser.c"
    return make_type_token_result("unsigned char", 2);
  }

# 272 "type-parser.c"
  if ((token_matches(a, "signed")&&token_matches(b, "char")))

# 272 "type-parser.c"
  {

# 273 "type-parser.c"
    return make_type_token_result("char", 2);
  }

# 276 "type-parser.c"
  if ((token_matches(a, "long")&&token_matches(b, "float")))

# 276 "type-parser.c"
  {

# 278 "type-parser.c"
    return make_type_token_result("double", 2);
  }

# 281 "type-parser.c"
  if (token_matches(a, "signed"))

# 281 "type-parser.c"
  {

# 282 "type-parser.c"
    return make_type_token_result("int", 1);
  }

# 285 "type-parser.c"
  if (token_matches(a, "char"))

# 285 "type-parser.c"
  {

# 286 "type-parser.c"
    return make_type_token_result("char", 1);
  }

# 289 "type-parser.c"
  if (token_matches(a, "int"))

# 289 "type-parser.c"
  {

# 290 "type-parser.c"
    return make_type_token_result("int", 1);
  }

# 293 "type-parser.c"
  if (token_matches(a, "long"))

# 293 "type-parser.c"
  {

# 294 "type-parser.c"
    return make_type_token_result("long", 1);
  }

# 297 "type-parser.c"
  if (token_matches(a, "float"))

# 297 "type-parser.c"
  {

# 298 "type-parser.c"
    return make_type_token_result("float", 1);
  }

# 301 "type-parser.c"
  if (token_matches(a, "double"))

# 301 "type-parser.c"
  {

# 302 "type-parser.c"
    return make_type_token_result("double", 1);
  }

# 307 "type-parser.c"
  return ((canonical_type_result_t) {.canonical_type = NULL, .consumed_tokens = 0});
}


# 323 "type-parser.c"
pstatus_t parse_function_type(pstate_t* pstate)
# 323 "type-parser.c"
{
  uint64_t saved_position = (pstate->position);
  token_t* fn_t_token = pstate_peek(pstate, 0);

# 328 "type-parser.c"
  if (((!pstate_expect_token_string(pstate, "fn_t"))||(!pstate_expect_token_string(pstate, "("))))

# 329 "type-parser.c"
  {

# 330 "type-parser.c"
    return pstate_propagate_error(pstate, saved_position);
  }
  type_node_t* result = malloc_type_node();

# 334 "type-parser.c"
  ((result->type_node_kind)=TYPE_NODE_KIND_TYPE_EXPRESSION);

# 335 "type-parser.c"
  ((result->type_name)=fn_t_token);

# 338 "type-parser.c"
  if ((!parse_type_node(pstate)))

# 338 "type-parser.c"
  {

# 339 "type-parser.c"
    return pstate_propagate_error(pstate, saved_position);
  }

# 341 "type-parser.c"
  node_list_add_node((&(result->type_args)), pstate_get_result_node(pstate));

# 344 "type-parser.c"
  while (parse_function_type_argument(pstate))

# 344 "type-parser.c"
  {

# 345 "type-parser.c"
    node_list_add_node((&(result->type_args)), pstate_get_result_node(pstate));
  }

# 348 "type-parser.c"
  if ((!pstate_expect_token_string(pstate, ")")))

# 348 "type-parser.c"
  {

# 349 "type-parser.c"
    return pstate_propagate_error(pstate, saved_position);
  }

# 352 "type-parser.c"
  return pstate_set_result_node(pstate, to_node(result));
}


# 364 "type-parser.c"
pstatus_t parse_function_type_argument(pstate_t* pstate)
# 364 "type-parser.c"
{
  uint64_t saved_position = (pstate->position);

# 368 "type-parser.c"
  if ((!pstate_expect_token_string(pstate, ",")))

# 368 "type-parser.c"
  {

# 369 "type-parser.c"
    return pstate_propagate_error(pstate, saved_position);
  }

# 373 "type-parser.c"
  if ((!parse_type_node(pstate)))

# 373 "type-parser.c"
  {

# 374 "type-parser.c"
    return pstate_propagate_error(pstate, saved_position);
  }
  token_t* suffix = pstate_peek(pstate, 0);

# 379 "type-parser.c"
  if (((suffix->type)==TOKEN_TYPE_IDENTIFIER))

# 379 "type-parser.c"
  {

# 380 "type-parser.c"
    pstate_advance(pstate);
  }

# 383 "type-parser.c"
  return true;
}


# 186 "user-type-parser.c"
pstatus_t parse_structure_node(pstate_t* pstate)
# 186 "user-type-parser.c"
{
  uint64_t saved_position = (pstate->position);

# 188 "user-type-parser.c"
  if ((!pstate_expect_token_string(pstate, "struct")))

# 188 "user-type-parser.c"
  {

# 189 "user-type-parser.c"
    return pstate_propagate_error(pstate, saved_position);
  }
  struct_node_t* result = malloc_struct_node();

# 193 "user-type-parser.c"
  if (pstate_expect_token_type(pstate, TOKEN_TYPE_IDENTIFIER))

# 193 "user-type-parser.c"
  {

# 194 "user-type-parser.c"
    ((result->name)=pstate_get_result_token(pstate));
  }
  else

# 195 "user-type-parser.c"
  {

# 196 "user-type-parser.c"
    pstate_ignore_error(pstate);
  }

# 199 "user-type-parser.c"
  if (pstate_expect_token_string(pstate, "{"))

# 199 "user-type-parser.c"
  {

# 200 "user-type-parser.c"
    ((result->partial_definition)=false);

# 201 "user-type-parser.c"
    while (true)

# 201 "user-type-parser.c"
    {

# 202 "user-type-parser.c"
      if (pstate_expect_token_string(pstate, "}"))

# 202 "user-type-parser.c"
      {

# 203 "user-type-parser.c"
        break;
      }

# 205 "user-type-parser.c"
      pstate_ignore_error(pstate);

# 206 "user-type-parser.c"
      if ((!parse_field_node(pstate)))

# 206 "user-type-parser.c"
      {

# 207 "user-type-parser.c"
        return pstate_propagate_error(pstate, saved_position);
      }

# 209 "user-type-parser.c"
      node_list_add_node((&(result->fields)), pstate_get_result_node(pstate));
    }
  }
  else

# 211 "user-type-parser.c"
  {

# 212 "user-type-parser.c"
    ((result->partial_definition)=true);

# 213 "user-type-parser.c"
    pstate_ignore_error(pstate);
  }

# 216 "user-type-parser.c"
  return pstate_set_result_node(pstate, to_node(result));
}


# 222 "user-type-parser.c"
pstatus_t parse_field_node(pstate_t* pstate)
# 222 "user-type-parser.c"
{
  uint64_t saved_position = (pstate->position);

# 224 "user-type-parser.c"
  if ((!parse_type_node(pstate)))

# 224 "user-type-parser.c"
  {

# 225 "user-type-parser.c"
    return pstate_propagate_error(pstate, saved_position);
  }
  type_node_t* field_type = to_type_node(pstate_get_result_node(pstate));

# 228 "user-type-parser.c"
  if ((!pstate_expect_token_type(pstate, TOKEN_TYPE_IDENTIFIER)))

# 228 "user-type-parser.c"
  {

# 229 "user-type-parser.c"
    log_warn("Allowing unnamed field in case the type is a union");

# 230 "user-type-parser.c"
    pstate_ignore_error(pstate);
  }
  token_t* field_name = pstate_get_result_token(pstate);

# 234 "user-type-parser.c"
  if (pstate_expect_token_string(pstate, ":"))

# 234 "user-type-parser.c"
  {

# 236 "user-type-parser.c"
    pstate_advance(pstate);
  }
  else

# 237 "user-type-parser.c"
  {

# 238 "user-type-parser.c"
    pstate_ignore_error(pstate);
  }
  value_array_t* suffixes = ((void *)0);

# 243 "user-type-parser.c"
  while (pstate_match_token_string(pstate, "["))

# 243 "user-type-parser.c"
  {

# 244 "user-type-parser.c"
    if ((!parse_balanced_construct(pstate)))

# 244 "user-type-parser.c"
    {

# 245 "user-type-parser.c"
      return pstate_propagate_error(pstate, saved_position);
    }

# 247 "user-type-parser.c"
    if ((suffixes==((void *)0)))

# 247 "user-type-parser.c"
    {

# 248 "user-type-parser.c"
      (suffixes=make_value_array(1));
    }

# 250 "user-type-parser.c"
    value_array_add(suffixes, ptr_to_value(pstate_get_result_node(pstate)));
  }

# 253 "user-type-parser.c"
  if ((!pstate_expect_token_string(pstate, ";")))

# 253 "user-type-parser.c"
  {

# 254 "user-type-parser.c"
    return pstate_propagate_error(pstate, saved_position);
  }
  field_node_t* result = malloc_field_node();

# 257 "user-type-parser.c"
  ((result->type)=field_type);

# 258 "user-type-parser.c"
  ((result->name)=field_name);

# 259 "user-type-parser.c"
  ((result->suffixes)=suffixes);

# 260 "user-type-parser.c"
  return pstate_set_result_node(pstate, to_node(result));
}


# 266 "user-type-parser.c"
pstatus_t parse_union_node(pstate_t* pstate)
# 266 "user-type-parser.c"
{
  uint64_t saved_position = (pstate->position);

# 268 "user-type-parser.c"
  if ((!pstate_expect_token_string(pstate, "union")))

# 268 "user-type-parser.c"
  {

# 269 "user-type-parser.c"
    return pstate_propagate_error(pstate, saved_position);
  }
  union_node_t* result = malloc_union_node();

# 273 "user-type-parser.c"
  if (pstate_expect_token_type(pstate, TOKEN_TYPE_IDENTIFIER))

# 273 "user-type-parser.c"
  {

# 274 "user-type-parser.c"
    ((result->name)=pstate_get_result_token(pstate));
  }
  else

# 275 "user-type-parser.c"
  {

# 276 "user-type-parser.c"
    pstate_ignore_error(pstate);
  }

# 279 "user-type-parser.c"
  if (pstate_expect_token_string(pstate, "{"))

# 279 "user-type-parser.c"
  {

# 280 "user-type-parser.c"
    ((result->partial_definition)=false);

# 281 "user-type-parser.c"
    while (true)

# 281 "user-type-parser.c"
    {

# 282 "user-type-parser.c"
      if (pstate_expect_token_string(pstate, "}"))

# 282 "user-type-parser.c"
      {

# 283 "user-type-parser.c"
        break;
      }

# 285 "user-type-parser.c"
      pstate_ignore_error(pstate);

# 286 "user-type-parser.c"
      if ((!parse_field_node(pstate)))

# 286 "user-type-parser.c"
      {

# 287 "user-type-parser.c"
        return pstate_propagate_error(pstate, saved_position);
      }

# 289 "user-type-parser.c"
      node_list_add_node((&(result->fields)), pstate_get_result_node(pstate));
    }
  }
  else

# 291 "user-type-parser.c"
  {

# 292 "user-type-parser.c"
    ((result->partial_definition)=true);

# 293 "user-type-parser.c"
    pstate_ignore_error(pstate);
  }

# 296 "user-type-parser.c"
  return pstate_set_result_node(pstate, to_node(result));
}


# 304 "user-type-parser.c"
pstatus_t parse_user_type_node(pstate_t* pstate)
# 304 "user-type-parser.c"
{
  uint64_t saved_position = (pstate->position);

# 306 "user-type-parser.c"
  if (((parse_enum_node(pstate)||parse_structure_node(pstate_ignore_error(pstate)))||parse_union_node(pstate_ignore_error(pstate))))

# 308 "user-type-parser.c"
  {

# 309 "user-type-parser.c"
    return true;
  }
  else

# 310 "user-type-parser.c"
  {

# 311 "user-type-parser.c"
    return pstate_propagate_error(pstate, saved_position);
  }
}


# 320 "user-type-parser.c"
pstatus_t parse_enum_node(pstate_t* pstate)
# 320 "user-type-parser.c"
{
  uint64_t saved_position = (pstate->position);

# 322 "user-type-parser.c"
  if ((!pstate_expect_token_string(pstate, "enum")))

# 322 "user-type-parser.c"
  {

# 323 "user-type-parser.c"
    return pstate_propagate_error(pstate, saved_position);
  }
  enum_node_t* result = malloc_enum_node();

# 327 "user-type-parser.c"
  if (pstate_expect_token_type(pstate, TOKEN_TYPE_IDENTIFIER))

# 327 "user-type-parser.c"
  {

# 328 "user-type-parser.c"
    ((result->name)=pstate_get_result_token(pstate));
  }
  else

# 329 "user-type-parser.c"
  {

# 330 "user-type-parser.c"
    pstate_ignore_error(pstate);
  }

# 333 "user-type-parser.c"
  if ((!pstate_expect_token_string(pstate, "{")))

# 333 "user-type-parser.c"
  {

# 334 "user-type-parser.c"
    pstate_ignore_error(pstate);

# 335 "user-type-parser.c"
    ((result->partial_definition)=true);

# 336 "user-type-parser.c"
    return pstate_set_result_node(pstate, to_node(result));
  }

# 339 "user-type-parser.c"
  while (true)

# 339 "user-type-parser.c"
  {

# 340 "user-type-parser.c"
    if (pstate_expect_token_string(pstate, "}"))

# 340 "user-type-parser.c"
    {

# 341 "user-type-parser.c"
      break;
    }

# 343 "user-type-parser.c"
    pstate_ignore_error(pstate);

# 344 "user-type-parser.c"
    if ((!parse_enum_element_node(pstate)))

# 344 "user-type-parser.c"
    {

# 345 "user-type-parser.c"
      return pstate_propagate_error(pstate, saved_position);
    }

# 347 "user-type-parser.c"
    node_list_add_node((&(result->elements)), pstate_get_result_node(pstate));
  }

# 350 "user-type-parser.c"
  return pstate_set_result_node(pstate, to_node(result));
}


# 359 "user-type-parser.c"
pstatus_t parse_enum_element_node(pstate_t* pstate)
# 359 "user-type-parser.c"
{
  uint64_t saved_position = (pstate->position);

# 361 "user-type-parser.c"
  if ((!pstate_expect_token_type(pstate, TOKEN_TYPE_IDENTIFIER)))

# 361 "user-type-parser.c"
  {

# 362 "user-type-parser.c"
    return pstate_propagate_error(pstate, saved_position);
  }
  token_t* name = pstate_get_result_token(pstate);
  parse_node_t* value_expr = ((void *)0);

# 366 "user-type-parser.c"
  if (pstate_expect_token_string(pstate, "="))

# 366 "user-type-parser.c"
  {

# 367 "user-type-parser.c"
    if ((!pratt_parse_expression(pstate, 0)))

# 367 "user-type-parser.c"
    {

# 368 "user-type-parser.c"
      return pstate_propagate_error(pstate, saved_position);
    }

# 370 "user-type-parser.c"
    (value_expr=pstate_get_result_node(pstate));
  }

# 373 "user-type-parser.c"
  if ((!pstate_expect_token_string(pstate, ",")))

# 373 "user-type-parser.c"
  {

# 374 "user-type-parser.c"
    pstate_ignore_error(pstate);
  }
  enum_element_t* result = malloc_enum_element();

# 378 "user-type-parser.c"
  ((result->name)=name);

# 379 "user-type-parser.c"
  ((result->value_expr)=value_expr);

# 381 "user-type-parser.c"
  return pstate_set_result_node(pstate, to_node(result));
}


# 45 "variable-definition-parser.c"
pstatus_t parse_expression(pstate_t* pstate)
# 45 "variable-definition-parser.c"
{

# 46 "variable-definition-parser.c"
  return pratt_parse_expression(pstate, 0);
}


# 54 "variable-definition-parser.c"
pstatus_t parse_initializer(pstate_t* pstate)
# 54 "variable-definition-parser.c"
{
  uint64_t saved_position = (pstate->position);

# 57 "variable-definition-parser.c"
  if (token_matches(pstate_peek(pstate, 0), "{"))

# 57 "variable-definition-parser.c"
  {

# 58 "variable-definition-parser.c"
    return parse_balanced_construct(pstate);
  }

# 60 "variable-definition-parser.c"
  return pstate_error(pstate, saved_position, PARSE_ERROR_CLOSE_BRACKET_EXPECTED);
}


# 70 "variable-definition-parser.c"
pstatus_t parse_variable_definition_node(pstate_t* pstate)
# 70 "variable-definition-parser.c"
{
  uint64_t saved_position = (pstate->position);
  token_t* storage_class_specifier = ((void *)0);

# 74 "variable-definition-parser.c"
  while ((pstate_expect_token_string(pstate, "static")||pstate_expect_token_string(pstate_ignore_error(pstate), "extern")))

# 75 "variable-definition-parser.c"
  {

# 76 "variable-definition-parser.c"
    if ((storage_class_specifier!=((void *)0)))

# 76 "variable-definition-parser.c"
    {

# 77 "variable-definition-parser.c"
      return pstate_error(pstate, saved_position, PARSE_ERROR_CONFLICTING_STORAGE_CLASS_SPECIFIER);
    }

# 80 "variable-definition-parser.c"
    (storage_class_specifier=pstate_get_result_token(pstate));
  }

# 82 "variable-definition-parser.c"
  pstate_ignore_error(pstate);

# 83 "variable-definition-parser.c"
  if ((!parse_type_node(pstate)))

# 83 "variable-definition-parser.c"
  {

# 84 "variable-definition-parser.c"
    return pstate_propagate_error(pstate, saved_position);
  }
  type_node_t* type_node = to_type_node(pstate_get_result_node(pstate));

# 87 "variable-definition-parser.c"
  if ((!pstate_expect_token_type(pstate, TOKEN_TYPE_IDENTIFIER)))

# 87 "variable-definition-parser.c"
  {

# 88 "variable-definition-parser.c"
    return pstate_error(pstate, saved_position, PARSE_ERROR_IDENTIFIER_EXPECTED);
  }
  token_t* name = pstate_get_result_token(pstate);
  variable_definition_node_t* result = malloc_variable_definition_node();

# 94 "variable-definition-parser.c"
  ((result->type)=type_node);

# 95 "variable-definition-parser.c"
  ((result->name)=name);

# 97 "variable-definition-parser.c"
  while (pstate_match_token_string(pstate, "["))

# 97 "variable-definition-parser.c"
  {

# 98 "variable-definition-parser.c"
    if ((!parse_balanced_construct(pstate)))

# 98 "variable-definition-parser.c"
    {

# 99 "variable-definition-parser.c"
      return pstate_propagate_error(pstate, saved_position);
    }

# 101 "variable-definition-parser.c"
    if (((result->suffixes)==((void *)0)))

# 101 "variable-definition-parser.c"
    {

# 102 "variable-definition-parser.c"
      ((result->suffixes)=make_value_array(1));
    }

# 104 "variable-definition-parser.c"
    value_array_add((result->suffixes), ptr_to_value(pstate_get_result_node(pstate)));
  }

# 108 "variable-definition-parser.c"
  if (pstate_expect_token_string(pstate, "="))

# 108 "variable-definition-parser.c"
  {

# 109 "variable-definition-parser.c"
    if ((parse_initializer(pstate)||parse_expression(pstate_ignore_error(pstate))))

# 110 "variable-definition-parser.c"
    {

# 111 "variable-definition-parser.c"
      ((result->value)=pstate_get_result_node(pstate));
    }
    else

# 112 "variable-definition-parser.c"
    {

# 113 "variable-definition-parser.c"
      return pstate_propagate_error(pstate, saved_position);
    }
  }
  else

# 115 "variable-definition-parser.c"
  {

# 116 "variable-definition-parser.c"
    pstate_ignore_error(pstate);
  }

# 119 "variable-definition-parser.c"
  if ((!pstate_expect_token_string(pstate, ";")))

# 119 "variable-definition-parser.c"
  {

# 120 "variable-definition-parser.c"
    return pstate_propagate_error(pstate, saved_position);
  }

# 123 "variable-definition-parser.c"
  return pstate_set_result_node(pstate, to_node(result));
}


# 57 "literal-parser.c"
pstatus_t parse_literal_node(pstate_t* pstate)
# 57 "literal-parser.c"
{
  uint64_t saved_position = (pstate->position);

# 59 "literal-parser.c"
  if (pstate_expect_token_type(pstate, TOKEN_TYPE_STRING_LITERAL))

# 59 "literal-parser.c"
  {
    literal_node_t* result = malloc_literal_node();

# 61 "literal-parser.c"
    ((result->tokens)=make_value_array(1));

# 62 "literal-parser.c"
    value_array_add((result->tokens), ptr_to_value(pstate_get_result_token(pstate)));

# 64 "literal-parser.c"
    while (pstate_expect_token_type(pstate, TOKEN_TYPE_STRING_LITERAL))

# 64 "literal-parser.c"
    {

# 65 "literal-parser.c"
      value_array_add((result->tokens), ptr_to_value(pstate_get_result_token(pstate)));
    }

# 68 "literal-parser.c"
    pstate_ignore_error(pstate);

# 69 "literal-parser.c"
    return pstate_set_result_node(pstate, to_node(result));
  }

# 72 "literal-parser.c"
  if (((pstate_expect_token_type(pstate_ignore_error(pstate), TOKEN_TYPE_INTEGER_LITERAL)||pstate_expect_token_type(pstate_ignore_error(pstate), TOKEN_TYPE_FLOAT_LITERAL))||pstate_expect_token_type(pstate_ignore_error(pstate), TOKEN_TYPE_CHARACTER_LITERAL)))

# 77 "literal-parser.c"
  {
    literal_node_t* result = malloc_literal_node();

# 79 "literal-parser.c"
    ((result->tokens)=make_value_array(1));

# 80 "literal-parser.c"
    value_array_add((result->tokens), ptr_to_value(pstate_get_result_token(pstate)));

# 82 "literal-parser.c"
    return pstate_set_result_node(pstate, to_node(result));
  }
  else

# 83 "literal-parser.c"
  {

# 84 "literal-parser.c"
    pstate_ignore_error(pstate);
  }

# 87 "literal-parser.c"
  if (pstate_match_token_string(pstate, "{"))

# 87 "literal-parser.c"
  {

# 88 "literal-parser.c"
    if ((!parse_balanced_construct(pstate)))

# 88 "literal-parser.c"
    {

# 89 "literal-parser.c"
      return pstate_propagate_error(pstate, saved_position);
    }
    literal_node_t* result = malloc_literal_node();

# 92 "literal-parser.c"
    ((result->initializer_node)=pstate_get_result_node(pstate));

# 93 "literal-parser.c"
    return pstate_set_result_node(pstate, to_node(result));
  }

# 99 "literal-parser.c"
  if (pstate_match_token_string(pstate, "compound_literal"))

# 99 "literal-parser.c"
  {

# 100 "literal-parser.c"
    pstate_advance(pstate);

# 101 "literal-parser.c"
    if (((!pstate_expect_token_string(pstate, "("))||(!parse_type_node(pstate))))

# 101 "literal-parser.c"
    {

# 102 "literal-parser.c"
      return pstate_propagate_error(pstate, saved_position);
    }
    parse_node_t* type_node = pstate_get_result_node(pstate);

# 105 "literal-parser.c"
    if ((!pstate_expect_token_string(pstate, ",")))

# 105 "literal-parser.c"
    {

# 106 "literal-parser.c"
      return pstate_propagate_error(pstate, saved_position);
    }

# 108 "literal-parser.c"
    if ((!parse_balanced_construct(pstate)))

# 108 "literal-parser.c"
    {

# 109 "literal-parser.c"
      return pstate_propagate_error(pstate, saved_position);
    }
    parse_node_t* initializer_node = pstate_get_result_node(pstate);

# 112 "literal-parser.c"
    if ((!pstate_expect_token_string(pstate, ")")))

# 112 "literal-parser.c"
    {

# 113 "literal-parser.c"
      return pstate_propagate_error(pstate, saved_position);
    }
    literal_node_t* result = malloc_literal_node();

# 117 "literal-parser.c"
    ((result->initializer_node)=initializer_node);

# 118 "literal-parser.c"
    ((result->initializer_type)=type_node);

# 119 "literal-parser.c"
    return pstate_set_result_node(pstate, to_node(result));
  }

# 122 "literal-parser.c"
  if (parse_compound_literal(pstate))

# 122 "literal-parser.c"
  {

# 123 "literal-parser.c"
    return true;
  }
  else

# 124 "literal-parser.c"
  {

# 125 "literal-parser.c"
    pstate_ignore_error(pstate);
  }

# 128 "literal-parser.c"
  if (((((((pstate_expect_token_string(pstate, "NULL")||pstate_expect_token_string(pstate_ignore_error(pstate), "nullptr"))||pstate_expect_token_string(pstate_ignore_error(pstate), "true"))||pstate_expect_token_string(pstate_ignore_error(pstate), "false"))||pstate_expect_token_type(pstate_ignore_error(pstate), TOKEN_TYPE_INTEGER_LITERAL))||pstate_expect_token_type(pstate_ignore_error(pstate), TOKEN_TYPE_FLOAT_LITERAL))||pstate_expect_token_type(pstate_ignore_error(pstate), TOKEN_TYPE_CHARACTER_LITERAL)))

# 137 "literal-parser.c"
  {
    literal_node_t* result = malloc_literal_node();

# 139 "literal-parser.c"
    ((result->token)=pstate_get_result_token(pstate));

# 140 "literal-parser.c"
    return pstate_set_result_node(pstate, to_node(result));
  }

# 143 "literal-parser.c"
  return pstate_error(pstate, saved_position, PARSE_ERROR_NOT_LITERAL_NODE);
}


# 147 "literal-parser.c"
pstatus_t parse_compound_literal(pstate_t* pstate)
# 147 "literal-parser.c"
{
  uint64_t saved_position = (pstate->position);

# 149 "literal-parser.c"
  if ((!pstate_expect_token_string(pstate, "(")))

# 149 "literal-parser.c"
  {

# 150 "literal-parser.c"
    return pstate_propagate_error(pstate, saved_position);
  }

# 152 "literal-parser.c"
  if ((!parse_type_node(pstate)))

# 152 "literal-parser.c"
  {

# 153 "literal-parser.c"
    return pstate_propagate_error(pstate, saved_position);
  }
  parse_node_t* type_node = pstate_get_result_node(pstate);

# 156 "literal-parser.c"
  if ((!pstate_expect_token_string(pstate, ")")))

# 156 "literal-parser.c"
  {

# 157 "literal-parser.c"
    return pstate_propagate_error(pstate, saved_position);
  }

# 159 "literal-parser.c"
  if ((!pstate_expect_token_string(pstate, "{")))

# 159 "literal-parser.c"
  {

# 160 "literal-parser.c"
    return pstate_propagate_error(pstate, saved_position);
  }
  node_list_t initializers = ((node_list_t) {0});

# 164 "literal-parser.c"
  while (true)

# 164 "literal-parser.c"
  {

# 165 "literal-parser.c"
    if (pstate_is_eof(pstate))

# 165 "literal-parser.c"
    {

# 166 "literal-parser.c"
      return pstate_propagate_error(pstate, saved_position);
    }

# 168 "literal-parser.c"
    if (pstate_match_token_string(pstate, "}"))

# 168 "literal-parser.c"
    {

# 169 "literal-parser.c"
      pstate_advance(pstate);

# 170 "literal-parser.c"
      break;
    }

# 173 "literal-parser.c"
    if ((node_list_length(initializers)>0))

# 173 "literal-parser.c"
    {

# 174 "literal-parser.c"
      if (pstate_match_token_string(pstate, ","))

# 174 "literal-parser.c"
      {

# 175 "literal-parser.c"
        pstate_advance(pstate);
      }
      else

# 176 "literal-parser.c"
      {

# 177 "literal-parser.c"
        return pstate_propagate_error(pstate, saved_position);
      }
    }

# 181 "literal-parser.c"
    if (parse_designated_initializer_node(pstate))

# 181 "literal-parser.c"
    {
      parse_node_t* initializer = pstate_get_result_node(pstate);

# 183 "literal-parser.c"
      node_list_add_node((&initializers), initializer);
    }
    else

# 184 "literal-parser.c"
    if (parse_literal_node(pstate))

# 184 "literal-parser.c"
    {
      parse_node_t* initializer = pstate_get_result_node(pstate);

# 186 "literal-parser.c"
      node_list_add_node((&initializers), initializer);
    }
  }
  compound_literal_node_t* result = malloc_compound_literal_node();

# 191 "literal-parser.c"
  ((result->type_node)=type_node);

# 192 "literal-parser.c"
  ((result->initializers)=initializers);

# 193 "literal-parser.c"
  pstate_set_result_node(pstate, to_node(result));

# 195 "literal-parser.c"
  return true;
}


# 199 "literal-parser.c"
pstatus_t parse_designated_initializer_node(pstate_t* pstate)
# 199 "literal-parser.c"
{
  uint64_t saved_position = (pstate->position);

# 204 "literal-parser.c"
  if ((!pstate_expect_token_string(pstate, ".")))

# 204 "literal-parser.c"
  {

# 205 "literal-parser.c"
    return pstate_propagate_error(pstate, saved_position);
  }

# 207 "literal-parser.c"
  if ((!pstate_expect_token_type(pstate, TOKEN_TYPE_IDENTIFIER)))

# 207 "literal-parser.c"
  {

# 208 "literal-parser.c"
    return pstate_propagate_error(pstate, saved_position);
  }
  token_t* member_name = pstate_get_result_token(pstate);

# 213 "literal-parser.c"
  if ((!pstate_expect_token_string(pstate, "=")))

# 213 "literal-parser.c"
  {

# 214 "literal-parser.c"
    return pstate_propagate_error(pstate, saved_position);
  }

# 217 "literal-parser.c"
  if ((!parse_expression(pstate)))

# 217 "literal-parser.c"
  {

# 218 "literal-parser.c"
    return pstate_propagate_error(pstate, saved_position);
  }
  designated_initializer_node_t* result = malloc_designated_initializer_node();

# 222 "literal-parser.c"
  ((result->member_name)=member_name);

# 223 "literal-parser.c"
  ((result->value)=pstate_get_result_node(pstate));

# 224 "literal-parser.c"
  pstate_set_result_node(pstate, to_node(result));

# 226 "literal-parser.c"
  return true;
}


# 48 "balanced-construct-parser.c"
pstatus_t parse_balanced_construct(pstate_t* pstate)
# 48 "balanced-construct-parser.c"
{
  uint64_t saved_position = (pstate->position);
  int open_parens = 0;
  int open_brackets = 0;
  int open_braces = 0;

# 54 "balanced-construct-parser.c"
  do
# 54 "balanced-construct-parser.c"
  {

# 55 "balanced-construct-parser.c"
    if (pstate_expect_token_string(pstate, "("))

# 55 "balanced-construct-parser.c"
    {

# 56 "balanced-construct-parser.c"
      (open_parens++);
    }
    else

# 57 "balanced-construct-parser.c"
    if (pstate_expect_token_string(pstate_ignore_error(pstate), "["))

# 57 "balanced-construct-parser.c"
    {

# 58 "balanced-construct-parser.c"
      (open_brackets++);
    }
    else

# 59 "balanced-construct-parser.c"
    if (pstate_expect_token_string(pstate_ignore_error(pstate), "{"))

# 59 "balanced-construct-parser.c"
    {

# 60 "balanced-construct-parser.c"
      (open_braces++);
    }
    else

# 61 "balanced-construct-parser.c"
    if (pstate_expect_token_string(pstate_ignore_error(pstate), ")"))

# 61 "balanced-construct-parser.c"
    {

# 62 "balanced-construct-parser.c"
      (open_parens--);
    }
    else

# 63 "balanced-construct-parser.c"
    if (pstate_expect_token_string(pstate_ignore_error(pstate), "]"))

# 63 "balanced-construct-parser.c"
    {

# 64 "balanced-construct-parser.c"
      (open_brackets--);
    }
    else

# 65 "balanced-construct-parser.c"
    if (pstate_expect_token_string(pstate_ignore_error(pstate), "}"))

# 65 "balanced-construct-parser.c"
    {

# 66 "balanced-construct-parser.c"
      (open_braces--);
    }
    else

# 67 "balanced-construct-parser.c"
    {

# 68 "balanced-construct-parser.c"
      pstate_advance(pstate_ignore_error(pstate));
    }
  }
  while (((((open_parens+open_brackets)+open_braces)>0)&&((pstate->position)<((pstate->tokens)->length))));

# 73 "balanced-construct-parser.c"
  if (((pstate->position)==(saved_position+1)))

# 73 "balanced-construct-parser.c"
  {

# 75 "balanced-construct-parser.c"
    return pstate_error(pstate, saved_position, PARSE_ERROR_OPEN_BRACE_EXPECTED);
  }
  balanced_construct_node_t* result = malloc_balanced_construct_node();

# 80 "balanced-construct-parser.c"
  ((result->start_token)=token_at((pstate->tokens), saved_position));

# 81 "balanced-construct-parser.c"
  ((result->end_token)=pstate_peek(pstate, (-1)));

# 83 "balanced-construct-parser.c"
  return pstate_set_result_node(pstate, to_node(result));
}


# 21 "printer.c"
printer_t* make_printer(buffer_t* buffer, symbol_table_t* symbol_table, int indent_width)
# 22 "printer.c"
{
  printer_t* result = malloc_struct(printer_t);

# 24 "printer.c"
  ((result->symbol_table)=symbol_table);

# 25 "printer.c"
  ((result->buffer)=buffer);

# 26 "printer.c"
  ((result->indent_width)=indent_width);

# 27 "printer.c"
  ((result->convert_nullptr)=true);

# 28 "printer.c"
  ((result->output_line_directives)=true);

# 29 "printer.c"
  return result;
}


# 32 "printer.c"
printer_t* append_string(printer_t* printer, char* string)
# 32 "printer.c"
{

# 33 "printer.c"
  buffer_append_string((printer->buffer), string);

# 34 "printer.c"
  return printer;
}


# 37 "printer.c"
printer_t* append_token(printer_t* printer, token_t* token)
# 37 "printer.c"
{

# 38 "printer.c"
  buffer_append_sub_buffer((printer->buffer), (token->start), (token->end), (token->buffer));

# 40 "printer.c"
  return printer;
}


# 43 "printer.c"
printer_t* printer_newline(printer_t* printer)
# 43 "printer.c"
{

# 44 "printer.c"
  buffer_append_byte((printer->buffer), '\n');

# 45 "printer.c"
  return printer;
}


# 48 "printer.c"
printer_t* printer_space(printer_t* printer)
# 48 "printer.c"
{

# 49 "printer.c"
  buffer_append_byte((printer->buffer), ' ');

# 50 "printer.c"
  return printer;
}


# 53 "printer.c"
printer_t* printer_indent(printer_t* printer)
# 53 "printer.c"
{

# 54 "printer.c"
  buffer_append_repeated_byte((printer->buffer), ' ', ((printer->indent_width)*(printer->indent_level)));

# 56 "printer.c"
  return printer;
}


# 59 "printer.c"
printer_t* printer_increase_indent(printer_t* printer)
# 59 "printer.c"
{

# 60 "printer.c"
  ((printer->indent_level)++);

# 61 "printer.c"
  return printer;
}


# 64 "printer.c"
printer_t* printer_decrease_indent(printer_t* printer)
# 64 "printer.c"
{

# 65 "printer.c"
  ((printer->indent_level)--);

# 66 "printer.c"
  return printer;
}


# 36 "main.c"
void do_print_tokens(value_array_t* tokens, char* message)
# 36 "main.c"
{

# 37 "main.c"
  if (FLAG_print_tokens_show_tokens)

# 37 "main.c"
  {
    buffer_t* buffer = make_buffer(((tokens->length)*60));

# 42 "main.c"
    for (
      int j = 0;
      (j<(tokens->length));
      (j++))

# 42 "main.c"
    {
      token_t* token = token_at(tokens, j);

# 44 "main.c"
      (buffer=append_token_debug_string(buffer, (*token)));

# 45 "main.c"
      (buffer=buffer_append_string(buffer, "\n"));
    }

# 47 "main.c"
    fprintf(stdout, "** %s **\n%s\n", message, buffer_to_c_string(buffer));
  }
}


# 51 "main.c"
void print_tokens(void)
# 51 "main.c"
{

# 52 "main.c"
  log_info("print_tokens()");
  value_array_t* files = read_files(FLAG_files);

# 55 "main.c"
  for (
    int i = 0;
    (i<(FLAG_files->length));
    (i++))

# 55 "main.c"
  {
    file_t* file = value_array_get_ptr(files, i, typeof(file_t*));

# 58 "main.c"
    fprintf(stdout, "====================================================\n");

# 59 "main.c"
    fprintf(stdout, "====> Processing %s\n", (file->file_name));

# 60 "main.c"
    fprintf(stdout, "====================================================\n");
    tokenizer_result_t tokenizer_result = tokenize((file->data));

# 64 "main.c"
    if ((tokenizer_result.tokenizer_error_code))

# 64 "main.c"
    {

# 65 "main.c"
      log_warn("Tokenizer error: \"%s\"::%d -- %d", (value_array_get(FLAG_files, i).str), (tokenizer_result.tokenizer_error_position), (tokenizer_result.tokenizer_error_code));

# 69 "main.c"
      continue;
    }
    value_array_t* tokens = (tokenizer_result.tokens);

# 74 "main.c"
    if (FLAG_print_tokens_show_appended_tokens)

# 74 "main.c"
    {
      buffer_t* appended_tokens = make_buffer(1);

# 76 "main.c"
      debug_append_tokens(appended_tokens, tokens);

# 77 "main.c"
      fprintf(stdout, "%s", buffer_to_c_string(appended_tokens));
    }

# 80 "main.c"
    do_print_tokens(tokens, "before xform tokens");

# 82 "main.c"
    (tokens=transform_tokens(tokens, ((token_transformer_options_t) {
                .keep_whitespace = FLAG_print_tokens_include_whitespace,
                .keep_comments = FLAG_print_tokens_include_comments,
                .keep_javadoc_comments = FLAG_print_tokens_include_comments,
                .keep_c_preprocessor_lines = false,
            })));

# 93 "main.c"
    do_print_tokens(tokens, "after xform tokens");

# 95 "main.c"
    if (FLAG_print_tokens_parse_and_print)

# 95 "main.c"
    {
      pstate_t pstate = ((pstate_t) {.tokens = tokens,
                     .use_statement_parser = FLAG_use_statement_parser});

# 99 "main.c"
      ;

# 100 "main.c"
      if ((!parse_declarations((&pstate))))

# 100 "main.c"
      {

# 101 "main.c"
        (((pstate.error).file_name)=(file->file_name));
        buffer_t* buffer = make_buffer(1);

# 103 "main.c"
        (buffer=buffer_append_human_readable_error(buffer, (&(pstate.error))));

# 104 "main.c"
        log_fatal("%s", buffer_to_c_string(buffer));

# 105 "main.c"
        fatal_error(ERROR_ILLEGAL_INPUT);
      }
      else

# 106 "main.c"
      {
        buffer_t* buffer = make_buffer(1024);

# 108 "main.c"
        buffer_append_dbg_parse_node(make_cdl_printer(buffer), pstate_get_result_node((&pstate)));

# 110 "main.c"
        fprintf(stdout, "** Parse Nodes %s **\n%s\n", (file->file_name), buffer_to_c_string(buffer));
      }
    }
  }
}


# 117 "main.c"
void configure_flags(void)
# 117 "main.c"
{

# 118 "main.c"
  flag_program_name("omni-c");

# 119 "main.c"
  flag_description("omni-c is a transpiler for the omni-c language as well as a code " "generation tool for ISO C.");

# 124 "main.c"
  flag_boolean("--print-command-line", (&FLAG_print_command_line));

# 125 "main.c"
  flag_boolean("--use-statement-parser", (&FLAG_use_statement_parser));

# 127 "main.c"
  configure_regular_commands();

# 129 "main.c"
  configure_print_tokens_command();

# 130 "main.c"
  configure_parse_expression();

# 131 "main.c"
  configure_parse_statement();
}


# 134 "main.c"
void configure_parse_expression(void)
# 134 "main.c"
{

# 135 "main.c"
  flag_command("parse-expression", (&FLAG_command));

# 136 "main.c"
  flag_description("** UNIT TESTING **");

# 137 "main.c"
  flag_string("--expression", (&FLAG_expression));

# 138 "main.c"
  flag_boolean("--to-c", (&FLAG_to_c));
}


# 141 "main.c"
void configure_parse_statement(void)
# 141 "main.c"
{

# 142 "main.c"
  flag_command("parse-statement", (&FLAG_command));

# 143 "main.c"
  flag_description("** UNIT TESTING **");

# 144 "main.c"
  flag_string("--statement", (&FLAG_statement));
}


# 147 "main.c"
void configure_print_tokens_command(void)
# 147 "main.c"
{

# 148 "main.c"
  flag_command("print-tokens", (&FLAG_command));

# 149 "main.c"
  flag_description("** UNIT TESTING **");

# 150 "main.c"
  flag_boolean("--show-tokens", (&FLAG_print_tokens_show_tokens));

# 151 "main.c"
  flag_boolean("--include-whitespace", (&FLAG_print_tokens_include_whitespace));

# 152 "main.c"
  flag_boolean("--include-comments", (&FLAG_print_tokens_include_comments));

# 153 "main.c"
  flag_boolean("--parse-and-print", (&FLAG_print_tokens_parse_and_print));

# 154 "main.c"
  flag_boolean("--show-appended-tokens", (&FLAG_print_tokens_show_appended_tokens));

# 156 "main.c"
  flag_file_args((&FLAG_files));
}


# 159 "main.c"
void configure_regular_commands(void)
# 159 "main.c"
{

# 160 "main.c"
  flag_command("generate-header-file", (&FLAG_command));

# 161 "main.c"
  flag_description("create a single C file 'library header file'; most users will prefer " "'build'");

# 164 "main.c"
  flag_string("--c-output-file", (&FLAG_c_output_file));

# 165 "main.c"
  flag_boolean("--generate-enum-convertors", (&FLAG_generate_enum_convertors));

# 166 "main.c"
  flag_boolean("--dump-symbol-table", (&FLAG_dump_symbol_table));

# 167 "main.c"
  flag_boolean("--use-statement-parser", (&FLAG_use_statement_parser));

# 168 "main.c"
  flag_boolean("--omit-c-armyknife-include", (&FLAG_omit_c_armyknife_include));

# 170 "main.c"
  flag_file_args((&FLAG_files));

# 172 "main.c"
  flag_command("generate-library", (&FLAG_command));

# 173 "main.c"
  flag_description("create a single C file 'library' of C99 code; most users will prefer " "'build'");

# 176 "main.c"
  flag_string("--c-output-file", (&FLAG_c_output_file));

# 177 "main.c"
  flag_boolean("--generate-enum-convertors", (&FLAG_generate_enum_convertors));

# 178 "main.c"
  flag_boolean("--dump-symbol-table", (&FLAG_dump_symbol_table));

# 179 "main.c"
  flag_boolean("--use-statement-parser", (&FLAG_use_statement_parser));

# 180 "main.c"
  flag_boolean("--omit-c-armyknife-include", (&FLAG_omit_c_armyknife_include));

# 181 "main.c"
  flag_file_args((&FLAG_files));

# 183 "main.c"
  flag_command("build", (&FLAG_command));

# 184 "main.c"
  flag_description("build an executable by generating the C code and invoking the C " "compiler");

# 187 "main.c"
  flag_string("--c-output-file", (&FLAG_c_output_file));

# 188 "main.c"
  flag_string("--binary-output-file", (&FLAG_binary_output_file));

# 189 "main.c"
  flag_boolean("--generate-enum-convertors", (&FLAG_generate_enum_convertors));

# 190 "main.c"
  flag_boolean("--dump-symbol-table", (&FLAG_dump_symbol_table));

# 191 "main.c"
  flag_boolean("--use-statement-parser", (&FLAG_use_statement_parser));

# 192 "main.c"
  flag_boolean("--omit-c-armyknife-include", (&FLAG_omit_c_armyknife_include));

# 193 "main.c"
  flag_string("--c-compiler", (&FLAG_c_compiler));

# 194 "main.c"
  flag_file_args((&FLAG_files));

# 196 "main.c"
  flag_command("archive", (&FLAG_command));

# 197 "main.c"
  flag_description("create an archive of unprocessed source files");

# 198 "main.c"
  flag_string("--archive-output-file", (&FLAG_archive_output_file));

# 199 "main.c"
  flag_description("the target path of the output archive");

# 200 "main.c"
  flag_file_args((&FLAG_files));
}


# 203 "main.c"
boolean_t is_inlined_function(function_node_t* node)
# 203 "main.c"
{

# 204 "main.c"
  return (token_matches((node->storage_class_specifier), "static")&&token_list_contains((node->function_specifiers), "inline"));
}


# 208 "main.c"
void dump_symbol_table(char* phase_name, symbol_table_t* symbol_table)
# 208 "main.c"
{

# 209 "main.c"
  if (FLAG_dump_symbol_table)

# 209 "main.c"
  {

# 210 "main.c"
    fprintf(stderr, "==================================================================" "====\n");

# 213 "main.c"
    fprintf(stderr, "%s\n", phase_name);

# 214 "main.c"
    fprintf(stderr, "==================================================================" "====\n\n");
    buffer_t* buffer = make_buffer(128);

# 218 "main.c"
    (buffer=symbol_table_stats(buffer, symbol_table));

# 219 "main.c"
    buffer_append_dgb_symbol_table(make_cdl_printer(buffer), symbol_table);

# 221 "main.c"
    fprintf(stderr, "%s", buffer_to_c_string(buffer));
  }
}


# 225 "main.c"
char* include_node_to_string(cpp_include_node_t* node)
# 225 "main.c"
{
  buffer_t* buffer = make_buffer(32);
  printer_t* printer = make_printer(buffer, make_symbol_table(), 2);

# 228 "main.c"
  append_cpp_include_node(printer, node);
  char* include_statement = buffer_to_c_string(buffer);

# 230 "main.c"
  return include_statement;
}


# 239 "main.c"
void generate_c_output_file(boolean_t is_library, buffer_t* command_line_overview_comment)
# 240 "main.c"
{
  boolean_t is_header_file = (!is_library);
  symbol_table_t* symbol_table = make_symbol_table();

# 245 "main.c"
  parse_and_add_top_level_definitions(symbol_table, FLAG_files, FLAG_use_statement_parser);

# 247 "main.c"
  dump_symbol_table("initial parse", symbol_table);

# 248 "main.c"
  if (FLAG_generate_enum_convertors)

# 248 "main.c"
  {

# 249 "main.c"
    srcgen_enum_to_string_converters(symbol_table);

# 250 "main.c"
    dump_symbol_table("enum to string generation", symbol_table);
  }

# 252 "main.c"
  split_structure_typedefs(symbol_table);

# 253 "main.c"
  dump_symbol_table("split structures", symbol_table);

# 254 "main.c"
  reorder_symbol_table_typedefs(symbol_table);

# 255 "main.c"
  dump_symbol_table("reorder typedefs", symbol_table);

# 256 "main.c"
  reorder_symbol_table_structures(symbol_table);

# 257 "main.c"
  dump_symbol_table("reorder structures", symbol_table);
  buffer_t* buffer = make_buffer((1024*8));
  printer_t* printer = make_printer(buffer, symbol_table, 2);

# 262 "main.c"
  add_generated_c_file_header(buffer);
  char* guard_name = "_HEADER_FILE_GUARD_";

# 266 "main.c"
  if (is_header_file)

# 266 "main.c"
  {

# 267 "main.c"
    buffer_printf(buffer, "#ifndef %s\n#define %s\n\n", guard_name, guard_name);
  }

# 270 "main.c"
  buffer_append_buffer(buffer, get_reflection_header_buffer());
  boolean_t append_newline_after_system_includes = false;

# 273 "main.c"
  buffer_append_string(buffer, "// ========== system includes ==========\n\n");
  string_hashtable_t* system_includes_set = make_string_hashtable(19);

# 275 "main.c"
  for (
    uint64_t i = 0;
    (i<((symbol_table->system_includes)->length));
    (i++))

# 275 "main.c"
  {

# 276 "main.c"
    (append_newline_after_system_includes=true);
    cpp_include_node_t* node = value_array_get_ptr((symbol_table->system_includes), i, typeof(cpp_include_node_t*));
    char* include_statement = include_node_to_string(node);

# 280 "main.c"
    if ((FLAG_omit_c_armyknife_include&&string_starts_with(include_statement, "#include <c-armyknife-lib")))

# 281 "main.c"
    {

# 282 "main.c"
      continue;
    }

# 284 "main.c"
    if ((!is_ok(string_ht_find(system_includes_set, include_statement))))

# 284 "main.c"
    {

# 285 "main.c"
      (system_includes_set=string_ht_insert(system_includes_set, include_statement, boolean_to_value(true)));

# 287 "main.c"
      buffer_append_string(buffer, include_statement);
    }
  }

# 290 "main.c"
  if (append_newline_after_system_includes)

# 290 "main.c"
  {

# 291 "main.c"
    buffer_append_string(buffer, "\n");
  }

# 296 "main.c"
  buffer_append_string(buffer, "// ========== defines ==========\n\n");

# 297 "main.c"
  for (
    uint64_t i = 0;
    (i<((symbol_table->defines)->length));
    (i++))

# 297 "main.c"
  {
    cpp_define_node_t* node = value_array_get_ptr((symbol_table->defines), i, typeof(cpp_define_node_t*));

# 300 "main.c"
    append_cpp_define_node(printer, node);

# 301 "main.c"
    append_string(printer, "\n");
  }

# 304 "main.c"
  buffer_append_string(buffer, "// ========== enums ==========\n\n");

# 306 "main.c"
  for (
    int i = 0;
    (i<(((symbol_table->enums)->ordered_bindings)->length));
    (i++))

# 306 "main.c"
  {
    symbol_table_binding_t* binding = value_array_get_ptr(((symbol_table->enums)->ordered_bindings), i, typeof(symbol_table_binding_t*));
    enum_node_t* enum_node = to_enum_node(value_array_get_ptr((binding->definition_nodes), 0, typeof(parse_node_t*)));

# 312 "main.c"
    append_enum_node(printer, enum_node);

# 313 "main.c"
    append_string(printer, ";\n\n");
  }

# 316 "main.c"
  buffer_append_string(buffer, "// ========== typedefs ==========\n\n");

# 317 "main.c"
  for (
    int i = 0;
    (i<(((symbol_table->typedefs)->ordered_bindings)->length));
    (i++))

# 317 "main.c"
  {
    symbol_table_binding_t* binding = value_array_get_ptr(((symbol_table->typedefs)->ordered_bindings), i, typeof(symbol_table_binding_t*));
    typedef_node_t* typedef_node = to_typedef_node((/*CAST*/(parse_node_t*) (value_array_get((binding->definition_nodes), 0).ptr)));

# 323 "main.c"
    append_typedef_node(printer, typedef_node);

# 324 "main.c"
    append_string(printer, "\n");
  }

# 327 "main.c"
  buffer_append_string(buffer, "// ========== stuctures/unions ==========\n\n");

# 328 "main.c"
  for (
    int i = 0;
    (i<(((symbol_table->structures)->ordered_bindings)->length));
    (i++))

# 328 "main.c"
  {
    symbol_table_binding_t* binding = value_array_get_ptr(((symbol_table->structures)->ordered_bindings), i, typeof(symbol_table_binding_t*));
    struct_node_t* struct_node = get_full_structure_definition_node(binding);

# 333 "main.c"
    if ((struct_node==((void *)0)))

# 333 "main.c"
    {

# 334 "main.c"
      (struct_node=value_array_get_ptr((binding->definition_nodes), 0, typeof(struct_node_t*)));
    }

# 337 "main.c"
    append_struct_node(printer, struct_node);

# 338 "main.c"
    append_string(printer, ";\n\n");
  }
  boolean_t append_newline_after_variables = false;

# 342 "main.c"
  buffer_append_string(buffer, "// ========== global variables ==========\n\n");

# 343 "main.c"
  for (
    int i = 0;
    (i<(((symbol_table->variables)->ordered_bindings)->length));
    (i++))

# 343 "main.c"
  {
    symbol_table_binding_t* binding = value_array_get_ptr(((symbol_table->variables)->ordered_bindings), i, typeof(symbol_table_binding_t*));

# 347 "main.c"
    append_variable_definition_node(printer, value_array_get_ptr((binding->definition_nodes), 0, typeof(variable_definition_node_t*)), is_library);

# 352 "main.c"
    append_string(printer, "\n");
  }

# 355 "main.c"
  if (append_newline_after_variables)

# 355 "main.c"
  {

# 356 "main.c"
    append_string(printer, "\n");
  }
  boolean_t append_newline_after_prototypes = false;

# 360 "main.c"
  buffer_append_string(buffer, "// ========== function prototypes ==========\n\n");

# 362 "main.c"
  for (
    int i = 0;
    (i<(((symbol_table->functions)->ordered_bindings)->length));
    (i++))

# 362 "main.c"
  {
    symbol_table_binding_t* binding = value_array_get_ptr(((symbol_table->functions)->ordered_bindings), i, typeof(symbol_table_binding_t*));
    function_node_t* function_node = to_function_node((/*CAST*/(parse_node_t*) (value_array_get((binding->definition_nodes), 0).ptr)));

# 368 "main.c"
    if ((!is_inlined_function(function_node)))

# 368 "main.c"
    {

# 369 "main.c"
      (append_newline_after_prototypes=true);

# 370 "main.c"
      append_c_function_node_prototype(printer, function_node);
    }
  }

# 373 "main.c"
  if (append_newline_after_prototypes)

# 373 "main.c"
  {

# 374 "main.c"
    append_string(printer, "\n");
  }
  boolean_t append_newline_after_inlines = false;

# 378 "main.c"
  buffer_append_string(buffer, "// ========== inlined functions ==========\n\n");

# 380 "main.c"
  for (
    int i = 0;
    (i<(((symbol_table->functions)->ordered_bindings)->length));
    (i++))

# 380 "main.c"
  {
    symbol_table_binding_t* binding = value_array_get_ptr(((symbol_table->functions)->ordered_bindings), i, typeof(symbol_table_binding_t*));
    function_node_t* function_node = to_function_node((/*CAST*/(parse_node_t*) (value_array_get((binding->definition_nodes), 0).ptr)));

# 386 "main.c"
    if (is_inlined_function(function_node))

# 386 "main.c"
    {

# 387 "main.c"
      (append_newline_after_inlines=true);

# 388 "main.c"
      append_c_function_node_and_body(printer, function_node);
    }
  }

# 392 "main.c"
  if (append_newline_after_inlines)

# 392 "main.c"
  {

# 393 "main.c"
    buffer_append_string(buffer, "\n");
  }
  boolean_t append_newline_after_functions = false;

# 397 "main.c"
  if (is_library)

# 397 "main.c"
  {

# 398 "main.c"
    buffer_append_string(buffer, "// ========== functions ==========\n\n");

# 399 "main.c"
    for (
      int i = 0;
      (i<(((symbol_table->functions)->ordered_bindings)->length));
      (i++))

# 400 "main.c"
    {
      symbol_table_binding_t* binding = value_array_get_ptr(((symbol_table->functions)->ordered_bindings), i, typeof(symbol_table_binding_t*));

# 404 "main.c"
      for (
        int j = 0;
        (j<((binding->definition_nodes)->length));
        (j++))

# 404 "main.c"
      {
        function_node_t* function_node = to_function_node((/*CAST*/(parse_node_t*) (value_array_get((binding->definition_nodes), j).ptr)));

# 407 "main.c"
        if (((!is_inlined_function(function_node))&&((function_node->body)!=((void *)0))))

# 408 "main.c"
        {

# 409 "main.c"
          (append_newline_after_functions=true);

# 410 "main.c"
          if (false)

# 410 "main.c"
          {

# 411 "main.c"
            buffer_printf(buffer, "/* i=%d j=%d */\n", i, j);
          }

# 413 "main.c"
          append_c_function_node_and_body(printer, function_node);
        }
      }
    }
  }

# 418 "main.c"
  if (append_newline_after_functions)

# 418 "main.c"
  {

# 419 "main.c"
    buffer_append_string(buffer, "\n");
  }

# 422 "main.c"
  if (is_header_file)

# 422 "main.c"
  {

# 423 "main.c"
    buffer_printf(buffer, "\n#endif /* %s */\n", guard_name);
  }

# 426 "main.c"
  buffer_append_buffer(buffer, command_line_overview_comment);

# 428 "main.c"
  if ((FLAG_c_output_file==((void *)0)))

# 428 "main.c"
  {

# 429 "main.c"
    fprintf(stdout, "%s\n", buffer_to_c_string(buffer));
  }
  else

# 430 "main.c"
  {

# 431 "main.c"
    log_info("Attempting to write buffer to %s", FLAG_c_output_file);

# 433 "main.c"
    buffer_write_file(buffer, FLAG_c_output_file);
  }
}


# 438 "main.c"
void add_generated_c_file_header(buffer_t* buffer)
# 438 "main.c"
{

# 439 "main.c"
  buffer_printf(buffer, "// -*- buffer-read-only: t -*-\n//\n");

# 440 "main.c"
  buffer_printf(buffer, "// This is a generated file, so you generally don't want to edit it!\n");

# 443 "main.c"
  buffer_printf(buffer, "// The bottom of the file has more information about it's " "creation.\n\n\n");
}


# 448 "main.c"
void parse_expression_string_and_print_parse_tree(char* expression)
# 448 "main.c"
{

# 449 "main.c"
  if ((expression==((void *)0)))

# 449 "main.c"
  {

# 450 "main.c"
    log_fatal("Expression not specified!");

# 451 "main.c"
    fatal_error(ERROR_ILLEGAL_INPUT);
  }
  tokenizer_result_t tokenizer_result = tokenize(buffer_append_string(make_buffer(1), expression));

# 455 "main.c"
  if ((tokenizer_result.tokenizer_error_code))

# 455 "main.c"
  {

# 456 "main.c"
    fatal_error(ERROR_ILLEGAL_INPUT);
  }
  value_array_t* tokens = (tokenizer_result.tokens);

# 459 "main.c"
  (tokens=transform_tokens(tokens, ((token_transformer_options_t) {
                                   .keep_whitespace = false,
                                   .keep_comments = false,
                                   .keep_javadoc_comments = false,
                                   .keep_c_preprocessor_lines = false,
                               })));
  pstate_t pstate = ((pstate_t) {0});

# 469 "main.c"
  ((pstate.use_statement_parser)=true);

# 470 "main.c"
  ((pstate.tokens)=tokens);

# 471 "main.c"
  if ((!parse_expression((&pstate))))

# 471 "main.c"
  {

# 472 "main.c"
    fprintf(stderr, "FAIL\n");

# 473 "main.c"
    exit(1);
  }
  parse_node_t* node = pstate_get_result_node((&pstate));

# 476 "main.c"
  if ((!pstate_expect_token_string((&pstate), ";")))

# 476 "main.c"
  {

# 477 "main.c"
    fprintf(stderr, "FAIL (expected ';')\n");

# 478 "main.c"
    exit(1);
  }
  buffer_t* output = make_buffer(1);

# 481 "main.c"
  buffer_append_dbg_parse_node(make_cdl_printer(output), node);

# 482 "main.c"
  if (FLAG_to_c)

# 482 "main.c"
  {

# 483 "main.c"
    buffer_append_string(output, "\n// C Output\n");
    printer_t* printer = make_printer(output, make_symbol_table(), 2);

# 485 "main.c"
    append_parse_node(printer, node);
  }

# 487 "main.c"
  fprintf(stdout, "%s\n", buffer_to_c_string(output));
}


# 490 "main.c"
void parse_statement_string_and_print_parse_tree(char* expression)
# 490 "main.c"
{
  tokenizer_result_t tokenizer_result = tokenize(buffer_append_string(make_buffer(1), expression));

# 493 "main.c"
  if ((tokenizer_result.tokenizer_error_code))

# 493 "main.c"
  {

# 494 "main.c"
    fatal_error(ERROR_ILLEGAL_INPUT);
  }
  value_array_t* tokens = (tokenizer_result.tokens);

# 497 "main.c"
  (tokens=transform_tokens(tokens, ((token_transformer_options_t) {
                                   .keep_whitespace = false,
                                   .keep_comments = false,
                                   .keep_javadoc_comments = false,
                                   .keep_c_preprocessor_lines = false,
                               })));
  pstate_t state = ((pstate_t) {0});

# 506 "main.c"
  ((state.use_statement_parser)=true);

# 507 "main.c"
  ((state.tokens)=tokens);
  pstatus_t status = parse_statement((&state));

# 509 "main.c"
  if ((!status))

# 509 "main.c"
  {

# 510 "main.c"
    fprintf(stderr, "FAIL");

# 511 "main.c"
    exit(1);
  }
  parse_node_t* node = pstate_get_result_node((&state));
  buffer_t* output = make_buffer(1);

# 515 "main.c"
  buffer_append_dbg_parse_node(make_cdl_printer(output), node);

# 516 "main.c"
  buffer_append_string(output, "\n// C Output\n");
  printer_t* printer = make_printer(output, make_symbol_table(), 2);

# 518 "main.c"
  append_parse_node(printer, node);

# 519 "main.c"
  fprintf(stdout, "%s\n", buffer_to_c_string(output));
}


# 543 "main.c"
buffer_t* git_hash_object(char* filename)
# 543 "main.c"
{
  value_array_t* argv = make_value_array(2);

# 545 "main.c"
  value_array_add(argv, str_to_value("git"));

# 546 "main.c"
  value_array_add(argv, str_to_value("hash-object"));

# 547 "main.c"
  value_array_add(argv, str_to_value(filename));
  sub_process_t* sub_process = make_sub_process(argv);

# 550 "main.c"
  sub_process_launch(sub_process);
  buffer_t* buffer = make_buffer(1);

# 553 "main.c"
  do
# 553 "main.c"
  {

# 554 "main.c"
    sub_process_read(sub_process, buffer, ((void *)0));

# 555 "main.c"
    usleep(5);
  }
  while (is_sub_process_running(sub_process));

# 557 "main.c"
  sub_process_read(sub_process, buffer, ((void *)0));

# 558 "main.c"
  sub_process_wait(sub_process);

# 560 "main.c"
  return buffer;
}


# 571 "main.c"
buffer_t* command_line_args_to_buffer(int argc, char** argv)
# 571 "main.c"
{
  buffer_t* output = make_buffer((argc*5));

# 574 "main.c"
  buffer_printf(output, "// Full Compiler Command Line:\n//\n");

# 575 "main.c"
  for (
    int i = 0;
    (i<argc);
    (i++))

# 575 "main.c"
  {

# 576 "main.c"
    buffer_printf(output, "//%s%s\n", ((i>0) ? "    " : " "), (argv[i]));
  }

# 579 "main.c"
  buffer_append_string(output, "\n");

# 580 "main.c"
  buffer_append_string(output, "// These checksums are currently easy to fake for example by using a\n");

# 583 "main.c"
  buffer_append_string(output, "// hacked git in the PATH at the time this compile was run.\n");

# 585 "main.c"
  buffer_append_string(output, "//\n");

# 587 "main.c"
  for (
    int i = 0;
    (i<(FLAG_files->length));
    (i++))

# 587 "main.c"
  {
    char* filename = (value_array_get(FLAG_files, i).str);
    buffer_t* git_hash = git_hash_object(filename);

# 591 "main.c"
    buffer_replace_all(git_hash, "\n", "");

# 592 "main.c"
    buffer_append_string(output, "// git cat-file -p ");

# 593 "main.c"
    buffer_append_buffer(output, git_hash);

# 594 "main.c"
    buffer_printf(output, " > %s\n", filename);
  }

# 597 "main.c"
  return output;
}


# 600 "main.c"
value_array_t* c_compiler_command_line(char* input_file, char* output_file)
# 600 "main.c"
{

# 601 "main.c"
  if ((((string_equal("clang", FLAG_c_compiler)||string_equal("gcc", FLAG_c_compiler))||string_equal("tcc", FLAG_c_compiler))||string_equal("zig", FLAG_c_compiler)))

# 604 "main.c"
  {
    value_array_t* argv = make_value_array(2);

# 606 "main.c"
    value_array_add(argv, str_to_value(FLAG_c_compiler));

# 607 "main.c"
    if (string_equal("zig", FLAG_c_compiler))

# 607 "main.c"
    {

# 608 "main.c"
      value_array_add(argv, str_to_value("cc"));
    }

# 610 "main.c"
    value_array_add(argv, str_to_value("-g"));

# 611 "main.c"
    value_array_add(argv, str_to_value("-rdynamic"));

# 612 "main.c"
    value_array_add(argv, str_to_value("-O3"));

# 613 "main.c"
    value_array_add(argv, str_to_value("-std=gnu99"));

# 614 "main.c"
    value_array_add(argv, str_to_value("-o"));

# 615 "main.c"
    value_array_add(argv, str_to_value(output_file));

# 616 "main.c"
    value_array_add(argv, str_to_value(input_file));

# 617 "main.c"
    value_array_add(argv, str_to_value("-lgc"));

# 618 "main.c"
    return argv;
  }
  else

# 619 "main.c"
  {

# 620 "main.c"
    log_fatal("Unknown C compiler %s\n", FLAG_c_compiler);

# 621 "main.c"
    fatal_error(ERROR_ILLEGAL_INPUT);
  }
}


# 625 "main.c"
int invoke_c_compiler(char* input_file, char* output_file)
# 625 "main.c"
{
  value_array_t* argv = c_compiler_command_line(input_file, output_file);

# 628 "main.c"
  log_warn("Invoking C compiler with these arguments: %s", buffer_to_c_string(join_array_of_strings(argv, " ")));
  sub_process_t* sub_process = make_sub_process(argv);

# 631 "main.c"
  sub_process_launch(sub_process);
  buffer_t* buffer = make_buffer(1);

# 634 "main.c"
  do
# 634 "main.c"
  {

# 635 "main.c"
    sub_process_read(sub_process, buffer, buffer);

# 636 "main.c"
    usleep(5);
  }
  while (is_sub_process_running(sub_process));

# 638 "main.c"
  sub_process_read(sub_process, buffer, buffer);

# 639 "main.c"
  sub_process_wait(sub_process);

# 641 "main.c"
  log_warn(">>> Exit Status %d <<<\n%s", (sub_process->exit_code), buffer_to_c_string(buffer));

# 644 "main.c"
  return (sub_process->exit_code);
}


# 647 "main.c"
void generate_archive_file(void)
# 647 "main.c"
{

# 648 "main.c"
  if ((FLAG_archive_output_file==((void *)0)))

# 648 "main.c"
  {

# 649 "main.c"
    log_fatal("Must specify the archive output file name");

# 650 "main.c"
    exit((-1));
  }

# 652 "main.c"
  if (((FLAG_files==((void *)0))||((FLAG_files->length)==0)))

# 652 "main.c"
  {

# 653 "main.c"
    log_warn("No archive members specified.");

# 654 "main.c"
    exit((-1));
  }
  FILE* out = fopen(FLAG_archive_output_file, "w");

# 659 "main.c"
  for (
    int i = 0;
    (i<(FLAG_files->length));
    (i++))

# 659 "main.c"
  {

# 660 "main.c"
    oarchive_append_header_and_file_contents(out, (value_array_get(FLAG_files, i).str));
  }

# 663 "main.c"
  fclose(out);
}


# 666 "main.c"
int main(int argc, char** argv)
# 666 "main.c"
{

# 667 "main.c"
  configure_fatal_errors(((fatal_error_config_t) {
                                                 .catch_sigsegv = true,
                                             }));

# 671 "main.c"
  logger_init();

# 673 "main.c"
  configure_flags();
  char* error = flag_parse_command_line(argc, argv);

# 676 "main.c"
  if (error)

# 676 "main.c"
  {

# 677 "main.c"
    flag_print_help(stderr, error);

# 678 "main.c"
    exit(1);
  }

# 681 "main.c"
  if (FLAG_print_command_line)

# 681 "main.c"
  {

# 682 "main.c"
    fprintf(stderr, "Command Line:");

# 683 "main.c"
    for (
      int i = 0;
      (i<argc);
      (i++))

# 683 "main.c"
    {

# 684 "main.c"
      fprintf(stderr, " %s", (argv[i]));
    }

# 686 "main.c"
    fprintf(stderr, "\n");
  }

# 689 "main.c"
  if ((FLAG_command==((void *)0)))

# 689 "main.c"
  {

# 694 "main.c"
    fatal_error(ERROR_BAD_COMMAND_LINE);
  }
  else

# 695 "main.c"
  if (string_equal("archive", FLAG_command))

# 695 "main.c"
  {

# 696 "main.c"
    generate_archive_file();

# 697 "main.c"
    log_info("Exiting normally.");

# 698 "main.c"
    exit(0);
  }
  else

# 699 "main.c"
  if (string_equal("generate-header-file", FLAG_command))

# 699 "main.c"
  {

# 700 "main.c"
    generate_c_output_file(false, command_line_args_to_buffer(argc, argv));

# 701 "main.c"
    log_info("Exiting normally.");

# 702 "main.c"
    exit(0);
  }
  else

# 703 "main.c"
  if (string_equal("generate-library", FLAG_command))

# 703 "main.c"
  {

# 704 "main.c"
    generate_c_output_file(true, command_line_args_to_buffer(argc, argv));

# 705 "main.c"
    log_info("Exiting normally.");

# 706 "main.c"
    exit(0);
  }
  else

# 707 "main.c"
  if (string_equal("build", FLAG_command))

# 707 "main.c"
  {

# 708 "main.c"
    if (string_is_null_or_empty(FLAG_c_output_file))

# 708 "main.c"
    {

# 709 "main.c"
      log_fatal("Must supply --c-output-file");

# 710 "main.c"
      fatal_error(ERROR_ILLEGAL_INPUT);
    }

# 712 "main.c"
    if (string_is_null_or_empty(FLAG_binary_output_file))

# 712 "main.c"
    {

# 713 "main.c"
      log_fatal("Must supply --binary-output-file");

# 714 "main.c"
      fatal_error(ERROR_ILLEGAL_INPUT);
    }

# 716 "main.c"
    generate_c_output_file(true, command_line_args_to_buffer(argc, argv));
    int status = invoke_c_compiler(FLAG_c_output_file, FLAG_binary_output_file);

# 718 "main.c"
    if ((status==0))

# 718 "main.c"
    {

# 719 "main.c"
      log_info("Exiting normally.");

# 720 "main.c"
      exit(0);
    }
    else

# 721 "main.c"
    {

# 722 "main.c"
      log_warn("Exiting abnormally.");

# 723 "main.c"
      exit(status);
    }
  }
  else

# 725 "main.c"
  if (string_equal("parse-expression", FLAG_command))

# 725 "main.c"
  {

# 726 "main.c"
    parse_expression_string_and_print_parse_tree(FLAG_expression);
  }
  else

# 727 "main.c"
  if (string_equal("parse-statement", FLAG_command))

# 727 "main.c"
  {

# 728 "main.c"
    parse_statement_string_and_print_parse_tree(FLAG_statement);
  }
  else

# 729 "main.c"
  if (string_equal("print-tokens", FLAG_command))

# 729 "main.c"
  {

# 730 "main.c"
    print_tokens();
  }
  else

# 731 "main.c"
  {

# 732 "main.c"
    fprintf(stderr, "Unknown command: %s\n", FLAG_command);
  }

# 735 "main.c"
  exit(0);
}


# 2 "/home/jawilson/src/omni-c/build-dir/gen-files/reflection-header.c"
buffer_t* get_reflection_header_buffer(void)
# 2 "/home/jawilson/src/omni-c/build-dir/gen-files/reflection-header.c"
{
  uint8_t reflection_header[] = {
    0x23, 0x69, 0x66, 0x6E, 0x64, 0x65, 0x66, 0x20, 0x5F, 0x52, 0x45, 0x46, 0x4C, 0x45, 0x43, 0x54,  // #ifndef _REFLECT
    0x49, 0x4F, 0x4E, 0x5F, 0x48, 0x5F, 0x0A, 0x23, 0x64, 0x65, 0x66, 0x69, 0x6E, 0x65, 0x20, 0x5F,  // ION_H_.#define _
    0x52, 0x45, 0x46, 0x4C, 0x45, 0x43, 0x54, 0x49, 0x4F, 0x4E, 0x5F, 0x48, 0x5F, 0x0A, 0x0A, 0x23,  // REFLECTION_H_..#
    0x64, 0x65, 0x66, 0x69, 0x6E, 0x65, 0x20, 0x66, 0x6E, 0x5F, 0x74, 0x28, 0x72, 0x65, 0x74, 0x75,  // define fn_t(retu
    0x72, 0x6E, 0x5F, 0x74, 0x79, 0x70, 0x65, 0x2C, 0x20, 0x2E, 0x2E, 0x2E, 0x29, 0x20, 0x74, 0x79,  // rn_type, ...) ty
    0x70, 0x65, 0x6F, 0x66, 0x28, 0x72, 0x65, 0x74, 0x75, 0x72, 0x6E, 0x5F, 0x74, 0x79, 0x70, 0x65,  // peof(return_type
    0x28, 0x2A, 0x29, 0x28, 0x5F, 0x5F, 0x56, 0x41, 0x5F, 0x41, 0x52, 0x47, 0x53, 0x5F, 0x5F, 0x29,  // (*)(__VA_ARGS__)
    0x29, 0x0A, 0x0A, 0x23, 0x69, 0x6E, 0x63, 0x6C, 0x75, 0x64, 0x65, 0x20, 0x3C, 0x73, 0x74, 0x72,  // )..#include <str
    0x69, 0x6E, 0x67, 0x2E, 0x68, 0x3E, 0x0A, 0x0A, 0x2F, 0x2A, 0x20, 0x3D, 0x3D, 0x3D, 0x3D, 0x3D,  // ing.h>../* =====
    0x3D, 0x3D, 0x3D, 0x3D, 0x3D, 0x3D, 0x3D, 0x3D, 0x3D, 0x3D, 0x3D, 0x3D, 0x3D, 0x3D, 0x3D, 0x3D,  // ================
    0x3D, 0x3D, 0x3D, 0x3D, 0x3D, 0x3D, 0x3D, 0x3D, 0x3D, 0x3D, 0x3D, 0x3D, 0x3D, 0x3D, 0x3D, 0x3D,  // ================
    0x3D, 0x3D, 0x3D, 0x3D, 0x3D, 0x3D, 0x3D, 0x3D, 0x3D, 0x3D, 0x3D, 0x3D, 0x3D, 0x3D, 0x3D, 0x3D,  // ================
    0x3D, 0x3D, 0x3D, 0x3D, 0x3D, 0x3D, 0x3D, 0x3D, 0x3D, 0x3D, 0x3D, 0x3D, 0x3D, 0x3D, 0x3D, 0x3D,  // ================
    0x3D, 0x20, 0x2A, 0x2F, 0x0A, 0x2F, 0x2F, 0x20, 0x54, 0x68, 0x65, 0x73, 0x65, 0x20, 0x61, 0x72,  // = */.// These ar
    0x65, 0x20, 0x74, 0x68, 0x65, 0x20, 0x72, 0x65, 0x66, 0x6C, 0x65, 0x63, 0x74, 0x69, 0x6F, 0x6E,  // e the reflection
    0x20, 0x41, 0x50, 0x49, 0x20, 0x64, 0x61, 0x74, 0x61, 0x20, 0x73, 0x74, 0x72, 0x75, 0x63, 0x74,  //  API data struct
    0x75, 0x72, 0x65, 0x73, 0x20, 0x66, 0x6F, 0x72, 0x20, 0x61, 0x20, 0x70, 0x72, 0x6F, 0x67, 0x72,  // ures for a progr
    0x61, 0x6D, 0x20, 0x63, 0x6F, 0x6D, 0x70, 0x69, 0x6C, 0x65, 0x64, 0x0A, 0x2F, 0x2F, 0x20, 0x77,  // am compiled.// w
    0x69, 0x74, 0x68, 0x20, 0x4F, 0x6D, 0x6E, 0x69, 0x20, 0x43, 0x2E, 0x20, 0x57, 0x65, 0x20, 0x75,  // ith Omni C. We u
    0x73, 0x65, 0x20, 0x6C, 0x69, 0x6E, 0x6B, 0x65, 0x64, 0x20, 0x6C, 0x69, 0x73, 0x74, 0x73, 0x20,  // se linked lists 
    0x69, 0x6E, 0x73, 0x74, 0x65, 0x61, 0x64, 0x20, 0x6F, 0x66, 0x20, 0x76, 0x61, 0x6C, 0x75, 0x65,  // instead of value
    0x5F, 0x61, 0x72, 0x72, 0x61, 0x79, 0x5F, 0x74, 0x20, 0x74, 0x6F, 0x20, 0x6B, 0x65, 0x65, 0x70,  // _array_t to keep
    0x0A, 0x2F, 0x2F, 0x20, 0x63, 0x6F, 0x6D, 0x70, 0x69, 0x6C, 0x65, 0x64, 0x20, 0x70, 0x72, 0x6F,  // .// compiled pro
    0x67, 0x72, 0x61, 0x6D, 0x73, 0x20, 0x69, 0x6E, 0x64, 0x65, 0x70, 0x65, 0x6E, 0x64, 0x65, 0x6E,  // grams independen
    0x74, 0x20, 0x6F, 0x66, 0x20, 0x61, 0x6E, 0x79, 0x20, 0x70, 0x61, 0x72, 0x74, 0x69, 0x63, 0x75,  // t of any particu
    0x6C, 0x61, 0x72, 0x20, 0x6C, 0x69, 0x62, 0x72, 0x61, 0x72, 0x79, 0x0A, 0x2F, 0x2F, 0x20, 0x64,  // lar library.// d
    0x61, 0x74, 0x61, 0x2D, 0x73, 0x74, 0x72, 0x75, 0x63, 0x74, 0x75, 0x72, 0x65, 0x73, 0x2E, 0x0A,  // ata-structures..
    0x2F, 0x2A, 0x20, 0x3D, 0x3D, 0x3D, 0x3D, 0x3D, 0x3D, 0x3D, 0x3D, 0x3D, 0x3D, 0x3D, 0x3D, 0x3D,  // /* =============
    0x3D, 0x3D, 0x3D, 0x3D, 0x3D, 0x3D, 0x3D, 0x3D, 0x3D, 0x3D, 0x3D, 0x3D, 0x3D, 0x3D, 0x3D, 0x3D,  // ================
    0x3D, 0x3D, 0x3D, 0x3D, 0x3D, 0x3D, 0x3D, 0x3D, 0x3D, 0x3D, 0x3D, 0x3D, 0x3D, 0x3D, 0x3D, 0x3D,  // ================
    0x3D, 0x3D, 0x3D, 0x3D, 0x3D, 0x3D, 0x3D, 0x3D, 0x3D, 0x3D, 0x3D, 0x3D, 0x3D, 0x3D, 0x3D, 0x3D,  // ================
    0x3D, 0x3D, 0x3D, 0x3D, 0x3D, 0x3D, 0x3D, 0x3D, 0x3D, 0x20, 0x2A, 0x2F, 0x0A, 0x0A, 0x2F, 0x2F,  // ========= */..//
    0x20, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D,  //  ---------------
    0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D,  // ----------------
    0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D,  // ----------------
    0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D,  // ----------------
    0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x0A, 0x2F, 0x2F, 0x20, 0x45, 0x6E, 0x75, 0x6D, 0x65,  // -------.// Enume
    0x72, 0x61, 0x74, 0x69, 0x6F, 0x6E, 0x73, 0x0A, 0x2F, 0x2F, 0x20, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D,  // rations.// -----
    0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D,  // ----------------
    0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D,  // ----------------
    0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D,  // ----------------
    0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D,  // ----------------
    0x2D, 0x0A, 0x0A, 0x74, 0x79, 0x70, 0x65, 0x64, 0x65, 0x66, 0x20, 0x73, 0x74, 0x72, 0x75, 0x63,  // -..typedef struc
    0x74, 0x20, 0x65, 0x6E, 0x75, 0x6D, 0x5F, 0x65, 0x6C, 0x65, 0x6D, 0x65, 0x6E, 0x74, 0x5F, 0x6D,  // t enum_element_m
    0x65, 0x74, 0x61, 0x64, 0x61, 0x74, 0x61, 0x5F, 0x53, 0x20, 0x7B, 0x0A, 0x20, 0x20, 0x73, 0x74,  // etadata_S {.  st
    0x72, 0x75, 0x63, 0x74, 0x20, 0x65, 0x6E, 0x75, 0x6D, 0x5F, 0x65, 0x6C, 0x65, 0x6D, 0x65, 0x6E,  // ruct enum_elemen
    0x74, 0x5F, 0x6D, 0x65, 0x74, 0x61, 0x64, 0x61, 0x74, 0x61, 0x5F, 0x53, 0x2A, 0x20, 0x6E, 0x65,  // t_metadata_S* ne
    0x78, 0x74, 0x3B, 0x0A, 0x20, 0x20, 0x63, 0x68, 0x61, 0x72, 0x2A, 0x20, 0x6E, 0x61, 0x6D, 0x65,  // xt;.  char* name
    0x3B, 0x0A, 0x20, 0x20, 0x6C, 0x6F, 0x6E, 0x67, 0x20, 0x76, 0x61, 0x6C, 0x75, 0x65, 0x3B, 0x0A,  // ;.  long value;.
    0x7D, 0x20, 0x65, 0x6E, 0x75, 0x6D, 0x5F, 0x65, 0x6C, 0x65, 0x6D, 0x65, 0x6E, 0x74, 0x5F, 0x6D,  // } enum_element_m
    0x65, 0x74, 0x61, 0x64, 0x61, 0x74, 0x61, 0x5F, 0x74, 0x3B, 0x0A, 0x0A, 0x2F, 0x2A, 0x2A, 0x0A,  // etadata_t;../**.
    0x20, 0x2A, 0x20, 0x40, 0x73, 0x74, 0x72, 0x75, 0x63, 0x74, 0x75, 0x72, 0x65, 0x20, 0x65, 0x6E,  //  * @structure en
    0x75, 0x6D, 0x5F, 0x6D, 0x65, 0x74, 0x61, 0x64, 0x61, 0x74, 0x61, 0x5F, 0x74, 0x0A, 0x20, 0x2A,  // um_metadata_t. *
    0x0A, 0x20, 0x2A, 0x20, 0x54, 0x68, 0x65, 0x20, 0x72, 0x75, 0x6E, 0x74, 0x69, 0x6D, 0x65, 0x20,  // . * The runtime 
    0x6D, 0x65, 0x74, 0x61, 0x64, 0x61, 0x74, 0x61, 0x20, 0x66, 0x6F, 0x72, 0x20, 0x61, 0x20, 0x72,  // metadata for a r
    0x65, 0x66, 0x6C, 0x65, 0x63, 0x74, 0x65, 0x64, 0x20, 0x65, 0x6E, 0x75, 0x6D, 0x65, 0x72, 0x61,  // eflected enumera
    0x74, 0x69, 0x6F, 0x6E, 0x2E, 0x0A, 0x20, 0x2A, 0x2F, 0x0A, 0x74, 0x79, 0x70, 0x65, 0x64, 0x65,  // tion.. */.typede
    0x66, 0x20, 0x73, 0x74, 0x72, 0x75, 0x63, 0x74, 0x20, 0x7B, 0x0A, 0x20, 0x20, 0x63, 0x68, 0x61,  // f struct {.  cha
    0x72, 0x2A, 0x20, 0x6E, 0x61, 0x6D, 0x65, 0x3B, 0x0A, 0x20, 0x20, 0x65, 0x6E, 0x75, 0x6D, 0x5F,  // r* name;.  enum_
    0x65, 0x6C, 0x65, 0x6D, 0x65, 0x6E, 0x74, 0x5F, 0x6D, 0x65, 0x74, 0x61, 0x64, 0x61, 0x74, 0x61,  // element_metadata
    0x5F, 0x74, 0x2A, 0x20, 0x65, 0x6C, 0x65, 0x6D, 0x65, 0x6E, 0x74, 0x73, 0x3B, 0x0A, 0x7D, 0x20,  // _t* elements;.} 
    0x65, 0x6E, 0x75, 0x6D, 0x5F, 0x6D, 0x65, 0x74, 0x61, 0x64, 0x61, 0x74, 0x61, 0x5F, 0x74, 0x3B,  // enum_metadata_t;
    0x0A, 0x0A, 0x2F, 0x2F, 0x20, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D,  // ..// -----------
    0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D,  // ----------------
    0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D,  // ----------------
    0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D,  // ----------------
    0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x0A, 0x2F, 0x2F, 0x20, 0x53,  // -----------.// S
    0x74, 0x72, 0x75, 0x63, 0x74, 0x75, 0x72, 0x65, 0x73, 0x0A, 0x2F, 0x2F, 0x20, 0x2D, 0x2D, 0x2D,  // tructures.// ---
    0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D,  // ----------------
    0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D,  // ----------------
    0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D,  // ----------------
    0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D,  // ----------------
    0x2D, 0x2D, 0x2D, 0x0A, 0x0A, 0x74, 0x79, 0x70, 0x65, 0x64, 0x65, 0x66, 0x20, 0x73, 0x74, 0x72,  // ---..typedef str
    0x75, 0x63, 0x74, 0x20, 0x66, 0x69, 0x65, 0x6C, 0x64, 0x5F, 0x6D, 0x65, 0x74, 0x61, 0x64, 0x61,  // uct field_metada
    0x74, 0x61, 0x5F, 0x53, 0x20, 0x7B, 0x0A, 0x20, 0x20, 0x73, 0x74, 0x72, 0x75, 0x63, 0x74, 0x20,  // ta_S {.  struct 
    0x66, 0x69, 0x65, 0x6C, 0x64, 0x5F, 0x6D, 0x65, 0x74, 0x61, 0x64, 0x61, 0x74, 0x61, 0x5F, 0x53,  // field_metadata_S
    0x2A, 0x20, 0x6E, 0x65, 0x78, 0x74, 0x3B, 0x0A, 0x20, 0x20, 0x63, 0x68, 0x61, 0x72, 0x2A, 0x20,  // * next;.  char* 
    0x6E, 0x61, 0x6D, 0x65, 0x3B, 0x0A, 0x20, 0x20, 0x63, 0x68, 0x61, 0x72, 0x2A, 0x20, 0x74, 0x79,  // name;.  char* ty
    0x70, 0x65, 0x5F, 0x6E, 0x61, 0x6D, 0x65, 0x5F, 0x73, 0x74, 0x72, 0x69, 0x6E, 0x67, 0x3B, 0x0A,  // pe_name_string;.
    0x20, 0x20, 0x69, 0x6E, 0x74, 0x20, 0x73, 0x74, 0x61, 0x72, 0x74, 0x5F, 0x6F, 0x66, 0x66, 0x73,  //   int start_offs
    0x65, 0x74, 0x3B, 0x0A, 0x7D, 0x20, 0x66, 0x69, 0x65, 0x6C, 0x64, 0x5F, 0x6D, 0x65, 0x74, 0x61,  // et;.} field_meta
    0x64, 0x61, 0x74, 0x61, 0x5F, 0x74, 0x3B, 0x0A, 0x0A, 0x2F, 0x2A, 0x2A, 0x0A, 0x20, 0x2A, 0x20,  // data_t;../**. * 
    0x40, 0x73, 0x74, 0x72, 0x75, 0x63, 0x74, 0x75, 0x72, 0x65, 0x20, 0x73, 0x74, 0x72, 0x75, 0x63,  // @structure struc
    0x74, 0x75, 0x72, 0x65, 0x5F, 0x6D, 0x65, 0x74, 0x61, 0x64, 0x61, 0x74, 0x61, 0x5F, 0x74, 0x0A,  // ture_metadata_t.
    0x20, 0x2A, 0x0A, 0x20, 0x2A, 0x20, 0x54, 0x68, 0x65, 0x20, 0x72, 0x75, 0x6E, 0x74, 0x69, 0x6D,  //  *. * The runtim
    0x65, 0x20, 0x6D, 0x65, 0x74, 0x61, 0x64, 0x61, 0x74, 0x61, 0x20, 0x66, 0x6F, 0x72, 0x20, 0x61,  // e metadata for a
    0x20, 0x72, 0x65, 0x66, 0x6C, 0x65, 0x63, 0x74, 0x65, 0x64, 0x20, 0x73, 0x74, 0x72, 0x75, 0x63,  //  reflected struc
    0x74, 0x75, 0x72, 0x65, 0x2E, 0x0A, 0x20, 0x2A, 0x2F, 0x0A, 0x74, 0x79, 0x70, 0x65, 0x64, 0x65,  // ture.. */.typede
    0x66, 0x20, 0x73, 0x74, 0x72, 0x75, 0x63, 0x74, 0x20, 0x7B, 0x0A, 0x20, 0x20, 0x63, 0x68, 0x61,  // f struct {.  cha
    0x72, 0x2A, 0x20, 0x6E, 0x61, 0x6D, 0x65, 0x3B, 0x0A, 0x20, 0x20, 0x69, 0x6E, 0x74, 0x20, 0x73,  // r* name;.  int s
    0x69, 0x7A, 0x65, 0x3B, 0x0A, 0x20, 0x20, 0x69, 0x6E, 0x74, 0x20, 0x61, 0x6C, 0x69, 0x67, 0x6E,  // ize;.  int align
    0x6D, 0x65, 0x6E, 0x74, 0x3B, 0x0A, 0x20, 0x20, 0x66, 0x69, 0x65, 0x6C, 0x64, 0x5F, 0x6D, 0x65,  // ment;.  field_me
    0x74, 0x61, 0x64, 0x61, 0x74, 0x61, 0x5F, 0x74, 0x2A, 0x20, 0x66, 0x69, 0x65, 0x6C, 0x64, 0x73,  // tadata_t* fields
    0x3B, 0x0A, 0x7D, 0x20, 0x73, 0x74, 0x72, 0x75, 0x63, 0x74, 0x75, 0x72, 0x65, 0x5F, 0x6D, 0x65,  // ;.} structure_me
    0x74, 0x61, 0x64, 0x61, 0x74, 0x61, 0x5F, 0x74, 0x3B, 0x0A, 0x0A, 0x2F, 0x2F, 0x20, 0x2D, 0x2D,  // tadata_t;..// --
    0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D,  // ----------------
    0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D,  // ----------------
    0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D,  // ----------------
    0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D,  // ----------------
    0x2D, 0x2D, 0x2D, 0x2D, 0x0A, 0x2F, 0x2F, 0x20, 0x55, 0x6E, 0x69, 0x6F, 0x6E, 0x73, 0x20, 0x0A,  // ----.// Unions .
    0x2F, 0x2F, 0x20, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D,  // // -------------
    0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D,  // ----------------
    0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D,  // ----------------
    0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D,  // ----------------
    0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x0A, 0x0A, 0x2F, 0x2A, 0x0A, 0x20, 0x2A,  // ---------../*. *
    0x20, 0x55, 0x6E, 0x69, 0x6F, 0x6E, 0x73, 0x20, 0x63, 0x6F, 0x75, 0x6C, 0x64, 0x20, 0x62, 0x65,  //  Unions could be
    0x20, 0x74, 0x72, 0x65, 0x61, 0x74, 0x65, 0x64, 0x20, 0x65, 0x78, 0x61, 0x63, 0x74, 0x6C, 0x79,  //  treated exactly
    0x20, 0x6C, 0x69, 0x6B, 0x65, 0x20, 0x73, 0x74, 0x72, 0x75, 0x63, 0x74, 0x75, 0x72, 0x65, 0x73,  //  like structures
    0x20, 0x68, 0x6F, 0x77, 0x65, 0x76, 0x65, 0x72, 0x20, 0x74, 0x68, 0x65, 0x72, 0x65, 0x20, 0x69,  //  however there i
    0x73, 0x20, 0x6E, 0x6F, 0x0A, 0x20, 0x2A, 0x20, 0x75, 0x6E, 0x69, 0x66, 0x6F, 0x72, 0x6D, 0x20,  // s no. * uniform 
    0x77, 0x61, 0x79, 0x20, 0x74, 0x6F, 0x20, 0x74, 0x61, 0x67, 0x20, 0x61, 0x20, 0x75, 0x6E, 0x69,  // way to tag a uni
    0x6F, 0x6E, 0x20, 0x73, 0x6F, 0x20, 0x6D, 0x61, 0x6B, 0x69, 0x6E, 0x67, 0x20, 0x75, 0x73, 0x65,  // on so making use
    0x20, 0x6F, 0x66, 0x20, 0x74, 0x68, 0x65, 0x20, 0x6D, 0x65, 0x74, 0x61, 0x64, 0x61, 0x74, 0x61,  //  of the metadata
    0x20, 0x69, 0x73, 0x20, 0x61, 0x20, 0x62, 0x69, 0x74, 0x0A, 0x20, 0x2A, 0x20, 0x70, 0x72, 0x6F,  //  is a bit. * pro
    0x62, 0x6C, 0x65, 0x6D, 0x61, 0x74, 0x69, 0x63, 0x2E, 0x20, 0x46, 0x6F, 0x72, 0x20, 0x74, 0x68,  // blematic. For th
    0x61, 0x74, 0x20, 0x72, 0x65, 0x61, 0x73, 0x6F, 0x6E, 0x20, 0x28, 0x61, 0x6E, 0x64, 0x20, 0x62,  // at reason (and b
    0x65, 0x63, 0x61, 0x75, 0x73, 0x65, 0x20, 0x6F, 0x6D, 0x6E, 0x69, 0x2D, 0x63, 0x20, 0x64, 0x6F,  // ecause omni-c do
    0x65, 0x73, 0x6E, 0x27, 0x74, 0x20, 0x6E, 0x65, 0x65, 0x64, 0x20, 0x74, 0x68, 0x65, 0x6D, 0x0A,  // esn't need them.
    0x20, 0x2A, 0x20, 0x79, 0x65, 0x74, 0x29, 0x2C, 0x20, 0x77, 0x65, 0x20, 0x61, 0x72, 0x65, 0x20,  //  * yet), we are 
    0x73, 0x6B, 0x69, 0x70, 0x70, 0x69, 0x6E, 0x67, 0x20, 0x74, 0x68, 0x65, 0x6D, 0x20, 0x66, 0x6F,  // skipping them fo
    0x72, 0x20, 0x6E, 0x6F, 0x77, 0x2E, 0x0A, 0x20, 0x2A, 0x2F, 0x0A, 0x0A, 0x2F, 0x2F, 0x20, 0x2D,  // r now.. */..// -
    0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D,  // ----------------
    0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D,  // ----------------
    0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D,  // ----------------
    0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D,  // ----------------
    0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x0A, 0x2F, 0x2F, 0x20, 0x46, 0x75, 0x6E, 0x63, 0x74, 0x69, 0x6F,  // -----.// Functio
    0x6E, 0x73, 0x0A, 0x2F, 0x2F, 0x20, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D,  // ns.// ----------
    0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D,  // ----------------
    0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D,  // ----------------
    0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D,  // ----------------
    0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x0A, 0x0A, 0x74, 0x79,  // ------------..ty
    0x70, 0x65, 0x64, 0x65, 0x66, 0x20, 0x73, 0x74, 0x72, 0x75, 0x63, 0x74, 0x20, 0x66, 0x75, 0x6E,  // pedef struct fun
    0x63, 0x74, 0x69, 0x6F, 0x6E, 0x5F, 0x61, 0x72, 0x67, 0x5F, 0x6D, 0x65, 0x74, 0x61, 0x64, 0x61,  // ction_arg_metada
    0x74, 0x61, 0x5F, 0x53, 0x20, 0x7B, 0x0A, 0x20, 0x20, 0x73, 0x74, 0x72, 0x75, 0x63, 0x74, 0x20,  // ta_S {.  struct 
    0x66, 0x75, 0x6E, 0x63, 0x74, 0x69, 0x6F, 0x6E, 0x5F, 0x61, 0x72, 0x67, 0x5F, 0x6D, 0x65, 0x74,  // function_arg_met
    0x61, 0x64, 0x61, 0x74, 0x61, 0x5F, 0x74, 0x2A, 0x20, 0x6E, 0x65, 0x78, 0x74, 0x3B, 0x0A, 0x20,  // adata_t* next;. 
    0x20, 0x63, 0x68, 0x61, 0x72, 0x2A, 0x20, 0x6E, 0x61, 0x6D, 0x65, 0x3B, 0x0A, 0x20, 0x20, 0x63,  //  char* name;.  c
    0x68, 0x61, 0x72, 0x2A, 0x20, 0x74, 0x79, 0x70, 0x65, 0x5F, 0x73, 0x74, 0x72, 0x69, 0x6E, 0x67,  // har* type_string
    0x3B, 0x0A, 0x7D, 0x20, 0x66, 0x75, 0x6E, 0x63, 0x74, 0x69, 0x6F, 0x6E, 0x5F, 0x61, 0x72, 0x67,  // ;.} function_arg
    0x5F, 0x6D, 0x65, 0x74, 0x61, 0x64, 0x61, 0x74, 0x61, 0x5F, 0x74, 0x3B, 0x0A, 0x0A, 0x2F, 0x2A,  // _metadata_t;../*
    0x2A, 0x0A, 0x20, 0x2A, 0x20, 0x40, 0x73, 0x74, 0x72, 0x75, 0x63, 0x74, 0x75, 0x72, 0x65, 0x20,  // *. * @structure 
    0x66, 0x75, 0x6E, 0x63, 0x74, 0x69, 0x6F, 0x6E, 0x5F, 0x6D, 0x65, 0x74, 0x61, 0x64, 0x61, 0x74,  // function_metadat
    0x61, 0x5F, 0x74, 0x0A, 0x20, 0x2A, 0x0A, 0x20, 0x2A, 0x20, 0x54, 0x68, 0x65, 0x20, 0x72, 0x75,  // a_t. *. * The ru
    0x6E, 0x74, 0x69, 0x6D, 0x65, 0x20, 0x6D, 0x65, 0x74, 0x61, 0x64, 0x61, 0x74, 0x61, 0x20, 0x66,  // ntime metadata f
    0x6F, 0x72, 0x20, 0x61, 0x20, 0x72, 0x65, 0x66, 0x6C, 0x65, 0x63, 0x74, 0x65, 0x64, 0x20, 0x73,  // or a reflected s
    0x74, 0x72, 0x75, 0x63, 0x74, 0x75, 0x72, 0x65, 0x2E, 0x0A, 0x20, 0x2A, 0x2F, 0x0A, 0x74, 0x79,  // tructure.. */.ty
    0x70, 0x65, 0x64, 0x65, 0x66, 0x20, 0x73, 0x74, 0x72, 0x75, 0x63, 0x74, 0x20, 0x7B, 0x0A, 0x20,  // pedef struct {. 
    0x20, 0x63, 0x68, 0x61, 0x72, 0x2A, 0x20, 0x6E, 0x61, 0x6D, 0x65, 0x3B, 0x0A, 0x20, 0x20, 0x66,  //  char* name;.  f
    0x75, 0x6E, 0x63, 0x74, 0x69, 0x6F, 0x6E, 0x5F, 0x61, 0x72, 0x67, 0x5F, 0x6D, 0x65, 0x74, 0x61,  // unction_arg_meta
    0x64, 0x61, 0x74, 0x61, 0x5F, 0x74, 0x2A, 0x20, 0x61, 0x72, 0x67, 0x75, 0x6D, 0x65, 0x6E, 0x74,  // data_t* argument
    0x73, 0x3B, 0x0A, 0x7D, 0x20, 0x66, 0x75, 0x6E, 0x63, 0x74, 0x69, 0x6F, 0x6E, 0x5F, 0x6D, 0x65,  // s;.} function_me
    0x74, 0x61, 0x64, 0x61, 0x74, 0x61, 0x5F, 0x74, 0x3B, 0x0A, 0x0A, 0x23, 0x65, 0x6E, 0x64, 0x69,  // tadata_t;..#endi
    0x66, 0x20, 0x2F, 0x2A, 0x20, 0x5F, 0x52, 0x45, 0x46, 0x4C, 0x45, 0x43, 0x54, 0x49, 0x4F, 0x4E,  // f /* _REFLECTION
    0x5F, 0x48, 0x5F, 0x20, 0x2A, 0x2F, 0x0A,                                                        // _H_ */.
    };
  buffer_t* result = make_buffer(2455);

# 160 "/home/jawilson/src/omni-c/build-dir/gen-files/reflection-header.c"
  for (
    int i = 0;
    (i<2455);
    (i++))

# 160 "/home/jawilson/src/omni-c/build-dir/gen-files/reflection-header.c"
  {

# 161 "/home/jawilson/src/omni-c/build-dir/gen-files/reflection-header.c"
    buffer_append_byte(result, (reflection_header[i]));
  }

# 163 "/home/jawilson/src/omni-c/build-dir/gen-files/reflection-header.c"
  return result;
}

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
enum_metadata_t* error_code_metadata(){
    static enum_element_metadata_t var_0 = (enum_element_metadata_t) {
        .next = ((void*)0),
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
enum_metadata_t* non_fatal_error_code_metadata(){
    static enum_element_metadata_t var_0 = (enum_element_metadata_t) {
        .next = ((void*)0),
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
enum_metadata_t* flag_type_metadata(){
    static enum_element_metadata_t var_0 = (enum_element_metadata_t) {
        .next = ((void*)0),
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
enum_metadata_t* sub_process_exit_status_metadata(){
    static enum_element_metadata_t var_0 = (enum_element_metadata_t) {
        .next = ((void*)0),
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
enum_metadata_t* input_mode_metadata(){
    static enum_element_metadata_t var_0 = (enum_element_metadata_t) {
        .next = ((void*)0),
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
output_mode_t string_to_output_mode(char* value){
  if (strcmp(value, "OUTPUT_MODE_STANDARD_C") == 0) {
    return OUTPUT_MODE_STANDARD_C;
  }
  if (strcmp(value, "OUTPUT_MODE_C_PLUS_PLUS") == 0) {
    return OUTPUT_MODE_C_PLUS_PLUS;
  }
  return 0;
}
enum_metadata_t* output_mode_metadata(){
    static enum_element_metadata_t var_0 = (enum_element_metadata_t) {
        .next = ((void*)0),
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
enum_metadata_t* file_tag_metadata(){
    static enum_element_metadata_t var_0 = (enum_element_metadata_t) {
        .next = ((void*)0),
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
enum_metadata_t* tokenizer_error_metadata(){
    static enum_element_metadata_t var_0 = (enum_element_metadata_t) {
        .next = ((void*)0),
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
enum_metadata_t* parse_error_code_metadata(){
    static enum_element_metadata_t var_0 = (enum_element_metadata_t) {
        .next = ((void*)0),
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
enum_metadata_t* token_type_metadata(){
    static enum_element_metadata_t var_0 = (enum_element_metadata_t) {
        .next = ((void*)0),
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
enum_metadata_t* numeric_literal_encoding_metadata(){
    static enum_element_metadata_t var_0 = (enum_element_metadata_t) {
        .next = ((void*)0),
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
  case PARSE_NODE_COMPOUND_LITERAL:
    return "PARSE_NODE_COMPOUND_LITERAL";
  case PARSE_NODE_DESIGNATED_INITIALIZER:
    return "PARSE_NODE_DESIGNATED_INITIALIZER";
  default:
    return "<<unknown-parse_node_type>>";
  }
}
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
  if (strcmp(value, "PARSE_NODE_COMPOUND_LITERAL") == 0) {
    return PARSE_NODE_COMPOUND_LITERAL;
  }
  if (strcmp(value, "PARSE_NODE_DESIGNATED_INITIALIZER") == 0) {
    return PARSE_NODE_DESIGNATED_INITIALIZER;
  }
  return 0;
}
enum_metadata_t* parse_node_type_metadata(){
    static enum_element_metadata_t var_0 = (enum_element_metadata_t) {
        .next = ((void*)0),
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
    static enum_element_metadata_t var_40 = (enum_element_metadata_t) {
        .next = &var_39,
        .name = "PARSE_NODE_COMPOUND_LITERAL",
        .value = PARSE_NODE_COMPOUND_LITERAL
    };
    static enum_element_metadata_t var_41 = (enum_element_metadata_t) {
        .next = &var_40,
        .name = "PARSE_NODE_DESIGNATED_INITIALIZER",
        .value = PARSE_NODE_DESIGNATED_INITIALIZER
    };
    static enum_metadata_t enum_metadata_result = (enum_metadata_t) {
        .name = "parse_node_type_t",
        .elements = &var_41
    };
    return &enum_metadata_result;
}
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
enum_metadata_t* pratt_parser_operation_metadata(){
    static enum_element_metadata_t var_0 = (enum_element_metadata_t) {
        .next = ((void*)0),
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
associativity_t string_to_associativity(char* value){
  if (strcmp(value, "LEFT_TO_RIGHT") == 0) {
    return LEFT_TO_RIGHT;
  }
  if (strcmp(value, "RIGHT_TO_LEFT") == 0) {
    return RIGHT_TO_LEFT;
  }
  return 0;
}
enum_metadata_t* associativity_metadata(){
    static enum_element_metadata_t var_0 = (enum_element_metadata_t) {
        .next = ((void*)0),
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
enum_metadata_t* precedence_metadata(){
    static enum_element_metadata_t var_0 = (enum_element_metadata_t) {
        .next = ((void*)0),
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
enum_metadata_t* type_qualifier_metadata(){
    static enum_element_metadata_t var_0 = (enum_element_metadata_t) {
        .next = ((void*)0),
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
enum_metadata_t* type_node_kind_metadata(){
    static enum_element_metadata_t var_0 = (enum_element_metadata_t) {
        .next = ((void*)0),
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

// Full Compiler Command Line:
//
// /home/jawilson/src/omni-c/build-dir/bin/omni-c-stable
//    generate-library
//    --use-statement-parser=true
//    --omit-c-armyknife-include=true
//    --c-output-file=/home/jawilson/src/omni-c/build-dir/omni-c.c
//    lib/omni-c.c
//    lib/min-max.c
//    lib/boolean.c
//    lib/compound-literal.c
//    lib/fn.c
//    lib/leb128.c
//    lib/fatal-error.c
//    lib/value.c
//    lib/gc-allocate.c
//    lib/uint64.c
//    lib/string-util.c
//    lib/logger.c
//    lib/utf8-decoder.c
//    lib/buffer.c
//    lib/byte-stream.c
//    lib/value-array.c
//    lib/value-alist.c
//    lib/string-alist.c
//    lib/value-hashtable.c
//    lib/string-hashtable.c
//    lib/value-tree.c
//    lib/string-tree.c
//    lib/flag.c
//    lib/io.c
//    lib/terminal.c
//    lib/tokenizer.c
//    lib/random.c
//    lib/cdl-printer.c
//    lib/sub-process.c
//    lib/splitjoin.c
//    lib/oarchive.c
//    lib/test.c
//    mode.c
//    keywords.c
//    file.c
//    file-reader.c
//    compiler-errors.c
//    lexer.c
//    token-list.c
//    token-transformer.c
//    parser.c
//    pstate.c
//    declaration-parser.c
//    node-list.c
//    debug-printer.c
//    c-file-printer.c
//    symbol-table.c
//    source-to-source.c
//    preprocessor.c
//    header-file-extractor.c
//    symbol-table-builder.c
//    srcgen.c
//    pratt-parser.c
//    statement-parser.c
//    type-parser.c
//    user-type-parser.c
//    variable-definition-parser.c
//    literal-parser.c
//    balanced-construct-parser.c
//    printer.c
//    global-includes.c
//    main.c
//    /home/jawilson/src/omni-c/build-dir/gen-files/reflection-header.c

// These checksums are currently easy to fake for example by using a
// hacked git in the PATH at the time this compile was run.
//
// git cat-file -p 613ecb5073e9f6b8b3d2a2deb77a3040a0388d62 > lib/omni-c.c
// git cat-file -p 58637a0049c50cf56fe40e9b2a9de543c095785a > lib/min-max.c
// git cat-file -p 34c0f39b6f480c7ce9cb93fd79586d8900323bdf > lib/boolean.c
// git cat-file -p bafe73cf643ee7eaf31f00c45d2ba446e8eba75a > lib/compound-literal.c
// git cat-file -p 410a738d9b347300ac5cb091d8ebe87c5d9c7588 > lib/fn.c
// git cat-file -p 35485aa76c4e839b7b7511a1e88913c5b7e54053 > lib/leb128.c
// git cat-file -p f0dc18c1fe99776d4fc87d538344c111342ad10e > lib/fatal-error.c
// git cat-file -p 89bcbe94972d0735935a4a43ccbfc7a67f6f4e6d > lib/value.c
// git cat-file -p 370981574e9bb6b426bf03982b46d340a9d510bc > lib/gc-allocate.c
// git cat-file -p fbd604e90e7d4f7b4c4d66801313e8d0e41025ab > lib/uint64.c
// git cat-file -p 78058c8251adf1a3664f16e043c4c19f4454a263 > lib/string-util.c
// git cat-file -p 20249a8bdf4b73beb31749c1a059600223b1dd37 > lib/logger.c
// git cat-file -p 01d1c058798c5549bc283296ea79d5419a1b8bd0 > lib/utf8-decoder.c
// git cat-file -p aa6ee8f5be5115c1c6585ee454754a22542480e5 > lib/buffer.c
// git cat-file -p 800182d499c344faba101baa94359c9c585a77a1 > lib/byte-stream.c
// git cat-file -p 1f8694a528399660a81e3a72fac3a3314c77eee2 > lib/value-array.c
// git cat-file -p e5cc5167198373956762e1b8f96f71bcb6343ba4 > lib/value-alist.c
// git cat-file -p c560ad207e743ce3b364b58540b81a7051072f0a > lib/string-alist.c
// git cat-file -p e8c7b621cbbcfe1f3807635aa3d329f73e0db429 > lib/value-hashtable.c
// git cat-file -p 741b85af593dda43aeb84bd32ec3ed36c8f85c55 > lib/string-hashtable.c
// git cat-file -p 1d0266094b9c0915098e1352edbc8d62ee77abb0 > lib/value-tree.c
// git cat-file -p 8d8502036da69d42e740206185219ba74887f34a > lib/string-tree.c
// git cat-file -p 1ccb428c8923e3d6aad2473fcf93f08c66c0f7db > lib/flag.c
// git cat-file -p f92b16dde49a695a28f50a43d46a753935e40024 > lib/io.c
// git cat-file -p 19ca1105e05dedea799ae80d8feeb6eb22b74976 > lib/terminal.c
// git cat-file -p f1d5e507966591d900a0301e08fad74c425b02e0 > lib/tokenizer.c
// git cat-file -p 421d3674f0800a77e5a52d255ae2b58f2c031ccd > lib/random.c
// git cat-file -p c4db2805aa9ad2dbf641dec703d2c57d087aad56 > lib/cdl-printer.c
// git cat-file -p c8e79201ff43c4457146a87b79243e7b7b4a6c05 > lib/sub-process.c
// git cat-file -p 846d04fa56c6bc8811a8ffdf1fd15991bd31e03d > lib/splitjoin.c
// git cat-file -p fbc6df051ad9a1e3a0c09f1e2b07274607eefc28 > lib/oarchive.c
// git cat-file -p 6d13af369d7152831ca19c4c8202aea995544067 > lib/test.c
// git cat-file -p e4066229527451dabf7ddebeaa5c2becab2bb136 > mode.c
// git cat-file -p 6c0a741ef33f143d100f562fbb6624a0e4b0bb39 > keywords.c
// git cat-file -p 3c9874790e23604a9ac3637dad2d489b9da77adb > file.c
// git cat-file -p 519bac76f7060874edf686e186bbd0127c492875 > file-reader.c
// git cat-file -p 004af75f97ab6e1a5c9176e991a9f17f6e07d515 > compiler-errors.c
// git cat-file -p 7188ecb3f4f5b2ddbb91624982c6d81868672f02 > lexer.c
// git cat-file -p 0b1ed82677427f66828e3ccbad7c004541d0a0ca > token-list.c
// git cat-file -p 570c78c6604478afa7842cdc1f7a1a03d88cb53c > token-transformer.c
// git cat-file -p bc27a1fa5e0e46e5be7efb58913bf35141b3d84d > parser.c
// git cat-file -p 93f67393615462ba6d0dfb65979007280ae43e50 > pstate.c
// git cat-file -p 33f0b96b151a7506a48abca978bed1a25410fe61 > declaration-parser.c
// git cat-file -p 67c4a0e6f2de51b5ed41c289ff9575d4a21a0c68 > node-list.c
// git cat-file -p fd66d6ae20a88b0cccad5b19b09e4fa61c28792a > debug-printer.c
// git cat-file -p 3c2da52b70818ac42e587f1a31e477ce7d046d6c > c-file-printer.c
// git cat-file -p 47fe4a94a6ba151764cfb98f6d995a8c47fb3e01 > symbol-table.c
// git cat-file -p b246b13119ee9f082fbf88b642b914b6d0b7fb7d > source-to-source.c
// git cat-file -p 1e79594833a28a98d4c1e991e46cc8247b2af3af > preprocessor.c
// git cat-file -p 52af4fa58ea28535587766f2912d174b3856816f > header-file-extractor.c
// git cat-file -p 7e7f49598c2d9701253f09009d7012618ade9339 > symbol-table-builder.c
// git cat-file -p 1ef0ccd414c4831d299ebfe679272b61dcc98699 > srcgen.c
// git cat-file -p 2c7444aaad7eb274e29ca6fa490305a17a88c862 > pratt-parser.c
// git cat-file -p f923613065cf23c91a715e7e4661647d6a8a08c6 > statement-parser.c
// git cat-file -p 3f07bb1029d51a41d0a1e7b7ffe8095f7df1b518 > type-parser.c
// git cat-file -p 9e037abaf89da4c6ea500ebf09b2bb3cd64a570a > user-type-parser.c
// git cat-file -p 17bb7334f2306be9c30ef56bbd46456bf20824c9 > variable-definition-parser.c
// git cat-file -p 9d05a19accd8aad90f0eebd7490a271f447b0e52 > literal-parser.c
// git cat-file -p 1d763bad1a86fe024fae36db07e66a5c160393c9 > balanced-construct-parser.c
// git cat-file -p a1246b1440b1757d6547a3b3f595d5bb0646849a > printer.c
// git cat-file -p ea1a646a86f833872d5be645ae3403283baf459f > global-includes.c
// git cat-file -p e6c66e3900bdf556d55ae31ee6fc5f826c43afe9 > main.c
// git cat-file -p 5468645d54d6be77dddb3bd24c1b49a23dae2e45 > /home/jawilson/src/omni-c/build-dir/gen-files/reflection-header.c
