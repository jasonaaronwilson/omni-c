/**
 * @function roci_primitive_platform
 *
 * Determines the platform string. Currently only "linux" and
 * "darwin".
 */
void roci_primitive_platform(roci_vm_state_t* state) {
  if (state->n_args != 0) {
    roci_debug_error(state, "platform expects 0 argument");
  }
  roci_push_string(state, "linux");
}
