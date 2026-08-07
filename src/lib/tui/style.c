// This is automatically generated code from the script style.roci

typedef style_t = uint64_t;

static inline uint32_t get_background(style_t style) {
  return (style >> 0) & 0xffffffULL;
}
static inline style_t set_background(style_t style, uint32_t x) {
  return (style & ~(0xffffffULL << 0)) | cast(uint64_t, x) << 0;
}

static inline uint8_t get_background_blue(style_t style) {
  return (style >> 0) & 0xffULL;
}
static inline style_t set_background_blue(style_t style, uint8_t x) {
  return (style & ~(0xffULL << 0)) | cast(uint64_t, x) << 0;
}

static inline uint8_t get_background_green(style_t style) {
  return (style >> 8) & 0xffULL;
}
static inline style_t set_background_green(style_t style, uint8_t x) {
  return (style & ~(0xffULL << 8)) | cast(uint64_t, x) << 8;
}

static inline uint8_t get_background_red(style_t style) {
  return (style >> 16) & 0xffULL;
}
static inline style_t set_background_red(style_t style, uint8_t x) {
  return (style & ~(0xffULL << 16)) | cast(uint64_t, x) << 16;
}

static inline uint32_t get_foreground(style_t style) {
  return (style >> 24) & 0xffffffULL;
}
static inline style_t set_foreground(style_t style, uint32_t x) {
  return (style & ~(0xffffffULL << 24)) | cast(uint64_t, x) << 24;
}

static inline uint8_t get_foreground_blue(style_t style) {
  return (style >> 24) & 0xffULL;
}
static inline style_t set_foreground_blue(style_t style, uint8_t x) {
  return (style & ~(0xffULL << 24)) | cast(uint64_t, x) << 24;
}

static inline uint8_t get_foreground_green(style_t style) {
  return (style >> 32) & 0xffULL;
}
static inline style_t set_foreground_green(style_t style, uint8_t x) {
  return (style & ~(0xffULL << 32)) | cast(uint64_t, x) << 32;
}

static inline uint8_t get_foreground_red(style_t style) {
  return (style >> 40) & 0xffULL;
}
static inline style_t set_foreground_red(style_t style, uint8_t x) {
  return (style & ~(0xffULL << 40)) | cast(uint64_t, x) << 40;
}

static inline boolean_t get_underline(style_t style) {
  return (style >> 49) & 1ULL;
}
static inline style_t set_underline(style_t style, boolean_t x) {
  return (style & ~(1ULL << 49)) | cast(uint64_t, x) << 49;
}

static inline boolean_t get_bold(style_t style) { return (style >> 50) & 1ULL; }
static inline style_t set_bold(style_t style, boolean_t x) {
  return (style & ~(1ULL << 50)) | cast(uint64_t, x) << 50;
}

static inline boolean_t get_dim(style_t style) { return (style >> 51) & 1ULL; }
static inline style_t set_dim(style_t style, boolean_t x) {
  return (style & ~(1ULL << 51)) | cast(uint64_t, x) << 51;
}

static inline boolean_t get_italic(style_t style) {
  return (style >> 52) & 1ULL;
}
static inline style_t set_italic(style_t style, boolean_t x) {
  return (style & ~(1ULL << 52)) | cast(uint64_t, x) << 52;
}

static inline boolean_t get_underline(style_t style) {
  return (style >> 53) & 1ULL;
}
static inline style_t set_underline(style_t style, boolean_t x) {
  return (style & ~(1ULL << 53)) | cast(uint64_t, x) << 53;
}

static inline boolean_t get_overline(style_t style) {
  return (style >> 54) & 1ULL;
}
static inline style_t set_overline(style_t style, boolean_t x) {
  return (style & ~(1ULL << 54)) | cast(uint64_t, x) << 54;
}

static inline boolean_t get_strikethrough(style_t style) {
  return (style >> 55) & 1ULL;
}
static inline style_t set_strikethrough(style_t style, boolean_t x) {
  return (style & ~(1ULL << 55)) | cast(uint64_t, x) << 55;
}

static inline boolean_t get_superscript(style_t style) {
  return (style >> 56) & 1ULL;
}
static inline style_t set_superscript(style_t style, boolean_t x) {
  return (style & ~(1ULL << 56)) | cast(uint64_t, x) << 56;
}

static inline boolean_t get_subscript(style_t style) {
  return (style >> 57) & 1ULL;
}
static inline style_t set_subscript(style_t style, boolean_t x) {
  return (style & ~(1ULL << 57)) | cast(uint64_t, x) << 57;
}

static inline boolean_t get_slow_blink(style_t style) {
  return (style >> 58) & 1ULL;
}
static inline style_t set_slow_blink(style_t style, boolean_t x) {
  return (style & ~(1ULL << 58)) | cast(uint64_t, x) << 58;
}

static inline boolean_t get_fast_blink(style_t style) {
  return (style >> 59) & 1ULL;
}
static inline style_t set_fast_blink(style_t style, boolean_t x) {
  return (style & ~(1ULL << 59)) | cast(uint64_t, x) << 59;
}
