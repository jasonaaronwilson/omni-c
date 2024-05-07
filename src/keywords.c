#line 2 "keywords.c"
#ifndef _KEYWORDS_H_
#define _KEYWORDS_H_

#include "mode.h"
#include <c-armyknife-lib.h>

boolean_t is_reserved_word(input_mode_t mode, char* str);
boolean_t is_builtin_type_name(input_mode_t mode, char* str);

#endif /* _KEYWORDS_H_ */

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

string_hashtable_t* c_keywords_ht = NULL;
string_hashtable_t* c_builtin_types_ht = NULL;

/*
cpp_keywords = {
    // C++98 Keywords (includes most C keywords)
    'asm': None,           // Inline assembly
    'class': None,         // Define a class
    'new': None,           // Dynamic memory allocation
    'delete': None,        // Dynamic memory deallocation
    'template': None,      // Templates (generic programming)
    'typename': None,      // Used in templates
    'this': None,          // Pointer to current object
    'try': None,           // Start of try block (exception handling)
    'catch': None,         // Start of catch block (exception handling)
    'throw': None,         // Throw an exception

    // C++11 Keywords
    'nullptr': None,       // Null pointer literal
    'constexpr': None,     // Constant expressions
    'decltype': None,      // Deduce type
    'noexcept': None,      // Function doesn't throw exceptions
    'override': None,      // Explicitly override virtual function
    'final': None,         // Prevent virtual function overriding/class
inheritance 'default': None,       // Used in switch, or to request default
constructor/destructor 'auto': None,          // Automatic type deduction

    // C++14 Keywords
    'declval': None,       // Used in SFINAE (complicated, advanced usage)

    // C++17 Keywords
    'static_assert': None, // Compile-time assertions
    'inline': None,        // Can now be used for variables

    // C++20 Keywords
    'char8_t': None,       // Type for UTF-8 characters
    'concept': None,       // Define constraints for templates
    'requires': None,      // Specify requirements for template parameters
    'coroutine': None,     // Keyword related to coroutines (advanced)
    'co_await': None,      // Used with coroutines
    'co_yield': None,      // Used with coroutines
    'co_return': None,     // Used with coroutines
};

*/

void initialize_keyword_maps(void) {
  // Put the keywords into a hashtable. A binary search would work as
  // well but having seperate arrays is more workable for building up
  // the necessary map's needed without duplicating as many things
  // (for example C++ keywords will sensibly include all keywords from
  // C).
  int num_keywords = sizeof(c_keywords_array) / sizeof(c_keywords_array[0]);
  c_keywords_ht = make_string_hashtable(2 * num_keywords);
  for (int i = 0; i < num_keywords; i++) {
    c_keywords_ht
        = string_ht_insert(c_keywords_ht, c_keywords_array[i], u64_to_value(1));
  }

  int num_types
      = sizeof(c_builtin_types_array) / sizeof(c_builtin_types_array[0]);
  c_builtin_types_ht = make_string_hashtable(2 * num_types);

  for (int i = 0; i < num_types; i++) {
    c_builtin_types_ht = string_ht_insert(
        c_builtin_types_ht, c_builtin_types_array[i], u64_to_value(1));
  }
}

/**
 * @function is_reserved_word
 *
 * Determines if the given str corresponds to a reserved word
 * according to the input mode.
 */
boolean_t is_reserved_word(input_mode_t mode, char* str) {
  if (c_keywords_ht == NULL) {
    initialize_keyword_maps();
  }
  return is_ok(string_ht_find(c_keywords_ht, str));
}

/**
 * @function is_builtin_type_name
 *
 * Determines if the given str corresponds to a predefined type
 * according to the input mode.
 */
boolean_t is_builtin_type_name(input_mode_t mode, char* str) {
  if (c_builtin_types_ht == NULL) {
    initialize_keyword_maps();
  }
  return is_ok(string_ht_find(c_builtin_types_ht, str));
}
