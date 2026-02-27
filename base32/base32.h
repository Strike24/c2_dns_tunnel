#ifndef BASE32_H
#define BASE32_H

#include <stddef.h>

const char BASE32_ALPHABET[];

/**
 * Encode binary data into RFC4648-style Base32 text with '=' padding.
 *
 * Returns 0 on success, -1 on error.
 */
int encode_base32(const unsigned char *data,
                  size_t length,
                  char *encoded,
                  size_t encoded_size,
                  size_t *out_len);

/**
 * Decode Base32 text into binary data.
 * Whitespace is ignored and both uppercase/lowercase letters are accepted.
 *
 * Returns 0 on success, -1 on error.
 */
int decode_base32(const char *data,
                  unsigned char *decoded,
                  size_t decoded_size,
                  size_t *out_len);

#endif // BASE32_H