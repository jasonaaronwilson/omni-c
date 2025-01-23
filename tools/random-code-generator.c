// This program will be written in "pure" Omni C itself despite the .c
// extensions.

#define C_ARMYKNIFE_LIB_IMPL

#include <c-armyknife-lib.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define NUM_FRAGMENTS 100

typedef enum {
  FRAGMENT_ENUM,
  FRAGMENT_ENUM_TYPEDEF,
  FRAGMENT_STRUCT,
  FRAGMENT_STRUCT_TYPEDEF,
} fragment_type_t;

// Fragment structure (simplified)
typedef struct fragment_s {
  fragment_type_t fragment_type;
  char* type_name; // Sometimes struct foo, and sometimes just foo.
  char* c_definition_text;
} fragment_t;

fragment_t* fragments[NUM_FRAGMENTS];
int fragment_count = 0;

static inline int random_under(int limit) {
  return (int) random_next_uint64_below(random_state(), limit);
}

char* generate_random_name(char* str) {
  return string_printf("%s_%d", str, random_under(INT_MAX));
}

fragment_t* get_random_type_fragment() {
  int n = random_under(fragment_count);
  if (fragment_count > 10) {
    n = n & (-1 << 1);
  }
  return fragments[n];
}

char* get_random_type_suffix() {
  if (random_under(2) == 0) {
    return "*";
  } else {
    return "";
  }
}

char* get_random_type_name() {
  char* type_name = NULL;
  if (random_under(2) == 0) {
    // fixme, choose one at random.
    type_name = "int";
  } else {
    type_name = get_random_type_fragment(random_state)->type_name;
  }

  return string_printf("%s%s", type_name, get_random_type_suffix());
}

fragment_t* generate_enum_fragment() {
  fragment_t* result = malloc_struct(fragment_t);
  result->fragment_type = FRAGMENT_ENUM;
  char* raw_type_name = generate_random_name("gen_enum");
  result->type_name = string_printf("enum %s", raw_type_name);
  result->c_definition_text = string_printf("enum %s {%s};\n", raw_type_name,
                                            generate_random_name("ENUM_VALUE"));
  return result;
}

fragment_t* generate_struct_fragment() {
  fragment_t* result = malloc_struct(fragment_t);
  result->fragment_type = FRAGMENT_STRUCT;
  char* raw_type_name = generate_random_name("gen_struct");
  result->type_name = string_printf("struct %s", raw_type_name);
  buffer_t* text = make_buffer(1);
  buffer_printf(text, "struct %s {\n", raw_type_name);
  if (fragment_count > 0) {
    int num_fields = random_under(8) + 1;
    for (int i = 0; i < num_fields; i++) {
      char* field_type = get_random_type_name();
      char* field_name = generate_random_name("field");
      buffer_printf(text, "   %s %s;\n", field_type, field_name);
    }
  }
  buffer_printf(text, "};\n");
  result->c_definition_text = buffer_to_c_string(text);
  return result;
}

// Function to generate a fragment
fragment_t* generate_fragment() {
  if (random_under(2) == 0) {
    return generate_enum_fragment(random_state);
  } else {
    return generate_struct_fragment(random_state);
  }

#if 0
    switch (random_next_uint64_below(random_state, FRAGMENT_STRUCT_TYPEDEF)) {
    case FRAGMENT_ENUM:
      return generate_enum_fragment(random_state);
    case FRAGMENT_ENUM_TYPEDEF:
      return generate_enum_typedef_fragment(random_state);
    case FRAGMENT_STRUCT:
      return generate_struct_fragment(random_state);
    case FRAGMENT_STRUCT_TYPEDEF:
      return generate_struct_typedef_fragment(random_state);
    }
#endif /* 0 */
}

void shuffle_array(fragment_t* arr[], int size) {
  for (int i = size - 1; i > 0; i--) {
    int j = random_under(i + 1); // Generate random index between 0 and i
    // Swap arr[i] and arr[j]
    fragment_t* temp = arr[i];
    arr[i] = arr[j];
    arr[j] = temp;
  }
}

// Main function
int main(int argc, char** argv) {
  // random_state_t state = random_state_for_test();
  // random_state = &state;

  // Generate fragments. This will naturally be in the correct order
  // for C to handle as we are generating based off of a tree. This is
  // not perfect because some-times we should generate what seems like
  // a circular dependency but one that is broken because of a
  // pointer...
  for (int i = 0; i < NUM_FRAGMENTS; i++) {
    fragments[i] = generate_fragment(random_state);
    fragment_count = i;
  }

  // Now shuffle these definitions as omni-c source code need not be
  // defined in a particular order.
  shuffle_array(fragments, fragment_count);

  // Start with a prelude
  fprintf(stdout, "#include <stdlib.h>\n");

  // Now we can print out the definitions.
  for (int i = 0; i < NUM_FRAGMENTS; i++) {
    fprintf(stdout, "%s\n", fragments[i]->c_definition_text);
  }

  // Finally, a dummy main routine
  fprintf(stdout, "int main(int argc, char** args) {\n");
  fprintf(stdout, "    exit(0);\n");
  fprintf(stdout, "}\n");

  exit(0);
}
