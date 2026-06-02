/**
 * @file roci-value.c
 *
 * Defines roci_value_t, the dynamically typed values that the Roci VM
 * operates on. Potentially for the worse, we also use these as the
 * values stored in environments where the tag and raw data are
 * manipulated.
 */

/**
 * @typedef roci_value_t
 *
 * This is the primary roci dynamic value and is likely 128bits though
 * we only need 8-bits for the tag. This is what bindings point at
 * (and potentially all of the values of the stack...)
 */
typedef roci_value_t = struct {
  // A raw 64bit value to be interpreted many ways based on the tag.
  uint64_t raw;
  roci_tag_t tag;
};

/**
 * @function roci_append_value
 *
 * Append the human readable version of the roci value.
 */
void roci_append_value(buffer_t* buffer, roci_value_t value) {
  switch (value.tag) {
  case ROCI_TAG_STRING:
    buffer_printf(buffer, "\"%s\"", cast(char*, value.raw));
    break;
  case ROCI_TAG_INTEGER:
    // FIXME: it's an integer (not unsigned) stupid!
    buffer_printf(buffer, "%s", uint64_to_string(value.raw));
    break;
  case ROCI_TAG_C_PRIMITIVE:
    buffer_printf(buffer, "primitive<%s>", uint64_to_string(value.raw));
    break;
  case ROCI_TAG_CLOSURE:
    roci_closure_t* closure = cast(roci_closure_t*, value.raw);
    buffer_printf(buffer, "closure<%s,%s>",
                  uint64_to_string(cast(uint64_t, closure->entry_point)),
                  uint64_to_string(cast(uint64_t, closure->env)));
    break;
  default:
    log_fatal("unhandled tag %s", roci_tag_to_string(value.tag));
    fatal_error(ERROR_ILLEGAL_STATE);
  }
}

char* roci_value_to_c_string(roci_value_t value) {
  buffer_t* buf = make_buffer(10);
  roci_append_value(buf, value);
  return buffer_to_c_string(buf);
}
