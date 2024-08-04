#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

// MD5 context structure
typedef struct {
    uint32_t state[4];
    uint32_t count[2];
    unsigned char buffer[64];
} MD5_CTX;

void MD5Transform(uint32_t state[4], const unsigned char block[64]);
void MD5Init(MD5_CTX *context);
void MD5Update(MD5_CTX *context, const unsigned char *input, unsigned int inputLen);
void MD5Final(unsigned char digest[16], MD5_CTX *context);
void Encode(unsigned char *output, const uint32_t *input, unsigned int len);
void Decode(uint32_t *output, const unsigned char *input, unsigned int len);

#define S11 7
#define S12 12
#define S13 17
#define S14 22
#define S21 5
#define S22 9
#define S23 14
#define S24 20
#define S31 4
#define S32 11
#define S33 16
#define S34 23
#define S41 6
#define S42 10
#define S43 15
#define S44 21

static unsigned char PADDING[64] = {
  0x80
};

static const char HEX_DIGITS[] = "0123456789abcdef";

void MD5Transform(uint32_t state[4], const unsigned char block[64]) {
    uint32_t a = state[0], b = state[1], c = state[2], d = state[3], x[16];
    Decode(x, block, 64);

    /* Round 1 */
    #define FF(a, b, c, d, x, s, ac) { \
        a += ((b & c) | (~b & d)) + x + (uint32_t)(ac); \
        a = ((a << s) | ((a & 0xffffffff) >> (32 - s))); \
        a += b; \
    }
    FF(a, b, c, d, x[ 0], S11, 0xd76aa478); /* 1 */
    FF(a, b, c, d, x[ 1], S12, 0xe8c7b756); /* 2 */
    FF(a, b, c, d, x[ 2], S13, 0x242070db); /* 3 */
    FF(a, b, c, d, x[ 3], S14, 0xc1bdceee); /* 4 */
    FF(a, b, c, d, x[ 4], S11, 0xf57c0faf); /* 5 */
    FF(a, b, c, d, x[ 5], S12, 0x4787c62a); /* 6 */
    FF(a, b, c, d, x[ 6], S13, 0xa8304613); /* 7 */
    FF(a, b, c, d, x[ 7], S14, 0xfd469501); /* 8 */
    FF(a, b, c, d, x[ 8], S11, 0x698098d8); /* 9 */
    FF(a, b, c, d, x[ 9], S12, 0x8b44f7af); /* 10 */
    FF(a, b, c, d, x[10], S13, 0xffff5bb1); /* 11 */
    FF(a, b, c, d, x[11], S14, 0x895cd7be); /* 12 */
    FF(a, b, c, d, x[12], S11, 0x6b901122); /* 13 */
    FF(a, b, c, d, x[13], S12, 0xfd987193); /* 14 */
    FF(a, b, c, d, x[14], S13, 0xa679438e); /* 15 */
    FF(a, b, c, d, x[15], S14, 0x49b40821); /* 16 */

    /* Round 2 */
    #define GG(a, b, c, d, x, s, ac) { \
        a += ((b & d) | (c & ~d)) + x + (uint32_t)(ac); \
        a = ((a << s) | ((a & 0xffffffff) >> (32 - s))); \
        a += b; \
    }
    GG(a, b, c, d, x[ 1], S21, 0xf61e2562); /* 17 */
    GG(a, b, c, d, x[ 6], S22, 0xc040b340); /* 18 */
    GG(a, b, c, d, x[11], S23, 0x265e5a51); /* 19 */
    GG(a, b, c, d, x[ 0], S24, 0xe9b6c7aa); /* 20 */
    GG(a, b, c, d, x[ 5], S21, 0xd62f105d); /* 21 */
    GG(a, b, c, d, x[10], S22, 0x02441453); /* 22 */
    GG(a, b, c, d, x[15], S23, 0xd8a1e681); /* 23 */
    GG(a, b, c, d, x[ 4], S24, 0xe7d3fbc8); /* 24 */
    GG(a, b, c, d, x[ 9], S21, 0x21e1cde6); /* 25 */
    GG(a, b, c, d, x[14], S22, 0xc33707d6); /* 26 */
    GG(a, b, c, d, x[ 3], S23, 0xf4d50d87); /* 27 */
    GG(a, b, c, d, x[ 8], S24, 0x455a14ed); /* 28 */
    GG(a, b, c, d, x[13], S21, 0xa9e3e905); /* 29 */
    GG(a, b, c, d, x[ 2], S22, 0xfcefa3f8); /* 30 */
    GG(a, b, c, d, x[ 7], S23, 0x676f02d9); /* 31 */
    GG(a, b, c, d, x[12], S24, 0x8d2a4c8a); /* 32 */

    /* Round 3 */
    #define HH(a, b, c, d, x, s, ac) { \
        a += (b ^ c ^ d) + x + (uint32_t)(ac); \
        a = ((a << s) | ((a & 0xffffffff) >> (32 - s))); \
        a += b; \
    }
    HH(a, b, c, d, x[ 5], S31, 0xfffa3942); /* 33 */
    HH(a, b, c, d, x[ 8], S32, 0x8771f681); /* 34 */
    HH(a, b, c, d, x[11], S33, 0x6d9d6122); /* 35 */
    HH(a, b, c, d, x[14], S34, 0xfde5380c); /* 36 */
    HH(a, b, c, d, x[ 1], S31, 0xa4beea44); /* 37 */
    HH(a, b, c, d, x[ 4], S32, 0x4bdecfa9); /* 38 */
    HH(a, b, c, d, x[ 7], S33, 0xf6bb4b60); /* 39 */
    HH(a, b, c, d, x[10], S34, 0xbebfbc70); /* 40 */
    HH(a, b, c, d, x[13], S31, 0x289b7ec6); /* 41 */
    HH(a, b, c, d, x[ 0], S32, 0xeaa127fa); /* 42 */
    HH(a, b, c, d, x[ 3], S33, 0xd4ef3085); /* 43 */
    HH(a, b, c, d, x[ 6], S34, 0x04881d05); /* 44 */
    HH(a, b, c, d, x[ 9], S31, 0xd9d4d039); /* 45 */
    HH(a, b, c, d, x[12], S32, 0xe6db99e5); /* 46 */
    HH(a, b, c, d, x[15], S33, 0x1fa27cf8); /* 47 */
    HH(a, b, c, d, x[ 2], S34, 0xc4ac5665); /* 48 */

    /* Round 4 */
    #define II(a, b, c, d, x, s, ac) { \
        a += (c ^ (b | ~d)) + x + (uint32_t)(ac); \
        a = ((a << s) | ((a & 0xffffffff) >> (32 - s))); \
        a += b; \
    }
    II(a, b, c, d, x[ 0], S41, 0xf4292244); /* 49 */
    II(a, b, c, d, x[ 7], S42, 0x432aff97); /* 50 */
    II(a, b, c, d, x[14], S43, 0xab9423a7); /* 51 */
    II(a, b, c, d, x[ 5], S44, 0xfc93a039); /* 52 */
    II(a, b, c, d, x[12], S41, 0x655b59c3); /* 53 */
    II(a, b, c, d, x[ 3], S42, 0x8f0ccc92); /* 54 */
    II(a, b, c, d, x[10], S43, 0xffeff47d); /* 55 */
    II(a, b, c, d, x[ 1], S44, 0x85845dd1); /* 56 */
    II(a, b, c, d, x[ 8], S41, 0x6fa87e4f); /* 57 */
    II(a, b, c, d, x[15], S42, 0xfe2ce6e0); /* 58 */
    II(a, b, c, d, x[ 6], S43, 0xa3014314); /* 59 */
    II(a, b, c, d, x[13], S44, 0x4e0811a1); /* 60 */
    II(a, b, c, d, x[ 4], S41, 0xf7537e82); /* 61 */
    II(a, b, c, d, x[11], S42, 0xbd3af235); /* 62 */
    II(a, b, c, d, x[ 2], S43, 0x2ad7d2bb); /* 63 */
    II(a, b, c, d, x[ 9], S44, 0xeb86d391); /* 64 */

    state[0] += a;
    state[1] += b;
    state[2] += c;
    state[3] += d;

    memset((unsigned char *)x, 0, sizeof(x));
}

void MD5Init(MD5_CTX *context) {
    context->count[0] = context->count[1] = 0;
    context->state[0] = 0x67452301;
    context->state[1] = 0xefcdab89;
    context->state[2] = 0x98badcfe;
    context->state[3] = 0x10325476;
}

void MD5Update(MD5_CTX *context, const unsigned char *input, unsigned int inputLen) {
    unsigned int i, index, partLen;

    index = (unsigned int)((context->count[0] >> 3) & 0x3F);
    if ((context->count[0] += ((uint32_t)inputLen << 3)) < ((uint32_t)inputLen << 3)) {
        context->count[1]++;
    }
    context->count[1] += ((uint32_t)inputLen >> 29);

    partLen = 64 - index;

    if (inputLen >= partLen) {
        memcpy(&context->buffer[index], input, partLen);
        MD5Transform(context->state, context->buffer);

        for (i = partLen; i + 63 < inputLen; i += 64) {
            MD5Transform(context->state, &input[i]);
        }

        index = 0;
    } else {
        i = 0;
    }

    memcpy(&context->buffer[index], &input[i], inputLen - i);
}

void MD5Final(unsigned char digest[16], MD5_CTX *context) {
    unsigned char bits[8];
    unsigned int index, padLen;

    Encode(bits, context->count, 8);

    index = (unsigned int)((context->count[0] >> 3) & 0x3f);
    padLen = (index < 56) ? (56 - index) : (120 - index);
    MD5Update(context, PADDING, padLen);

    MD5Update(context, bits, 8);
    Encode(digest, context->state, 16);

    memset((unsigned char *)context, 0, sizeof(*context));
}

void Encode(unsigned char *output, const uint32_t *input, unsigned int len) {
    unsigned int i, j;

    for (i = 0, j = 0; j < len; i++, j += 4) {
        output[j] = (unsigned char)(input[i] & 0xff);
        output[j + 1] = (unsigned char)((input[i] >> 8) & 0xff);
        output[j + 2] = (unsigned char)((input[i] >> 16) & 0xff);
        output[j + 3] = (unsigned char)((input[i] >> 24) & 0xff);
    }
}

void Decode(uint32_t *output, const unsigned char *input, unsigned int len) {
    unsigned int i, j;

    for (i = 0, j = 0; j < len; i++, j += 4) {
        output[i] = ((uint32_t)input[j]) | (((uint32_t)input[j + 1]) << 8) |
            (((uint32_t)input[j + 2]) << 16) | (((uint32_t)input[j + 3]) << 24);
    }
}

void PrintMD5(unsigned char digest[16]) {
    for (int i = 0; i < 16; i++) {
        dp("%02x", digest[i]);
    }
    dp("\n");
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <filename>\n", argv[0]);
        return 1;
    }

    FILE *file = fopen(argv[1], "rb");
    if (!file) {
        perror("fopen");
        return 1;
    }

    MD5_CTX context;
    unsigned char buffer[1024];
    size_t bytesRead;
    unsigned char digest[16];

    MD5Init(&context);

    while ((bytesRead = fread(buffer, 1, sizeof(buffer), file)) > 0) {
        MD5Update(&context, buffer, bytesRead);
    }

    MD5Final(digest, &context);

    PrintMD5(digest);

    fclose(file);
    return 0;
}
