#include <stdint.h>
#include <string.h>
#include <stdio.h>

#include "platform.h"

#if defined(USE_DSHOT_TELEMETRY)

#include "common/maths.h"
#include "common/utils.h"
#include "drivers/dshot.h"
#include "drivers/telemetry_decode.h"

#define MIN_VALID_BBSAMPLES ((21 - 2) * 3)
#define MAX_VALID_BBSAMPLES ((21 + 2) * 3)

#ifdef DEBUG_BBDECODE
uint16_t bbBuffer[134];
#endif

FAST_CODE uint32_t decode_bb( uint16_t buffer[], uint32_t count, uint32_t mask)
{
//    printf("start\n");
#ifdef DEBUG_BBDECODE
    uint32_t sequence[MAX_GCR_EDGES];
    memset(sequence, 0, sizeof(sequence));
    int sequenceIndex = 0;
#endif

    uint16_t lastValue = 0;
    uint32_t value = 0;

    uint16_t* p = buffer;
    uint16_t* endP = p + count - MIN_VALID_BBSAMPLES;
    while (p < endP) {
        if (__builtin_expect(!(*p++ & mask), 0) ||
            __builtin_expect(!(*p++ & mask), 0) ||
            __builtin_expect(!(*p++ & mask), 0) ||
            __builtin_expect(!(*p++ & mask), 0)) {
            break;
        }
    }

    if(*p & mask) {
        // not returning telemetry is ok if the cpu is overburdened.
        return BB_NOEDGE;
    }

    int remaining = MIN(count - (p - buffer), (unsigned int)MAX_VALID_BBSAMPLES);

    uint16_t* oldP = p;
    uint32_t bits = 0;
    endP = p + remaining;

#ifdef DEBUG_BBDECODE
    sequence[sequenceIndex++] = p - buffer;
#endif

    while ( endP > p ) {
        if (__builtin_expect((*p++ & mask) == lastValue, 1) &&
            __builtin_expect((*p++ & mask) == lastValue, 1) &&
            __builtin_expect((*p++ & mask) == lastValue, 1) &&
            __builtin_expect((*p++ & mask) == lastValue, 1)) {
            continue;
        }

#ifdef DEBUG_BBDECODE
        sequence[sequenceIndex++] = p - buffer;
#endif
        const int len = (p - oldP + 1) / 3;
        bits += len;
        value <<= len;
        value |= 1 << (len - 1);
        oldP = p;
        lastValue = *(p-1) & mask;
    }

    // length of last sequence has to be inferred since the last bit with inverted dshot is high
    const int nlen = 21 - bits;
#ifdef DEBUG_BBDECODE
    sequence[sequenceIndex] = sequence[sequenceIndex] + (nlen) * 3;
    sequenceIndex++;
#endif

    if (nlen > 0) {
        value <<= nlen;
        value |= 1 << (nlen - 1);
    }
    value &= 0xfffff;

    if (nlen < 0) {
        value = BB_INVALID;
    } else {

        // GCR decode the 5 nibbles
        static const uint32_t decode[32] = {
            0, 0, 0, 0, 0, 0, 0, 0, 0, 9, 10, 11, 0, 13, 14, 15,
            0, 0, 2, 3, 0, 5, 6, 7, 0, 0, 8, 1, 0, 4, 12, 0 };

        uint32_t decodedValue = decode[value & 0x1f];
        decodedValue |= decode[(value >> 5) & 0x1f] << 4;
        decodedValue |= decode[(value >> 10) & 0x1f] << 8;
        decodedValue |= decode[(value >> 15) & 0x1f] << 12;

        uint32_t csum = decodedValue;
        csum = csum ^ (csum >> 8); // xor bytes
        csum = csum ^ (csum >> 4); // xor nibbles

        if ((csum & 0xf) != 0xf) {
#ifdef DEBUG_BBDECODE
            memcpy(dshotTelemetryState.inputBuffer, sequence, sizeof(sequence));
            for (int i = 0; i < count; i++) {
                bbBuffer[i] = !!(buffer[i] & mask);
            }
#endif
            value = BB_INVALID;
        } else {
            value = decodedValue >> 4;

            if (value == 0x0fff) {
                return 0;
            }
            value = (value & 0x000001ff) << ((value & 0xfffffe00) >> 9);
            if (!value) {
                return BB_INVALID;
            }
            value = (1000000 * 60 / 100 + value / 2) / value;
        }
    }
    return value;
}

#endif
