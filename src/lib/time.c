/**
 * @function
 *
 * Wall-clock time (milliseconds since Unix epoch).
 */
uint64_t current_time_millis(void) {
  struct timespec ts = {0};
  clock_gettime(CLOCK_REALTIME, &ts);
  return cast(uint64_t, ts.tv_sec) * 1000ULL + cast(uint64_t, ts.tv_nsec) / 1000000ULL;
  return 0;
}
