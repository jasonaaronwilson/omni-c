/**
 * @function to_platform_path
 *
 * Converts to the internal forward-slash path to a native Windows
 * backslash path. This may or may not return a copy. For now, drive
 * letters are ignored an may cause problems.
 */
char* to_platform_path(char* internal_path) {
    if (internal_path == NULL) {
        return nullptr;
    }

    char* platform_path = string_duplicate(internal_path);
    size_t len = strlen(platform_path);

    for (size_t i = 0; i < len; ++i) {
        if (platform_path[i] == '/') {
            platform_path[i] = '\\';
        }
    }

    return platform_path;
}

/**
 * @function from_platform_path
 *
 * Converts a possibly native Windows backslash path to an internal
 * forward-slash path. This may or may not return a copy.
 */
char* from_platform_path(char* platform_path) {
    if (platform_path == NULL) {
        return nullptr;
    }

    char* internal_path = string_duplicate(platform_path);
    size_t len = strlen(internal_path);

    for (size_t i = 0; i < len; ++i) {
        if (internal_path[i] == '\\') {
            internal_path[i] = '/';
        }
    }

    return internal_path;
}
