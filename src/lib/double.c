/**
 * Reinterprets the bits of a double as a 64-bit unsigned integer.
 */
uint64_t double_as_uint64(double d) {
    uint64_t u;
    memcpy(&u, &d, sizeof(d));
    return u;
}

/**
 * Reinterprets the bits of a 64-bit unsigned integer as a double.
 */
uint64_t uint64_as_double(uint64_t u) {
    double d;
    memcpy(&d, &u, sizeof(u));
    return d;
}
