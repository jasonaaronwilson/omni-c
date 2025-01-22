typedef color_t = enum {
  COLOR_UNKNOWN = 0,
  COLOR_RED = 1,
  COLOR_GREEN = 2,
  COLOR_BLUE = 3,
};

int main(int argc, char** argv) {
  if (strcmp("COLOR_RED", color_to_string(COLOR_RED)) != 0) {
    return 1;
  } else {
    return 0;
  }
}
