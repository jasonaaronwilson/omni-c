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

boolean_t path_is_directory(const char* path) {
  if (!path) {
    return false;
  }
  DWORD attrs = GetFileAttributesA(path);
  if (attrs == INVALID_FILE_ATTRIBUTES) {
    return false;
  }
  return cast(boolean_t, (attrs & FILE_ATTRIBUTE_DIRECTORY) != 0);
}

boolean_t path_is_file(const char* path) {
  if (!path) {
    return false;
  }
  DWORD attrs = GetFileAttributesA(path);
  if (attrs == INVALID_FILE_ATTRIBUTES) {
    return false;
  }
  return cast(boolean_t, (attrs & FILE_ATTRIBUTE_DIRECTORY) == 0);
}

/**
 * Checks if the file path already has a recognizable extension (e.g., .exe, .bat).
 */
boolean_t has_file_extension(char* path) {
    char* dot = strrchr(path, '.');
    char* slash = strrchr(path, '/');
    char* backslash = strrchr(path, '\\');

    // If dot exists after any path separator, it has an extension
    if (dot != NULL) {
        if ((slash == NULL || dot > slash) && (backslash == NULL || dot > backslash)) {
            return true;
        }
    }
    return false;
}

/**
 * Resolves a bare executable name or relative path to an absolute executable path.
 * Returns a newly allocated string (via malloc), or NULL if not found.
 */
char* resolve_executable_path(char* name) {
    if (name == NULL || *name == '\0') {
        return NULL;
    }

    // 1. If it already contains slashes/backslashes, treat as relative/absolute path
    if (strchr(name, '/') != NULL || strchr(name, '\\') != NULL) {
        DWORD len = GetFullPathNameA(name, 0, NULL, NULL);
        if (len == 0) return NULL;

        char* resolved = cast(char*, malloc_bytes(len));
        if (GetFullPathNameA(name, len, resolved, NULL) == 0) {
            // free/fail
            return NULL;
        }

        // Verify the file actually exists
        DWORD attrs = GetFileAttributesA(resolved);
        if (attrs != INVALID_FILE_ATTRIBUTES && !(attrs & FILE_ATTRIBUTE_DIRECTORY)) {
            return resolved;
        }

        // If not found and lacks an extension, try appending .exe
        if (!has_file_extension(resolved)) {
            char with_ext[MAX_PATH];
            snprintf(with_ext, sizeof(with_ext), "%s.exe", resolved);
            attrs = GetFileAttributesA(with_ext);
            if (attrs != INVALID_FILE_ATTRIBUTES && !(attrs & FILE_ATTRIBUTE_DIRECTORY)) {
                // Return copy of with_ext
                size_t ext_len = strlen(with_ext) + 1;
                char* result = cast(char*, malloc_bytes(ext_len));
                memcpy(result, with_ext, ext_len);
                return result;
            }
        }
        return NULL;
    }

    // 2. Bare command name: Search standard Windows path & PATH environment variable.
    // If it has no extension, default to searching with ".exe".
    char* default_ext = has_file_extension(name) ? NULL : ".exe";

    // Query required buffer size
    DWORD len = SearchPathA(NULL, name, default_ext, 0, NULL, NULL);
    if (len == 0) {
        // If searching with default_ext failed (or was skipped), try other common PATHEXT extensions
        if (default_ext != NULL) {
            char* fallback_exts[] = { ".cmd", ".bat", ".com", NULL };
            for (int i = 0; fallback_exts[i] != NULL; i++) {
                len = SearchPathA(NULL, name, fallback_exts[i], 0, NULL, NULL);
                if (len > 0) {
                    default_ext = fallback_exts[i];
                    break;
                }
            }
        }
    }

    if (len == 0) {
        return NULL; // Executable not found in PATH
    }

    char* resolved = cast(char*, malloc_bytes(len + 1));
    char* file_part = NULL;
    DWORD res = SearchPathA(NULL, name, default_ext, len + 1, resolved, &file_part);
    if (res == 0 || res > len) {
        return NULL;
    }

    return resolved;
}
