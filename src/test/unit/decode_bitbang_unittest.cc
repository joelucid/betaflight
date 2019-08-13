#include "stdint.h"
#include "string.h"


extern "C" {
#include "drivers/telemetry_decode.h"
}

#include "gtest/gtest.h"

static uint16_t makeDshotValue(uint16_t value) 
{
    uint32_t crc = (~ (((value >> 8) ^ (value >> 4) ^ value))) & 0xf;
    value <<= 4;
    value |= crc;
    return value;
}

static uint32_t makeGcrValue(uint16_t value)
{
    value = makeDshotValue(value);
    static const uint32_t codingTable[] = {
        0x19, 0x1b, 0x12, 0x13, 0x1d, 0x15, 0x16, 0x17, 0x1a, 0x09, 0x0a, 0x0b, 0x1e, 0x0d, 0x0e, 0x0f
    };
    return codingTable[(value >> 12) & 0xf] << 15 | codingTable[(value >> 8) & 0xf] << 10 | codingTable[(value >> 4) & 0xf] << 5 | codingTable[(value) & 0xf];
}

static void makeArray(
    uint32_t array[], uint32_t count, uint32_t values[],
    uint32_t masks[], float speeds[], uint32_t offsets[], uint32_t bitcount)
{
    for (unsigned int n = 0; n < bitcount; n++) {
        for (unsigned int i = 0; i < offsets[n]; i++) {
            array[i] |= masks[n]; // starts out with 1s
        }
        int bit = 0;
        float pos = 0;
        unsigned int offset = offsets[n];
        unsigned int bitpos = 1 << 19;
        do {
            pos += speeds[n];
            while(offset - offsets[n] < pos) {
                if (bit) {
                    array[offset] |= masks[n];
                }
                offset++;
            }
            if (values[n] & bitpos) {
                bit = !bit;
            }
        } while (bitpos >>= 1);
        while (offset < count) {
            array[offset++] |= masks[n];
        }
    }
}


TEST(BitbangDecodeTest, Test1)
{
    uint32_t masks[] = { 1, 0x10, 0x100 };
    float speeds[] = { 3.0f, 3.3f, 2.7f};
    uint32_t offsets[] = { 0, 20, 30 };

    for (uint32_t v = 0; v < 0xfff; v++) {
        uint32_t values[] = { makeGcrValue(v), makeGcrValue(v), makeGcrValue(v) };
        uint32_t valuesOut[] = { 0, 0, 0 };

        uint32_t array[200];
        memset(array, 0, sizeof(array));
        makeArray(array, sizeof(array)/sizeof(*array), values, masks, speeds, offsets, 3);
        
        decodeBitBangTelemetryPacket( array, 200, valuesOut, masks, 3);
        
        EXPECT_EQ(v, valuesOut[0]);
        EXPECT_EQ(v, valuesOut[1]);
        EXPECT_EQ(v, valuesOut[2]);
    }
}


