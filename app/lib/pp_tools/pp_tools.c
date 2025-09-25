#include "pp_tools/pp_tools.h"
#include "pp_assert/pp_assert.h"

uint16_t pp_tools_crc16(uint8_t *pdata, uint16_t n) {
    PP_ASSERT(pdata != NULL);
    unsigned short c = 0xffff;
    for (int k = 0; k < n; k++) {
        unsigned short b = (((unsigned char *)pdata)[k]);
        for (char i = 0; i < 8; i++) {
            c = ((b ^ c) & 1) ? (c >> 1) ^ 0xA001 : (c >> 1);
            b >>= 1;
        }
    }
    return (c << 8) | (c >> 8);
}

uint8_t pp_tools_crc8(uint8_t *pdata, uint16_t n) {
    PP_ASSERT(pdata != NULL);
    uint8_t crc = 0;
    uint8_t crcdata;
    uint16_t i, j;
    for (i = 0; i < n; i++) {
        crcdata = pdata[i];
        for (j = 0x80; j != 0; j >>= 1) {
            if ((crc & 0x80) != 0) {
                crc <<= 1;
                crc ^= 0x07;
            } else
                crc <<= 1;
            if ((crcdata & j) != 0)
                crc ^= 0x07;
        }
    }
    return crc;
}

uint8_t pp_check_sum(uint8_t *data, uint8_t length) {
    PP_ASSERT(data != NULL);
    uint8_t res = 0;
    while(length--) {
        res += *data++;
    }
    return - res;
}

void pp_TEA_code(volatile long *data, volatile long *key) {
    PP_ASSERT(data != NULL);
    PP_ASSERT(key != NULL);
    unsigned long y = data[0], z = data[1], sum = 0, /* set up */
        delta = 0x9e3779b9, n = 32;            /* a keyey schedule constant */
    while (n-- > 0) {/* basic cycle start */
        sum += delta;
        y += (z << 4) + key[0] ^ z + sum ^ (z >> 5) + key[1];
        z += (y << 4) + key[2] ^ y + sum ^ (y >> 5) + key[3]; /* end cycle */
    }
    data[0] = y;
    data[1] = z;
}

void pp_TEA_decode(volatile long *data, volatile long *key) {
    PP_ASSERT(data != NULL);
    PP_ASSERT(key != NULL);
    unsigned long n = 32, sum, y = data[0], z = data[1],
                  delta = 0x9e3779b9;
    sum = delta << 5;
    /* start cycle */
    while (n-- > 0) {
        z -= (y << 4) + key[2] ^ y + sum ^ (y >> 5) + key[3];
        y -= (z << 4) + key[0] ^ z + sum ^ (z >> 5) + key[1];
        sum -= delta;
    }
    /* end cycle */
    data[0] = y;
    data[1] = z;
}
