/**
 * @function current_time_millis
 *
 * Wall-clock time (milliseconds since Unix epoch).
 */
uint64_t current_time_millis(void) {
    FILETIME ft = {0};
    GetSystemTimeAsFileTime(&ft);

    /* Combine 32-bit high and low parts into a 64-bit integer */
    ULARGE_INTEGER uli = {0};
    uli.LowPart = ft.dwLowDateTime;
    uli.HighPart = ft.dwHighDateTime;

    /*
     * Windows epoch (Jan 1, 1601) to Unix epoch (Jan 1, 1970) offset
     * in 100-nanosecond intervals.
     */
    uint64_t win_epoch_offset = 116444736000000000ULL;

    if (uli.QuadPart < win_epoch_offset) {
        return 0ULL;
    }

    uint64_t intervals = uli.QuadPart - win_epoch_offset;

    /* 1 millisecond = 10,000 100-nanosecond intervals */
    return intervals / 10000ULL;
}
