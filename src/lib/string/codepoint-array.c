/**
 * @file codepoint-array.c
 *
 * A very simple growable array of code-points.
 */

typedef struct {
  uint32_t* chars;
  size_t length;
  size_t capacity;
} codepoint_array_t;

codepoint_array_t* cp_array_make(void) {
  codepoint_array_t* arr = cast(codepoint_array_t*, malloc_bytes(sizeof(codepoint_array_t)));
  arr->capacity = 32;
  arr->length = 0;
  arr->chars = cast(uint32_t*, malloc_bytes(sizeof(uint32_t) * arr->capacity));
  return arr;
}

void cp_array_insert(codepoint_array_t* arr, size_t index, uint32_t cp) {
  if (index > arr->length) return;
  if (arr->length >= arr->capacity) {
    arr->capacity = arr->capacity * 2;
    arr->chars = GC_realloc(arr->chars, sizeof(uint32_t) * arr->capacity);
  }
  // Safe shift without clearing downstream content
  if (index < arr->length) {
    memmove(&arr->chars[index + 1], &arr->chars[index], (arr->length - index) * sizeof(uint32_t));
  }
  arr->chars[index] = cp;
  arr->length++;
}

void cp_array_delete(codepoint_array_t* arr, size_t index) {
  if (index >= arr->length) return;
  if (index + 1 < arr->length) {
    memmove(&arr->chars[index], &arr->chars[index + 1], (arr->length - index - 1) * sizeof(uint32_t));
  }
  arr->length--;
}

buffer_t* cp_array_to_buffer(const codepoint_array_t* arr) {
  buffer_t* buf = make_buffer(arr->length * 2 + 1);
  for (size_t i = 0; i < arr->length; i++) {
    buffer_append_code_point(buf, arr->chars[i]);
  }
  return buf;
}
