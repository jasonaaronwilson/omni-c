#line 2 "compress.c"

/**
 * @file compress.c
 *
 * This file contains a simple compression and decompression
 * implementation with a focus on simplicity over compression ratio or
 * compression/decompression speed (though decompression should be
 * pretty fast and we care more about that than the compression
 * speed).
 *
 * The algorithm is an implementation of LZSS but byte oriented and
 * lacks Huffman encoding since that would make the implementation
 * larger.
 *
 * The overall format is roughly:
 * 
 * ```
 *  // ===== HEADER =====
 *  // (also encodes the parameters for decoding)
 *  [32 bit magic number]
 *
 *  // Depending on the header, probably 64bit LE integer though an
 *  // ULEB-128 might be a better choice and isn't that hard to
 *  // encode/decode.
 *  [optional length]
 *
 *  // ===== BODY =====
 *
 *  // Now we have a series of "data instructions". Each control byte
 *  // specifies either 4 or 8 of the next data instructions (lowest
 *  // bits are used first).
 *  [control byte]
 *
 *      0b00 single byte
 *      0b01 64 bytes?
 *      0b10 copy 8-bit offset byte + 8-bit length
 *      0b11 copy 16-bit offset byte + 16-bit length (little endian)
 * ```
 *
 * If the length isn't specified, then knowing when to stop
 * decompression has to come "externally", such as the end of a
 * file/stream.
 *
 * Once this is implemented we will compare compression ratios and
 * compression/decompression speed against gzip (since that will make
 * it easy to compare with essentially all compression/decompression
 * algorithms). I'm considering using ULEB-128 for the '11' big
 * data-block and the length and we could also use it for the "big
 * block" literal to allow extremely long uncompressible stuff to have
 * low space overhead but possibly at the expense of compression
 * ratios for compressible entries.
 *
 * The inclusion of big literal blocks should mean minimal overhead
 * when compressing uncompressable data which is likely part of our
 * intended use case (i.e., resources ("byte sources") that can be
 * accessed at runtime to provide text (possibly localized and
 * arranged like a directory of [ISO
 * 639][https://en.wikipedia.org/wiki/ISO_639]), images, etc.
 */

#ifndef _COMPRESS_H_
#define _COMPRESS_H_

typedef enum {
  DATA_BLOCK_BYTE_LITERAL,
  DATA_BLOCK_BLOCK_LITERAL,
  DATA_BLOCK_8BIT_COPY,
  DATA_BLOCK_16BIT_COPY,
} test_enum_t;

byte compress(byte_buffer_t* byte_source, 
		  byte_sink_t* byte_sink,
		  byte_source_t* dictionary_byte_source,
		  boolean_t include_length);

uint64_t decompress(byte_source_t* byte_source, 
		    byte_sink_t* byte_sink, 
		    byte_source_t* dictionary_byte_source);

#endif /* _COMPRESS_H_ */

/**
 * @function compress
 *
 * compress the data in byte_source sending the compressed bytes to
 * the byte_sink.
 */
uint64_t compress(byte_source_t byte_source, byte_sink_t byte_sink, boolean_t include_length) {
  return 0;
}

/**
 * @function decompress
 *
 * decompress the data in byte_source sending the compressed bytes to
 * the byte_sink.
 */
uint64_t decompress(byte_source_t byte_source, byte_sink_t byte_sink) {
  return 0;
}
