/*
 * Centaurean SpookyHash
 *
 * Copyright (c) 2015, Guillaume Voirin
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of Centaurean nor the
 *       names of its contributors may be used to endorse or promote products
 *       derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * 25/01/15 12:21
 *
 * ----------
 * SpookyHash
 * ----------
 *
 * Author(s)
 * Bob Jenkins (http://burtleburtle.net/bob/hash/spooky.html)
 *
 * Description
 * Very fast non cryptographic hash
 */

#include "spookyhash.h"

SPOOKYHASH_FORCE_INLINE uint_fast64_t spookyhash_rotate(uint_fast64_t x, uint_fast8_t k) {
    return (x << k) | (x >> (64 - k));
}

SPOOKYHASH_FORCE_INLINE void spookyhash_short_end(uint_fast64_t *restrict h0, uint_fast64_t *restrict h1, uint_fast64_t *restrict h2, uint_fast64_t *restrict h3) {
    *h3 ^= *h2;
    *h2 = spookyhash_rotate(*h2, 15);
    *h3 += *h2;
    *h0 ^= *h3;
    *h3 = spookyhash_rotate(*h3, 52);
    *h0 += *h3;
    *h1 ^= *h0;
    *h0 = spookyhash_rotate(*h0, 26);
    *h1 += *h0;
    *h2 ^= *h1;
    *h1 = spookyhash_rotate(*h1, 51);
    *h2 += *h1;
    *h3 ^= *h2;
    *h2 = spookyhash_rotate(*h2, 28);
    *h3 += *h2;
    *h0 ^= *h3;
    *h3 = spookyhash_rotate(*h3, 9);
    *h0 += *h3;
    *h1 ^= *h0;
    *h0 = spookyhash_rotate(*h0, 47);
    *h1 += *h0;
    *h2 ^= *h1;
    *h1 = spookyhash_rotate(*h1, 54);
    *h2 += *h1;
    *h3 ^= *h2;
    *h2 = spookyhash_rotate(*h2, 32);
    *h3 += *h2;
    *h0 ^= *h3;
    *h3 = spookyhash_rotate(*h3, 25);
    *h0 += *h3;
    *h1 ^= *h0;
    *h0 = spookyhash_rotate(*h0, 63);
    *h1 += *h0;
}

SPOOKYHASH_FORCE_INLINE void spookyhash_short_mix(uint_fast64_t *restrict h0, uint_fast64_t *restrict h1, uint_fast64_t *restrict h2, uint_fast64_t *restrict h3) {
    *h2 = spookyhash_rotate(*h2, 50);
    *h2 += *h3;
    *h0 ^= *h2;
    *h3 = spookyhash_rotate(*h3, 52);
    *h3 += *h0;
    *h1 ^= *h3;
    *h0 = spookyhash_rotate(*h0, 30);
    *h0 += *h1;
    *h2 ^= *h0;
    *h1 = spookyhash_rotate(*h1, 41);
    *h1 += *h2;
    *h3 ^= *h1;
    *h2 = spookyhash_rotate(*h2, 54);
    *h2 += *h3;
    *h0 ^= *h2;
    *h3 = spookyhash_rotate(*h3, 48);
    *h3 += *h0;
    *h1 ^= *h3;
    *h0 = spookyhash_rotate(*h0, 38);
    *h0 += *h1;
    *h2 ^= *h0;
    *h1 = spookyhash_rotate(*h1, 37);
    *h1 += *h2;
    *h3 ^= *h1;
    *h2 = spookyhash_rotate(*h2, 62);
    *h2 += *h3;
    *h0 ^= *h2;
    *h3 = spookyhash_rotate(*h3, 34);
    *h3 += *h0;
    *h1 ^= *h3;
    *h0 = spookyhash_rotate(*h0, 5);
    *h0 += *h1;
    *h2 ^= *h0;
    *h1 = spookyhash_rotate(*h1, 36);
    *h1 += *h2;
    *h3 ^= *h1;
}

SPOOKYHASH_FORCE_INLINE void spookyhash_short(const void *restrict message, size_t length, uint_fast64_t *restrict hash1, uint_fast64_t *restrict hash2) {
#if !SPOOKYHASH_SPOOKY_HASH_ALLOW_UNALIGNED_READS
    uint_fast64_t buffer[2 * sc_numVars];
#endif

    union {
        const uint_fast8_t *p8;
        uint_fast32_t *p32;
        uint_fast64_t *p64;
        size_t i;
    } u;
    u.p8 = (const uint_fast8_t *) message;

#if !SPOOKYHASH_SPOOKY_HASH_ALLOW_UNALIGNED_READS
    if (u.i & 0x7) {
        memcpy(buffer, message, length);
        u.p64 = buffer;
    }
#endif

    size_t remainder = length % 32;
    uint_fast64_t a = *hash1;
    uint_fast64_t b = *hash2;
    uint_fast64_t c = sc_const;
    uint_fast64_t d = sc_const;

    if (length > 15) {
        const uint_fast64_t *end = u.p64 + (length / 32) * 4;
        for (; u.p64 < end; u.p64 += 4) {
            c += u.p64[0];
            d += u.p64[1];
            spookyhash_short_mix(&a, &b, &c, &d);
            a += u.p64[2];
            b += u.p64[3];
        }

        if (remainder >= 16) {
            c += u.p64[0];
            d += u.p64[1];
            spookyhash_short_mix(&a, &b, &c, &d);
            u.p64 += 2;
            remainder -= 16;
        }
    }

    d += ((uint_fast64_t) length) << 56;
    switch (remainder) {
        default:
            break;
        case 15:
            d += ((uint_fast64_t) u.p8[14]) << 48;
        case 14:
            d += ((uint_fast64_t) u.p8[13]) << 40;
        case 13:
            d += ((uint_fast64_t) u.p8[12]) << 32;
        case 12:
            d += u.p32[2];
            c += u.p64[0];
            break;
        case 11:
            d += ((uint_fast64_t) u.p8[10]) << 16;
        case 10:
            d += ((uint_fast64_t) u.p8[9]) << 8;
        case 9:
            d += (uint_fast64_t) u.p8[8];
        case 8:
            c += u.p64[0];
            break;
        case 7:
            c += ((uint_fast64_t) u.p8[6]) << 48;
        case 6:
            c += ((uint_fast64_t) u.p8[5]) << 40;
        case 5:
            c += ((uint_fast64_t) u.p8[4]) << 32;
        case 4:
            c += u.p32[0];
            break;
        case 3:
            c += ((uint_fast64_t) u.p8[2]) << 16;
        case 2:
            c += ((uint_fast64_t) u.p8[1]) << 8;
        case 1:
            c += (uint_fast64_t) u.p8[0];
            break;
        case 0:
            c += sc_const;
            d += sc_const;
    }
    spookyhash_short_end(&a, &b, &c, &d);
    *hash1 = a;
    *hash2 = b;
}

SPOOKYHASH_FORCE_INLINE void spookyhash_mix(const uint_fast64_t *restrict data, uint_fast64_t *restrict s0, uint_fast64_t *restrict s1, uint_fast64_t *restrict s2, uint_fast64_t *restrict s3, uint_fast64_t *restrict s4, uint_fast64_t *restrict s5, uint_fast64_t *restrict s6, uint_fast64_t *restrict s7, uint_fast64_t *restrict s8, uint_fast64_t *restrict s9, uint_fast64_t *restrict s10, uint_fast64_t *restrict s11) {
    *s0 += data[0];
    *s2 ^= *s10;
    *s11 ^= *s0;
    *s0 = spookyhash_rotate(*s0, 11);
    *s11 += *s1;
    *s1 += data[1];
    *s3 ^= *s11;
    *s0 ^= *s1;
    *s1 = spookyhash_rotate(*s1, 32);
    *s0 += *s2;
    *s2 += data[2];
    *s4 ^= *s0;
    *s1 ^= *s2;
    *s2 = spookyhash_rotate(*s2, 43);
    *s1 += *s3;
    *s3 += data[3];
    *s5 ^= *s1;
    *s2 ^= *s3;
    *s3 = spookyhash_rotate(*s3, 31);
    *s2 += *s4;
    *s4 += data[4];
    *s6 ^= *s2;
    *s3 ^= *s4;
    *s4 = spookyhash_rotate(*s4, 17);
    *s3 += *s5;
    *s5 += data[5];
    *s7 ^= *s3;
    *s4 ^= *s5;
    *s5 = spookyhash_rotate(*s5, 28);
    *s4 += *s6;
    *s6 += data[6];
    *s8 ^= *s4;
    *s5 ^= *s6;
    *s6 = spookyhash_rotate(*s6, 39);
    *s5 += *s7;
    *s7 += data[7];
    *s9 ^= *s5;
    *s6 ^= *s7;
    *s7 = spookyhash_rotate(*s7, 57);
    *s6 += *s8;
    *s8 += data[8];
    *s10 ^= *s6;
    *s7 ^= *s8;
    *s8 = spookyhash_rotate(*s8, 55);
    *s7 += *s9;
    *s9 += data[9];
    *s11 ^= *s7;
    *s8 ^= *s9;
    *s9 = spookyhash_rotate(*s9, 54);
    *s8 += *s10;
    *s10 += data[10];
    *s0 ^= *s8;
    *s9 ^= *s10;
    *s10 = spookyhash_rotate(*s10, 22);
    *s9 += *s11;
    *s11 += data[11];
    *s1 ^= *s9;
    *s10 ^= *s11;
    *s11 = spookyhash_rotate(*s11, 46);
    *s10 += *s0;
}

SPOOKYHASH_FORCE_INLINE void spookyhash_end_partial(uint_fast64_t *restrict h0, uint_fast64_t *restrict h1, uint_fast64_t *restrict h2, uint_fast64_t *restrict h3, uint_fast64_t *restrict h4, uint_fast64_t *restrict h5, uint_fast64_t *restrict h6, uint_fast64_t *restrict h7, uint_fast64_t *restrict h8, uint_fast64_t *restrict h9, uint_fast64_t *restrict h10, uint_fast64_t *restrict h11) {
    *h11 += *h1;
    *h2 ^= *h11;
    *h1 = spookyhash_rotate(*h1, 44);
    *h0 += *h2;
    *h3 ^= *h0;
    *h2 = spookyhash_rotate(*h2, 15);
    *h1 += *h3;
    *h4 ^= *h1;
    *h3 = spookyhash_rotate(*h3, 34);
    *h2 += *h4;
    *h5 ^= *h2;
    *h4 = spookyhash_rotate(*h4, 21);
    *h3 += *h5;
    *h6 ^= *h3;
    *h5 = spookyhash_rotate(*h5, 38);
    *h4 += *h6;
    *h7 ^= *h4;
    *h6 = spookyhash_rotate(*h6, 33);
    *h5 += *h7;
    *h8 ^= *h5;
    *h7 = spookyhash_rotate(*h7, 10);
    *h6 += *h8;
    *h9 ^= *h6;
    *h8 = spookyhash_rotate(*h8, 13);
    *h7 += *h9;
    *h10 ^= *h7;
    *h9 = spookyhash_rotate(*h9, 38);
    *h8 += *h10;
    *h11 ^= *h8;
    *h10 = spookyhash_rotate(*h10, 53);
    *h9 += *h11;
    *h0 ^= *h9;
    *h11 = spookyhash_rotate(*h11, 42);
    *h10 += *h0;
    *h1 ^= *h10;
    *h0 = spookyhash_rotate(*h0, 54);
}

SPOOKYHASH_FORCE_INLINE void spookyhash_end(const uint_fast64_t *restrict data, uint_fast64_t *restrict h0, uint_fast64_t *restrict h1, uint_fast64_t *restrict h2, uint_fast64_t *restrict h3, uint_fast64_t *restrict h4, uint_fast64_t *restrict h5, uint_fast64_t *restrict h6, uint_fast64_t *restrict h7, uint_fast64_t *restrict h8, uint_fast64_t *restrict h9, uint_fast64_t *restrict h10, uint_fast64_t *restrict h11) {
    *h0 += data[0];
    *h1 += data[1];
    *h2 += data[2];
    *h3 += data[3];
    *h4 += data[4];
    *h5 += data[5];
    *h6 += data[6];
    *h7 += data[7];
    *h8 += data[8];
    *h9 += data[9];
    *h10 += data[10];
    *h11 += data[11];
    spookyhash_end_partial(h0, h1, h2, h3, h4, h5, h6, h7, h8, h9, h10, h11);
    spookyhash_end_partial(h0, h1, h2, h3, h4, h5, h6, h7, h8, h9, h10, h11);
    spookyhash_end_partial(h0, h1, h2, h3, h4, h5, h6, h7, h8, h9, h10, h11);
}

SPOOKYHASH_FORCE_INLINE void spookyhash_128(const void *restrict message, size_t length, uint_fast64_t *restrict hash1, uint_fast64_t *restrict hash2) {
    if (length < sc_bufSize) {
        spookyhash_short(message, length, hash1, hash2);
        return;
    }

    uint_fast64_t h0, h1, h2, h3, h4, h5, h6, h7, h8, h9, h10, h11;
    uint_fast64_t buf[sc_numVars];
    uint_fast64_t *end;
    union {
        const uint_fast8_t *p8;
        uint_fast64_t *p64;
        size_t i;
    } u;
    size_t remainder;

    h0 = h3 = h6 = h9 = *hash1;
    h1 = h4 = h7 = h10 = *hash2;
    h2 = h5 = h8 = h11 = sc_const;

    u.p8 = (const uint_fast8_t *) message;
    end = u.p64 + (length / sc_blockSize) * sc_numVars;

    if (SPOOKYHASH_ALLOW_UNALIGNED_READS || ((u.i & 0x7) == 0)) {
        while (u.p64 < end) {
            spookyhash_mix(u.p64, &h0, &h1, &h2, &h3, &h4, &h5, &h6, &h7, &h8, &h9, &h10, &h11);
            u.p64 += sc_numVars;
        }
    } else {
        while (u.p64 < end) {
            memcpy(buf, u.p64, sc_blockSize);
            spookyhash_mix(buf, &h0, &h1, &h2, &h3, &h4, &h5, &h6, &h7, &h8, &h9, &h10, &h11);
            u.p64 += sc_numVars;
        }
    }

    remainder = (length - ((const uint_fast8_t *) end - (const uint_fast8_t *) message));
    memcpy(buf, end, remainder);
    memset(((uint_fast8_t *) buf) + remainder, 0, sc_blockSize - remainder);
    ((uint_fast8_t *) buf)[sc_blockSize - 1] = (uint_fast8_t) remainder;

    spookyhash_end(buf, &h0, &h1, &h2, &h3, &h4, &h5, &h6, &h7, &h8, &h9, &h10, &h11);
    *hash1 = h0;
    *hash2 = h1;
}

SPOOKYHASH_FORCE_INLINE uint_fast64_t spookyhash_64(const void *message, size_t length, uint_fast64_t seed) {
    uint_fast64_t hash1 = seed;
    spookyhash_128(message, length, &hash1, &seed);
    return hash1;
}

SPOOKYHASH_FORCE_INLINE uint_fast32_t spookyhash_32(const void *message, size_t length, uint_fast32_t seed) {
    uint_fast64_t hash1 = seed, hash2 = seed;
    spookyhash_128(message, length, &hash1, &hash2);
    return (uint_fast32_t) hash1;
}

SPOOKYHASH_FORCE_INLINE void spookyhash_update(spookyhash_context *restrict context, const void *restrict message, size_t length) {
    uint_fast64_t h0, h1, h2, h3, h4, h5, h6, h7, h8, h9, h10, h11;
    size_t newLength = length + context->m_remainder;
    uint_fast8_t remainder;
    union {
        const uint_fast8_t *p8;
        uint_fast64_t *p64;
        size_t i;
    } u;
    const uint_fast64_t *end;

    if (newLength < sc_bufSize) {
        memcpy(&((uint_fast8_t *) context->m_data)[context->m_remainder], message, length);
        context->m_length = length + context->m_length;
        context->m_remainder = (uint_fast8_t) newLength;
        return;
    }

    if (context->m_length < sc_bufSize) {
        h0 = h3 = h6 = h9 = context->m_state[0];
        h1 = h4 = h7 = h10 = context->m_state[1];
        h2 = h5 = h8 = h11 = sc_const;
    }
    else {
        h0 = context->m_state[0];
        h1 = context->m_state[1];
        h2 = context->m_state[2];
        h3 = context->m_state[3];
        h4 = context->m_state[4];
        h5 = context->m_state[5];
        h6 = context->m_state[6];
        h7 = context->m_state[7];
        h8 = context->m_state[8];
        h9 = context->m_state[9];
        h10 = context->m_state[10];
        h11 = context->m_state[11];
    }
    context->m_length = length + context->m_length;

    if (context->m_remainder) {
        uint_fast8_t prefix = (uint_fast8_t) (sc_bufSize - context->m_remainder);
        memcpy(&(((uint_fast8_t *) context->m_data)[context->m_remainder]), message, prefix);
        u.p64 = context->m_data;
        spookyhash_mix(u.p64, &h0, &h1, &h2, &h3, &h4, &h5, &h6, &h7, &h8, &h9, &h10, &h11);
        spookyhash_mix(&u.p64[sc_numVars], &h0, &h1, &h2, &h3, &h4, &h5, &h6, &h7, &h8, &h9, &h10, &h11);
        u.p8 = ((const uint_fast8_t *) message) + prefix;
        length -= prefix;
    } else {
        u.p8 = (const uint_fast8_t *) message;
    }

    end = u.p64 + (length / sc_blockSize) * sc_numVars;
    remainder = (uint_fast8_t) (length - ((const uint_fast8_t *) end - u.p8));
    if (SPOOKYHASH_ALLOW_UNALIGNED_READS || (u.i & 0x7) == 0) {
        while (u.p64 < end) {
            spookyhash_mix(u.p64, &h0, &h1, &h2, &h3, &h4, &h5, &h6, &h7, &h8, &h9, &h10, &h11);
            u.p64 += sc_numVars;
        }
    }
    else {
        while (u.p64 < end) {
            memcpy(context->m_data, u.p8, sc_blockSize);
            spookyhash_mix(context->m_data, &h0, &h1, &h2, &h3, &h4, &h5, &h6, &h7, &h8, &h9, &h10, &h11);
            u.p64 += sc_numVars;
        }
    }

    context->m_remainder = remainder;
    memcpy(context->m_data, end, remainder);

    context->m_state[0] = h0;
    context->m_state[1] = h1;
    context->m_state[2] = h2;
    context->m_state[3] = h3;
    context->m_state[4] = h4;
    context->m_state[5] = h5;
    context->m_state[6] = h6;
    context->m_state[7] = h7;
    context->m_state[8] = h8;
    context->m_state[9] = h9;
    context->m_state[10] = h10;
    context->m_state[11] = h11;
}


SPOOKYHASH_FORCE_INLINE void spookyhash_final(spookyhash_context *restrict context, uint_fast64_t *restrict hash1, uint_fast64_t *restrict hash2) {
    if (context->m_length < sc_bufSize) {
        *hash1 = context->m_state[0];
        *hash2 = context->m_state[1];
        spookyhash_short(context->m_data, context->m_length, hash1, hash2);
        return;
    }

    const uint_fast64_t *data = (const uint_fast64_t *) context->m_data;
    uint_fast8_t remainder = context->m_remainder;

    uint_fast64_t h0 = context->m_state[0];
    uint_fast64_t h1 = context->m_state[1];
    uint_fast64_t h2 = context->m_state[2];
    uint_fast64_t h3 = context->m_state[3];
    uint_fast64_t h4 = context->m_state[4];
    uint_fast64_t h5 = context->m_state[5];
    uint_fast64_t h6 = context->m_state[6];
    uint_fast64_t h7 = context->m_state[7];
    uint_fast64_t h8 = context->m_state[8];
    uint_fast64_t h9 = context->m_state[9];
    uint_fast64_t h10 = context->m_state[10];
    uint_fast64_t h11 = context->m_state[11];

    if (remainder >= sc_blockSize) {
        spookyhash_mix(data, &h0, &h1, &h2, &h3, &h4, &h5, &h6, &h7, &h8, &h9, &h10, &h11);
        data += sc_numVars;
        remainder -= sc_blockSize;
    }

    memset(&((uint_fast8_t *) data)[remainder], 0, (sc_blockSize - remainder));

    ((uint_fast8_t *) data)[sc_blockSize - 1] = remainder;

    spookyhash_end(data, &h0, &h1, &h2, &h3, &h4, &h5, &h6, &h7, &h8, &h9, &h10, &h11);

    *hash1 = h0;
    *hash2 = h1;
}
