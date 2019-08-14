#include <stdint.h>
#include <string.h>
#include <stdio.h>

#include "platform.h"

#if defined(USE_DSHOT_TELEMETRY)

#include "common/utils.h"
#include "drivers/telemetry_decode.h"

uint32_t decodeBitBangTelemetryPacket2(
    uint32_t buffer[], uint32_t count, uint32_t mask)
{
    UNUSED(count);
    
//    printf("start\n");
    
    uint32_t lastValue = 0;
    uint32_t value = 0;

    uint32_t* p = buffer;
    for (uint32_t i = count; i--;) {
        if (!(*p & mask)) {
            break;
        } else {
            p++;
        }
    }

    uint32_t remaining = count - (p - buffer);
    if (remaining > 22 * 3) {
        remaining = 22 * 3;
    }
    
    uint32_t* oldP = p;
    uint32_t bits = 0;
    for (; remaining--; ) {
        const uint32_t v = *p++ & mask;
        if (__builtin_expect(v == lastValue, 1)) {
            continue;
        } else {
            const int len = (p - oldP + 1) / 3;
            bits += len;
//            printf("len: %u, diff: %u\n",len, (int)(p - oldP + 1));
            value <<= len;
            value |= 1 << (len - 1);
            oldP = p;
            lastValue = v;
        }
    }

    int len = 21 - bits;
//    printf("len: %u\n",len);
    if (len > 0) {
        value <<= len;
        value |= 1 << (len - 1);
    }
    value &= 0xfffff;

    if (len < 0) {
        value = 0xffff;
    } else {
        
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
            value = 0xffff;
        } else {
            value = decodedValue >> 4;
        }
    }
    return value;
}

    
void decodeBitBangTelemetryPacket(
    uint32_t buffer[], uint32_t count, uint32_t invalues[], uint32_t bitMasks[], uint32_t bitCount)
{
    uint32_t lastValue = 0;
    uint32_t sequenceLen[bitCount];
    uint32_t started = 0;
    uint32_t bits[bitCount];

    memset(bits, 0, sizeof(bits));
    for (unsigned int i = 0; i < bitCount; i++) {
        sequenceLen[i] = 1;
    }
    
    uint32_t values[bitCount];
    memset(values, 0, sizeof(uint32_t) * bitCount);
    for (unsigned int i = 0; i < bitCount; i++) {
        values[i] = invalues[i];
    }
    
    for (unsigned int i = 0; i < count; i++) {
        const uint32_t v = buffer[i];
        const uint32_t changed = v ^ lastValue;
        
        for (unsigned int n = 0; n < bitCount; n++) {
            const uint32_t curMask = bitMasks[n];
            if (started & curMask) {
                if (changed & curMask) {
                    int len = (sequenceLen[n] + 1) / 3;
                    values[n] <<= len;
                    values[n] |= 1 << (len - 1);
                    bits[n] += len;
                    sequenceLen[n] = 1;
                } else {
                    sequenceLen[n]++;
                }
            }
        }
        lastValue = v;
        started |= ~v;
    }
    for (unsigned int n = 0; n < bitCount; n++) {
        int len = 21 - bits[n];
        if (len > 0) {
            values[n] <<= len;
            values[n] |= 1 << (len - 1);
        }
        values[n] &= 0xfffff;

        if (len < 0) {
            values[n] = 0xffff;
        } else {
            const uint32_t value = values[n];
                                           
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
                values[n] = 0xffff;
            } else {
                values[n] = decodedValue >> 4;
            }
        }
    }
    for (unsigned int i = 0; i < bitCount; i++) {
        invalues[i] = values[i];
    }
}

uint32_t convertTelemetryValueToRpmDiv100(uint32_t value)
{
    if (value == 0x0fff) {
        return 0;
    }
    value = (value & 0x000001ff) << ((value & 0xfffffe00) >> 9);
    if (!value) {
        return 0xffff;
    }
    return (1000000 * 60 / 100 + value / 2) / value;
}


#endif
