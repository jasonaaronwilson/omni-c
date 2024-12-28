#include <stdint.h>

#define ERROR_INSUFFICIENT_INPUT -1
#define ERROR_TOO_BIG -2

typedef struct {
  uint64_t number;
  // Negative values mean an error occurred.
  int size;
} unsigned_decode_result;

typedef struct {
  uint64_t number;
  // Negative values mean an error occurred.
  int size;
} signed_decode_result;

extern unsigned encodeSLEB128(int64_t Value, uint8_t *p);
extern unsigned encodeULEB128(uint64_t Value, uint8_t *p);
extern unsigned_decode_result decodeULEB128(const uint8_t *p, const uint8_t *end);
extern signed_decode_result decodeSLEB128(const uint8_t *p, const uint8_t *end);

extern unsigned split_loop_encodeSLEB128(int64_t Value, uint8_t *p);

extern unsigned array_notation_encodeSLEB128(int64_t Value, uint8_t *p);

extern unsigned parallel_encodeULEB128(int64_t Value, uint8_t *p);
