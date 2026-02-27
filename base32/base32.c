// External code for base32 encoding and decoding
// Not written by me, but included in the project for convenience

#include <stddef.h>
#include "base32.h"

const char BASE32_ALPHABET[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ234567";

static int base32_char_value(char c)
{
    if (c >= 'A' && c <= 'Z')
        return c - 'A';
    if (c >= '2' && c <= '7')
        return 26 + (c - '2');
    if (c >= 'a' && c <= 'z')
        return c - 'a';
    return -1;
}

static int is_base32_whitespace(char c)
{
    return c == ' ' || c == '\t' || c == '\n' || c == '\r';
}

static size_t base32_encoded_length(size_t input_length)
{
    return ((input_length + 4) / 5) * 8;
}

int encode_base32(const unsigned char *data, size_t length, char *encoded, size_t encoded_size, size_t *out_len)
{
    if (!encoded || (!data && length != 0))
        return -1;

    size_t required = base32_encoded_length(length) + 1;
    if (encoded_size < required)
        return -1;

    size_t out = 0;
    unsigned int buffer = 0;
    int bits_left = 0;

    for (size_t i = 0; i < length; i++)
    {
        buffer = (buffer << 8) | data[i];
        bits_left += 8;
        while (bits_left >= 5)
        {
            int index = (buffer >> (bits_left - 5)) & 0x1F;
            encoded[out++] = BASE32_ALPHABET[index];
            bits_left -= 5;
        }
    }

    if (bits_left > 0)
    {
        int index = (buffer << (5 - bits_left)) & 0x1F;
        encoded[out++] = BASE32_ALPHABET[index];
    }

    while (out % 8 != 0)
        encoded[out++] = '=';

    encoded[out] = '\0';
    if (out_len)
        *out_len = out;
    return 0;
}

int decode_base32(const char *data, unsigned char *decoded, size_t decoded_size, size_t *out_len)
{
    if (!data || !decoded)
        return -1;

    size_t valid_count = 0;
    for (size_t i = 0; data[i] != '\0'; i++)
    {
        if (data[i] == '=')
            break;
        if (is_base32_whitespace(data[i]))
            continue;
        if (base32_char_value(data[i]) < 0)
            return -1;
        valid_count++;
    }

    size_t required = (valid_count * 5) / 8;
    if (decoded_size < required)
        return -1;

    size_t out = 0;
    unsigned int buffer = 0;
    int bits_left = 0;

    for (size_t i = 0; data[i] != '\0'; i++)
    {
        if (data[i] == '=')
            break;
        if (is_base32_whitespace(data[i]))
            continue;

        int val = base32_char_value(data[i]);
        if (val < 0)
            return -1;

        buffer = (buffer << 5) | (unsigned int)val;
        bits_left += 5;

        while (bits_left >= 8)
        {
            decoded[out++] = (unsigned char)((buffer >> (bits_left - 8)) & 0xFF);
            bits_left -= 8;
        }
    }

    if (out_len)
        *out_len = out;
    return 0;
}